mkdir build_spdlog
mkdir build

cd build_spdlog && cmake ../../../lib/spdlog && make install
cd ../build && cmake ../../.. && make install

#TODO Embed spdlog into main CMakeLists.txt 