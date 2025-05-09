// Yılan Oyunu - Arduino ile LCD ve 8x8 Dot Matrix
// --------------------------------------------------
// Bu program, üç farklı kullanıcı profili (K1, K2, K3) ve üç zorluk seviyesi (Kolay, Orta, Zor) sunar.
// - Kullanıcı seçme ekranında üst/alt tuşlarıyla profil seçilir.
// - SELECT tuşuna basınca profil onaylanır.
// - SAĞ tuşa basınca seçili kullanıcının high score’u (HS) sıfırlamak için “Emin misiniz?” sorulur.
//   • SAĞ tuş = Evet → HS sıfırlanır.
//   • SOL tuş = Hayır → işlem iptal edilir.
// Zorluk seçme ekranında benzer tuşlar kullanılır ve seçime göre yılan hızı ayarlanır.
// Oyun sırasında LCD’de skor, dot matrix’te yılan ve elma gösterilir.
// Oyun bittiğinde skor ve HS ekranda görünür, tekrar başlatmak için SELECT tuşuna basılır.

#include <LiquidCrystal.h>   // LCD kontrol kütüphanesi
#include <LedControl.h>      // 8x8 dot matrix kontrol kütüphanesi
#include <EEPROM.h>          // EEPROM’dan veri oku/yaz için kütüphane

// ---------- Donanım Tanımlamaları ----------
// LCD pin bağlantıları: RS=8, E=9, D4=4, D5=5, D6=6, D7=7
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Dot matrix bağlantıları: DIN=11, CLK=13, CS=12, 1 cihaz portu
LedControl lc(11, 13, 12, 1);

// Analog tuş okuma eşiklerine karşılık gelen sabit tanımlar
#define sag   0   // Sağa git / HS sıfırla onayı
#define ust   1   // Yukarı git / menüde yukarı
#define alt   2   // Aşağı git / menüde aşağı
#define sol   3   // Sola git / Hayır onayı
#define secim 4   // Seçim onayı
#define yok   5   // Hiçbir tuşa basılmadı

// ---------- Küresel Oyun Değişkenleri ----------
// Yılan ve elma koordinat dizileri (maksimum yılan uzunluğu 20)
byte Col[20], Row[20];

// Yılan uzunluğu ve skor
int uzunluk      = 3;     // Başlangıçta 3 blok
int skor         = 0;     // Anlık skor

// Hareket yön vektörleri (satır ve sütun değişimi)
int move_r       = 1;     // Başlangıçta sağa hareket
int move_c       = 0;

// Elmanın yanıp-sönme durumu ve oyunun aktifliği
bool elmaGorunur = true;  
bool oyunAktif   = false; 

// ---------- Zorluk Ayarları ----------
// 0 = Kolay, 1 = Orta, 2 = Zor
int zorluk                   = 1;              // Varsayılan Orta
int hizlar[]                 = {400, 300, 150}; // ms cinsinden hareket aralıkları
unsigned long hareketGecikmesi = hizlar[zorluk];
unsigned long oncekiZaman       = 0;             // Son hareket zaman damgası

// Elmanın yanıp-sönme kontrolü
unsigned long elmaYanipSonmeZaman = 0;
const int yanipSonmeHizi          = 400;         // Yanıp-sönme aralığı (ms)

// ---------- Kullanıcı Profili Ayarları ----------
// 3 farklı kullanıcı: K1, K2, K3
int aktifKullanici = 0;                   
const char* kullanicilar[] = {"K1","K2","K3"};

// ---------- LCD İçin Özel Karakter (ok imleci) ----------
byte okKarakteri[] = {
  B00100,
  B01110,
  B10101,
  B00100,
  B00100,
  B10101,
  B01110,
  B00100
};

// ---------- Fonksiyon Prototipleri ----------
int  tus_oku();
void tus_kontrol();
void matrix_temizle();
bool elma_yilanda_varmi();
void yeni_elma_uret();
void yilan_goster();
void elma_goster(bool);
void skor_goster();
bool confirm_reset();
void kullanici_sec();
void zorluk_sec();
void oyun_baslat();

// ---------- Buton Okuma: analog girişten tuş değeri dönüşü ----------
// A0 pininden analog okumaya göre 6 farklı geri dönüş
int tus_oku() {
  int v = analogRead(A0);
  if (v > 1000) return yok;
  if (v <   50) return sag;
  if (v <  195) return ust;
  if (v <  380) return alt;
  if (v <  555) return sol;
  if (v <  790) return secim;
  return yok;
}

