#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool validate_double(char* input, double low, double high, double* number, char* err_msg);

int input_double(char* prompt, double low, double high, double* number) {
    char* input = NULL;
    size_t input_size = 0;
    char err_msg[100];

    while (1) {
        printf("%s ", prompt);
        ssize_t chars_read = getline(&input, &input_size, stdin);

        if (chars_read == -1) {
            printf("Error reading input from stdin.\n");
            free(input);
            return -1;
        }

        if (!validate_double(input, low, high, number, err_msg)) {
            printf("%s\n", err_msg);
        } else {
            free(input);
            return 0;
        }
    }
}

bool validate_double(char* input, double low, double high, double* number, char* err_msg) {
    int scanned_chars = 0;
    double value = 0.0;
    /*
        sscanf returns the number of input items scanned.
        %lf - Scans a double.
        %n  - Nothing is expected; instead, the number of characters consumed thus far  from
              the  input is stored through the next pointer, which must be a pointer to int.
              An input string of "1234.56 789" is not accepted.
    */
    if (sscanf(input, "%lf%n", &value, &scanned_chars) == 1) {
        if (scanned_chars == (int)strlen(input) - 1) {  // subtract one to account for newline char
            if (value >= low && value <= high) {
                *number = value;
                return true;
            } else {
                snprintf(err_msg, 100, "Entry must be greater than %.2f and less than or equal to %.2f. Please try again.", low, high);
            }
        } else {
            snprintf(err_msg, 100, "Entry must be a single numeric value. Please try again.");
        }
    } else {
        snprintf(err_msg, 100, "Entry must be numeric. Please try again.");
    }
    return false;
}

#ifndef UNIT_TEST
int main(void) {
    double number = 0.0;
    input_double("Enter a number:", 0.0, 10.0, &number);
    printf("%f\n", number);

    return 0;
}
#endif