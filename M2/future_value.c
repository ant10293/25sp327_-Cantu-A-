#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEARS 50
#define MAX_ROWS 2 // two columns for interest and value

void printUsage() {
    fprintf(stderr, "Usage: ./future_value <monthly-investment> <annual-interest-rate> <number-of-years>\n\n");
    fprintf(stderr, "  monthly-investment: numeric value in range [1, 1000] representing the amount invested each month.\n");
    fprintf(stderr, "annual-interest-rate: decimal value in range [1, 15] representing the annual interest rate.\n");
    fprintf(stderr, "     number-of-years: integer value in range [1, 50] representing the duration of the investment in years.\n");
}

int inputParseValidation(char *argv[], double *monthly_investment, double *annual_interest_rate, int *years) {
    char *end;

    *monthly_investment = strtod(argv[1], &end);
    if (*end != '\0' || *monthly_investment < 1 || *monthly_investment > 1000) {
        fprintf(stderr, "Error: Invalid input for monthly investment.\n");
        return 0;
    }

    *annual_interest_rate = strtod(argv[2], &end);
    if (*end != '\0' || *annual_interest_rate < 1 || *annual_interest_rate > 15) {
        fprintf(stderr, "Error: Invalid input for annual interest rate.\n");
        return 0;
    }

    *years = strtol(argv[3], &end, 10);
    if (*end != '\0' || *years < 1 || *years > 50) {
        fprintf(stderr, "Error: Invalid input for number of years.\n");
        return 0;
    }

    return 1;
}

void calculateFutureValue(double monthly_investment, double annual_interest_rate, int years, double results[MAX_YEARS][MAX_ROWS]) {
    double monthly_interest_rate = annual_interest_rate / 12 / 100;
    int months = years * 12;
    double future_value = 0;

    for (int i = 0; i < months; ++i) {
        future_value += monthly_investment;
        double monthly_interest_amount = future_value * monthly_interest_rate;
        future_value += monthly_interest_amount;
        
        if ((i + 1) % 12 == 0) { // At the end of each year
            int year_idx = (i + 1) / 12 - 1;
            double interest_this_year = future_value - (monthly_investment * 12 * (year_idx + 1));
            results[year_idx][0] = interest_this_year;
            results[year_idx][1] = future_value;
        }
    }
}

void displayResults(double monthly_investment, double annual_interest_rate, int years, double results[MAX_YEARS][MAX_ROWS]) {
    printf("Monthly Investment: %.2f\n", monthly_investment);
    printf("Yearly Interest Rate: %.2f%%\n", annual_interest_rate);
    printf("Years: %d\n\n", years);

    printf("Future value after %d years is $%.2f.\n\n", years, results[years - 1][1]);
    printf("Year Accum Interest        Value\n");
    printf("==== ============== ============\n");

    for (int i = 0; i < years; i++) {
        printf("%4d %14.2f %12.2f\n", i + 1, results[i][0], results[i][1]);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printUsage();
        return EXIT_FAILURE;
    }

    double monthly_investment, annual_interest_rate;
    int years;
    double results[MAX_YEARS][MAX_ROWS];

    if (!inputParseValidation(argv, &monthly_investment, &annual_interest_rate, &years)) {
        printUsage();
        return EXIT_FAILURE;
    }

    calculateFutureValue(monthly_investment, annual_interest_rate, years, results);
    displayResults(monthly_investment, annual_interest_rate, years, results);

    return EXIT_SUCCESS;
}
