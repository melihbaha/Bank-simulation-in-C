#pragma once

#include <stdint.h>
#include <stddef.h>

char* input_get_string(void);
bool input_read_string(char* out_buf, size_t buf_size);
bool input_read_password(char* out_buf, size_t buf_size);
bool input_get_uint8_t(uint8_t* out_val);
bool input_get_uint16_t(uint16_t* out_val);
bool input_get_uint32_t(uint32_t* out_val);
bool input_get_uint64_t(uint64_t* out_val);
bool input_get_int8_t(int8_t* out_val);
bool input_get_int16_t(int16_t* out_val);
bool input_get_int32_t(int32_t* out_val);
bool input_get_int64_t(int64_t* out_val);
