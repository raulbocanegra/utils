#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <common/Optional.h>

using namespace rboc::utils::optional;

TEST_CASE("Optional storage constructors", "[storage_constructors]") 
{
    SECTION("OptionalStorage default contructor should work")
    {
        details::OptionalStorage<int> st_int;
        CHECK_FALSE(st_int._has_value);
    }

    SECTION("Nullopt OptionalStorage constructor should work")
    {
        details::OptionalStorage<int> st_int = nullopt;
        CHECK_FALSE(st_int._has_value);
    }
    
    SECTION("OptionalStorage value contructor should work")
    {
        int test = 1;
        details::OptionalStorage<int> st_int(test);
        CHECK(st_int._has_value);
        CHECK(st_int._value == test);

        // Implicit conversion
        details::OptionalStorage<int> st_imp_int = 1;
        CHECK(st_imp_int._has_value);
        CHECK(st_imp_int._value == 1);
    }

    SECTION("OptionalStorage convertible value contructor should work")
    {
        short test = 1;
        details::OptionalStorage<int> st_int(test);
        CHECK(st_int._has_value);
        CHECK(st_int._value == test);
    }

    SECTION("OptionalStorage copy constructor for trivially destructibles T should work")
    {
        details::OptionalStorage<int> st_int(1);
        details::OptionalStorage<int> st_int_copy(st_int);        
        CHECK(st_int_copy._has_value);
        CHECK(st_int_copy._value == st_int._value);
    }

    SECTION("OptionalStorage copy constructor for non trivially destructibles T should work")
    {
        details::OptionalStorage<std::string> st_str("test_string_for_non_trivially_destructibles");
        details::OptionalStorage<std::string> st_str_copy(st_str);        
        CHECK(st_str_copy._has_value);
        CHECK(st_str_copy._value == st_str._value);

        std::vector<int> test { 0, 1, 2, 3, 4 };
        details::OptionalStorage<std::vector<int>> st_vector{ test };
        CHECK(st_vector._has_value);
        CHECK(st_vector._value == test);
    }

    SECTION("OptionalStorage move constructor for non trivially destructibles T should work")
    {
        details::OptionalStorage<std::string> st_str("test_string_for_non_trivially_destructibles");
        details::OptionalStorage<std::string> st_str_moved(std::move(st_str));
        CHECK(st_str_moved._has_value);
        CHECK(st_str_moved._value == "test_string_for_non_trivially_destructibles");
        CHECK(st_str._value.empty());

        std::vector<int> test { 0, 1, 2, 3, 4 };
        details::OptionalStorage<std::vector<int>> st_vector{ std::move(test) };
        CHECK(st_vector._has_value);
        CHECK(st_vector._value == std::vector<int>{ 0, 1, 2, 3, 4 });
        CHECK(test.empty());
    }

    SECTION("OptionalStorage constructor from value of convertible type should work")
    {        
        // From lvalue
        details::OptionalStorage<short> st_short(1);
        details::OptionalStorage<int> st_int(st_short);
        CHECK(st_short._has_value);
        CHECK(st_int._has_value);
        CHECK(st_short._value == st_int._value);

        // From rvalue        
        details::OptionalStorage<int> from_rvalue_st_int(std::move(st_short));
        CHECK_FALSE(st_short._has_value);
        CHECK(from_rvalue_st_int._has_value);
        CHECK(st_short._value == from_rvalue_st_int._value);

        // From NullOpt value
        details::OptionalStorage<short> st_null;
        details::OptionalStorage<int> st_int_from_null(st_null);
        CHECK_FALSE(st_null._has_value);
        CHECK_FALSE(st_int_from_null._has_value);
    }
}

