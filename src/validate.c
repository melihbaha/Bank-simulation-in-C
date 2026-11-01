#include "validate.h"
#include "error.h"
#include "bank.h"
#include "customer.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


static int iban_mod97_push(int rem, int digit) {
    return (rem * 10 + digit) % 97;
}

static int iban_mod97_push_letter(int rem, char letter) {
    int value = (letter - 'A') + 10; // A=10 .. Z=35
    rem = iban_mod97_push(rem, value / 10);
    rem = iban_mod97_push(rem, value % 10);
    return rem;
}

CbankResult iban_validate(const char* iban) {
    if (iban == nullptr) {
        return CBANK_ERR_NULL;
    }
    if (iban[0] == '\0') {
        return CBANK_ERR_EMPTY;
    }
    if (strlen(iban) != IBAN_TR_LEN) {
        return CBANK_ERR_LENGTH;
    }

    if (iban[0] != BANK_COUNTRY[0] || iban[1] != BANK_COUNTRY[1]) {
        return CBANK_ERR_BANK_COUNTRY;
    }

    for (int i = 0; i < BANK_CODE_LEN; ++i) {
        if (iban[IBAN_BBAN_START + i] != BANK_CODE[i]) {
            return CBANK_ERR_BANK_CODE;
        }
    }

    if (iban[IBAN_RESERVE_INDEX] != BANK_RESERVE_DIGIT) {
        return CBANK_ERR_RESERVE_DIGIT;
    }

    for (int i = IBAN_CHECK_HI; i < IBAN_TR_LEN; ++i) {
        if (iban[i] < '0' || iban[i] > '9') {
            return CBANK_ERR_FORMAT;
        }
    }

    int rem = 0;
    for (int i = IBAN_BBAN_START; i < IBAN_TR_LEN; ++i) {
        rem = iban_mod97_push(rem, iban[i] - '0');
    }
    rem = iban_mod97_push_letter(rem, BANK_COUNTRY[0]);
    rem = iban_mod97_push_letter(rem, BANK_COUNTRY[1]);
    rem = iban_mod97_push(rem, 0);
    rem = iban_mod97_push(rem, 0);

    int expected = 98 - rem;
    int written = (iban[IBAN_CHECK_HI] - '0') * 10
        + (iban[IBAN_CHECK_LO] - '0');
    if (written != expected) {
        return CBANK_ERR_FORMAT;
    }

    return CBANK_OK;
}

CbankResult  name_validate(const char* name) {
    if (name == nullptr) return CBANK_ERR_NULL;
    if (name[0] == '\0') return CBANK_ERR_EMPTY;

    size_t len = strlen(name);

    if (len >= CUSTOMER_NAME_LEN) return CBANK_ERR_LENGTH;

    for (size_t i = 0;i < len;i++) {
        if(isdigit(name[i])) return CBANK_ERR_NAME;
    }

    return CBANK_OK;
}

CbankResult tc_validate(const char* tc) {
    if (tc == NULL) return CBANK_ERR_NULL;

    uint8_t singles = 0;
    uint8_t doubles = 0;
    uint8_t all_first_ten_sum = 0;

    for (size_t i = 0; i < 11; i++) {
        if (tc[i] == '\0') return CBANK_ERR_LENGTH;
        
        if (!isdigit((unsigned char)tc[i])) return CBANK_ERR_TC;

        uint8_t digit = (uint8_t)(tc[i] - '0');

        if (i == 0 && digit == 0) return CBANK_ERR_TC_FIRST_DIGIT;
        if (i == 10 && (digit % 2 != 0)) return CBANK_ERR_LAST_DIGIT;

        if (i < 9) {
            if (i % 2 == 0) {
                singles += digit;
            } else {
                doubles += digit;
            }
        }
        
        if (i < 10) {
            all_first_ten_sum += digit;
        }
    }

    if (tc[11] != '\0') return CBANK_ERR_LENGTH;

    uint8_t expected_tenth = ((((singles * 7) - doubles) % 10) + 10) % 10;
    uint8_t expected_eleventh = all_first_ten_sum % 10;

    uint8_t actual_tenth = (uint8_t)(tc[9] - '0');
    uint8_t actual_eleventh = (uint8_t)(tc[10] - '0');

    if (expected_tenth != actual_tenth || expected_eleventh != actual_eleventh) {
        return CBANK_ERR_LAST_TWO_DIGIT;
    }

    return CBANK_OK;
}

CbankResult password_validate(const char* password) {
    if(password == nullptr) return CBANK_ERR_NULL;
    if(password[0] == '\0') return CBANK_ERR_EMPTY;
    
    size_t len = strlen(password);

    if(len >= CUSTOMER_PASSWORD_LEN) return CBANK_ERR_LENGTH;

    return CBANK_OK;
}

CbankResult customer_id_validate(const uint32_t customer_id) {
    if (customer_id == 0) return  CBANK_ERR_EMPTY;
    if (customer_id > UINT32_MAX) return CBANK_ERR_LENGTH;

    return CBANK_OK;
}

CbankResult account_number_validate(const uint64_t account_number) {
    if (account_number == 0) return CBANK_ERR_EMPTY;
    if (account_number > UINT64_MAX) return CBANK_ERR_LENGTH;
    
    return CBANK_OK;
}