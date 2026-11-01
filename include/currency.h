#pragma once
#include <stdint.h>

// currency repository
typedef enum {
    CURRENCY_TRY,
    CURRENCY_USD,
    CURRENCY_EUR,
    CURRENCY_GBP,
    CURRENCY_CHF,
    CURRENCY_JPY,
    CURRENCY_COUNT
}Currency;

const char* currency_name(Currency currency);
uint64_t currency_value(Currency currency);
uint64_t currency_convert(uint64_t amount, Currency from, Currency to);
void currency_print_rates(void);
