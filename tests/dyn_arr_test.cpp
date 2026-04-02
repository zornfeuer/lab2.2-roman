#include "dynamic_array.hpp"
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <iterator>
#include <stdexcept>

// NOTE(coverage-map): детальная матрица — комментарии в шапке и у реализаций в
//   dynamic_array.hpp.

TEST_CASE("dyn_arr constructors", "[dyn_arr]") {
  // TODO: std::size(arr) вместо литерала 3 в конструкторах (T*, size).
  int arr[] = {1, 2, 3};

  SECTION("default constructor") {
    dynamic_array<int> darr_def;
    CHECK(darr_def.get_size() == 0);
    // TODO(coverage): пустой массив — begin() == end(); const get/[].
  }

  SECTION("size constructor") {
    dynamic_array<int> darr_size(5);
    CHECK(darr_size.get_size() == 5);
    // TODO(coverage): значения элементов после dynamic_array(size_t) (int: 0?
    //   для других T — дефолт-конструктор?).
  }

  SECTION("full constructor") {
    dynamic_array<int> darr_full(arr, 3);

    REQUIRE(darr_full.get_size() == 3);

    // TODO(refactor): вынести копирование в сырой массив / сравнение в хелпер.
    size_t i = 0;
    int arr_full[3];
    for (auto el : darr_full) {
      arr_full[i++] = el;
    }

    CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_full)));
  }

  SECTION("copy constructor") {
    dynamic_array<int> darr_full(arr, 3);
    dynamic_array<int> darr_copy(darr_full);

    REQUIRE(darr_copy.get_size() == 3);
    REQUIRE(darr_full.get_size() == 3);

    // TODO(refactor): вынести копирование в сырой массив / сравнение в хелпер.
    size_t i = 0;
    int arr_copy[3];
    for (auto el : darr_copy) {
      arr_copy[i++] = el;
    }

    darr_full.set(0, 342);
    darr_full.set(2, 4);
    // TODO(refactor): вынести копирование в сырой массив / сравнение в хелпер.
    i = 0;
    int arr_copied_from[3];
    for (auto el : darr_full) {
      arr_copied_from[i++] = el;
    }
    int new_arr[] = {342, 2, 4};

    CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_copy)));
    CHECK(std::equal(std::begin(new_arr), std::end(new_arr),
                     std::begin(arr_copied_from)));
  }

  SECTION("initializer list") {
    dynamic_array<int> darr = {1, 2, 3, 4};
    int check_arr[] = {1, 2, 3, 4};

    // FIXME: закомментированный CHECK с {1,2,3,4} — удалить или заменить
    //   (временный список в std::equal не скомпилируется как задумано).
    // CHECK(std::equal(std::begin({1,2,3,4}), std::end({1,2,3,4}),
    // darr.begin()));
    CHECK(std::equal(std::begin(check_arr), std::end(check_arr), darr.begin()));
  }
}

// NOTE(coverage): копирующее присваивание и self-assignment; ветка this==&other
// в impl.
// FIXME(typo): opertor= → operator=
TEST_CASE("dyn_arr opertor=", "[dyn_arr]") {

  float arr[] = {1.123, 2.4214, 3.412};
  dynamic_array<float> darr(arr, 3);

  SECTION("copy assignment") {
    dynamic_array<float> darr_copy;
    darr_copy = darr;

    REQUIRE(darr_copy.get_size() == 3);
    REQUIRE(darr.get_size() == 3);

    // TODO(refactor): вынести копирование в сырой массив / сравнение в хелпер.
    size_t i = 0;
    float arr_copy[3];
    for (auto el : darr_copy) {
      arr_copy[i++] = el;
    }

    darr.set(0, -2);
    // TODO(refactor): вынести копирование в сырой массив / сравнение в хелпер.
    i = 0;
    float arr_copied_from[3];
    for (auto el : darr) {
      arr_copied_from[i++] = el;
    }

    float new_arr[] = {-2, arr[1], arr[2]};

    CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_copy)));
    CHECK(std::equal(std::begin(new_arr), std::end(new_arr),
                     std::begin(arr_copied_from)));
    // TODO(coverage): operator= когда other другого размера (больше / меньше).
  }

  SECTION("self-assignment") {
    darr = darr;

    REQUIRE(darr.get_size() == 3);

    float arr_self[] = {darr.get(0), darr.get(1), darr.get(2)};

    CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_self)));
  }
}

// NOTE(coverage): только set(1,…) + get(1); остальные индексы и границы — в
// других кейсах.
TEST_CASE("dyn_arr set", "[dyn_arr]") {
  int arr[] = {1, 3, 3, 7};
  dynamic_array<int> darr(arr, 4);

  int new_val = 234;

  darr.set(1, new_val);

  CHECK(darr.get(1) == new_val);
}

