# C Bank — Banking Simulator in C (C23)

**C Bank (`cbank`)**, modern **ISO C23** standardı ile geliştirilmiş, modüler mimariye sahip bir konsol bankacılık simülasyonudur.

---

## Özellikler

- **ISO C23 Uyumlu Mimari:** `nullptr`, `bool`, sabit genişlikli tamsayılar (`uint64_t`, `uint32_t`, `PRIu64`) ve sıfır derleyici uyarısı (`-Wall -Wextra -Wpedantic -Werror`).
- **Gerçek Algoritmik Doğrulamalar ([`validate.c`](src/validate.c)):**
  - **TR IBAN Doğrulama & Üretim:** ISO 7064 **MOD-97** algoritması ile 26 haneli TR IBAN doğrulaması (`iban_validate`) ve otomatik IBAN üretimi (`iban_generate`).
  - **T.C. Kimlik No Doğrulaması:** 10. ve 11. hane kontrol basamaklarını denetleyen resmi algoritma (`tc_validate`).
  - **Tarih, Telefon, İsim ve Hesap Kontrolleri:** Artık yıl (leap year) destekli doğum tarihi, alan kodu ve telefon numarası doğrulamaları.
- **Güvenlik ve Kimlik Doğrulama ([`customer.c`](src/customer.c), [`input.c`](src/input.c)):**
  - **Maskeli Şifre Girişi:** Konsolda şifre yazılırken karakterler ekranda açık görünmez, `*` ile maskelenir (`_getch`).
  - **128-bit FNV-1a + Salt:** Şifreler düz metin yerine dual-lane hash (`$CBH$...`) ile saklanır.
  - **Zamanlama Saldırısı Koruması (Constant-time Comparison):** Şifre karşılaştırmalarında `secure_str_equals` ile yan kanal (side-channel timing) saldırıları engellenir.
  - **Güvenli Bellek Temizleme (Memory Scrubbing):** Bellekten silinen düğümler ve şifre geçici tamponları `secure_zero_memory` ile sıfırlanarak RAM sızıntıları önlenir.
  - **Aritmetik Taşma ve Enjeksiyon Koruması:** Döviz dönüşümlerinde `UINT64_MAX` taşma denetimi ve işlem geçmişinde log injection sanitizasyonu yapılır.
  - **Atomik Dosya Kaydı:** Veritabanı yazılırken geçici dosyaya (`.tmp`) yazılıp atomik olarak değiştirilir; elektrik kesintisi veya ani kapanmalarda veritabanı bozulmaz.
- **Çekirdek Bankacılık İşlemleri ([`bank.c`](src/bank.c)):**
  - Para Yatırma (`bank_deposit`) ve Para Çekme (`bank_withdraw`)
  - IBAN ile Havale / EFT (`bank_transfer` — farklı döviz cinsine sahip hesaplar arasında otomatik kur dönüşümü destekli)
  - Kredi Kullanımı (`bank_take_loan`) ve Borç Ödeme (`bank_pay_debt`)
  - Çoklu Para Birimi (`TRY`, `USD`, `EUR`, `GBP`, `CHF`, `JPY`) ve Kur Dönüşümü (`bank_exchange_currency`)
  - Hesap Hareketleri / İşlem Geçmişi Kaydı (`cbank_transactions.log`)
- **Bağlı Liste ve Veri Kalıcılığı ([`customer.c`](src/customer.c)):**
  - Tek yönlü bağlı liste (`Node`) üzerinde ekleme, silme ve T.C./IBAN/ID/Hesap No bazlı arama.
  - İkili (binary) veritabanı dosyası (`cbank_customers.dat`) ile oturumlar arası otomatik veri kaydetme ve yükleme.

---

## Proje Dizin Yapısı

```text
Bank-simulation-in-C/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── bank.h        # Banka sabitleri, finansal işlemler ve işlem geçmişi
│   ├── currency.h    # Döviz birimleri ve kur dönüşüm fonksiyonları
│   ├── customer.h    # Müşteri modeli, bağlı liste ve kalıcılık bildirimleri
│   ├── error.h       # Hata kodları (CbankResult)
│   ├── input.h       # Güvenli konsol girdi okuma fonksiyonları
│   ├── menu.h        # Kullanıcı ve yönetici menü arayüzleri
│   └── validate.h    # IBAN (MOD-97), T.C. Kimlik ve alan doğrulamaları
└── src/
    ├── bank.c
    ├── currency.c
    ├── customer.c
    ├── error.c
    ├── input.c
    ├── main.c
    ├── menu.c
    └── validate.c
```

---

## Derleme ve Çalıştırma

### GCC ile Doğrudan Derleme
```bash
gcc -std=c23 -Wall -Wextra -Wpedantic -Werror -Iinclude src/*.c -o cbank
```

### CMake ile Derleme
```bash
cmake -B build
cmake --build build
```

### Çalıştırma
- **Etkileşimli Banka Simülasyonu:**
  ```bash
  ./cbank
  ```
- **Otomatik Birim Testleri (Self-Test):**
  ```bash
  ./cbank --test
  ```
