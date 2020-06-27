#include <MCUFRIEND_kbv.h>   // Hardware-specific library
#include <EEPROM.h>
#include <Keypad.h>
#include "src/images/vault_logo.h"  // картинка - лого Волт-Тек
#include "src/images/vault_logo_upset.h"
#include "src/images/vault_logo_angry.h"
#include "src/images/vault_logo_thumbs_up.h"
#include "src/Password/Password.h"

// Настройки еепром
#define EEPROM_INIT_ADDR 1023  // Адрес ячейки еепром, чтобы понять, сохранено ли у нас что-нибудь в еепром
#define EEPROM_INIT_KEY 128
#define EEPROM_PASS_ADDR 0  // Адрес ячейки еепром для пароля

// Настройки дисплея
#define BLACK      0x0000
#define WHITE      0xFFFF
#define GREEN      0x07E0
#define MAIN_GREEN 0x0560  // #00ae00 Грязновато-зелёный цвет, чтоб было не так ярко
#define DISPLAY_ROTATION 3  // От 1 до 4
MCUFRIEND_kbv tft;  // Создаём дисплей

// Размеры текста
#define TEXTSIZE 2 // Размер буквы
#define TEXT_W 11 // Ширина буквы
#define TEXT_H 10 // Высота буквы

// Клавиатура
#define ROWS 4 // количество строк в матрице клавиатуры
#define COLS 4 // количество столбцов
char keys[ROWS][COLS] = { // таблица соответствия кнопок символам
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // пины подключенных строк
byte colPins[COLS] = {5, 4, 3, 2}; // пины подключенных столбцов
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); // создаем объект клавиатуры для работы с ней

// Пины
#define PIN_SUCCESS 13

// Настройки игры
#define MAX_TRIES 3 // Количество попыток
#define BLOCK_TIMEOUT 1800 // Время устройства в отключке
Password secret = Password("11111111"); // Дефолтный пароль

// Возможные состояния автомата
enum States
{
  PLAY,
  ADMIN,
  SUCCESS,
  BLOCK,
};

// Текущее состояние автомата
States current;

//====================================================================================================

void drawText(String msg, int x = tft.width() + 1, int y = tft.height() + 1)
{
  tft.setTextSize(TEXTSIZE);
  tft.setTextColor(MAIN_GREEN);
  if (x != tft.width() + 1 and y != tft.height() + 1) {
    tft.setCursor(x, y);
  }
  tft.print(utf8rus(msg));
}

void drawTextCentered(String msg, int16_t vertical_offset)
{
  size_t len = utf8rus(msg).length();
  return drawText(msg, tft.width() / 2 - len * TEXT_W / 2,  tft.height() / 2 + vertical_offset * TEXT_H);
}

void drawHead()
{
  drawText("Волт-Тек Г.Э.К.К.", tft.width() / 2 - 17 * TEXT_W / 2, 0); // 19 - количество букв
  tft.drawFastHLine(0, TEXT_H * 2, tft.width(), MAIN_GREEN);
}

void drawHello()
{
  tft.fillScreen(BLACK);
  tft.drawRGBBitmap(tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H, vault_logo, VAULT_LOGO_W, VAULT_LOGO_H);
  drawTextCentered("ВОЛТ-ТЕК", 3);
  drawTextCentered("Г.Э.К.К.", 5);
  delay(3000);
}

void drawPasswordRequired(Password pass)
{
  tft.fillScreen(BLACK);
  drawHead();
  drawTextCentered("Требуется пароль", -3);
  String input;
  for (int i = 0; i < pass.guessLength(); i++) {
    input += "*";
  }
  for (int i = 0; i < pass.length() - pass.guessLength(); i++) {
    input += "_";
  }
  drawTextCentered(input, 3);
}

void drawWrongPass(uint8_t tries)
{
  tft.fillScreen(BLACK);
  drawHead();
  tft.drawRGBBitmap(tft.width() / 2 - VAULT_LOGO_UPSET_W / 2, tft.height() / 2 - VAULT_LOGO_UPSET_H / 2 - 3 * TEXT_H, vault_logo_upset, VAULT_LOGO_UPSET_W, VAULT_LOGO_UPSET_H);
  drawTextCentered("Вход запрещён", 3);
  drawTextCentered("Осталось попыток: ", 5);
  drawText(String(tries));
  delay(3000);
}

void drawBlocked(uint16_t seconds)
{
  tft.fillScreen(BLACK);
  drawHead();
  tft.drawRGBBitmap(tft.width() / 2 - VAULT_LOGO_ANGRY_W / 2, tft.height() / 2 - VAULT_LOGO_ANGRY_H / 2 - 3 * TEXT_H, vault_logo_angry, VAULT_LOGO_ANGRY_W, VAULT_LOGO_ANGRY_H);
  drawTextCentered("Устройство заблокировано", 3);
  drawTextCentered(formatTime(seconds), 5);
  delay(3000);
}

void drawSuccess()
{
  tft.fillScreen(BLACK);
  drawHead();
  tft.drawRGBBitmap(tft.width() / 2 - VAULT_LOGO_THUMBS_UP_W / 2, tft.height() / 2 - VAULT_LOGO_THUMBS_UP_H / 2 - 3 * TEXT_H, vault_logo_thumbs_up, VAULT_LOGO_THUMBS_UP_W, VAULT_LOGO_THUMBS_UP_H);
  drawTextCentered("Пароль принят", 3);
  drawTextCentered("Добро пожаловать", 5);
  delay(3000);
}

void drawMainPage()
{
  //
}

//====================================================================================================

