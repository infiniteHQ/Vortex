#!/bin/sh

DISABLE_UI=$1

# Read the version from version.conf
VERSION=$(cat ../version.conf)

mkdir build_spdlog
mkdir build

cd build_spdlog
cmake ../../lib/spdlog
make -j$(nproc)

cd ../build

if [ "$DISABLE_UI" = "--disable_ui" ]; then
    sudo cmake ../.. -DVORTEX_DISABLE_UI=ON
else
    sudo cmake ../.. -DVORTEX_DISABLE_UI=OFF
fi

make -j$(nproc) install

cd ..

cp -r ./build/bin/* dist/${VERSION}/bin
cp -r ./build/bin/* dist/${VERSION}/bin
cp -r ./handle_crash.sh dist/${VERSION}/bin

# Prepare shipping folders
rm -rf shipping/*
mkdir -p shipping/vortex/linux

# Archive the files using the version from version.conf
TAR_FILE=./shipping/vortex/linux/vortex_${VERSION}.tar.gz
tar -cvzf "$TAR_FILE" dist/

# Generate checksum
cd shipping/vortex/linux
sha256sum "vortex_${VERSION}.tar.gz" > "vortex_${VERSION}.tar.gz.sha256"
cd ../../../