#include "catch2/catch_test_macros.hpp"
#include "linked_list.hpp"
#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <vector>

// NOTE(coverage-map): сводка в шапке linked_list.hpp. Пробелы: ctor от
//   initializer_list при **создании** списка {…}, member operator<< в поток,
//   явный clear (см. TODO в секциях). Сравнение с ожидаемым — .operator==({…}).

template <class T> static T *to_array(linked_list<T> &list) {
  // TODO(refactor): свернуть to_array + std::equal + delete[] в хелпер; где
  //   уместно — сверять через list.operator==({…}) без сырого массива.
  // FIXME: при падении assertion между new[] и delete[] возможна утечка;
  //   надёжнее std::vector / std::unique_ptr[].
  size_t size = list.get_size();
  auto arr = new T[size];
  auto it = list.begin();

  for (size_t i = 0; it != list.end(); ++i, ++it) {
    arr[i] = *it;
  }
  return arr;
}

// NOTE(coverage): ctors default / (T*,n) / n==0 / copy; секция «initializer
// list»
//   всё ещё через (ptr,size), не linked_list{…} — см. TODO в секции.
TEST_CASE("linked_list constructor", "[linked_list]") {
  // TODO: при изменении длины массива использовать std::size(arr) вместо
  //   литералов в конструкторах (T*, size), чтобы не разойтись с фактическим
  //   числом элементов.
  int arr[] = {1233, 124, 412, 123, 34531};

  SECTION("default constructor") {
    linked_list<int> list;

    CHECK(list.get_size() == 0);
    // TODO(coverage): пустой список — begin() == end(), range-for не заходит;
    //   список из одного элемента — проход итератором; при необходимости —
    //   инвалидация итератора при модификации во время обхода (shared_ptr).
  }

  SECTION("full constructor") {
    linked_list<int> list(arr, 5);
    size_t size = list.get_size();

    REQUIRE(size == 5);

    int *check_arr = to_array(list);
    CHECK(std::equal(check_arr, check_arr + size, arr));
    delete[] check_arr;
  }
  SECTION("full constructor (empty case)") {
    linked_list<int> list(arr, 0);
    size_t size = list.get_size();

    REQUIRE(size == 0);

    int *check_arr = to_array(list);
    CHECK(std::equal(check_arr, check_arr + size, arr));
    delete[] check_arr;
  }

  // TODO(test-design): смешаны проверка копии и insert_at на исходнике —
  //   лучше отдельные TEST_CASE (копия независима; вставка не ломает копию).
  SECTION("copy constructor") {
    linked_list<int> list(arr, 5);
    linked_list<int> copy_list(list);
    size_t size = copy_list.get_size();

    list.insert_at(2, 5);
    int new_arr[] = {arr[0], arr[1], 5, arr[2], arr[3], arr[4]};

    REQUIRE(size == 5);
    REQUIRE(list.get_size() == 6); // Тестирование втавки

    int *check_arr = to_array(copy_list);
    int *arr_copied_from = to_array(list); // Тестирование вставки
    CHECK(std::equal(check_arr, check_arr + size, arr));
    CHECK(std::equal(new_arr, new_arr + size + 1,
                     arr_copied_from)); // Тестирование вставки
    delete[] check_arr;
    delete[] arr_copied_from;
  }

  SECTION("copy constructor empty") {
    linked_list<int> list;
    linked_list<int> empty_copy(list);

    CHECK(empty_copy.get_size() == 0);
  }

  // TODO(coverage): разрушить исходный список (scope/явно), убедиться что копия
  //   сохранила данные.

  // FIXME(typo): секция intializer → initializer
  SECTION("intializer list") {
    int arr[] = {1, 2, 3, 4, 5};
    linked_list<int> list(arr, 5);

    REQUIRE(list.get_size() == 5);

    // NOTE(coverage): member operator==({…}) вызывается; ctor списка всё ещё
    //   (ptr, size). TODO: добавить linked_list<int> list{1,…,5} для ctor
    //   ilist.
    CHECK(list.operator==({1, 2, 3, 4, 5}));
  }
}

// NOTE(coverage): operator= (копия + self-assignment); ветка clear() внутри =.
// FIXME(typo): имя теста opertor= → operator= (и фильтры Catch, если заданы).
TEST_CASE("linked_list opertor=", "[linked_list]") {

  float arr[] = {1.123, 2.4214, 3.412};
  linked_list<float> list(arr, 3);

  SECTION("copy assignment") {
    linked_list<float> list_copy;
    list_copy = list;

    list.prepend(3.14);
    list.insert_at(3, 0.69);

    REQUIRE(list_copy.get_size() == 3);
    REQUIRE(list.get_size() == 5); // Тестирование вставки

    float *arr_copy = to_array(list_copy);
    float *arr_copied_from = to_array(list); // Тестирование вставки
    float new_arr[] = {3.14, arr[0], arr[1], 0.69,
                       arr[2]}; // Тестирование вставки

    CHECK(std::equal(std::begin(arr), std::end(arr), arr_copy));
    CHECK(std::equal(std::begin(new_arr), std::end(new_arr),
                     arr_copied_from)); // Тестирование вставки
    delete[] arr_copy;
    delete[] arr_copied_from; // Тестирование вставки
  }

  SECTION("self-assignment") {
    list = list;

    REQUIRE(list.get_size() == 3);

    float *arr_self = to_array(list);

    CHECK(std::equal(std::begin(arr), std::end(arr), arr_self));
    delete[] arr_self;
  }
}

