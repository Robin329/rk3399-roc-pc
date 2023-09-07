#!/bin/bash

make -j32 O=build
sudo cp build/arch/arm64/boot/Image /home/rock/tftpboot/
ls -alh /home/rock/tftpboot/*
python ./scripts/clang-tools/gen_compile_commands.py -d build
