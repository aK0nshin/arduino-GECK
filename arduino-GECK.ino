#include <EEPROM.h>
#include "src/MCUFRIEND_kbv/MCUFRIEND_kbv.h"   // Hardware-specific library
#include "src/Keypad/Keypad.h"
#include "src/Password/Password.h"
#include "src/SdFat/src/SdFat.h"

// Настройки еепром
#define EEPROM_INIT_ADDR 1023  // Адрес ячейки еепром, чтобы понять, сохранено ли у нас что-нибудь в еепром
#define EEPROM_INIT_KEY 128
#define EEPROM_PASS_ADDR 0  // Адрес ячейки еепром для пароля

// Настройки дисплея
#define BLACK      0x0000
#define WHITE      0xFFFF
#define GREEN      0x07E0
#define RED        0xF800
#define MAIN_GREEN 0x0560  // #00ae00 Грязновато-зелёный цвет, чтоб было не так ярко
#define DISPLAY_ROTATION 3  // От 1 до 4
MCUFRIEND_kbv tft;  // Создаём дисплей

// Размеры текста
#define TEXTSIZE 2 // Размер буквы
#define TEXT_W 12 // Ширина буквы
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
byte rowPins[ROWS] = {30, 29, 28, 27}; // пины подключенных строк
byte colPins[COLS] = {26, 25, 24, 23}; // пины подключенных столбцов
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); // создаем объект клавиатуры для работы с ней

// Пины
#define PIN_GARDEN_CREATED 20
#define PIN_ENTER_ADMIN 21
SdFatSoftSpi<12, 11, 13> SD;   // Создаём и цепляем SD к пинам
#define SD_CS 10               // SD card pin on your shield

// Настройки игры
#define HELLO_TIMEOUT 3000  // Время показа приветствия, в миллисекундах
#define PASSWORD_DENIED_TIMEOUT 3000  // Время показа неправильного пароля, в миллисекундах
#define PASSWORD_ACCEPTED_TIMEOUT 3000  // Время показа правильного пароля, в миллисекундах
#define BLOCKED_TIMEOUT 1800000 // Время устройства в отключке, в миллисекундах
#define GARDEN_CREATION_TIMEOUT 600000 // Время создания кущ, в миллисекундах
#define GARDEN_CREATION_DOTS_TIMEOUT 600 // Период мигания точек в создании кущ, в миллисекундах

#define MAX_TRIES 3 // Количество попыток ввода пароля
Password secret = Password("11111111"); // Дефолтный пароль

// Всякие технические настройки
#define DEBOUNCE_DELAY 500  // Устранение дребезга, в миллисекундах
unsigned long lastDebounceTime = 0;
uint32_t blockStart = 0;
uint8_t tries = MAX_TRIES;
const double garden_creation_timeout = GARDEN_CREATION_TIMEOUT;

//====================================================================================================

// Возможные состояния автомата
typedef enum
{
  INITIAL,
  HELLO,
  PASSWORD_REQUIRED,
  PASSWORD_DENIED,
  BLOCKED,
  PASSWORD_ACCEPTED,
  MAIN_MENU,
  DIAGNOSTIC,
  VAULT_CODES,
  GARDEN_CREATION,
  GARDEN_CREATED,
  ADMIN_MENU,
  PASSWORD_SET,
  STATE_CHANGE,
} state_t;

//====================================================================================================

void setup(void)
{
  // Настраиваем дисплей
  uint16_t ID = tft.readID();
  if (ID == 0xD3) ID = 0x9481;
  tft.begin(ID);
  drawBackground();
  tft.cp437(true);
  tft.setRotation(DISPLAY_ROTATION);
  Serial.begin(9600);

  // Запускаем SD
  bool good = SD.begin(SD_CS);
  if (!good) {
    Serial.println(F("Cannot start SD"));
  }

  // Настраиваем еепром
  if (EEPROM.read(EEPROM_INIT_ADDR) != EEPROM_INIT_KEY) { // Если в еепром ничего нет
    EEPROM.write(EEPROM_INIT_ADDR, EEPROM_INIT_KEY);
    writeEepromString(EEPROM_PASS_ADDR, secret.getPassword());
  }
  secret = Password(readEepromString(EEPROM_PASS_ADDR));

  // Настраиваем пины
  pinMode(PIN_GARDEN_CREATED, OUTPUT);
  pinMode(PIN_ENTER_ADMIN, INPUT);
  digitalWrite(PIN_GARDEN_CREATED, LOW);
}

void loop(void)
{
  run_machine();
  delay(300);
}
