#include "error.h"

static const char* const CBANK_ERRORS[] = {
    [CBANK_OK]                       = "CBANK_OK",
    [CBANK_ERR_NULL]                 = "CBANK_ERR_NULL",
    [CBANK_ERR_EMPTY]                = "CBANK_ERR_EMPTY",
    [CBANK_ERR_LENGTH]               = "CBANK_ERR_LENGTH",
    [CBANK_ERR_FORMAT]               = "CBANK_ERR_FORMAT",
    [CBANK_ERR_DUPLICATE]            = "CBANK_ERR_DUPLICATE",
    [CBANK_ERR_NOT_FOUND]            = "CBANK_ERR_NOT_FOUND",
    [CBANK_ERR_FULL]                 = "CBANK_ERR_FULL",
    [CBANK_ERR_BANK_COUNTRY]         = "CBANK_ERR_BANK_COUNTRY",
    [CBANK_ERR_BANK_CODE]            = "CBANK_ERR_BANK_CODE",
    [CBANK_ERR_NAME]                 = "CBANK_ERR_NAME",
    [CBANK_ERR_TC]                   = "CBANK_ERR_TC",
    [CBANK_ERR_TC_FIRST_DIGIT]       = "CBANK_ERR_TC_FIRST_DIGIT",
    [CBANK_ERR_LAST_DIGIT]           = "CBANK_ERR_LAST_DIGIT",
    [CBANK_ERR_LAST_TWO_DIGIT]       = "CBANK_ERR_LAST_TWO_DIGIT",
    [CBANK_ERR_RESERVE_DIGIT]        = "CBANK_ERR_RESERVE_DIGIT",
    [CBANK_ERR_INSUFFICIENT_BALANCE] = "CBANK_ERR_INSUFFICIENT_BALANCE",
    [CBANK_ERR_INVALID_AMOUNT]       = "CBANK_ERR_INVALID_AMOUNT",
    [CBANK_ERR_AUTH]                 = "CBANK_ERR_AUTH",
    [CBANK_ERR_HAS_BALANCE_OR_DEBT]  = "CBANK_ERR_HAS_BALANCE_OR_DEBT",
    [CBANK_ERR_IO]                   = "CBANK_ERR_IO",
    [CBANK_ERR_ALLOC]                = "CBANK_ERR_ALLOC",
};

static const char* const CBANK_ERROR_MESSAGES[] = {
    [CBANK_OK]                       = "Islem basarili.",
    [CBANK_ERR_NULL]                 = "Gecersiz (NULL) referans.",
    [CBANK_ERR_EMPTY]                = "Alan bos birakilamaz.",
    [CBANK_ERR_LENGTH]               = "Uzunluk siniri gecersiz.",
    [CBANK_ERR_FORMAT]               = "Format gecersiz.",
    [CBANK_ERR_DUPLICATE]            = "Kayit zaten mevcut.",
    [CBANK_ERR_NOT_FOUND]            = "Kayit bulunamadi.",
    [CBANK_ERR_FULL]                 = "Kapasite dolu.",
    [CBANK_ERR_BANK_COUNTRY]         = "IBAN ulke kodu gecersiz (TR olmali).",
    [CBANK_ERR_BANK_CODE]            = "IBAN banka kodu gecersiz (99001 olmali).",
    [CBANK_ERR_NAME]                 = "Isim rakam veya gecersiz karakter iceremez.",
    [CBANK_ERR_TC]                   = "T.C. Kimlik No sadece rakamlardan olusmalidir.",
    [CBANK_ERR_TC_FIRST_DIGIT]       = "T.C. Kimlik No ilk hanesi 0 olamaz.",
    [CBANK_ERR_LAST_DIGIT]           = "T.C. Kimlik No son hanesi cift sayi olmalidir.",
    [CBANK_ERR_LAST_TWO_DIGIT]       = "T.C. Kimlik No algoritma dogrulamasi basarisiz.",
    [CBANK_ERR_RESERVE_DIGIT]        = "IBAN rezerv hanesi gecersiz ('0' olmali).",
    [CBANK_ERR_INSUFFICIENT_BALANCE] = "Yetersiz bakiye.",
    [CBANK_ERR_INVALID_AMOUNT]       = "Gecersiz islem tutari.",
    [CBANK_ERR_AUTH]                 = "Kimlik dogrulama hatasi (TC/ID veya sifre yanlis).",
    [CBANK_ERR_HAS_BALANCE_OR_DEBT]  = "Hesapta bakiye veya borc varken hesap kapatilamaz.",
    [CBANK_ERR_IO]                   = "Dosya okuma/yazma hatasi.",
    [CBANK_ERR_ALLOC]                = "Bellek tahsis hatasi.",
};

const char* cbank_errors(CbankResult error) {
    if ((int)error < (int)CBANK_OK || error >= CBANK_ERR_COUNT || CBANK_ERRORS[error] == nullptr) {
        return "Error number unspecified";
    }

    return CBANK_ERRORS[error];
}

const char* cbank_error_message(CbankResult error) {
    if ((int)error < (int)CBANK_OK || error >= CBANK_ERR_COUNT || CBANK_ERROR_MESSAGES[error] == nullptr) {
        return "Bilinmeyen hata.";
    }

    return CBANK_ERROR_MESSAGES[error];
}