// NOTE(coverage): get/set/[] с const-ссылкой; CHECK_THROWS_AS(out_of_range).
TEST_CASE("dyn_arr get/set/[] exceptions", "[dyn_arr][exceptions]") {
  int arr[] = {1, 3, 3, 7};
  dynamic_array<int> darr(arr, 4);
  const dynamic_array<int> &darrref = darr;
  int a;

  // NOTE: CHECK_THROWS_AS — хороший стиль; в linked_list_test выровнять так же.
  // TODO: задокументировать семантику индекса -1 (size_t).
  SECTION("get out_of_range") {
    CHECK_THROWS_AS(darr.get(4), std::out_of_range);
    CHECK_THROWS_AS(darr.get(-1), std::out_of_range);
  }
  SECTION("set out_of_range") {
    CHECK_THROWS_AS(darr.set(4, 123), std::out_of_range);
    CHECK_THROWS_AS(darr.set(-1, 123), std::out_of_range);
  }
  SECTION("operator[] out_of_range") {
    CHECK_THROWS_AS(darr[4] = 123, std::out_of_range);
    CHECK_THROWS_AS(darr[-1] = 123, std::out_of_range);
    CHECK_THROWS_AS(a = darr[4], std::out_of_range);
    CHECK_THROWS_AS(a = darr[-1], std::out_of_range);
    CHECK_THROWS_AS(a = darrref[4], std::out_of_range);
    CHECK_THROWS_AS(a = darrref[-1], std::out_of_range);
  }
}

// NOTE(coverage): resize меньше/больше/равно (ранний return в impl); см. NOTE в
// .hpp.
TEST_CASE("dyn_arr resize", "[dyn_arr]") {
  int arr[] = {1, 23, 41, 54};
  // TODO: std::size(arr) вместо литерала 4.
  dynamic_array<int> darr = dynamic_array(arr, 4);

  // TODO(coverage): resize(0); resize(N) от пустого default-constructed
  // массива.
  SECTION("reduce") {
    size_t new_size = 2;
    darr.resize(new_size);

    CHECK(darr.get_size() == new_size);

    CHECK(darr.get(0) == arr[0]);
    CHECK(darr.get(1) == arr[1]);
  }

  SECTION("expand") {
    size_t new_size = 6;
    darr.resize(new_size);

    CHECK(darr.get_size() == new_size);

    CHECK(darr.get(0) == arr[0]);
    CHECK(darr.get(1) == arr[1]);
    CHECK(darr.get(2) == arr[2]);
    CHECK(darr.get(3) == arr[3]);
    // NOTE: новые слоты [4],[5] после new T[6] для int могут быть
    // неинициализированы;
    //   читать get(4)/get(5) без set — спорно; зафиксировать ожидание в тесте
    //   или изменить реализацию (value-initialize).
  }

  SECTION("same size") {
    size_t new_size = 4;
    darr.resize(new_size);

    CHECK(darr.get_size() == new_size);

    CHECK(darr.get(0) == arr[0]);
    CHECK(darr.get(1) == arr[1]);
    CHECK(darr.get(2) == arr[2]);
    CHECK(darr.get(3) == arr[3]);
  }
}

// NOTE(coverage): const [] и неконст []/присваивание; итератор только косвенно
// (TODO в секции).
// FIXME(typo): dyna_arr → dyn_arr
TEST_CASE("dyna_arr operator[]", "[dyn_arr]") {
  char arr[] = {1, 2, 3, 4};
  dynamic_array<char> darr(arr, 4);
  const auto &crdarr = darr;

  SECTION("read-only") {
    CHECK(crdarr[0] == 1);
    CHECK(crdarr[1] == 2);
    CHECK(crdarr[3] == 4);
  }
  SECTION("rw") {
    CHECK(darr[0] == 1);
    CHECK(darr[1] == 2);
    CHECK(darr[3] == 4);

    darr[0] = 'p';
    darr[1] = 'q';
    darr[3] = 'r';

    CHECK(darr[0] == 'p');
    CHECK(darr[1] == 'q');
    CHECK(darr[3] == 'r');
    // TODO(coverage): итератор ++ / != end согласованы с operator[].
  }
}

// NOTE(coverage — инструменты): gcov/lcov по
// CMakeFiles/tests.dir/.../dyn_arr_test.cpp.o
//   после сборки с --coverage (см. конец linked_list_test.cpp).
// NOTE(coverage — аудит): operator= с другим размером other не выделен;
// dynamic_array(0)
//   не тестируется; прямой вызов iterator::operator++ только через range-for.
// TODO(coverage): типы кроме int/float/char; если get_size() станет const —
// тест
//   на const dynamic_array и get_size().
