/*
 * cpu_info.cpp
 *
 *  Created on: Dec 14, 2019
 *      Author: devel
 */

// https://github.com/anyrtcIO-Community/anyRTC-RTMP-OpenSource/blob/1.0Release/webrtc/base/basictypes.h
// Detect compiler is for x86 or x64.
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define CPU_X86 1
#elif  defined(__arm__) || defined(__aarch64__) || defined(_M_ARM)
#define CPU_ARM 1
#else
#error "Unsupported architecture"
#endif


#if defined(CPU_X86)
#include <cpuid.h>
#elif defined(CPU_ARM)
#include <fstream>
#endif

#include "../cpu_info.hpp"
#include <memory.h>
#include <string>
#include <unordered_set>

namespace license {
namespace os {
using namespace std;

#if defined(CPU_X86)
struct CPUVendorID {
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;

	string toString() const { return string(reinterpret_cast<const char *>(this), 12); }
};

static string get_cpu_vendor() {
	unsigned int level = 0, eax = 0, ebx = 0, ecx = 0, edx = 0;
	// hypervisor flag false, try to get the vendor name, see if it's a virtual cpu
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	CPUVendorID vendorID{.ebx = ebx, .edx = edx, .ecx = ecx};
	return vendorID.toString();
}
#elif defined(CPU_ARM)
static string get_cpu_vendor() { return "ARM"; }
#endif

#if defined(CPU_X86)
// https://en.wikipedia.org/wiki/CPUID
static string get_cpu_brand() {
	string result;
	uint32_t brand[0x10];

	if (!__get_cpuid_max(0x80000004, NULL)) {
		result = "NA";
	} else {
		memset(brand, 0, sizeof(brand));
		__get_cpuid(0x80000002, brand + 0x0, brand + 0x1, brand + 0x2, brand + 0x3);
		__get_cpuid(0x80000003, brand + 0x4, brand + 0x5, brand + 0x6, brand + 0x7);
		__get_cpuid(0x80000004, brand + 0x8, brand + 0x9, brand + 0xa, brand + 0xb);
		result = string(reinterpret_cast<char *>(brand));
	}
	return result;
}

#elif defined(CPU_ARM)
static string get_cpu_brand() {
	std::ifstream cpuinfo("/proc/cpuinfo");
	std::string line;
	while (std::getline(cpuinfo, line)) {
		auto posModel = line.find("model name");
		if (posModel != std::string::npos) {
			static std::string seperator = ": ";
			auto posSeperator = line.find(seperator);
			if (posSeperator != std::string::npos) {
				return line.substr(posSeperator + seperator.size());
			}
		}
	}

	return "";
}
#endif

CpuInfo::CpuInfo() : m_vendor(get_cpu_vendor()), m_brand(get_cpu_brand()) {}

CpuInfo::~CpuInfo() {}
/**
 * Detect Virtual machine using hypervisor bit.
 * @return true if the cpu hypervisor bit is set to 1
 */

#if defined(CPU_X86)
bool CpuInfo::is_hypervisor_set() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);

	bool is_virtual = (((ecx >> 31) & 1) == 1);  // hypervisor flag
	return is_virtual;
}

#elif defined(CPU_ARM)
bool CpuInfo::is_hypervisor_set() const { return false; }
#endif

#if defined(CPU_X86)
uint32_t CpuInfo::model() const {
	uint32_t level = 1, eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(level, &eax, &ebx, &ecx, &edx);
	// ax bits 0-3 stepping,4-7 model,8-11 family id,12-13 processor type
	//        14-15 reserved, 16-19 extended model, 20-27 extended family, 27-31 reserved
	// bx bits 0-7 brand index
	return (eax & 0x3FFF) | (eax & 0x3FF8000) >> 2 | (ebx & 0xff) << 24;
}
#elif defined(CPU_ARM)
uint32_t CpuInfo::model() const { return 0; }
#endif

}  // namespace os
} /* namespace license */
