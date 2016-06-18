#!/bin/sh

if [ ! -d "googletest" ]; then
    echo "Downloading gtest source package..."
    wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz
    echo "Extracting gtest source package..."
    tar xf release-1.7.0.tar.gz
    mv googletest-release-1.7.0 googletest
    rm -rf release-1.7.0.tar.gz
    echo "Removed gtest source packager."
fi
