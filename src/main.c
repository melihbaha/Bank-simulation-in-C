#include "bank.h"
#include "customer.h"
#include "menu.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        return menu_run_self_tests();
    }

    Node* customer_list = nullptr;
    customer_list_load(&customer_list, CBANK_DEFAULT_DB_FILE);

    menu_run(&customer_list);

    customer_list_save(customer_list, CBANK_DEFAULT_DB_FILE);
    customer_list_free(&customer_list);

    return 0;
}
