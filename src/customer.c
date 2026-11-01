#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "customer.h"
#include "error.h"
#include "validate.h"

static void secure_zero_memory(void* ptr, size_t size) {
    if (ptr == nullptr) return;
    volatile unsigned char* p = (volatile unsigned char*)ptr;
    while (size--) {
        *p++ = 0;
    }
}

static bool secure_str_equals(const char* a, const char* b) {
    if (a == nullptr || b == nullptr) return false;
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    unsigned char diff = (unsigned char)(len_a ^ len_b);
    for (size_t i = 0; i < len_a && i < len_b; ++i) {
        diff |= (unsigned char)(a[i] ^ b[i]);
    }
    return diff == 0;
}

void clear_customer(Customer* customer) {
    if (customer == nullptr) return;

    secure_zero_memory(customer, sizeof(Customer));

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
    if ((int)type >= (int)ACCOUNT_NONE && type < ACCOUNT_TYPE_COUNT && ACCOUNT_TYPE_NAME[type] != nullptr) {
        return ACCOUNT_TYPE_NAME[type];
    }
    return "Unknown";
}

void list_customer_information(const Customer* customer) {
    if (customer == nullptr) return;

    printf("-------------------------------------\n");
    printf("Ad Soyad       : %s\n", customer->name);
    printf("IBAN           : %s\n", customer->iban);
    printf("T.C. Kimlik No : %s\n", customer->tc);
    printf("Sifre (Hash)   : ********\n");
    printf("Musteri ID     : %u\n", customer->customer_id);
    printf("Hesap Numarasi : %" PRIu64 "\n", customer->account_number);
    printf("Bakiye         : %" PRIu64 " %s\n", customer->balance, currency_name(customer->currency));
    printf("Borc           : %" PRIu64 " %s\n", customer->debt, currency_name(customer->currency));
    printf("Hesap Tipi     : %s\n", account_type_name(customer->account_type));
    printf("Dogum Tarihi   : %02u/%02u/%04u\n",
           customer->birthday.day, customer->birthday.month, customer->birthday.year);
    printf("Telefon        : +%s %s\n", customer->phone.area_code, customer->phone.number);
    printf("Para Birimi    : %s\n", currency_name(customer->currency));
    printf("-------------------------------------\n");
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

CbankResult customer_set_balance(Customer* customer, const uint64_t balance) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = balance_validate(balance);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->balance = balance;

    return CBANK_OK;
}

CbankResult customer_set_debt(Customer* customer, const uint64_t debt) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = debt_validate(debt);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->debt = debt;

    return CBANK_OK;
}

CbankResult customer_set_account_type(Customer* customer, const AccountType account_type) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = account_type_validate(account_type);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->account_type = account_type;

    return CBANK_OK;
}

CbankResult customer_set_birthday(Customer* customer, const Birthday birthday) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = birthday_validate(birthday);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->birthday = birthday;

    return CBANK_OK;
}

CbankResult customer_set_phone(Customer* customer, const Phone phone) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = phone_validate(phone);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->phone = phone;

    return CBANK_OK;
}

CbankResult customer_set_currency(Customer* customer, const Currency currency) {
    if (customer == nullptr) return CBANK_ERR_NULL;

    CbankResult is_valid = currency_validate(currency);
    if (is_valid != CBANK_OK) {
        return is_valid;
    }

    customer->currency = currency;

    return CBANK_OK;
}

// ============================================================================
// Sifre Hashleme ve Kimlik Dogrulama (FNV-1a 128-bit Dual Lane + Salt)
// ============================================================================

CbankResult password_hash(const char* raw_password, char* out_hash, size_t out_size) {
    if (raw_password == nullptr || out_hash == nullptr) return CBANK_ERR_NULL;

    CbankResult valid = password_validate(raw_password);
    if (valid != CBANK_OK) return valid;

    if (out_size < 40) return CBANK_ERR_LENGTH;

    uint64_t h1 = 14695981039346656037ULL;
    uint64_t h2 = 1099511628211ULL ^ 0x4342414E4B323032ULL; // "CBANK202" salt

    for (size_t i = 0; raw_password[i] != '\0'; ++i) {
        uint8_t byte = (uint8_t)raw_password[i];
        h1 ^= byte;
        h1 *= 1099511628211ULL;
        h2 ^= (uint8_t)(byte ^ (uint8_t)(i * 31U + 17U));
        h2 *= 14695981039346656037ULL;
    }

    int written = snprintf(out_hash, out_size, "$CBH$%016" PRIx64 "%016" PRIx64, h1, h2);
    if (written < 0 || (size_t)written >= out_size) return CBANK_ERR_FORMAT;

    return CBANK_OK;
}

