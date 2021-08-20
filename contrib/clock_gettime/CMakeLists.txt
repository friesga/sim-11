cmake_minimum_required(VERSION 3.0)
project(clock_gettime VERSION 1.0.0)

# Dirs
set (MAIN_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set (MAIN_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# Source dirs
file (GLOB MAIN_SRC
  "${MAIN_SRC_DIR}/*.c"
)
#Includes
file(GLOB INCLUDE_FILES
  "${MAIN_INC_DIR}/*.h"
)

include_directories(
  "${MAIN_INC_DIR}/"
)


# Add target
add_library(clock_gettime_shared SHARED ${MAIN_SRC})
set_target_properties(clock_gettime_shared PROPERTIES OUTPUT_NAME clock_gettime)
set_target_properties(clock_gettime_shared PROPERTIES VERSION ${PROJECT_VERSION})
add_library(clock_gettime_static STATIC ${MAIN_SRC})
set_target_properties(clock_gettime_static PROPERTIES OUTPUT_NAME clock_gettime)

# Install rules
install(TARGETS clock_gettime_shared CONFIGURATIONS Release LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include)
install(TARGETS clock_gettime_static CONFIGURATIONS Release ARCHIVE DESTINATION lib)
install(FILES ${INCLUDE_FILES} DESTINATION include)