#!/bin/sh

cd build
make
rm ../lattia-vm
mv ./lattia-vm ..
