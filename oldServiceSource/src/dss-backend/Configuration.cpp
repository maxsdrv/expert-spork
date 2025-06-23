#include "Configuration.h"

#include "ConfigParser.h"

#ifdef USE_ALIA
#include "alia/AliaController.h"
#endif
#ifdef USE_RADAR_IQ
#include "radariq/RadarIqController.h"
#endif
#ifdef USE_REST
#include "rest/RestController.h"
#endif
#ifdef USE_SKYCOPE
#include "skycope/SkycopeController.h"
#endif
#ifdef USE_TAMERLAN
#include "tamerlan/TamerlanController.h"
#endif

namespace mpk::dss::backend
{

std::vector<std::unique_ptr<BackendController>> createBackend(
    const YAML::Node& config, const BackendComponents& components)
{
    BackendFactory backendFactory;

    // Common modules

    // Specific modules (keep sorting alphabetically)
#ifdef USE_ALIA
    backendFactory.registerNodeParser(
        alia::nodeKey, alia::AliaController::fromDescription);
#endif
#ifdef USE_RADAR_IQ
    backendFactory.registerNodeParser(
        radariq::nodeKey, radariq::RadarIqController::fromDescription);
#endif
#ifdef USE_REST
    backendFactory.registerNodeParser(
        rest::nodeKey, rest::RestController::fromDescription);
#endif
#ifdef USE_SKYCOPE
    backendFactory.registerNodeParser(
        skycope::nodeKey, skycope::SkycopeController::fromDescription);
#endif
#ifdef USE_TAMERLAN
    backendFactory.registerNodeParser(
        tamerlan::nodeKey, tamerlan::TamerlanController::fromDescription);
#endif

    return backendFactory.createBackends(config, components);
}

} // namespace mpk::dss::backend
