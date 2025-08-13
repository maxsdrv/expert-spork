cmake_minimum_required(VERSION 3.5)

project(skycope-client)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Gui)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME}
  OASkycopeActions.h
  OASkycopeAuth_token.h
  OASkycopeDefenseStatus.h
  OASkycopeDetectingFrequency.h
  OASkycopeDiskIoUsage.h
  OASkycopeDrone.h
  OASkycopeDroneSensorLdResult.h
  OASkycopeDroneWifiDevice.h
  OASkycopeFcKhzAndCount.h
  OASkycopeFixedGain.h
  OASkycopeFrequencyOption.h
  OASkycopeGps.h
  OASkycopeLAN1.h
  OASkycopeLAN2.h
  OASkycopeMassDefense.h
  OASkycopeMcuStatus.h
  OASkycopeMgmtDetails.h
  OASkycopeModel.h
  OASkycopeModule1.h
  OASkycopeNetworkUsage.h
  OASkycopeNfzSpoofStatus.h
  OASkycopePairedRC.h
  OASkycopeSensor.h
  OASkycopeSharedNames.h
  OASkycopeSomStatus.h
  OASkycopeSomTemp.h
  OASkycopeSpoofStatus.h
  OASkycopeStatus.h
  OASkycopeThreat.h
  OASkycopeWifiStatus.h
  OASkycopeDefaultApi.h
  OASkycopeHelpers.h
  OASkycopeHttpRequest.h
  OASkycopeObject.h
  OASkycopeEnum.h
  OASkycopeHttpFileElement.h
  OASkycopeServerConfiguration.h
  OASkycopeServerVariable.h
  OASkycopeOauth.h
  OASkycopeActions.cpp
  OASkycopeAuth_token.cpp
  OASkycopeDefenseStatus.cpp
  OASkycopeDetectingFrequency.cpp
  OASkycopeDiskIoUsage.cpp
  OASkycopeDrone.cpp
  OASkycopeDroneSensorLdResult.cpp
  OASkycopeDroneWifiDevice.cpp
  OASkycopeFcKhzAndCount.cpp
  OASkycopeFixedGain.cpp
  OASkycopeFrequencyOption.cpp
  OASkycopeGps.cpp
  OASkycopeLAN1.cpp
  OASkycopeLAN2.cpp
  OASkycopeMassDefense.cpp
  OASkycopeMcuStatus.cpp
  OASkycopeMgmtDetails.cpp
  OASkycopeModel.cpp
  OASkycopeModule1.cpp
  OASkycopeNetworkUsage.cpp
  OASkycopeNfzSpoofStatus.cpp
  OASkycopePairedRC.cpp
  OASkycopeSensor.cpp
  OASkycopeSharedNames.cpp
  OASkycopeSomStatus.cpp
  OASkycopeSomTemp.cpp
  OASkycopeSpoofStatus.cpp
  OASkycopeStatus.cpp
  OASkycopeThreat.cpp
  OASkycopeWifiStatus.cpp
  OASkycopeDefaultApi.cpp
  OASkycopeHelpers.cpp
  OASkycopeHttpRequest.cpp
  OASkycopeHttpFileElement.cpp
  OASkycopeOauth.cpp
  )

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Gui

)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

