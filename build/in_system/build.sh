mkdir build_spdlog
mkdir build

cd build_spdlog && cmake ../../../lib/spdlog && make install
cd ../build && cmake ../../.. && make install

#TODO If vortex working, install all builtin modules in system.
#TODO Embed spdlog into main CMakeLists.txt 