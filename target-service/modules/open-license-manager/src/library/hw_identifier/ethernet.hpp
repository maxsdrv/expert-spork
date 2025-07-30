/*
 * ethernet.hpp
 *
 *  Created on: Jan 11, 2020
 *      Author: devel
 */

#ifndef SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_
#define SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_

#include "identification_strategy.hpp"

#include <unordered_set>
#include <optional>

namespace license {
namespace hw_identifier {

class Ethernet : public IdentificationStrategy {
public:
	Ethernet(bool use_ip);
	~Ethernet() override;

	LCC_API_HW_IDENTIFICATION_STRATEGY identification_strategy() const override;
	std::vector<HwIdentifier> alternative_ids() const override;

	using Interfaces = std::unordered_set<std::string>;
	using InterfacesOpt = std::optional<Interfaces>;

	static InterfacesOpt getInterfacesFromEnv();

private:
	static Interfaces split(const std::string &string, char delimiter = ',');

private:
	const bool use_ip;
};

}  // namespace hw_identifier
} /* namespace license */

#endif /* SRC_LIBRARY_PC_IDENTIFIER_ETHERNET_HPP_ */