TEST_CASE("Optional storage assignment", "[storage_assignment]") 
{
    SECTION("Nullopt OptionalStorage assignment should work")
    {
        details::OptionalStorage<int> st_int{1};
        CHECK(st_int._has_value);
        CHECK(st_int._value == 1);
        st_int = nullopt;
        CHECK_FALSE(st_int._has_value);
    }

    SECTION("OptionalStorage copy assignment should work")
    {
        int test = 1;
        details::OptionalStorage<int> st_int;
        details::OptionalStorage<int> st_int_other(test);
        CHECK_FALSE(st_int._has_value);
        CHECK(st_int_other._has_value);
        CHECK(st_int_other._value == test);
        st_int = st_int_other;
        CHECK(st_int._has_value);
        CHECK(st_int._value == test);

        std::string test_str = "a test string for a optional storage";
        details::OptionalStorage<std::string> st_str;
        details::OptionalStorage<std::string> st_str_other(test_str);
        CHECK_FALSE(st_str._has_value);
        st_str = st_str_other;
        CHECK(st_str._has_value);
        CHECK(st_str._value == test_str);
    }

    SECTION("OptionalStorage move assignment should work")
    {
        details::OptionalStorage<std::string> st_str;
        CHECK_FALSE(st_str._has_value);
        st_str = std::string("a test string for a optional storage");
        CHECK(st_str._has_value);
        CHECK(st_str._value == "a test string for a optional storage");
    }
}

TEST_CASE("Optional class constructor", "[optional_constructors]")
{
    SECTION("Default constructor should work")
    {        
        Optional<int> opt;
        CHECK_FALSE(opt.has_value());
        CHECK_THROWS(opt.value());        
    }
    
    SECTION("Nullopt constructor should work")
    {        
        Optional<int> opt(nullopt);
        CHECK_FALSE(opt.has_value());
        CHECK_THROWS(opt.value());        
    }

    SECTION("Optional implicit constructor for trivially destructible T should work")
    {
        Optional<int> opt = 1;        
        CHECK(opt.has_value());
        CHECK_NOTHROW(opt.value());
        CHECK(opt.value() == 1);
    }

    SECTION("Optional implicit constructor for non trivially destructible T should work")
    {
        Optional<std::string> opt = "optional_string_test";        
        CHECK(opt.has_value());
        CHECK_NOTHROW(opt.value());
        CHECK(opt.value() == "optional_string_test");
    }

    SECTION("Optional value constructor for trivially destructible T should work")
    {
        int test = 5;
        Optional<int> opt(test);
        CHECK(opt.has_value());
        CHECK_NOTHROW(opt.value());
        CHECK(opt.value() == test);
    }

    SECTION("Optional value constructor for non trivially destructible T should work")
    {
        std::string str = "optional_string_test";
        Optional<std::string> opt(str);
        CHECK(opt.has_value());
        CHECK_NOTHROW(opt.value());
        CHECK(opt.value() == str);
    }

    SECTION("Optional value move constructor for non trivially destructible T should work")
    {
        std::string str = "optional_string_test";
        Optional<std::string> opt(std::move(str));
        CHECK(str.empty());
        CHECK(opt.has_value());
        CHECK_NOTHROW(opt.value());
        CHECK(opt.value() == "optional_string_test");
    }

    SECTION("Optional copy constructor for non trivially destructible T should work")
    {        
        std::string str = "optional_string_test";
        Optional<std::string> opt(str);
        Optional<std::string> opt_other = opt;
        CHECK(opt_other.has_value());
        CHECK_NOTHROW(opt_other.value());
        CHECK(opt_other.value() == opt.value());
    }

    SECTION("Optional move constructor for non trivially destructible T should work")
    {        
        std::string str = "optional_string_test";
        Optional<std::string> opt(str);        
        Optional<std::string> opt_other = std::move(opt);
        CHECK(opt_other.has_value());
        CHECK_NOTHROW(opt_other.value());
        CHECK(opt_other.value() == str);
        CHECK_FALSE(opt.has_value());
    }

    SECTION("Optional conversion constructor for non trivially destructible T should work")
    {
        // From lvalue.
        Optional<short> opt_short = 1;
        Optional<int> opt_int(opt_short);
        CHECK(opt_short.has_value());
        CHECK(opt_int.has_value());
        CHECK(opt_short.value() == opt_int.value());

        // From rvalue.        
        Optional<int> opt_int_from_rvalue(std::move(opt_short));
        CHECK_FALSE(opt_short.has_value());
        CHECK(opt_int_from_rvalue.has_value());
        CHECK(opt_int_from_rvalue.value() == opt_int.value());
    }
}