// ---------- Hareket Buton Kontrolü ----------
// Debounce ve ters yönde dönmeyi engelleyen mantık
void tus_kontrol() {
  static int onceki = yok;
  int t = tus_oku();
  if (t != yok && t != onceki) {
    switch (t) {
      case sag: if (move_r !=  1) { move_r = -1; move_c = 0; } break;
      case ust: if (move_c !=  1) { move_c = -1; move_r = 0; } break;
      case alt: if (move_c != -1) { move_c =  1; move_r = 0; } break;
      case sol: if (move_r != -1) { move_r =  1; move_c = 0; } break;
    }
    onceki = t;
  } else if (t == yok) {
    onceki = yok;
  }
}

// ---------- Dot Matrix Temizle ----------
// 8 satırı da sıfır vererek tüm LED’leri kapatır
void matrix_temizle() {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, 0);
  }
}

// ---------- Elma-Yılan Çakışma Kontrolü ----------
// Elmanın mevcut yılan hücresinde olup olmadığını kontrol eder
bool elma_yilanda_varmi() {
  for (int i = 1; i < uzunluk; i++) {
    if (Row[i] == Row[0] && Col[i] == Col[0]) return true;
  }
  return false;
}

// ---------- Yeni Elma Üret ----------
// Elma pozisyonu boş bir hücreye rastgele atanır
void yeni_elma_uret() {
  do {
    Row[0] = random(0, 8);
    Col[0] = random(0, 8);
  } while (elma_yilanda_varmi());
}

// ---------- Yılanı Göster ----------
// Kuyruğu matrix üzerinde çizer
void yilan_goster() {
  matrix_temizle();
  for (int i = 1; i < uzunluk; i++) {
    int y = 7 - Row[i];
    int x = 7 - Col[i];
    lc.setLed(0, y, x, true);
  }
}

// ---------- Elmayı Göster/Gizle ----------
// Yanıp-sönme efekti için parametreyle kontrol
void elma_goster(bool g) {
  int y = 7 - Row[0];
  int x = 7 - Col[0];
  lc.setLed(0, y, x, g);
}

// ---------- Skor Göster ----------
// LCD’nin belirli konumuna “SKOR” ve sayıyı basar
void skor_goster() {
  lcd.setCursor(5, 0);
  lcd.print("SKOR");
  lcd.setCursor(7, 1);
  if (skor < 10) lcd.print(" ");
  lcd.print(skor);
}

// ---------- Reset Onay Fonksiyonu ----------
// “Emin misiniz?” sorusunu gösterir ve sola/sağa göre true/false döner
bool confirm_reset() {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Emin misiniz?");
  lcd.setCursor(0, 1); lcd.print("Hayir   Evet");
  // Mevcut buton bırakılana kadar bekle
  while (tus_oku() != yok) delay(10);
  // Sola veya sağa basılana kadar döngü
  while (true) {
    int c = tus_oku();
    if (c == sag) {
      while (tus_oku() != yok) delay(10);
      return true;   // Evet
    } else if (c == sol) {
      while (tus_oku() != yok) delay(10);
      return false;  // Hayır
    }
  }
}

// ---------- Kullanıcı Seçme Menüsü ----------
// • Üst/alt ile K1–K3 arasında gezinme
// • SELECT tuşu → profil onayı
// • SAĞ tuşu → önce confirm_reset(), Evet ise HS sıfırlama
void kullanici_sec() {
  int idx = 0, prev = -1;
  lcd.createChar(0, okKarakteri);
  while (true) {
    if (idx != prev) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Kullanici Sec");
      lcd.setCursor(15, 0); lcd.write(byte(0)); // ok imleci
      lcd.setCursor(0, 1);
      lcd.print(kullanicilar[idx]);
      lcd.print(" HS:");
      lcd.print(EEPROM.read(idx));
      prev = idx;
    }
    int t = tus_oku();
    if (t == ust) {
      idx = (idx + 2) % 3; delay(200);
    }
    else if (t == alt) {
      idx = (idx + 1) % 3; delay(200);
    }
    else if (t == secim) {
      aktifKullanici = idx; delay(300); break;
    }
    else if (t == sag) {
      if (confirm_reset()) {
        // HS sıfırlama
        EEPROM.write(idx, 0);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(kullanicilar[idx]);
        lcd.print(" HS Reset!");
        delay(1000);
      }
      prev = -1; // Menü yeniden çizilsin
    }
    delay(50);
  }
}

