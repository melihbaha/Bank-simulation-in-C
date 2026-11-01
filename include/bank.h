#pragma once

#include <stdint.h>
#include "customer.h"
#include "error.h"
#include "currency.h"

enum {
    BANK_COUNTRY_LEN = 2,
    BANK_CODE_LEN = 5
};

static const char BANK_COUNTRY[] = "TR";
static const char BANK_CODE[] = "99001";
static const char BANK_NAME[] = "C Bank";
static const char BANK_RESERVE_DIGIT = '0';

static const char CBANK_DEFAULT_DB_FILE[] = "cbank_customers.dat";
static const char CBANK_DEFAULT_TX_FILE[] = "cbank_transactions.log";

typedef enum {
    TX_DEPOSIT,
    TX_WITHDRAW,
    TX_TRANSFER_OUT,
    TX_TRANSFER_IN,
    TX_LOAN_TAKE,
    TX_DEBT_PAY,
    TX_CURRENCY_EXCHANGE
} TransactionType;

typedef struct {
    uint32_t customer_id;
    TransactionType type;
    uint64_t amount;
    Currency currency;
    char details[64];
} Transaction;

const char* bank_transaction_type_name(TransactionType type);

// Core Banking Operations
CbankResult bank_deposit(Customer* customer, uint64_t amount);
CbankResult bank_withdraw(Customer* customer, uint64_t amount);
CbankResult bank_transfer(Node* head, Customer* sender, const char* target_iban, uint64_t amount);
CbankResult bank_take_loan(Customer* customer, uint64_t amount);
CbankResult bank_pay_debt(Customer* customer, uint64_t amount);
CbankResult bank_exchange_currency(Customer* customer, Currency new_currency);

// Transaction History & Audit Log
void bank_log_transaction(uint32_t customer_id, TransactionType type, uint64_t amount, Currency currency, const char* details);
void bank_print_customer_transactions(uint32_t customer_id);
void bank_clear_transactions(void);
