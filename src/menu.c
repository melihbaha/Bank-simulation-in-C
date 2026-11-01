#include "menu.h"
#include "bank.h"
#include "currency.h"
#include "customer.h"
#include "error.h"
#include "input.h"
#include "validate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

static void print_Result(CbankResult res) {
    if (res == CBANK_OK) {
        printf("[BASARILI] %s\n", cbank_error_message(res));
    } else {
        printf("[HATA - %s] %s\n", cbank_errors(res), cbank_error_message(res));
    }
}

static void menu_register_customer(Node** head) {
    if (head == nullptr) return;

    Customer new_cust;
    clear_customer(&new_cust);

    printf("\n================ YENI MUSTERI KAYDI ================\n");

    char buf[256];
    while (true) {
        printf("Ad Soyad: ");
        if (!input_read_string(buf, sizeof(buf))) {
            if (feof(stdin)) return;
            printf("[HATA] Gecersiz giris.\n");
            continue;
        }
        CbankResult res = customer_set_name(&new_cust, buf);
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        printf("T.C. Kimlik No (11 hane): ");
        if (!input_read_string(buf, sizeof(buf))) {
            if (feof(stdin)) return;
            printf("[HATA] Gecersiz giris.\n");
            continue;
        }
        if (customer_list_find_by_tc(*head, buf) != nullptr) {
            print_Result(CBANK_ERR_DUPLICATE);
            continue;
        }
        CbankResult res = customer_set_tc(&new_cust, buf, sizeof(buf));
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        printf("Sifre Belirleyin (en az 4 karakter): ");
        if (!input_read_password(buf, sizeof(buf))) {
            if (feof(stdin)) return;
            printf("[HATA] Gecersiz giris.\n");
            continue;
        }
        CbankResult res = customer_set_password_hashed(&new_cust, buf);
        memset(buf, 0, sizeof(buf));
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        Birthday bday = {0, 0, 0};
        printf("Dogum Gunu (1-31): ");
        if (!input_get_uint8_t(&bday.day)) {
            printf("[HATA] Gecersiz gun.\n");
            continue;
        }
        printf("Dogum Ayi (1-12): ");
        if (!input_get_uint8_t(&bday.month)) {
            printf("[HATA] Gecersiz ay.\n");
            continue;
        }
        printf("Dogum Yili (1900-2026): ");
        if (!input_get_uint16_t(&bday.year)) {
            printf("[HATA] Gecersiz yil.\n");
            continue;
        }
        CbankResult res = customer_set_birthday(&new_cust, bday);
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        Phone phone;
        memset(&phone, 0, sizeof(phone));
        printf("Telefon Ulke/Alan Kodu (Orn: 90): ");
        if (!input_read_string(buf, sizeof(buf))) return;
        strncpy(phone.area_code, buf, sizeof(phone.area_code) - 1);

        printf("Telefon Numarasi (10 hane, Orn: 5551234567): ");
        if (!input_read_string(buf, sizeof(buf))) return;
        strncpy(phone.number, buf, sizeof(phone.number) - 1);

        CbankResult res = customer_set_phone(&new_cust, phone);
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        printf("Hesap Tipi Secin ([1] CHECKING, [2] SAVINGS, [3] INVESTMENT): ");
        uint8_t type_choice = 0;
        if (!input_get_uint8_t(&type_choice)) {
            printf("[HATA] Gecersiz secim.\n");
            continue;
        }
        CbankResult res = customer_set_account_type(&new_cust, (AccountType)type_choice);
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    while (true) {
        printf("Hesap Para Birimi ([0] TRY, [1] USD, [2] EUR, [3] GBP, [4] CHF, [5] JPY): ");
        uint8_t curr_choice = 0;
        if (!input_get_uint8_t(&curr_choice)) {
            printf("[HATA] Gecersiz secim.\n");
            continue;
        }
        CbankResult res = customer_set_currency(&new_cust, (Currency)curr_choice);
        if (res == CBANK_OK) break;
        print_Result(res);
    }

    uint32_t next_id = customer_next_id(*head);
    uint64_t next_acc = customer_next_account_number(*head);
    char generated_iban[CUSTOMER_IBAN_LEN];

    CbankResult res = customer_set_customer_id(&new_cust, next_id);
    if (res == CBANK_OK) res = customer_set_account_number(&new_cust, next_acc);
    if (res == CBANK_OK) res = iban_generate(next_acc, generated_iban, sizeof(generated_iban));
    if (res == CBANK_OK) res = customer_set_iban(&new_cust, generated_iban);
    if (res == CBANK_OK) res = customer_set_balance(&new_cust, 0);
    if (res == CBANK_OK) res = customer_set_debt(&new_cust, 0);
    if (res == CBANK_OK) res = customer_list_add(head, &new_cust);

    if (res == CBANK_OK) {
        customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
        printf("\n[TEBRIKLER] Musteri kaydiniz basariyla olusturuldu!\n");
        list_customer_information(&new_cust);
    } else {
        print_Result(res);
    }
}

void menu_customer_dashboard(Node** head, Customer* current_customer) {
    if (head == nullptr || current_customer == nullptr) return;

    while (true) {
        printf("\n========== %s MUSTERI PANELI (%s) ==========\n", BANK_NAME, current_customer->name);
        printf(" Bakiye: %" PRIu64 " %s | Borc: %" PRIu64 " %s | IBAN: %s\n",
               current_customer->balance,
               currency_name(current_customer->currency),
               current_customer->debt,
               currency_name(current_customer->currency),
               current_customer->iban);
        printf("----------------------------------------------------------\n");
        printf(" [1] Hesap Bilgilerini Goruntule\n");
        printf(" [2] Para Yatir\n");
        printf(" [3] Para Cek\n");
        printf(" [4] IBAN ile Para Transferi (Havale / EFT)\n");
        printf(" [5] Kredi Kullan\n");
        printf(" [6] Borc Ode\n");
        printf(" [7] Hesap Para Birimini Degistir (Kur Donusumu)\n");
        printf(" [8] Hesap Hareketlerini Goruntule\n");
        printf(" [9] Sifre Degistir\n");
        printf(" [10] Hesabi Kapat\n");
        printf(" [0] Oturumu Kapat\n");
        printf("Seciminiz: ");

        uint8_t choice = 0;
        if (!input_get_uint8_t(&choice)) {
            printf("[HATA] Lutfen gecerli bir menu numarasi girin.\n");
            continue;
        }

        if (choice == 0) {
            customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
            printf("Oturum kapatildi.\n");
            break;
        }

        switch (choice) {
            case 1:
                list_customer_information(current_customer);
                break;

            case 2: {
                printf("Yatirilacak Tutar (%s): ", currency_name(current_customer->currency));
                uint64_t amount = 0;
                if (!input_get_uint64_t(&amount)) {
                    printf("[HATA] Gecersiz tutar.\n");
                    break;
                }
                CbankResult res = bank_deposit(current_customer, amount);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 3: {
                printf("Cekilecek Tutar (%s): ", currency_name(current_customer->currency));
                uint64_t amount = 0;
                if (!input_get_uint64_t(&amount)) {
                    printf("[HATA] Gecersiz tutar.\n");
                    break;
                }
                CbankResult res = bank_withdraw(current_customer, amount);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 4: {
                char target_iban[64];
                printf("Alici IBAN (26 karakter, TR...): ");
                if (!input_read_string(target_iban, sizeof(target_iban))) break;

                printf("Gonderilecek Tutar (%s): ", currency_name(current_customer->currency));
                uint64_t amount = 0;
                if (!input_get_uint64_t(&amount)) {
                    printf("[HATA] Gecersiz tutar.\n");
                    break;
                }
                CbankResult res = bank_transfer(*head, current_customer, target_iban, amount);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 5: {
                printf("Kullanilacak Kredi Tutari (%s): ", currency_name(current_customer->currency));
                uint64_t amount = 0;
                if (!input_get_uint64_t(&amount)) {
                    printf("[HATA] Gecersiz tutar.\n");
                    break;
                }
                CbankResult res = bank_take_loan(current_customer, amount);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 6: {
                printf("Odenecek Borc Tutari (Mevcut Borc: %" PRIu64 " %s): ",
                       current_customer->debt, currency_name(current_customer->currency));
                uint64_t amount = 0;
                if (!input_get_uint64_t(&amount)) {
                    printf("[HATA] Gecersiz tutar.\n");
                    break;
                }
                CbankResult res = bank_pay_debt(current_customer, amount);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 7: {
                currency_print_rates();
                printf("Yeni Para Birimi Secin ([0] TRY, [1] USD, [2] EUR, [3] GBP, [4] CHF, [5] JPY): ");
                uint8_t new_curr = 0;
                if (!input_get_uint8_t(&new_curr)) {
                    printf("[HATA] Gecersiz secim.\n");
                    break;
                }
                CbankResult res = bank_exchange_currency(current_customer, (Currency)new_curr);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                break;
            }

            case 8:
                bank_print_customer_transactions(current_customer->customer_id);
                break;

            case 9: {
                char new_pass[CUSTOMER_PASSWORD_LEN];
                printf("Yeni Sifreniz (en az 4 karakter): ");
                if (!input_read_password(new_pass, sizeof(new_pass))) break;
                CbankResult res = customer_set_password_hashed(current_customer, new_pass);
                print_Result(res);
                if (res == CBANK_OK) customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                memset(new_pass, 0, sizeof(new_pass));
                break;
            }

            case 10: {
                char confirm_buf[16];
                printf("DUR: Hesabinizi kalici olarak kapatmak istediginize emin misiniz? (E/H): ");
                if (!input_read_string(confirm_buf, sizeof(confirm_buf))) break;
                if (confirm_buf[0] != 'E' && confirm_buf[0] != 'e') {
                    printf("Hesap kapatma islemi iptal edildi.\n");
                    break;
                }

                uint32_t id = current_customer->customer_id;
                CbankResult res = customer_list_remove_by_id(head, id);
                print_Result(res);
                if (res == CBANK_OK) {
                    customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
                    printf("Hesabiniz basariyla kapatildi. Ana menuye donuluyor.\n");
                    return;
                }
                break;
            }

            default:
                printf("[HATA] Gecersiz secim.\n");
                break;
        }
    }
}

static void menu_admin_panel(Node* head) {
    while (true) {
        printf("\n================ BANKA / YONETICI PANELI ================\n");
        printf(" Toplam Kayitli Musteri Sayisi: %zu\n", customer_list_count(head));
        printf("---------------------------------------------------------\n");
        printf(" [1] Tum Musterileri Listele\n");
        printf(" [2] T.C. Kimlik No ile Musteri Sorgula\n");
        printf(" [3] IBAN ile Musteri Sorgula\n");
        printf(" [4] Guncel Doviz Kurlarini Goruntule\n");
        printf(" [0] Ana Menuye Don\n");
        printf("Seciminiz: ");

        uint8_t choice = 0;
        if (!input_get_uint8_t(&choice)) continue;
        if (choice == 0) break;

        switch (choice) {
            case 1:
                if (head == nullptr) {
                    printf("Kayitli musteri bulunmamaktadir.\n");
                } else {
                    for (const Node* curr = head; curr != nullptr; curr = curr->next) {
                        list_customer_information(&curr->customer);
                    }
                }
                break;

            case 2: {
                char tc[32];
                printf("Aranacak T.C. Kimlik No: ");
                if (!input_read_string(tc, sizeof(tc))) break;
                Customer* c = customer_list_find_by_tc(head, tc);
                if (c != nullptr) {
                    list_customer_information(c);
                } else {
                    print_Result(CBANK_ERR_NOT_FOUND);
                }
                break;
            }

            case 3: {
                char iban[64];
                printf("Aranacak IBAN: ");
                if (!input_read_string(iban, sizeof(iban))) break;
                Customer* c = customer_list_find_by_iban(head, iban);
                if (c != nullptr) {
                    list_customer_information(c);
                } else {
                    print_Result(CBANK_ERR_NOT_FOUND);
                }
                break;
            }

            case 4:
                currency_print_rates();
                break;

            default:
                printf("[HATA] Gecersiz secim.\n");
                break;
        }
    }
}

int menu_run_self_tests(void) {
    printf("\n================ SISTEM BIRIM TESTLERI (SELF-TEST) ================\n");
    int failures = 0;

    // 1. IBAN Generation & MOD-97 Validation Test
    char iban1[CUSTOMER_IBAN_LEN];
    char iban2[CUSTOMER_IBAN_LEN];
    CbankResult r1 = iban_generate(1000000001ULL, iban1, sizeof(iban1));
    CbankResult r2 = iban_generate(1000000002ULL, iban2, sizeof(iban2));
    if (r1 != CBANK_OK || iban_validate(iban1) != CBANK_OK ||
        r2 != CBANK_OK || iban_validate(iban2) != CBANK_OK) {
        printf(" [FAIL] IBAN uretimi ve MOD-97 dogrulama testi.\n");
        failures++;
    } else {
        printf(" [PASS] IBAN uretimi ve MOD-97 dogrulama (%s)\n", iban1);
    }

    // 2. TC Validation Test (10000000146 is a valid algorithmic TR identity number)
    if (tc_validate("10000000146") != CBANK_OK ||
        tc_validate("00000000146") != CBANK_ERR_TC_FIRST_DIGIT ||
        tc_validate("10000000145") != CBANK_ERR_LAST_DIGIT ||
        tc_validate("10000000156") != CBANK_ERR_LAST_TWO_DIGIT) {
        printf(" [FAIL] T.C. Kimlik algoritma dogrulama testi.\n");
        failures++;
    } else {
        printf(" [PASS] T.C. Kimlik algoritma ve hata kodlari testi.\n");
    }

    // 3. Customer Creation, Password Hashing & Linked List Test
    Node* test_head = nullptr;
    Customer c1;
    clear_customer(&c1);
    Birthday b1 = {15, 5, 1995};
    Phone p1 = {"90", "5551234567"};

    if (customer_set_name(&c1, "Ahmet Yilmaz") != CBANK_OK ||
        customer_set_tc(&c1, "10000000146", 12) != CBANK_OK ||
        customer_set_password_hashed(&c1, "GizliSifre123!") != CBANK_OK ||
        customer_set_customer_id(&c1, 1001) != CBANK_OK ||
        customer_set_account_number(&c1, 1000000001ULL) != CBANK_OK ||
        customer_set_iban(&c1, iban1) != CBANK_OK ||
        customer_set_account_type(&c1, CHECKING) != CBANK_OK ||
        customer_set_birthday(&c1, b1) != CBANK_OK ||
        customer_set_phone(&c1, p1) != CBANK_OK ||
        customer_set_currency(&c1, CURRENCY_TRY) != CBANK_OK ||
        customer_list_add(&test_head, &c1) != CBANK_OK) {
        printf(" [FAIL] Musteri olusturma ve listeye ekleme testi.\n");
        failures++;
    } else if (!customer_verify_password(&c1, "GizliSifre123!") ||
               customer_verify_password(&c1, "YanlisSifre")) {
        printf(" [FAIL] Sifre hashleme ve dogrulama testi.\n");
        failures++;
    } else {
        printf(" [PASS] Musteri olusturma, sifre hashleme ve bagli liste testi.\n");
    }

    // Second customer for transfer tests
    Customer c2 = c1;
    customer_set_name(&c2, "Ayse Demir");
    customer_set_tc(&c2, "11111111110", 12);
    customer_set_customer_id(&c2, 1002);
    customer_set_account_number(&c2, 1000000002ULL);
    customer_set_iban(&c2, iban2);
    customer_list_add(&test_head, &c2);

    // 4. Core Banking Operations Test (Deposit, Withdraw, Transfer, Loan, Debt)
    Customer* p_c1 = customer_list_find_by_id(test_head, 1001);
    Customer* p_c2 = customer_list_find_by_id(test_head, 1002);
    if (p_c1 == nullptr || p_c2 == nullptr ||
        bank_deposit(p_c1, 5000) != CBANK_OK ||
        bank_withdraw(p_c1, 1000) != CBANK_OK ||
        bank_transfer(test_head, p_c1, iban2, 1500) != CBANK_OK ||
        p_c1->balance != 2500 || p_c2->balance != 1500 ||
        bank_take_loan(p_c1, 1000) != CBANK_OK ||
        bank_pay_debt(p_c1, 600) != CBANK_OK ||
        p_c1->debt != 400 || p_c1->balance != 2900 ||
        bank_transfer(test_head, p_c1, iban1, 100) != CBANK_ERR_DUPLICATE) {
        printf(" [FAIL] Finansal islemler (Yatirma/Cekme/Transfer/Kredi) testi.\n");
        failures++;
    } else {
        printf(" [PASS] Finansal islemler (Yatirma/Cekme/Transfer/Kredi) testi.\n");
    }

    // 5. Security & Boundary Edge Cases Test
    if (password_validate("123") != CBANK_ERR_LENGTH ||
        name_validate("   ") != CBANK_ERR_EMPTY ||
        name_validate("Ahmet\n123") != CBANK_ERR_FORMAT ||
        currency_convert(UINT64_MAX, CURRENCY_USD, CURRENCY_TRY) != 0) {
        printf(" [FAIL] Guvenlik ve sinir durumlari (Boundary / Overflow) testi.\n");
        failures++;
    } else {
        printf(" [PASS] Guvenlik ve sinir durumlari (Boundary / Overflow / Sanitization) testi.\n");
    }

    // 6. Persistence Save & Load Test
    const char* test_db = "cbank_test_temp.dat";
    if (customer_list_save(test_head, test_db) != CBANK_OK) {
        printf(" [FAIL] Dosya kaydetme testi.\n");
        failures++;
    } else {
        Node* loaded_head = nullptr;
        if (customer_list_load(&loaded_head, test_db) != CBANK_OK ||
            customer_list_count(loaded_head) != 2) {
            printf(" [FAIL] Dosya yukleme testi.\n");
            failures++;
        } else {
            printf(" [PASS] Veri kaliciligi (Save / Load) testi.\n");
        }
        customer_list_free(&loaded_head);
        remove(test_db);
    }

    customer_list_free(&test_head);
    printf("===================================================================\n");
    if (failures == 0) {
        printf(" TUM TESTLER BASARIYLA TAMAMLANDI (0 Hata).\n");
    } else {
        printf(" TOPLAM %d TEST BASARISIZ OLDU!\n", failures);
    }
    return failures;
}

void menu_run(Node** head) {
    if (head == nullptr) return;

    while (true) {
        printf("\n==================================================\n");
        printf("             %s BANKACILIK SIMULASYONU            \n", BANK_NAME);
        printf("==================================================\n");
        printf(" [1] Musteri Girisi Yap\n");
        printf(" [2] Yeni Musteri Ol (Hesap Ac)\n");
        printf(" [3] Banka / Yonetici Paneli\n");
        printf(" [4] Sistem Birim Testlerini Calistir\n");
        printf(" [0] Kaydet ve Cikis\n");
        printf("--------------------------------------------------\n");
        printf("Seciminiz: ");

        uint8_t choice = 0;
        if (!input_get_uint8_t(&choice)) {
            if (feof(stdin)) break;
            printf("[HATA] Lutfen gecerli bir secim girin.\n");
            continue;
        }

        if (choice == 0) {
            customer_list_save(*head, CBANK_DEFAULT_DB_FILE);
            printf("Veriler kaydedildi. Iyi gunler dileriz!\n");
            break;
        }

        switch (choice) {
            case 1: {
                char tc_or_id[32];
                char password[CUSTOMER_PASSWORD_LEN];
                printf("T.C. Kimlik No veya Musteri ID: ");
                if (!input_read_string(tc_or_id, sizeof(tc_or_id))) break;
                printf("Sifre: ");
                if (!input_read_password(password, sizeof(password))) break;

                Customer* logged_in = customer_authenticate(*head, tc_or_id, password);
                memset(password, 0, sizeof(password));
                if (logged_in != nullptr) {
                    printf("\nHos geldiniz, %s!\n", logged_in->name);
                    menu_customer_dashboard(head, logged_in);
                } else {
                    print_Result(CBANK_ERR_AUTH);
                }
                break;
            }

            case 2:
                menu_register_customer(head);
                break;

            case 3:
                menu_admin_panel(*head);
                break;

            case 4:
                menu_run_self_tests();
                break;

            default:
                printf("[HATA] Gecersiz secim.\n");
                break;
        }
    }
}
