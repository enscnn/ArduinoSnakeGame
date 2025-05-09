# Arduino Snake Game

**Arduino Uno** üzerinde çalışan, **LCD Keypad Shield** ve 8×8 LED dot-matrix (MAX7219) kullanarak oynanan klasik yılan oyunu.

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
- [İletişim](#iletişim)  

---

## Açıklama

Bu proje; bir Arduino Uno ve üzerine takılan LCD Keypad Shield ile bir 8×8 dot-matrix (MAX7219) modülünü kullanarak klasik yılan oyununu hayata geçirir.  
Oyunda üç farklı kullanıcı profili (K1, K2, K3), üç zorluk seviyesi (Kolay, Orta, Zor) ve her kullanıcıya ait yüksek skor (HS) saklama/​resetleme imkânı vardır.

---

## Özellikler

- **3 kullanıcı profili** (EEPROM’da rekor kaydı)  
- **3 zorluk seviyesi** (hız ayarı)  
- **Skor göstergesi**: LCD’de anlık skor, dot-matrix’te yılan ve elma  
- **High-Score Reset**: SAĞ tuşa basınca “Emin misiniz?” onayıyla HS sıfırlama  
- **Kullanıcı dostu menü**: UP/DOWN/SELECT/LEFT/RIGHT tuşlarıyla gezinme  

---

## Donanım Gereksinimleri

- **Arduino Uno** (veya uyumlu bir kart)  
- **LCD Keypad Shield**  
  - Entegre 16×2 karakter LCD (HD44780 tabanlı)  
  - 5 yön tuşu (SELECT, LEFT, RIGHT, UP, DOWN)  
  - Butonlar A0 üzerinden analog olarak okunur  
- **8×8 LED Dot-Matrix** + **MAX7219** sürücü modülü  
- Bağlantı kabloları  

---

## Bağlantı Şeması

| Bileşen                  | Arduino Pin                                  |
|--------------------------|-----------------------------------------------|
| **LCD Keypad Shield**    | Shield doğrudan Arduino’ya takılır:           |
| &nbsp;&nbsp;RS           | D8                                            |
| &nbsp;&nbsp;E            | D9                                            |
| &nbsp;&nbsp;D4           | D4                                            |
| &nbsp;&nbsp;D5           | D5                                            |
| &nbsp;&nbsp;D6           | D6                                            |
| &nbsp;&nbsp;D7           | D7                                            |
| &nbsp;&nbsp;Butonlar     | A0 (analog okuma, `tus_oku()` fonksiyonunda)  |
| **Dot-Matrix (MAX7219)** | DIN → D11 │ CLK → D13 │ CS → D12              |

> **Not:** LCD Keypad Shield üzerindeki tüm tuşlar, shield’ın A0 hattına bağlanan bir gerilim bölücü ağ ile analog okunarak algılanır.

---

## Yazılım Gereksinimleri

- **Arduino IDE** (≥1.8.x)  
- Aşağıdaki kütüphaneler yüklü olmalı:  
  - `LiquidCrystal`  
  - `LedControl`  
  - `EEPROM` (Arduino’nun standart kütüphanesi)  

---

## Kurulum & Yükleme

1. Depoyu klonlayın veya ZIP olarak indirin:  
   ```bash
   git clone https://github.com/YourUser/ArduinoSnakeGame.git