// NOTE(coverage): insert_at — ветки index 0 / size / середина; get_node через
// insert.
// FIXME(typo): isnert → insert
TEST_CASE("linked_list isnert", "[linked_list]") {
  int arr[] = {1, 2, 3, 4};
  linked_list<int> list(arr, 4);

  SECTION("first (prepend)") {
    list.insert_at(0, 0);

    int new_arr[] = {0, 1, 2, 3, 4};
    int *list_arr = to_array(list);

    CHECK(std::equal(std::begin(new_arr), std::end(new_arr), list_arr));

    delete[] list_arr;
  }
  SECTION("last (append)") {
    list.insert_at(4, 5);

    int new_arr[] = {1, 2, 3, 4, 5};
    int *list_arr = to_array(list);

    CHECK(std::equal(std::begin(new_arr), std::end(new_arr), list_arr));

    delete[] list_arr;
  }
  SECTION("insert middle") {
    list.insert_at(2, 23);

    int new_arr[] = {1, 2, 23, 3, 4};
    int *list_arr = to_array(list);

    CHECK(std::equal(std::begin(new_arr), std::end(new_arr), list_arr));

    delete[] list_arr;
  }
}

// NOTE(coverage): operator+ (пустой операнд, цепочка), operator+=; не покрыто:
// оба пустые.
TEST_CASE("linked_list concat operators", "[linked_list]") {
  int arr1[] = {1, 2, 3};
  int arr2[] = {4, 5};
  int arr_expect[] = {1, 2, 3, 4, 5};

  linked_list<int> list1(arr1, 3);
  // NOTE: размер должен совпадать с числом элементов в arr2 (раньше было 3 при
  //   двух элементах — UB и маскировала слабая проверка цепочки +).
  linked_list<int> list2(arr2, 2);

  SECTION("operator+") {
    SECTION("with empty") {
      linked_list<int> empty;
      linked_list<int> list_concat1 = empty + list1;
      linked_list<int> list_concat2 = list1 + empty;

      REQUIRE(list_concat1.get_size() == list1.get_size());
      REQUIRE(list_concat2.get_size() == list1.get_size());

      int *arr_1 = to_array(list_concat1);
      int *arr_2 = to_array(list_concat2);

      CHECK(list_concat1.operator==({1, 2, 3}));
      CHECK(std::equal(arr1, arr1 + 3, arr_1));
      CHECK(std::equal(arr1, arr1 + 3, arr_2));

      delete[] arr_1;
      delete[] arr_2;
    }
    linked_list<int> list_concat = list1 + list2;

    REQUIRE(list_concat.get_size() == list1.get_size() + list2.get_size());

    int *arr_concat = to_array(list_concat);

    CHECK(std::equal(arr_expect, arr_expect + 5, arr_concat));
    delete[] arr_concat;
  }

  SECTION("chaining operator+") {
    int arr3[] = {6, 7, 8};
    int arr_expect[] = {1, 2, 3, 4, 5, 6, 7, 8};

    linked_list<int> list3(arr3, 3);
    linked_list<int> list_concat = list1 + list2 + list3;

    REQUIRE(list_concat.get_size() ==
            list1.get_size() + list2.get_size() + list3.get_size());

    int *arr_concat = to_array(list_concat);

    CHECK(std::equal(arr_expect, arr_expect + 8, arr_concat));
    delete[] arr_concat;
  }

  // TODO(coverage): operator+ с двумя пустыми списками; operator+= когда пустой
  //   левый или правый операнд.
  SECTION("operator+=") {
    size_t old_size = list1.get_size();
    list1 += list2;

    REQUIRE(list1.get_size() == old_size + list2.get_size());

    int *arr_concat = to_array(list1);

    CHECK(std::equal(arr_expect, arr_expect + 5, arr_concat));
    delete[] arr_concat;
  }
}

// NOTE(coverage): исключения get / insert_at / get_sub_list; CHECK_THROWS без
// _AS.
TEST_CASE("linked_list get/get_sublist/insert_at exceptions", "[linked_list]") {
  int arr[] = {1, 2, 3};
  linked_list<int> list(arr, 3);

  // TODO: выровнять стиль — CHECK_THROWS_AS(..., std::out_of_range) как в
  //   dyn_arr_test; явно задокументировать семантику отрицательных литералов
  //   (преобразование в size_t).
  SECTION("get out_of_range") {
    CHECK_THROWS(list.get(3));
    CHECK_THROWS(list.get(-1));
  }

  SECTION("insert_at out_of_range") {
    CHECK_THROWS(list.insert_at(4, 3));
    CHECK_THROWS(list.insert_at(-1, 3));
    CHECK_NOTHROW(list.insert_at(3, 3));
  }

  SECTION("get_sub_list") {
    CHECK_THROWS(list.get_sub_list(3, 0));
    CHECK_THROWS(list.get_sub_list(-1, 0));
    CHECK_THROWS(list.get_sub_list(0, 4));
    CHECK_THROWS(list.get_sub_list(0, -1));
    CHECK_NOTHROW(list.get_sub_list(0, 3));
    // TODO(coverage): start_index > end_index, если в реализации отдельная
    // ветка.
  }
}

