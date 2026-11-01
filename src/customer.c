#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "customer.h"
#include "error.h"
#include "validate.h"

void clear_customer(Customer* customer) {
    if (customer == nullptr) return;

    memset(customer, 0, sizeof(Customer));

    customer->account_type = ACCOUNT_NONE;
    customer->currency = CURRENCY_TRY;
}

static const char* const ACCOUNT_TYPE_NAME[] = {
    [ACCOUNT_NONE] = "ACCOUNT_NONE",
    [CHECKING] = "CHECKING",
    [SAVINGS] = "SAVINGS",
    [INVESTMENT] = "INVESTMENT",
};

const char* account_type_name(AccountType type) {
    if (type < ACCOUNT_TYPE_COUNT && ACCOUNT_TYPE_NAME[type] != nullptr) {
        return ACCOUNT_TYPE_NAME[type];
    }
    return "Unknown";
}

void list_customer_information(const Customer* customer) {
    printf("-------------------------------------\n");
    printf("%s\n",customer->name);
    printf("Iban: %s\n",customer->iban);
    printf("Tc: %s\n",customer->tc);
    printf("Password: %s\n",customer->password);
    printf("Id: %u\n",customer->customer_id);
    printf("Account Number: %" PRIu64 "\n", customer->account_number);
    printf("balance: %lu\n",customer->balance);
    printf("debt: %lu\n",customer->debt);
    printf("Account Type: %s\n", account_type_name(customer->account_type));
    printf("Phone: +%s %s\n",customer->phone.area_code, customer->phone.number);
    printf("Currency: %s\n", currency_name(customer->currency));
}

CbankResult customer_set_name(Customer* customer, const char* name) {
    if (customer == nullptr || name == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = name_validate(name);
    if (is_valid != CBANK_OK) {
        return is_valid;
    } 

    strncpy(customer->name, name, CUSTOMER_NAME_LEN - 1);
    customer->name[CUSTOMER_NAME_LEN - 1] = '\0'; 
    return CBANK_OK;
}

CbankResult customer_set_iban(Customer* customer,const char* iban) {
    if (customer == nullptr || iban == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = iban_validate(iban);
    if(is_valid != CBANK_OK) {
        return is_valid;
    }

    strncpy(customer->iban, iban, CUSTOMER_IBAN_LEN - 1);
    customer->iban[CUSTOMER_IBAN_LEN - 1] = '\0';

    return CBANK_OK;
}

CbankResult customer_set_tc(Customer* customer, const char* tc, size_t tc_buf_len) {
    if (customer == nullptr || tc == nullptr || tc_buf_len == 0) return CBANK_ERR_NULL;
    
    // Gelen buffer en az 12 bayt olmalı (11 hane + '\0')
    if (tc_buf_len < CUSTOMER_TC_LEN) return CBANK_ERR_LENGTH;

    CbankResult is_valid = tc_validate(tc);
    if (is_valid != CBANK_OK) return is_valid;

    // Uzunluk doğrulandığı için güvenli kopyalama
    memcpy(customer->tc, tc, 11);
    customer->tc[11] = '\0';

    return CBANK_OK;
}

CbankResult customer_set_password(Customer* customer, const char* password) {
    if (customer == nullptr || password == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = password_validate(password);
    if(is_valid != CBANK_OK) return is_valid;

    strncpy(customer->password , password, CUSTOMER_PASSWORD_LEN - 1);
    customer->password[CUSTOMER_PASSWORD_LEN -1] = '\0';

    return CBANK_OK;
}

CbankResult customer_set_customer_id(Customer* customer, const uint32_t customer_id) {
    if(customer == nullptr || customer_id == 0) return CBANK_ERR_NULL;

    CbankResult is_valid = customer_id_validate(customer_id);
    if(is_valid != CBANK_OK) return is_valid;

    customer->customer_id = customer_id;

    return CBANK_OK;
}

CbankResult customer_set_account_number(Customer* customer, const uint64_t account_number) {
    if (customer == nullptr || account_number == 0) return CBANK_ERR_NULL;

    CbankResult is_valid = account_number_validate(account_number);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->account_number = account_number;

    return CBANK_OK;
}
