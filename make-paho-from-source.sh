#!/bin/bash

cd $(mktemp -d)
set -e

# Get prerequisites
sudo apt-get install build-essential gcc make cmake cmake-gui cmake-curses-gui libssl-dev

# Build PAHO C
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout 'v1.3.8'

cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON \
	-DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig

cd ..

# Build PAHO C++
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON \
	-DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=FALSE
sudo cmake --build build/ --target install
sudo ldconfig

cd ..
