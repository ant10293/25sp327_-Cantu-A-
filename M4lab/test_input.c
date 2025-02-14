#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define UNUSED(x) (void)(x)

bool validate_double(char* input, double low, double high, double* number, char* err_msg);

void test_validate_double_valid(void **state) {
    UNUSED(state);  // This will avoid the unused parameter warning

    double number;
    char err_msg[100];
    assert_true(validate_double("5.5\n", 0.0, 10.0, &number, err_msg));
    assert_double_equal(number, 5.5, 0.001);
}

void test_validate_double_below_range(void **state) {
    UNUSED(state);
    double number;
    char err_msg[100];
    assert_false(validate_double("-0.1\n", 0.0, 10.0, &number, err_msg));
}

void test_validate_double_above_range(void **state) {
    UNUSED(state);
    double number;
    char err_msg[100];
    assert_false(validate_double("10.1\n", 0.0, 10.0, &number, err_msg));
}

void test_validate_double_non_numeric(void **state) {
    UNUSED(state);
    double number;
    char err_msg[100];
    assert_false(validate_double("abc\n", 0.0, 10.0, &number, err_msg));
}

void test_validate_double_extra_characters(void **state) {
    UNUSED(state);
    double number;
    char err_msg[100];
    assert_false(validate_double("5.5 extra\n", 0.0, 10.0, &number, err_msg));
}

void test_validate_double_exact_boundaries(void **state) {
    UNUSED(state);
    double number;
    char err_msg[100];
    assert_true(validate_double("0.0\n", 0.0, 10.0, &number, err_msg));
    assert_double_equal(number, 0.0, 0.001);
    assert_true(validate_double("10.0\n", 0.0, 10.0, &number, err_msg));
    assert_double_equal(number, 10.0, 0.001);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_validate_double_valid),
        cmocka_unit_test(test_validate_double_below_range),
        cmocka_unit_test(test_validate_double_above_range),
        cmocka_unit_test(test_validate_double_non_numeric),
        cmocka_unit_test(test_validate_double_extra_characters),
        cmocka_unit_test(test_validate_double_exact_boundaries),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