// NOTE(coverage): append/prepend/insert_at на пустом и непустом; get_first/last
// здесь
//   не на пустом после clear (см. TODO в секции).
TEST_CASE("linked_list adding an element", "[linked_list]") {
  int arr[] = {1, 2, 3};
  linked_list<int> list(arr, 3);

  // TODO(coverage): clear(); get_first/get_last на пустом списке (ожидаемое
  //   поведение — исключение / UB — зафиксировать тестом).
  SECTION("append") {
    SECTION("to empty") {
      linked_list<int> empty;
      empty.append(0);
      CHECK(empty.get(0) == 0);
      CHECK(empty.get_first() == 0);
      CHECK(empty.get_last() == 0);
    }
    SECTION("to existing") {
      list.append(4);
      int expect_arr[] = {1, 2, 3, 4};
      int *app_arr = to_array(list);

      REQUIRE(list.get_size() == 4);

      CHECK(list.get_first() == 1);
      CHECK(list.get_last() == 4);
      CHECK(std::equal(expect_arr, expect_arr + 4, app_arr));
      delete[] app_arr;
    }
  }

  SECTION("prepend") {
    SECTION("to empty") {
      linked_list<int> empty;
      empty.prepend(0);

      CHECK(empty.get(0) == 0);
      CHECK(empty.get_first() == 0);
      CHECK(empty.get_last() == 0);
    }
    SECTION("to existing") {
      list.prepend(0);
      int expect_arr[] = {0, 1, 2, 3};
      int *app_arr = to_array(list);

      REQUIRE(list.get_size() == 4);

      CHECK(std::equal(expect_arr, expect_arr + 4, app_arr));
      CHECK(list.get_first() == 0);
      CHECK(list.get_last() == 3);
      delete[] app_arr;
    }
  }

  SECTION("insert_at") {
    SECTION("to empty") {
      linked_list<int> empty;
      empty.insert_at(0, 0);

      CHECK(empty.get(0) == 0);
      CHECK(empty.get_first() == 0);
      CHECK(empty.get_last() == 0);
    }
    SECTION("to existing") {
      list.insert_at(2, 23);
      int expect_arr[] = {1, 2, 23, 3};
      int *app_arr = to_array(list);

      REQUIRE(list.get_size() == 4);

      CHECK(std::equal(expect_arr, expect_arr + 4, app_arr));
      CHECK(list.get_first() == 1);
      CHECK(list.get_last() == 3);
      delete[] app_arr;
    }
  }
}

// NOTE(coverage): get_sub_list — середина/края/весь/пустой диапазон; CHECK
// через
//   sub.operator==({…}).
TEST_CASE("linked_list get_sub_list", "[linked_list]") {
  int arr[] = {1, 2, 3, 4, 5, 6, 7};
  linked_list<int> list(arr, 7);

  SECTION("middle sublist") {
    linked_list<int> sub = list.get_sub_list(2, 5);

    CHECK(sub.operator==({3, 4, 5}));
  }
  SECTION("start sublist") {
    linked_list<int> sub = list.get_sub_list(0, 4);

    CHECK(sub.operator==({1, 2, 3, 4}));
  }
  SECTION("end sublist") {
    linked_list<int> sub = list.get_sub_list(4, 7);

    CHECK(sub.operator==({5, 6, 7}));
  }
  SECTION("full sublist") {
    linked_list<int> sub = list.get_sub_list(0, 7);

    CHECK(sub.operator==({1, 2, 3, 4, 5, 6, 7}));
  }
  SECTION("empty sublist") {
    linked_list<int> sub1 = list.get_sub_list(4, 4);
    linked_list<int> sub2 = list.get_sub_list(7, 7);
    linked_list<int> sub3 = list.get_sub_list(0, 0);

    CHECK(sub1.operator==({}));
    CHECK(sub2.operator==({}));
    CHECK(sub3.operator==({}));
  }
}

// NOTE(coverage — инструменты): CMake: -O0 -g --coverage и --coverage при
// линковке
//   target tests; ctest или ./build/tests; lcov --capture --directory build -o
//   coverage.info && genhtml coverage.info -o coverage_html → index.html;
//   Clang: llvm-profdata + llvm-cov. Без инструментов: сверка grep вызовов с
//   TEST_CASE.
// NOTE(coverage — аудит): member operator== покрыт; operator<< в поток и ctor
//   linked_list({…}) при создании — всё ещё без прогона (см. TODO в .hpp).
// TODO(coverage): шаблоны с T кроме int/float (например std::string).