TEST_CASE("Optional class assignments", "[optional_assignments]")
{
    SECTION("Optional NullOpt assignment for trivially destructible T should work")
    {   
        Optional<int> opt = 1;        
        opt = nullopt;
        CHECK_FALSE(opt.has_value());
        CHECK_THROWS(opt.value());        
    }

    SECTION("Optional copy assignment for trivially destructible T should work")
    {        
        int test = 1;
        Optional<int> opt(test);
        Optional<int> opt_other;
        opt_other = opt;
        CHECK(opt_other.has_value());
        CHECK_NOTHROW(opt_other.value());
        CHECK(opt_other.value() == opt.value());
    }

    SECTION("Optional copy assignment for non trivially destructible T should work")
    {
        Optional<std::string> opt_other;
        std::string opt_str = "optional_string_test";
        Optional<std::string> opt(opt_str);
        opt_other = opt;
        CHECK(opt_other.has_value());
        CHECK_NOTHROW(opt_other.value());
        CHECK(opt_other.value() == opt.value());
    }
    
    SECTION("Optional move assignment for non trivially destructible T should work")
    {
        Optional<std::string> opt_other;
        std::string opt_str = "optional_string_test";
        Optional<std::string> opt(opt_str);
        opt_other = std::move(opt);
        CHECK(opt_other.has_value());
        CHECK_NOTHROW(opt_other.value());
        CHECK(opt_other.value() == opt_str);
    }

    SECTION("Optional conversion assignmentd from lvalue should work")
    {
        Optional<int> opt_int;
        Optional<short> opt_short = 1;
        opt_int = opt_short;
        CHECK(opt_int.has_value());
        CHECK_NOTHROW(opt_int.value());
        CHECK(opt_int.value() == opt_short.value());
    }    

    SECTION("Optional conversion assignment from rvalue should work")
    {
        Optional<int> opt_int;
        Optional<short> opt_short = 1;
        opt_int = std::move(opt_short);
        CHECK(opt_int.has_value());
        CHECK_NOTHROW(opt_int.value());
        CHECK_FALSE(opt_short.has_value());
        CHECK_THROWS(opt_short.value());
        CHECK(opt_int.value() == 1);
    }
}

TEST_CASE("Optional interface", "[public_interface]")
{
    int value = 1;
    Optional<int> opt(value);
    Optional<int> empty_opt;
    
    SECTION("Optional operator ->()")
    {
        std::string value_str = "value";
        Optional<std::string> opt_str(value_str);
        auto not_empty = !opt_str->empty();
        CHECK(not_empty);
        Optional<std::string> opt_str_other;
        CHECK_THROWS(opt_str_other->empty());
    }
    
    SECTION("Optional T& operator *()")
    {
        Optional<std::string> opt_str;
        CHECK_THROWS(*opt_str);

        std::string value_str = "value";
        opt_str = value_str;
        
        auto& contained_value = *opt_str;
        CHECK(contained_value == value_str);
        CHECK(!contained_value.empty());
        // Modify the value through the reference and check.
        contained_value.append(" modified");
        CHECK(contained_value == "value modified");

        const auto& const_contained_value = *opt_str;
        CHECK(const_contained_value == "value modified");        
    }

    SECTION("Optional operator bool()")
    {
        CHECK(bool(opt));
        CHECK_FALSE(bool(empty_opt));
    }
    
    SECTION("Optional has_value()")
    {
        CHECK(opt.has_value());
        CHECK_FALSE(empty_opt.has_value());
    }

    SECTION("Optional value() &")
    {
        int test = opt.value();
        CHECK(test == value);
        CHECK_THROWS(empty_opt.value());
    }

    SECTION("Optional value() &&")
    {
        int test = std::move(opt).value();
        CHECK(test == value);
        CHECK_THROWS(std::move(empty_opt).value());
    }

    SECTION("Optional value_or() &")
    {
        int test = empty_opt.value_or(value);
        CHECK(test == value);
        CHECK_THROWS(empty_opt.value());
        test = opt.value_or(2);
        CHECK(test == value);
    }

    SECTION("Optional value_or() &&")
    {
        int test = std::move(empty_opt).value_or(value);
        CHECK(test == value);
        CHECK_THROWS(std::move(empty_opt).value());
        test = std::move(opt).value_or(2);
        CHECK(test == value);
    }
}

