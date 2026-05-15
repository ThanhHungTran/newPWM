# This file was generated using the 4DIAC FORTE Export Filter 3.1.0.202603111726!

cmake_minimum_required(VERSION 3.30)
project(forte-custom
        DESCRIPTION "1.0.0"
        VERSION "1.0.0"
        LANGUAGES C CXX
)

if (NOT TARGET forte)
        find_package(forte 3.0 REQUIRED)
endif ()

add_library(forte-custom)
target_link_libraries(forte-custom PUBLIC forte-core)
get_target_property(forte_IMPORTED forte IMPORTED)
if (NOT forte_IMPORTED)
        target_link_libraries(forte PUBLIC $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,forte-custom,$<LINK_LIBRARY:WHOLE_ARCHIVE,forte-custom>>)
endif ()

add_subdirectory(include)
add_subdirectory(src)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(ConfigPackageLocation lib/cmake/forte-custom-${PROJECT_VERSION})

configure_package_config_file(
        forte-custom-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/forte-custom-config.cmake
        INSTALL_DESTINATION ${ConfigPackageLocation}
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/forte-custom-config-version.cmake
        COMPATIBILITY AnyNewerVersion
)

install(TARGETS forte-custom EXPORT forte-custom-export FILE_SET HEADERS)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/forte-custom-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/forte-custom-config-version.cmake
        DESTINATION ${ConfigPackageLocation}
)

install(EXPORT forte-custom-export
        DESTINATION ${ConfigPackageLocation}
        EXPORT_LINK_INTERFACE_LIBRARIES
)