#include "dynamic_array.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <iterator>
#include <stdexcept>

TEST_CASE("dyn_arr constructors", "[dyn_arr]"){

    int arr[] = {1,2,3};

    SECTION("default constructor"){
        dynamic_array<int> darr_def;
        CHECK(darr_def.get_size() == 0);
    }

    SECTION("size constructor"){
        dynamic_array<int> darr_size(5);
        CHECK(darr_size.get_size() == 5);
    }
    
    SECTION("full constructor"){
        dynamic_array<int> darr_full(arr, 3);

        REQUIRE(darr_full.get_size() == 3);

        int arr_full[] = {
            darr_full.get(0),
            darr_full.get(1),
            darr_full.get(2)
        };

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_full)));
    }
    
    SECTION("copy constructor"){
        dynamic_array<int> darr_full(arr, 3);
        dynamic_array<int> darr_copy(darr_full);

        REQUIRE(darr_copy.get_size() == 3);
        REQUIRE(darr_full.get_size() == 3);

        int arr_copy[] = {
            darr_copy.get(0),
            darr_copy.get(1),
            darr_copy.get(2)
        };

        darr_full.set(0, 342);
        darr_full.set(2, 4);
        int arr_copied_from[] = {
            darr_full.get(0),
            darr_full.get(1),
            darr_full.get(2)
        };
        int new_arr[] = {342, 2, 4};

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_copy)));
        CHECK(std::equal(std::begin(new_arr), std::end(new_arr), std::begin(arr_copied_from)));
    }
}

TEST_CASE("dyn_arr opertor=", "[dyn_arr]"){

    float arr[] = {1.123,2.4214,3.412};
    dynamic_array<float> darr(arr, 3);
        
    SECTION("copy assignment"){
        dynamic_array<float> darr_copy;
        darr_copy = darr;


        REQUIRE(darr_copy.get_size() == 3);
        REQUIRE(darr.get_size() == 3);

        float arr_copy[] = {
            darr_copy.get(0),
            darr_copy.get(1),
            darr_copy.get(2)
        };

        darr.set(0, -2);
        float arr_copied_from[] = {
            darr.get(0),
            darr.get(1),
            darr.get(2)
        };
        float new_arr[] = {-2, arr[1], arr[2]};

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_copy)));
        CHECK(std::equal(std::begin(new_arr), std::end(new_arr), std::begin(arr_copied_from)));
    }

    SECTION("self-assignment"){
        darr = darr;

        REQUIRE(darr.get_size() == 3);        

        float arr_self[] = {
            darr.get(0),
            darr.get(1),
            darr.get(2)
        };

        CHECK(std::equal(std::begin(arr), std::end(arr), std::begin(arr_self)));
    }
}

TEST_CASE("dyn_arr set", "[dyn_arr]"){
    int arr[] = {1,3,3,7};
    dynamic_array<int> darr(arr, 4);

    int new_val = 234;

    darr.set(1, new_val);

    CHECK(darr.get(1) == new_val);
}

TEST_CASE("dyn_arr get/set exceptions", "[dyn_arr][exceptions]"){
    int arr[] = {1,3,3,7};
    dynamic_array<int> darr(arr, 4);

    SECTION("get out_of_range"){
        CHECK_THROWS_AS(darr.get(4), std::out_of_range);
        CHECK_THROWS_AS(darr.get(-1), std::out_of_range);
    }
    SECTION("set out_of_range"){
        CHECK_THROWS_AS(darr.set(4, 123), std::out_of_range);
        CHECK_THROWS_AS(darr.set(-1, 123), std::out_of_range);
    }
}

TEST_CASE("dyn_arr resize", "[dyn_arr]"){
    int arr[] = {1,23,41,54};
    dynamic_array<int> darr = dynamic_array(arr, 4);

    SECTION("reduce"){
        size_t new_size = 2;
        darr.resize(new_size);

        CHECK(darr.get_size() == new_size);

        CHECK(darr.get(0) == arr[0]);        
        CHECK(darr.get(1) == arr[1]);        
    }
    

    SECTION("expand"){
        size_t new_size = 6;
        darr.resize(new_size);

        CHECK(darr.get_size() == new_size);

        CHECK(darr.get(0) == arr[0]);        
        CHECK(darr.get(1) == arr[1]);        
        CHECK(darr.get(2) == arr[2]);        
        CHECK(darr.get(3) == arr[3]);        
    }

    SECTION("same size"){
        size_t new_size = 4;
        darr.resize(new_size);

        CHECK(darr.get_size() == new_size);

        CHECK(darr.get(0) == arr[0]);        
        CHECK(darr.get(1) == arr[1]);        
        CHECK(darr.get(2) == arr[2]);        
        CHECK(darr.get(3) == arr[3]);  
    }
}