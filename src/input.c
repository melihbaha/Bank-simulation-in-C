#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>
#include "input.h"

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#endif

char* input_get_string(void) {
    char buffer[1024];

    if (fgets(buffer, sizeof(buffer), stdin) != nullptr) {
        char* p = strchr(buffer, '\n');
        if (p != nullptr) {
            *p = '\0';
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\r') {
                buffer[len - 1] = '\0';
            }
        } else {
            int c;
            while ((c = getchar()) != EOF && c != '\n');
            return nullptr;
        }

        size_t len = strlen(buffer) + 1;
        char* dynamic_str = malloc(len);
        if (dynamic_str == nullptr) return nullptr;

        memcpy(dynamic_str, buffer, len);
        return dynamic_str;
    }
    return nullptr;
}

bool input_read_string(char* out_buf, size_t buf_size) {
    if (out_buf == nullptr || buf_size == 0) return false;

    char* str = input_get_string();
    if (str == nullptr) {
        out_buf[0] = '\0';
        return false;
    }

    size_t len = strlen(str);
    if (len >= buf_size) {
        free(str);
        out_buf[0] = '\0';
        return false;
    }

    memcpy(out_buf, str, len + 1);
    free(str);
    return true;
}

bool input_read_password(char* out_buf, size_t buf_size) {
    if (out_buf == nullptr || buf_size < 2) return false;

#ifdef _WIN32
    if (_isatty(_fileno(stdin))) {
        size_t idx = 0;
        while (true) {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0) {
                (void)_getch();
                continue;
            }
            if (ch == '\r' || ch == '\n') {
                putchar('\n');
                fflush(stdout);
                break;
            } else if (ch == '\b') {
                if (idx > 0) {
                    idx--;
                    out_buf[idx] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (ch == 3) { // Ctrl+C
                putchar('\n');
                out_buf[0] = '\0';
                return false;
            } else if (ch >= 32 && ch <= 126) {
                if (idx + 1 < buf_size) {
                    out_buf[idx++] = (char)ch;
                    putchar('*');
                    fflush(stdout);
                }
            }
        }
        out_buf[idx] = '\0';
        return true;
    }
#endif

    return input_read_string(out_buf, buf_size);
}



// ============================================================================
// DÜZELTME: Bütün parametrelerin tipleri eksiksiz tanımlandı
// ============================================================================
static bool parse_integer(bool is_signed, uint64_t min_limit, uint64_t max_limit, uint64_t* out_val) {
    if (out_val == nullptr) return false;

    char* str = input_get_string();
    if (str == nullptr) return false;

    char* start = str;
    while (*start == ' ' || *start == '\t') start++;

    // İşaretsiz (unsigned) tipler için eksi işareti denetimi
    if (!is_signed && *start == '-') {
        free(str);
        return false;
    }

    char* endptr = nullptr;
    errno = 0;

    if (is_signed) {
        intmax_t val = strtoimax(str, &endptr, 10);
        if (endptr == str || *endptr != '\0' || errno == ERANGE || 
            val < (intmax_t)min_limit || val > (intmax_t)max_limit) {
            free(str);
            return false;
        }
        *out_val = (uint64_t)val;
    } else {
        uintmax_t val = strtoumax(str, &endptr, 10);
        if (endptr == str || *endptr != '\0' || errno == ERANGE || 
            val > (uintmax_t)max_limit) {
            free(str);
            return false;
        }
        *out_val = (uint64_t)val;
    }

    free(str);
    return true;
}

// ============================================================================
// Makro Tanımları (Function Generation)
// ============================================================================

#define DEFINE_INPUT_GET_UNSIGNED(type, max_val)            \
bool input_get_##type(type* out_val) {                      \
    uint64_t raw_val = 0;                                   \
    if (!parse_integer(false, 0, max_val, &raw_val)) {      \
        return false;                                       \
    }                                                       \
    *out_val = (type)raw_val;                               \
    return true;                                            \
}

#define DEFINE_INPUT_GET_SIGNED(type, min_val, max_val)     \
bool input_get_##type(type* out_val) {                      \
    uint64_t raw_val = 0;                                   \
    if (!parse_integer(true, (uint64_t)(min_val), (uint64_t)(max_val), &raw_val)) { \
        return false;                                       \
    }                                                       \
    *out_val = (type)raw_val;                               \
    return true;                                            \
}

// --- Fonksiyon Gövdelerinin Üretilmesi ---
DEFINE_INPUT_GET_UNSIGNED(uint8_t,  UINT8_MAX)
DEFINE_INPUT_GET_UNSIGNED(uint16_t, UINT16_MAX)
DEFINE_INPUT_GET_UNSIGNED(uint32_t, UINT32_MAX)
DEFINE_INPUT_GET_UNSIGNED(uint64_t, UINT64_MAX)

DEFINE_INPUT_GET_SIGNED(int8_t,  INT8_MIN,  INT8_MAX)
DEFINE_INPUT_GET_SIGNED(int16_t, INT16_MIN, INT16_MAX)
DEFINE_INPUT_GET_SIGNED(int32_t, INT32_MIN, INT32_MAX)
DEFINE_INPUT_GET_SIGNED(int64_t, INT64_MIN, INT64_MAX)