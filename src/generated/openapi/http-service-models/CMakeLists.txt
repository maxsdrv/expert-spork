cmake_minimum_required(VERSION 3.5)

project(http-service-models)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network Gui)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME}
  OAServiceAlarm_status.h
  OAServiceBand.h
  OAServiceCamera_id.h
  OAServiceCamera_track.h
  OAServiceClass_name.h
  OAServiceDevice_id_mapping.h
  OAServiceDevice_settings.h
  OAServiceError.h
  OAServiceGeo_coordinate.h
  OAServiceGeo_direction.h
  OAServiceGeo_position.h
  OAServiceGeo_position_mode.h
  OAServiceGet_api_version_200_response.h
  OAServiceGet_jammer_200_response.h
  OAServiceGet_jammer_groups_200_response.h
  OAServiceGet_jammer_mode_200_response.h
  OAServiceGet_position_mode_200_response.h
  OAServiceGet_sensor_info_200_response.h
  OAServiceHw_info.h
  OAServiceJammer_group.h
  OAServiceJammer_info.h
  OAServiceJammer_list.h
  OAServiceJammer_mode.h
  OAServiceJammer_timeout_status.h
  OAServiceLicense_status.h
  OAServiceSensor_info.h
  OAServiceSensor_list.h
  OAServiceSensor_type.h
  OAServiceSet_disabled_request.h
  OAServiceSet_jammer_bands_request.h
  OAServiceSet_jammer_mode_request.h
  OAServiceSet_position_mode_request.h
  OAServiceSet_position_request.h
  OAServiceSettings.h
  OAServiceState.h
  OAServiceTarget_attributes_RFD.h
  OAServiceTarget_data.h
  OAServiceTarget_data_attributes.h
  OAServiceTarget_data_position.h
  OAServiceTarget_data_position_direction.h
  OAServiceTarget_list.h
  OAServiceTrack_data.h
  OAServiceTrack_list.h
  OAServiceTrack_point.h
  OAServiceUpload_license_request.h
  OAServiceWorkzone_sector.h
  OAServiceWs_reply.h
  OAServiceHelpers.h
  OAServiceObject.h
  OAServiceEnum.h
  OAServiceHttpFileElement.h
  OAServiceAlarm_status.cpp
  OAServiceBand.cpp
  OAServiceCamera_id.cpp
  OAServiceCamera_track.cpp
  OAServiceClass_name.cpp
  OAServiceDevice_id_mapping.cpp
  OAServiceDevice_settings.cpp
  OAServiceError.cpp
  OAServiceGeo_coordinate.cpp
  OAServiceGeo_direction.cpp
  OAServiceGeo_position.cpp
  OAServiceGeo_position_mode.cpp
  OAServiceGet_api_version_200_response.cpp
  OAServiceGet_jammer_200_response.cpp
  OAServiceGet_jammer_groups_200_response.cpp
  OAServiceGet_jammer_mode_200_response.cpp
  OAServiceGet_position_mode_200_response.cpp
  OAServiceGet_sensor_info_200_response.cpp
  OAServiceHw_info.cpp
  OAServiceJammer_group.cpp
  OAServiceJammer_info.cpp
  OAServiceJammer_list.cpp
  OAServiceJammer_mode.cpp
  OAServiceJammer_timeout_status.cpp
  OAServiceLicense_status.cpp
  OAServiceSensor_info.cpp
  OAServiceSensor_list.cpp
  OAServiceSensor_type.cpp
  OAServiceSet_disabled_request.cpp
  OAServiceSet_jammer_bands_request.cpp
  OAServiceSet_jammer_mode_request.cpp
  OAServiceSet_position_mode_request.cpp
  OAServiceSet_position_request.cpp
  OAServiceSettings.cpp
  OAServiceState.cpp
  OAServiceTarget_attributes_RFD.cpp
  OAServiceTarget_data.cpp
  OAServiceTarget_data_attributes.cpp
  OAServiceTarget_data_position.cpp
  OAServiceTarget_data_position_direction.cpp
  OAServiceTarget_list.cpp
  OAServiceTrack_data.cpp
  OAServiceTrack_list.cpp
  OAServiceTrack_point.cpp
  OAServiceUpload_license_request.cpp
  OAServiceWorkzone_sector.cpp
  OAServiceWs_reply.cpp
  OAServiceHelpers.cpp
  OAServiceHttpFileElement.cpp
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