CbankResult customer_set_password_hashed(Customer* customer, const char* raw_password) {
    if (customer == nullptr || raw_password == nullptr) return CBANK_ERR_NULL;

    char hashed[CUSTOMER_PASSWORD_LEN];
    CbankResult res = password_hash(raw_password, hashed, sizeof(hashed));
    if (res != CBANK_OK) return res;

    return customer_set_password(customer, hashed);
}

bool customer_verify_password(const Customer* customer, const char* raw_password) {
    if (customer == nullptr || raw_password == nullptr || raw_password[0] == '\0') {
        return false;
    }

    if (strncmp(customer->password, "$CBH$", 5) == 0) {
        char hashed[CUSTOMER_PASSWORD_LEN];
        if (password_hash(raw_password, hashed, sizeof(hashed)) != CBANK_OK) {
            return false;
        }
        bool ok = secure_str_equals(customer->password, hashed);
        secure_zero_memory(hashed, sizeof(hashed));
        return ok;
    }

    return secure_str_equals(customer->password, raw_password);
}

Customer* customer_authenticate(Node* head, const char* tc_or_id, const char* raw_password) {
    if (head == nullptr || tc_or_id == nullptr || raw_password == nullptr) return nullptr;

    Customer* found = nullptr;
    if (strlen(tc_or_id) == 11) {
        found = customer_list_find_by_tc(head, tc_or_id);
    }
    if (found == nullptr) {
        char* endptr = nullptr;
        unsigned long id_val = strtoul(tc_or_id, &endptr, 10);
        if (endptr != tc_or_id && *endptr == '\0' && id_val > 0 && id_val <= UINT32_MAX) {
            found = customer_list_find_by_id(head, (uint32_t)id_val);
        }
    }

    if (found != nullptr && customer_verify_password(found, raw_password)) {
        return found;
    }

    return nullptr;
}

// ============================================================================
// Bagli Liste (Linked List) Yonetimi
// ============================================================================

CbankResult customer_list_add(Node** head, const Customer* customer) {
    if (head == nullptr || customer == nullptr) return CBANK_ERR_NULL;

    if (customer_list_find_by_tc(*head, customer->tc) != nullptr ||
        customer_list_find_by_id(*head, customer->customer_id) != nullptr ||
        customer_list_find_by_iban(*head, customer->iban) != nullptr ||
        customer_list_find_by_account(*head, customer->account_number) != nullptr) {
        return CBANK_ERR_DUPLICATE;
    }

    Node* new_node = malloc(sizeof(Node));
    if (new_node == nullptr) return CBANK_ERR_ALLOC;

    new_node->customer = *customer;
    new_node->next = nullptr;

    if (*head == nullptr) {
        *head = new_node;
    } else {
        Node* curr = *head;
        while (curr->next != nullptr) {
            curr = curr->next;
        }
        curr->next = new_node;
    }

    return CBANK_OK;
}

Customer* customer_list_find_by_tc(Node* head, const char* tc) {
    if (tc == nullptr) return nullptr;

    for (Node* curr = head; curr != nullptr; curr = curr->next) {
        if (strcmp(curr->customer.tc, tc) == 0) {
            return &curr->customer;
        }
    }
    return nullptr;
}

Customer* customer_list_find_by_id(Node* head, uint32_t customer_id) {
    if (customer_id == 0) return nullptr;

    for (Node* curr = head; curr != nullptr; curr = curr->next) {
        if (curr->customer.customer_id == customer_id) {
            return &curr->customer;
        }
    }
    return nullptr;
}

Customer* customer_list_find_by_iban(Node* head, const char* iban) {
    if (iban == nullptr) return nullptr;

    for (Node* curr = head; curr != nullptr; curr = curr->next) {
        if (strcmp(curr->customer.iban, iban) == 0) {
            return &curr->customer;
        }
    }
    return nullptr;
}

Customer* customer_list_find_by_account(Node* head, uint64_t account_number) {
    if (account_number == 0) return nullptr;

    for (Node* curr = head; curr != nullptr; curr = curr->next) {
        if (curr->customer.account_number == account_number) {
            return &curr->customer;
        }
    }
    return nullptr;
}

CbankResult customer_list_remove_by_id(Node** head, uint32_t customer_id) {
    if (head == nullptr) return CBANK_ERR_NULL;
    if (*head == nullptr) return CBANK_ERR_NOT_FOUND;

    Node* curr = *head;
    Node* prev = nullptr;

    while (curr != nullptr) {
        if (curr->customer.customer_id == customer_id) {
            if (curr->customer.balance > 0 || curr->customer.debt > 0) {
                return CBANK_ERR_HAS_BALANCE_OR_DEBT;
            }
            if (prev == nullptr) {
                *head = curr->next;
            } else {
                prev->next = curr->next;
            }
            secure_zero_memory(&curr->customer, sizeof(Customer));
            free(curr);
            return CBANK_OK;
        }
        prev = curr;
        curr = curr->next;
    }

    return CBANK_ERR_NOT_FOUND;
}