TEST_CASE("Optional operators", "[operators]")
{
    SECTION("operator == for Optional should work")
    {
        Optional<int> opt_a(1);
        Optional<int> opt_b(1);
        Optional<int> opt_c(2);
        Optional<int> opt_null;
        
        CHECK(opt_a == opt_b);        
        CHECK_FALSE(opt_a == opt_c);
        CHECK_FALSE(opt_a == opt_null);
        CHECK_FALSE(opt_a == nullopt);
        CHECK_FALSE(nullopt == opt_a);
        CHECK(nullopt == opt_null);
        CHECK(opt_null == nullopt);

        int value = 1;
        CHECK(opt_a == value);
        CHECK(value == opt_a);
        CHECK_FALSE(opt_c == value);
        CHECK_FALSE(value == opt_c);
    }

    SECTION("operator != for Optional should work")
    {
        Optional<int> opt_a(1);
        Optional<int> opt_b(2);
        Optional<int> opt_c(2);
        Optional<int> opt_null;
        CHECK(opt_a != opt_b);        
        CHECK_FALSE(opt_b != opt_c);
        CHECK(opt_a != opt_null);
        CHECK(opt_a != nullopt);
        CHECK(nullopt != opt_a);
        CHECK_FALSE(nullopt != opt_null);
        CHECK_FALSE(opt_null != nullopt);
        int value = 1;
        CHECK(opt_b != value);
        CHECK(value != opt_b);
        CHECK_FALSE(opt_a != value);
        CHECK_FALSE(value != opt_a);
    }

    SECTION("operator < for Optional should work")
    {
        Optional<int> opt_lt(1);
        Optional<int> opt_eq(1);
        Optional<int> opt_gt(2);
        Optional<int> opt_null;

        CHECK(opt_lt < opt_gt);        
        CHECK_FALSE(opt_lt < opt_eq);
        CHECK_FALSE(opt_lt < opt_null);
        CHECK_FALSE(opt_lt < nullopt);
        CHECK(nullopt < opt_lt);
        CHECK_FALSE(nullopt < opt_null);
        CHECK_FALSE(opt_null < nullopt);

        int value = 0;
        CHECK(value < opt_gt);
        CHECK_FALSE(opt_lt < value);
        CHECK_FALSE(opt_null < value);
        CHECK_FALSE(value < opt_null);
    }

    SECTION("operator <= for Optional should work")
    {
        Optional<int> opt_lt(1);
        Optional<int> opt_eq(1);
        Optional<int> opt_gt(2);
        Optional<int> opt_null;

        CHECK(opt_lt <= opt_gt);        
        CHECK(opt_lt <= opt_eq);
        CHECK_FALSE(opt_lt < opt_eq);
        CHECK_FALSE(opt_lt <= opt_null);
        CHECK_FALSE(opt_lt <= nullopt);
        CHECK(nullopt <= opt_lt);
        CHECK(nullopt <= opt_null);
        CHECK(opt_null <= nullopt);

        int value = 1;
        CHECK(value <= opt_gt);
        CHECK_FALSE(opt_gt <= value);
        CHECK_FALSE(opt_null <= value);
        CHECK_FALSE(value <= opt_null);
    }

    SECTION("operator > for Optional should work")
    {
        Optional<int> opt_lt(1);
        Optional<int> opt_eq(1);
        Optional<int> opt_gt(2);
        Optional<int> opt_null;

        CHECK_FALSE(opt_lt > opt_gt);        
        CHECK_FALSE(opt_lt > opt_eq);
        CHECK(opt_lt > opt_null);
        CHECK(opt_lt > nullopt);
        CHECK_FALSE(nullopt > opt_lt);
        CHECK_FALSE(nullopt > opt_null);
        CHECK_FALSE(opt_null > nullopt);

        int value = 0;
        CHECK_FALSE(value > opt_gt);
        CHECK(opt_gt > value);
        CHECK_FALSE(opt_null > value);
        CHECK_FALSE(value > opt_null);
    }

    SECTION("operator >= for Optional should work")
    {
        Optional<int> opt_lt(1);
        Optional<int> opt_eq(1);
        Optional<int> opt_gt(2);
        Optional<int> opt_null;

        CHECK_FALSE(opt_lt >= opt_gt);        
        CHECK(opt_lt >= opt_eq);
        CHECK(opt_lt >= opt_null);
        CHECK(opt_lt >= nullopt);
        CHECK_FALSE(nullopt >= opt_lt);
        CHECK(nullopt >= opt_null);
        CHECK(opt_null >= nullopt);

        int value = 0;
        CHECK_FALSE(value >= opt_gt);
        CHECK(opt_gt >= value);
        CHECK_FALSE(opt_null >= value);
        CHECK_FALSE(value >= opt_null);
    }

}
