# Список рецептов: `just` или `just --list`
default:
    @just --list

build:
    cmake -B build -S .
    cmake --build build

test: build
    ctest --test-dir build --output-on-failure

# Покрытие: отдельная сборка с --coverage, затем lcov + genhtml (нужны lcov, genhtml).
coverage:
    #!/usr/bin/env bash
    set -euo pipefail
    cmake -B build_cov -S . -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_CXX_FLAGS="-O0 -g --coverage" \
        -DCMAKE_EXE_LINKER_FLAGS="--coverage"
    cmake --build build_cov
    ctest --test-dir build_cov --output-on-failure
    command -v lcov >/dev/null 2>&1 || {
        echo 'TODO: установите lcov (Arch: pacman -S lcov)' >&2
        exit 1
    }
    command -v genhtml >/dev/null 2>&1 || {
        echo 'TODO: установите genhtml (пакет lcov)' >&2
        exit 1
    }
    lcov --capture --directory build_cov --output-file coverage.info
    lcov --remove coverage.info '/usr/*' '*/_deps/*' -o coverage.filtered.info \
        || cp coverage.info coverage.filtered.info
    genhtml coverage.filtered.info --output-directory coverage_html --title "lab2 tests"
    echo "NOTE(coverage): отчёт — file://$(pwd)/coverage_html/index.html"

# Удалить артефакты покрытия и каталог build_cov
clean-coverage:
    rm -rf build_cov coverage.info coverage.filtered.info coverage_html

# Статический анализ (нужен clang-tidy; compile_commands.json из `build/`)
lint: build
    #!/usr/bin/env bash
    set -euo pipefail
    command -v clang-tidy >/dev/null 2>&1 || {
        echo 'TODO: установите clang-tidy (Arch: pacman -S clang)' >&2
        exit 1
    }
    test -f build/compile_commands.json || {
        echo 'FIXME: нет build/compile_commands.json — сначала just build' >&2
        exit 1
    }
    clang-tidy tests/dyn_arr_test.cpp tests/linked_list_test.cpp -p build --quiet