// ---------- Zorluk Seçme Menüsü ----------
// • Üst/alt ile Kolay/Orta/Zor gezinti
// • SELECT tuşu → hız ayarını belirler
void zorluk_sec() {
  const char* sev[] = {"KOLAY","ORTA ","ZOR  "};
  int prev = -1;
  while (true) {
    if (zorluk != prev) {
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("Zorluk Seciniz");
      lcd.setCursor(15, 0); lcd.write(byte(0));
      lcd.setCursor(5, 1); lcd.print(sev[zorluk]);
      prev = zorluk;
    }
    int t = tus_oku();
    if (t == ust)      { zorluk = (zorluk + 2) % 3; delay(200); }
    else if (t == alt) { zorluk = (zorluk + 1) % 3; delay(200); }
    else if (t == secim) {
      hareketGecikmesi = hizlar[zorluk];
      delay(200);
      break;
    }
  }
}

// ---------- Oyun Başlat ----------
// Skoru ve uzunluğu sıfırlar, başlangıç pozisyonunu atar, elma üretir
void oyun_baslat() {
  matrix_temizle();
  lcd.clear();
  skor     = 0;
  uzunluk  = 3;
  move_r   = 1;
  move_c   = 0;
  Row[1]   = 1; Col[1] = 1;
  Row[2]   = 2; Col[2] = 1;
  yeni_elma_uret();
  skor_goster();
  oyunAktif = true;
}

// ---------- Arduino Başlangıç Fonksiyonu ----------
void setup() {
  // LCD ve dot matrix başlatma
  lcd.begin(16, 2);
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  randomSeed(analogRead(A1));

  // Menü akışları
  kullanici_sec();
  zorluk_sec();
  oyun_baslat();
}

// ---------- Ana Döngü (loop) ----------
void loop() {
  if (!oyunAktif) return;   // Oyun aktif değilse çık

  tus_kontrol();            // Kullanıcı girdisi al
  unsigned long now = millis();

  // Elma yanıp-sönme kontrolü
  if (now - elmaYanipSonmeZaman >= yanipSonmeHizi) {
    elmaGorunur = !elmaGorunur;
    elmaYanipSonmeZaman = now;
  }

  // Hareket için zaman kontrolü
  if (now - oncekiZaman >= hareketGecikmesi) {
    oncekiZaman = now;

    // Kuyruk kaydırma
    for (int i = uzunluk; i > 1; i--) {
      Row[i] = Row[i - 1];
      Col[i] = Col[i - 1];
    }
    // Yeni baş konumunu uygula
    Row[1] = Row[2] + move_r;
    Col[1] = Col[2] + move_c;

    // Duvara çarptıysa oyun biter
    if (Row[1] < 0 || Row[1] > 7 || Col[1] < 0 || Col[1] > 7) {
      oyunAktif = false;
      int rekor = EEPROM.read(aktifKullanici);
      bool yeni  = (skor > rekor || rekor == 255);
      if (yeni) {
        EEPROM.write(aktifKullanici, skor);
        rekor = skor;
        // Yüksek skor bildirimi
        lcd.clear();
        lcd.setCursor(2, 0); lcd.print("TEBRIKLER");
        lcd.setCursor(1, 1); lcd.print("YUKSEK SKOR");
        delay(2000);
      }
      // Oyun sonu ekranı
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("SON SKOR:"); lcd.print(skor);
      lcd.setCursor(0, 1); lcd.print("HS:"); lcd.print(rekor);
      delay(2000);
      // Yeniden başlatma onayı
      lcd.clear();
      lcd.setCursor(0, 1); lcd.print("SEC tusuna bas");
      while (tus_oku() != secim);
      delay(300);
      // Menüye geri dön
      kullanici_sec();
      zorluk_sec();
      oyun_baslat();
      return;
    }

    // Elmayı yedi mi?
    if (Row[1] == Row[0] && Col[1] == Col[0]) {
      uzunluk++;
      skor += 5;
      skor_goster();
      yeni_elma_uret();
    }

    yilan_goster();  // Yılanı ekrana çiz
  }

  elma_goster(elmaGorunur);  // Elmayı göster/gizle
}
