#pragma once

#include <stdint.h>
#include <stddef.h>
#include "currency.h"
#include "error.h"

// a struct capable of storing account type
typedef enum {
    ACCOUNT_NONE,
    CHECKING,
    SAVINGS,
    INVESTMENT,
    ACCOUNT_TYPE_COUNT
} AccountType;


// a struct capable of storing birthday
typedef struct {
    uint8_t day;
    uint8_t month;
    uint16_t year;
}Birthday;

// a struct capable of storing phone information
typedef struct {
    char area_code[4];
    char number[11];
}Phone;

enum {
    CUSTOMER_NAME_LEN     = 64,
    CUSTOMER_IBAN_LEN     = 27,
    CUSTOMER_TC_LEN       = 12,
    CUSTOMER_PASSWORD_LEN = 128
};



// A struct capable of storing customer information.
typedef struct {
    char name[CUSTOMER_NAME_LEN];               // customer's name
    char iban[CUSTOMER_IBAN_LEN];               // customer's iban
    char tc[CUSTOMER_TC_LEN];                   // customer's Republic of Turkey identity number
    char password[CUSTOMER_PASSWORD_LEN];       // customer's password
    uint32_t customer_id;                       // customer's accessible ID
    uint64_t account_number;                    // customer's accessible number
    uint64_t balance;                           // customer's account balance
    uint64_t debt;                              // customer's account balance due
    AccountType account_type;                   // customer's account type
    Birthday birthday;                          // customer's birthday
    Phone phone;                                // customer's phone number and area code of phone number
    Currency currency;                          // customer's account currency
}Customer;

typedef struct Node {
    Customer customer;
    struct Node* next;
}Node;

void clear_customer(Customer* customer);
const char* account_type_name(AccountType type);
void list_customer_information(const Customer* customer);
CbankResult customer_set_name(Customer* customer, const char* name);
CbankResult customer_set_iban(Customer* customer, const char* iban);
CbankResult customer_set_tc(Customer* customer, const char* tc, size_t tc_buf_len);
CbankResult customer_set_password(Customer* customer, const char* password);
CbankResult customer_set_customer_id(Customer* customer, const uint32_t customer_id);
CbankResult customer_set_account_number(Customer* customer, const uint64_t account_number);
CbankResult customer_set_balance(Customer* customer, const uint64_t balance);
CbankResult customer_set_debt(Customer* customer, const uint64_t debt);
CbankResult customer_set_account_type(Customer* customer, const AccountType account_type);
CbankResult customer_set_birthday(Customer* customer, const Birthday birthday);
CbankResult customer_set_phone(Customer* customer, const Phone phone);
CbankResult customer_set_currency(Customer* customer, const Currency currency);

// Password Hashing & Authentication
CbankResult password_hash(const char* raw_password, char* out_hash, size_t out_size);
CbankResult customer_set_password_hashed(Customer* customer, const char* raw_password);
bool customer_verify_password(const Customer* customer, const char* raw_password);
Customer* customer_authenticate(Node* head, const char* tc_or_id, const char* raw_password);

// Linked List Operations
CbankResult customer_list_add(Node** head, const Customer* customer);
Customer* customer_list_find_by_tc(Node* head, const char* tc);
Customer* customer_list_find_by_id(Node* head, uint32_t customer_id);
Customer* customer_list_find_by_iban(Node* head, const char* iban);
Customer* customer_list_find_by_account(Node* head, uint64_t account_number);
CbankResult customer_list_remove_by_id(Node** head, uint32_t customer_id);
void customer_list_free(Node** head);
size_t customer_list_count(const Node* head);
uint32_t customer_next_id(const Node* head);
uint64_t customer_next_account_number(const Node* head);

// Persistence (Save / Load)
CbankResult customer_list_save(const Node* head, const char* filepath);
CbankResult customer_list_load(Node** head, const char* filepath);
