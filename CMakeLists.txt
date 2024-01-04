# A null exemple for API checking.
cmake_minimum_required(VERSION 3.16)
project(vortex)

add_executable(vortex
    ${CMAKE_CURRENT_SOURCE_DIR}/vortex.cpp
	main.cpp
)


# # Add devtools
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tools/devtools devtools_build)
target_link_libraries(vortex PUBLIC devtools)

# Install executable to /bin
install(TARGETS vortex DESTINATION /bin)