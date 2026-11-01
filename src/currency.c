#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "currency.h"

static const char* const CURRENCY_NAME[] = {
    [CURRENCY_TRY] = "TRY",
    [CURRENCY_USD] = "USD",
    [CURRENCY_EUR] = "EUR",
    [CURRENCY_GBP] = "GBP",
    [CURRENCY_CHF] = "CHF",
    [CURRENCY_JPY] = "JPY"
};

// Currency values in TRY terms (scaled by 100, i.e., 1 TRY = 100 kurus)
static const uint64_t EXCHANGE_RATES[] = {
    [CURRENCY_TRY] = 100,
    [CURRENCY_USD] = 3410,
    [CURRENCY_EUR] = 3805,
    [CURRENCY_GBP] = 4520,
    [CURRENCY_CHF] = 4015,
    [CURRENCY_JPY] = 24
};

const char* currency_name(Currency currency) {
    if ((int)currency >= (int)CURRENCY_TRY && currency < CURRENCY_COUNT && CURRENCY_NAME[currency] != nullptr) {
        return CURRENCY_NAME[currency];
    }
    return "Unknown";
}

uint64_t currency_value(Currency currency) {
    if ((int)currency >= (int)CURRENCY_TRY && currency < CURRENCY_COUNT) {
        return EXCHANGE_RATES[currency];
    }
    return 0;
}

uint64_t currency_convert(uint64_t amount, Currency from, Currency to) {
    if (from == to) return amount;
    uint64_t from_rate = currency_value(from);
    uint64_t to_rate = currency_value(to);
    if (from_rate == 0 || to_rate == 0) return 0;

    return (amount * from_rate) / to_rate;
}

void currency_print_rates(void) {
    printf("\n================ DOVIZ KURLARI ================\n");
    printf(" %-6s | %-12s | %-18s\n", "Kod", "Birim Deger", "TRY Karsiligi");
    printf("-----------------------------------------------\n");
    for (int i = CURRENCY_TRY; i < CURRENCY_COUNT; ++i) {
        Currency c = (Currency)i;
        uint64_t rate = currency_value(c);
        printf(" [%d] %-3s | %6" PRIu64 " puan | %2" PRIu64 ".%02" PRIu64 " TRY\n",
               i, currency_name(c), rate, rate / 100, rate % 100);
    }
    printf("===============================================\n");
}
