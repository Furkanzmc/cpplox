conf:
    cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DLOX_BUILD_TESTS=ON

conf-release:
    cmake -B ./build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo -DLOX_BUILD_TESTS=OFF

build:
    cmake --build ./build

run:
    ./build/lox_cli

clean:
    rm -rf ./build/*

rebuild: clean conf build
