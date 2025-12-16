# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(PROJECT_VER "22.22")
set(PROJECT_VER_NUMBER 22022)

set(EXTRA_COMPONENT_DIRS
    "$ENV{ESP_MATTER_PATH}/examples/common/app_reset"
    ${extra_components_dirs_append})


project(SLDRDemo)

# ------------------------------------------------------------
# Generate version_info.h from dependencies.lock
# ------------------------------------------------------------

find_package(Python3 REQUIRED)

set(DEPENDENCIES_LOCK_FILE ${CMAKE_SOURCE_DIR}/dependencies.lock)
set(GEN_SCRIPT ${CMAKE_SOURCE_DIR}/tools/gen_dep_versions_header.py)
set(GEN_HEADER ${CMAKE_BINARY_DIR}/gen_dependencies_version_info.h)

add_custom_command(
    OUTPUT ${GEN_HEADER}
    COMMAND ${Python3_EXECUTABLE} ${GEN_SCRIPT}
            ${DEPENDENCIES_LOCK_FILE}
            ${GEN_HEADER}
    DEPENDS ${DEPENDENCIES_LOCK_FILE} ${GEN_SCRIPT}
    COMMENT "Generating gen_dependencies_version_info.h from dependencies.lock"
    VERBATIM
)

add_custom_target(generate_version_header ALL
    DEPENDS ${GEN_HEADER}
)

# Make header visible to all components
idf_build_set_property(
    INCLUDE_DIRECTORIES
    ${CMAKE_BINARY_DIR}
    APPEND
)

# ------------------------------------------------------------
# Generate version_info.h from dependencies.lock
# ------------------------------------------------------------

idf_build_set_property(CXX_COMPILE_OPTIONS "-std=gnu++17;-Os;-DCHIP_HAVE_CONFIG_H;-Wno-overloaded-virtual" APPEND)
idf_build_set_property(C_COMPILE_OPTIONS "-Os" APPEND)
# For RISCV chips, project_include.cmake sets -Wno-format, but does not clear various
# flags that depend on -Wformat
idf_build_set_property(COMPILE_OPTIONS "-Wno-format-nonliteral;-Wno-format-security;-Wformat=0" APPEND)