void setup(void)
{
  Serial.begin(9600);

  // Настраиваем пины
  pinMode(PIN_SUCCESS, OUTPUT);

  digitalWrite(PIN_SUCCESS, LOW);

  // Настраиваем дисплей
  uint16_t ID = tft.readID();
  if (ID == 0xD3) ID = 0x9481;
  tft.begin(ID);
  tft.cp437(true);
  tft.setRotation(DISPLAY_ROTATION);

  // Настраиваем еепром
  if (EEPROM.read(EEPROM_INIT_ADDR) != EEPROM_INIT_KEY) { // Если в еепром ничего нет
    EEPROM.write(EEPROM_INIT_ADDR, EEPROM_INIT_KEY);
    writeEepromString(EEPROM_PASS_ADDR, secret.getPassword());
  }
  secret = Password(readEepromString(EEPROM_PASS_ADDR));

  // Рисуем приветствие
  drawHello();
}

void loop(void)
{
  secret.append('1'); secret.append('2'); secret.append('3');
  drawPasswordRequired(secret);
  delay(3000000);

  switch (current)
  {
    case PLAY:
      break;
    case ADMIN:
      break;
    case SUCCESS:
      break;
    case BLOCK:
      break;
  }

  char key = keypad.getKey();

  //  16383

  //  tft.setTextColor(CYAN);
  //  tft.setTextSize(4);
  //  tft.setCursor(0, 0);
  //  tft.println(utf8rus("Пошёл нахуй кожаный ублюдок"));
  //  tft.println(utf8rus("Ахах ахахах ахахаха хахаха хах ахахаха хахаха хах ахахаха хахаха хах ахахаха хахаха хах ахахаха хахаха хах"));
  //  delay(10000);

  //      showmsgXY(20, 10, 1, NULL, utf8rus("Кожаный ублюдок")); //System x1
  //      showmsgXY(20, 24, 2, NULL, utf8rus("Кожаный ублюдок")); // System x2
  //      showmsgXY(20, 60, 1, &FreeSans9pt7b, utf8rus("Кожаный ублюдок")); // FreeSans9pt7b
  //      showmsgXY(20, 80, 1, &FreeSans12pt7b, utf8rus("Кожаный ублюдок")); // FreeSans12pt7b
  //      showmsgXY(20, 100, 1, &FreeSerif12pt7b, utf8rus("Кожаный ублюдок")); // FreeSerif12pt7b
  //      showmsgXY(20, 120, 1, &FreeSmallFont, utf8rus("Кожаный ублюдок")); // FreeSmallFont
  //      showmsgXY(5, 180, 1, &FreeSevenSegNumFont, utf8rus("01234")); // 01234
  //      showmsgXY(5, 190, 1, NULL, utf8rus("Кожаный ублюдок")); // System Font is drawn from topline
  //      tft.setTextColor(CYAN);
  //      tft.setTextSize(2);
  //      tft.setCursor(0, 220);
  //      tft.print(utf8rus("Кожаный ублюдок"));
  //      delay(1000);
  //      tft.setCursor(0, 220);
  //      tft.print("Милый человек");
  //      delay(1000);
  //      showmsgXY(5, 260, 1, &FreeSans9pt7b, utf8rus("Кожаный ублюдок")); // Free Fonts from baseline
  //      showmsgXY(5, 285, 1, &FreeSans9pt7b, utf8rus("Кожаный ублюдок")); // Free Fonts transparent
  //      delay(1000);
  //      showmsgXY(5, 285, 1, &FreeSans9pt7b, utf8rus("Кожаный ублюдок")); // Free Fonts XXX
  //      delay(1000);
  //      showmsgXY(5, 310, 1, &FreeSans9pt7b, "erase backgnd with fillRect()");
  //  delay(10000);
}


















void writeEepromString(char address, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + _size, '\0'); //Add termination null character for String Data
}


String readEepromString(char address)
{
  int i;
  char data[100]; // Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(address);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(address + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}


//void showmsgXY(int x, int y, int sz, const GFXfont *f, String msg)
//{
//  int16_t x1, y1;
//  uint16_t wid, ht;
//  tft.drawFastHLine(0, y, tft.width(), WHITE);
//  tft.setFont(f);
//  tft.setCursor(x, y);
//  tft.setTextColor(GREEN);
//  tft.setTextSize(sz);
//  tft.print(utf8rus(msg));
//  delay(1000);
//}

/* Recode russian fonts from UTF-8 to Windows-1251 */
String utf8rus(String source)
{
  int i, k;
  String target;
  unsigned char n;
  char m[2] = { '0', '\0' };

  k = source.length(); i = 0;

  while (i < k) {
    n = source[i]; i++;

    if (n >= 0xC0) {
      switch (n) {
        case 0xD0: {
            n = source[i]; i++;
            if (n == 0x81) {
              n = 0xA8;
              break;
            }
            if (n >= 0x90 && n <= 0xBF) n = n + 0x30;
            break;
          }
        case 0xD1: {
            n = source[i]; i++;
            if (n == 0x91) {
              n = 0xB8;
              break;
            }
            if (n >= 0x80 && n <= 0x8F) n = n + 0x70;
            break;
          }
      }
    }
    m[0] = n; target = target + String(m);
  }
  return target;
}

String formatTime(uint16_t secCount)
{
  uint8_t minutes = secCount / 60;
  uint8_t hours = minutes / 60;
  minutes = minutes % 60;
  secCount = secCount % 60;
  return formatDyadicSection(hours) + ":" + formatDyadicSection(minutes) + ":" + formatDyadicSection(secCount);
}

String formatDyadicSection(uint16_t c)
{
  if (c < 1) {
    return String("00");
  }
  if (c < 10) {
    return "0" + String(c);
  }
  return String(c);
}
