# Arduino Snake Game

**Arduino Uno** üzerinde çalışan, 8×8 LED dot-matrix ve 16×2 LCD kullanarak oynanan klasik yılan oyunu.

---

## İçindekiler

- [Açıklama](#açıklama)  
- [Özellikler](#özellikler)  
- [Donanım Gereksinimleri](#donanım-gereksinimleri)  
- [Bağlantı Şeması](#bağlantı-şeması)  
- [Yazılım Gereksinimleri](#yazılım-gereksinimleri)  
- [Kurulum & Yükleme](#kurulum--yükleme)  
- [Nasıl Oynanır?](#nasıl-oynanır)  
- [Konfigürasyon](#konfigürasyon)  
- [Lisans](#lisans)  

---

## Açıklama

Bu proje; bir Arduino Uno, bir 8×8 dot-matrix (MAX7219), 16×2 LCD ekran ve beş adet buton kullanarak yılan oyununu hayata geçirir.  
Oyunda üç farklı kullanıcı profili (K1, K2, K3), üç zorluk seviyesi (Kolay, Orta, Zor) ve kullanıcı başına kayıtlı yüksek skor (HS) saklama/​resetleme imkânı vardır.

---

## Özellikler

- **3 kullanıcı profili** (EEPROM’da rekor kaydı)  
- **3 zorluk seviyesi** (hız ayarı)  
- **Skor göstergesi**: LCD üzerinde anlık skor, dot-matrix üzerinde yılan ve elma  
- **High-Score Reset**: Sağ tuşa basınca “Emin misiniz?” onayıyla HS sıfırlama  
- **Kullanıcı dostu menü**: Yukarı/Aşağı/Select/Left/Right tuşlarıyla gezinme  

---

## Donanım Gereksinimleri

- Arduino Uno (veya uyumlu bir board)  
- 8×8 LED Dot-Matrix + MAX7219 sürücü modülü  
- 16×2 karakter LCD (HD44780 tabanlı)  
- 5 adet buton (sağ, sol, yukarı, aşağı, select)  
- Bağlantı kabloları ve breadboard / lehimleme malzemesi  

---

## Bağlantı Şeması

| Bileşen             | Arduino Pin  |
|---------------------|--------------|
| **LCD**             |              |
|   RS                | 8            |
|   E                 | 9            |
|   D4                | 4            |
|   D5                | 5            |
|   D6                | 6            |
|   D7                | 7            |
| **Dot-Matrix (MAX7219)** |        |
|   DIN               | 11           |
|   CLK               | 13           |
|   CS                | 12           |
| **Butonlar**        |              |
|   Sağ (Right)       | A0 (okuma)   |
|   Yukarı (Up)       | A0 (okuma)   |
|   Aşağı (Down)      | A0 (okuma)   |
|   Sol (Left)        | A0 (okuma)   |
|   Select            | A0 (okuma)   |

> **Not:** Tüm butonlar analog A0’a bağlanarak gerilim bölücü yöntemiyle okunur.

---

## Yazılım Gereksinimleri

- Arduino IDE (en az 1.8.x)  
- Aşağıdaki kütüphaneler yüklü olmalı:  
  - `LiquidCrystal`  
  - `LedControl`  
  - `EEPROM` (standart Arduino kütüphanesi)  

---

## Kurulum & Yükleme

1. GitHub’dan projeyi klonla veya ZIP olarak indir:  
   ```bash
   git clone https://github.com/enscnn/ArduinoSnakeGame.git
