mkdir build_spdlog
mkdir build

cd build_spdlog && cmake ../../lib/spdlog && make -j$(nproc)
cd ../build && cmake ../.. && make -j$(nproc) install