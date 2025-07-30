#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOMedia.h>
#include <DiskArbitration/DiskArbitration.h>
#include <mach-o/dyld.h>
#include <sys/utsname.h>
#include <unistd.h>
#else
// Other platforms...
#endif

#include <cstring>
#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include "../../base/string_utils.h"
#include "../../base/logger.h"
#include "../os.h"

// Assume DiskInfo is defined somewhere with fields such as:
// int id;
// char device[MAX_PATH];
// unsigned char disk_sn[8];
// char label[255];
// bool preferred, label_initialized, sn_initialized;

//
// Helper: Compute a simple disk identifier from a string (fallback for UUID)
//
static void parse_disk_id(const char *input, unsigned char *buffer_out, size_t out_size) {
	unsigned int i;
	size_t len = strlen(input);
	memset(buffer_out, 0, out_size);
	for (i = 0; i < len; i++) {
		buffer_out[i % out_size] ^= input[i];
	}
}

//
// macOS implementation of getDiskInfos using IOKit and Disk Arbitration
//
FUNCTION_RETURN getDiskInfos(std::vector<DiskInfo> &disk_infos) {
#ifdef __APPLE__
	// Create a matching dictionary for IOMedia objects that represent whole disks.
	CFMutableDictionaryRef matchingDict = IOServiceMatching("IOMedia");
	if (matchingDict == NULL) {
		LOG_DEBUG("IOServiceMatching failed");
		return FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
	}
	// Only consider whole media (not partitions)
	CFDictionarySetValue(matchingDict, CFSTR("Whole"), kCFBooleanTrue);

	io_iterator_t iter;
	kern_return_t kr = IOServiceGetMatchingServices(kIOMasterPortDefault, matchingDict, &iter);
	if (kr != KERN_SUCCESS) {
		LOG_DEBUG("IOServiceGetMatchingServices failed");
		return FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
	}

	io_object_t media;
	int diskNum = 0;
	while ((media = IOIteratorNext(iter)) != 0) {
		DiskInfo diskInfo = {};
		diskInfo.id = diskNum++;

		// Retrieve the BSD Name (e.g. "disk0")
		CFStringRef bsdNameRef = (CFStringRef) IORegistryEntryCreateCFProperty(media, CFSTR("BSD Name"), kCFAllocatorDefault, 0);
		if (bsdNameRef) {
			char bsdName[256] = {0};
			if (CFStringGetCString(bsdNameRef, bsdName, sizeof(bsdName), kCFStringEncodingUTF8)) {
				strlcpy(diskInfo.device, bsdName, MAX_PATH);
			}
			CFRelease(bsdNameRef);
		}

		// For a disk identifier we try to obtain a UUID.
		// IOMedia may not always have a UUID property, so as a fallback we compute a hash of the BSD name.
		CFStringRef uuidRef = (CFStringRef) IORegistryEntryCreateCFProperty(media, CFSTR("UUID"), kCFAllocatorDefault, 0);
		if (uuidRef) {
			char uuidStr[256] = {0};
			if (CFStringGetCString(uuidRef, uuidStr, sizeof(uuidStr), kCFStringEncodingUTF8)) {
				// Here you might parse the UUID string into 8 bytes.
				// For simplicity, we use our fallback function.
				parse_disk_id(uuidStr, diskInfo.disk_sn, sizeof(diskInfo.disk_sn));
				diskInfo.sn_initialized = true;
			}
			CFRelease(uuidRef);
		} else {
			// Use the BSD name as a fallback identifier.
			parse_disk_id(diskInfo.device, diskInfo.disk_sn, sizeof(diskInfo.disk_sn));
			diskInfo.sn_initialized = true;
		}

		// Use Disk Arbitration to get a volume name (if the disk is mounted).
		DASessionRef session = DASessionCreate(kCFAllocatorDefault);
		if (session) {
			// Create a DADiskRef from the BSD name.
			DADiskRef disk = DADiskCreateFromBSDName(kCFAllocatorDefault, session, diskInfo.device);
			if (disk) {
				CFDictionaryRef diskDesc = DADiskCopyDescription(disk);
				if (diskDesc) {
					// kDADiskDescriptionVolumeNameKey contains the volume name.
					CFStringRef volName = (CFStringRef) CFDictionaryGetValue(diskDesc, kDADiskDescriptionVolumeNameKey);
					if (volName) {
						char volNameC[256] = {0};
						if (CFStringGetCString(volName, volNameC, sizeof(volNameC), kCFStringEncodingUTF8)) {
							strlcpy(diskInfo.label, volNameC, sizeof(diskInfo.label));
							diskInfo.label_initialized = true;
						}
					}
					CFRelease(diskDesc);
				}
				CFRelease(disk);
			}
			CFRelease(session);
		}

		// Set preferred flag based on your own criteria.
		// For example, you might mark internal disks as preferred.
		diskInfo.preferred = false; // (adjust as needed)

		disk_infos.push_back(diskInfo);
		IOObjectRelease(media);
	}
	IOObjectRelease(iter);
	return (disk_infos.size() > 0) ? FUNCTION_RETURN::FUNC_RET_OK : FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
#else
	return FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
#endif
}

//
// macOS implementation of getModuleName using _NSGetExecutablePath
//
FUNCTION_RETURN getModuleName(char buffer[MAX_PATH]) {
#ifdef __APPLE__
	uint32_t size = MAX_PATH;
	if (_NSGetExecutablePath(buffer, &size) == 0) {
		return FUNCTION_RETURN::FUNC_RET_OK;
	} else {
		// Buffer too small; size now contains the required size.
		LOG_DEBUG("Buffer too small for executable path; required size: %u", size);
		return FUNCTION_RETURN::FUNC_RET_ERROR;
	}
#else
	// Linux or other platforms
	return FUNCTION_RETURN::FUNC_RET_NOT_AVAIL;
#endif
}

//
// getMachineName remains similar since uname() is available on macOS
//
FUNCTION_RETURN getMachineName(unsigned char identifier[6]) {
	struct utsname u;
	if (uname(&u) < 0) {
		return FUNCTION_RETURN::FUNC_RET_ERROR;
	}
	memcpy(identifier, u.nodename, 6);
	return FUNCTION_RETURN::FUNC_RET_OK;
}
