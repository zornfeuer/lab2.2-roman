test:
    cmake -B build -S .
    cd build && make
    ./build/tests

build:
    cmake -B build -S .
    cd build && make
