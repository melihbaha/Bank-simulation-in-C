#include "bank.h"
#include "validate.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

enum {
    MAX_MEMORY_TRANSACTIONS = 512
};

static Transaction s_transactions[MAX_MEMORY_TRANSACTIONS];
static size_t s_transaction_count = 0;

const char* bank_transaction_type_name(TransactionType type) {
    switch (type) {
        case TX_DEPOSIT:           return "PARA YATIRMA";
        case TX_WITHDRAW:          return "PARA CEKME";
        case TX_TRANSFER_OUT:      return "GIDEN TRANSFER";
        case TX_TRANSFER_IN:       return "GELEN TRANSFER";
        case TX_LOAN_TAKE:         return "KREDI KULLANIMI";
        case TX_DEBT_PAY:          return "BORC ODEME";
        case TX_CURRENCY_EXCHANGE: return "DOVIZ DONUSUMU";
        default:                   return "BILINMEYEN ISLEM";
    }
}

void bank_log_transaction(uint32_t customer_id, TransactionType type, uint64_t amount, Currency currency, const char* details) {
    if (s_transaction_count < MAX_MEMORY_TRANSACTIONS) {
        Transaction* tx = &s_transactions[s_transaction_count++];
        tx->customer_id = customer_id;
        tx->type = type;
        tx->amount = amount;
        tx->currency = currency;
        if (details != nullptr) {
            strncpy(tx->details, details, sizeof(tx->details) - 1);
            tx->details[sizeof(tx->details) - 1] = '\0';
        } else {
            tx->details[0] = '\0';
        }
    }

    FILE* fp = fopen(CBANK_DEFAULT_TX_FILE, "a");
    if (fp != nullptr) {
        fprintf(fp, "%u|%-16s|%" PRIu64 "|%s|%s\n",
                customer_id,
                bank_transaction_type_name(type),
                amount,
                currency_name(currency),
                details != nullptr ? details : "-");
        fclose(fp);
    }
}

void bank_print_customer_transactions(uint32_t customer_id) {
    printf("\n=========== HESAP HAREKETLERI (Musteri #%u) ===========\n", customer_id);
    size_t found = 0;

    for (size_t i = 0; i < s_transaction_count; ++i) {
        const Transaction* tx = &s_transactions[i];
        if (tx->customer_id == customer_id) {
            printf(" [%zu] %-16s | %10" PRIu64 " %-3s | %s\n",
                   ++found,
                   bank_transaction_type_name(tx->type),
                   tx->amount,
                   currency_name(tx->currency),
                   tx->details);
        }
    }

    if (found == 0) {
        printf(" Bu oturumda kayitli islem hareketi bulunmamaktadir.\n");
    }
    printf("=========================================================\n");
}

void bank_clear_transactions(void) {
    s_transaction_count = 0;
}

CbankResult bank_deposit(Customer* customer, uint64_t amount) {
    if (customer == nullptr) return CBANK_ERR_NULL;
    if (amount == 0) return CBANK_ERR_INVALID_AMOUNT;
    if (UINT64_MAX - customer->balance < amount) return CBANK_ERR_FULL;

    customer->balance += amount;
    bank_log_transaction(customer->customer_id, TX_DEPOSIT, amount, customer->currency, "Hesaba nakit yatirma");
    return CBANK_OK;
}

CbankResult bank_withdraw(Customer* customer, uint64_t amount) {
    if (customer == nullptr) return CBANK_ERR_NULL;
    if (amount == 0) return CBANK_ERR_INVALID_AMOUNT;
    if (customer->balance < amount) return CBANK_ERR_INSUFFICIENT_BALANCE;

    customer->balance -= amount;
    bank_log_transaction(customer->customer_id, TX_WITHDRAW, amount, customer->currency, "Hesaptan nakit cekme");
    return CBANK_OK;
}

CbankResult bank_transfer(Node* head, Customer* sender, const char* target_iban, uint64_t amount) {
    if (head == nullptr || sender == nullptr || target_iban == nullptr) return CBANK_ERR_NULL;
    if (amount == 0) return CBANK_ERR_INVALID_AMOUNT;

    CbankResult iban_res = iban_validate(target_iban);
    if (iban_res != CBANK_OK) return iban_res;

    if (strcmp(sender->iban, target_iban) == 0) return CBANK_ERR_DUPLICATE;
    if (sender->balance < amount) return CBANK_ERR_INSUFFICIENT_BALANCE;

    Customer* receiver = customer_list_find_by_iban(head, target_iban);
    if (receiver == nullptr) return CBANK_ERR_NOT_FOUND;

    uint64_t credited_amount = currency_convert(amount, sender->currency, receiver->currency);
    if (credited_amount == 0) return CBANK_ERR_INVALID_AMOUNT;
    if (UINT64_MAX - receiver->balance < credited_amount) return CBANK_ERR_FULL;

    sender->balance -= amount;
    receiver->balance += credited_amount;

    char out_detail[64];
    snprintf(out_detail, sizeof(out_detail), "Alici: %.20s (%.26s)", receiver->name, receiver->iban);
    bank_log_transaction(sender->customer_id, TX_TRANSFER_OUT, amount, sender->currency, out_detail);

    char in_detail[64];
    snprintf(in_detail, sizeof(in_detail), "Gonderen: %.20s (%.26s)", sender->name, sender->iban);
    bank_log_transaction(receiver->customer_id, TX_TRANSFER_IN, credited_amount, receiver->currency, in_detail);

    return CBANK_OK;
}

CbankResult bank_take_loan(Customer* customer, uint64_t amount) {
    if (customer == nullptr) return CBANK_ERR_NULL;
    if (amount == 0) return CBANK_ERR_INVALID_AMOUNT;
    if (UINT64_MAX - customer->balance < amount || UINT64_MAX - customer->debt < amount) {
        return CBANK_ERR_FULL;
    }

    customer->balance += amount;
    customer->debt += amount;
    bank_log_transaction(customer->customer_id, TX_LOAN_TAKE, amount, customer->currency, "Kredi kullanimi");
    return CBANK_OK;
}

CbankResult bank_pay_debt(Customer* customer, uint64_t amount) {
    if (customer == nullptr) return CBANK_ERR_NULL;
    if (amount == 0 || customer->debt == 0) return CBANK_ERR_INVALID_AMOUNT;

    uint64_t actual_pay = amount > customer->debt ? customer->debt : amount;
    if (customer->balance < actual_pay) return CBANK_ERR_INSUFFICIENT_BALANCE;

    customer->balance -= actual_pay;
    customer->debt -= actual_pay;
    bank_log_transaction(customer->customer_id, TX_DEBT_PAY, actual_pay, customer->currency, "Kredi/Borc odemesi");
    return CBANK_OK;
}

CbankResult bank_exchange_currency(Customer* customer, Currency new_currency) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult valid = currency_validate(new_currency);
    if (valid != CBANK_OK) return valid;

    if (customer->currency == new_currency) return CBANK_OK;

    Currency old_currency = customer->currency;
    uint64_t new_balance = currency_convert(customer->balance, old_currency, new_currency);
    uint64_t new_debt = currency_convert(customer->debt, old_currency, new_currency);

    customer->balance = new_balance;
    customer->debt = new_debt;
    customer->currency = new_currency;

    char detail[64];
    snprintf(detail, sizeof(detail), "%s -> %s", currency_name(old_currency), currency_name(new_currency));
    bank_log_transaction(customer->customer_id, TX_CURRENCY_EXCHANGE, new_balance, new_currency, detail);

    return CBANK_OK;
}