void customer_list_free(Node** head) {
    if (head == nullptr) return;

    Node* curr = *head;
    while (curr != nullptr) {
        Node* next = curr->next;
        secure_zero_memory(&curr->customer, sizeof(Customer));
        free(curr);
        curr = next;
    }
    *head = nullptr;
}

size_t customer_list_count(const Node* head) {
    size_t count = 0;
    for (const Node* curr = head; curr != nullptr; curr = curr->next) {
        count++;
    }
    return count;
}

uint32_t customer_next_id(const Node* head) {
    uint32_t max_id = 1000;
    for (const Node* curr = head; curr != nullptr; curr = curr->next) {
        if (curr->customer.customer_id > max_id) {
            max_id = curr->customer.customer_id;
        }
    }
    return max_id + 1;
}

uint64_t customer_next_account_number(const Node* head) {
    uint64_t max_acc = 1000000000ULL;
    for (const Node* curr = head; curr != nullptr; curr = curr->next) {
        if (curr->customer.account_number > max_acc) {
            max_acc = curr->customer.account_number;
        }
    }
    return max_acc + 1;
}

// ============================================================================
// Kalicilik (Dosya Kaydetme / Yukleme)
// ============================================================================

static const uint32_t CBANK_DB_MAGIC = 0x43424E4BU; // "CBNK"
static const uint32_t CBANK_DB_VERSION = 1U;

CbankResult customer_list_save(const Node* head, const char* filepath) {
    if (filepath == nullptr) return CBANK_ERR_NULL;

    char tmp_filepath[512];
    int written = snprintf(tmp_filepath, sizeof(tmp_filepath), "%s.tmp", filepath);
    if (written < 0 || (size_t)written >= sizeof(tmp_filepath)) return CBANK_ERR_FORMAT;

    FILE* fp = fopen(tmp_filepath, "wb");
    if (fp == nullptr) return CBANK_ERR_IO;

    uint32_t count = (uint32_t)customer_list_count(head);
    if (fwrite(&CBANK_DB_MAGIC, sizeof(uint32_t), 1, fp) != 1 ||
        fwrite(&CBANK_DB_VERSION, sizeof(uint32_t), 1, fp) != 1 ||
        fwrite(&count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        remove(tmp_filepath);
        return CBANK_ERR_IO;
    }

    for (const Node* curr = head; curr != nullptr; curr = curr->next) {
        if (fwrite(&curr->customer, sizeof(Customer), 1, fp) != 1) {
            fclose(fp);
            remove(tmp_filepath);
            return CBANK_ERR_IO;
        }
    }

    if (fflush(fp) != 0) {
        fclose(fp);
        remove(tmp_filepath);
        return CBANK_ERR_IO;
    }

    fclose(fp);

    remove(filepath);
    if (rename(tmp_filepath, filepath) != 0) {
        return CBANK_ERR_IO;
    }

    return CBANK_OK;
}

CbankResult customer_list_load(Node** head, const char* filepath) {
    if (head == nullptr || filepath == nullptr) return CBANK_ERR_NULL;

    FILE* fp = fopen(filepath, "rb");
    if (fp == nullptr) return CBANK_ERR_NOT_FOUND;

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return CBANK_ERR_IO;
    }
    long file_size = ftell(fp);
    if (file_size < (long)(3 * sizeof(uint32_t))) {
        fclose(fp);
        return CBANK_ERR_FORMAT;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return CBANK_ERR_IO;
    }

    uint32_t magic = 0;
    uint32_t version = 0;
    uint32_t count = 0;

    if (fread(&magic, sizeof(uint32_t), 1, fp) != 1 ||
        fread(&version, sizeof(uint32_t), 1, fp) != 1 ||
        fread(&count, sizeof(uint32_t), 1, fp) != 1) {
        fclose(fp);
        return CBANK_ERR_IO;
    }

    if (magic != CBANK_DB_MAGIC || version != CBANK_DB_VERSION || count > 100000) {
        fclose(fp);
        return CBANK_ERR_FORMAT;
    }

    long expected_size = (long)(3 * sizeof(uint32_t) + (size_t)count * sizeof(Customer));
    if (file_size < expected_size) {
        fclose(fp);
        return CBANK_ERR_FORMAT;
    }

    customer_list_free(head);

    for (uint32_t i = 0; i < count; ++i) {
        Customer temp;
        clear_customer(&temp);
        if (fread(&temp, sizeof(Customer), 1, fp) != 1) {
            fclose(fp);
            return CBANK_ERR_IO;
        }
        CbankResult res = customer_list_add(head, &temp);
        if (res != CBANK_OK && res != CBANK_ERR_DUPLICATE) {
            fclose(fp);
            return res;
        }
    }

    fclose(fp);
    return CBANK_OK;
}

