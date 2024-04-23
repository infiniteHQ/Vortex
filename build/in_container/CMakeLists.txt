# A null exemple for API checking.
cmake_minimum_required(VERSION 3.16)
project(vx)


add_executable(vx 
    ../../bootstrapp.cpp
)
install(TARGETS vx DESTINATION /bin)