#!/bin/bash

if [[ -z $1 ]]; then
    echo "Usage: $0 <script_name>"
    exit 1
fi

SCRIPT_NAME=$1

if [[ -d "test.app" ]]; then
    echo "test.app already exists. Removing..."
    rm -rf test.app

    if [[ $? -ne 0 ]]; then
        echo "no test.app directory."
        exit 1
    fi
fi

mkdir -p test.app/Contents/MacOS
clang -c libraries/fssimplewindowobjc.m -o libraries/fssimplewindowobjc.o
clang -c libraries/yssimplesound.m -o libraries/yssimplesound.o
clang++ -c libraries/yspng.cpp -o libraries/yspng.o

if [[ $? -ne 0 ]]; then
    echo "compilation of files failed."
    exit 1
fi

clang++ "$SCRIPT_NAME" libraries/yspng.o libraries/yssimplesound.o libraries/yssimplesound.cpp libraries/fssimplewindowobjc.o libraries/fssimplewindowcpp.cpp -framework Cocoa -framework OpenGL -framework AVFoundation -o test.app/Contents/MacOS/exe

if [[ $? -ne 0 ]]; then
    echo "final compilation failed."
    exit 1
fi

echo "compilation Successful."
