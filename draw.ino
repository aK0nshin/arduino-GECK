#define VAULT_LOGO_W 190
#define VAULT_LOGO_H 81

// tft.width() = 320
// tft.height() = 240

String dots = "     ";
int16_t last_p = -1;

void drawBackground()
{
  tft.fillScreen(BLACK);
}

void drawClearPart(uint16_t top, uint16_t bot)
{
  tft.fillRect(0, top, tft.width(), bot - top, BLACK);
}

void drawProgressBar(double percent, int16_t x = 0, int16_t y = 0, bool with_text = true)
{
  int16_t max_width = tft.width() - x * 2;
  double w = round(percent * max_width / 100.0);
  if (w > max_width) {
    w = max_width;
  }
  tft.drawRect(x, y, max_width, 22, MAIN_GREEN); // 22 - ширина полоски
  tft.fillRect(x + 2, y + 2, w - 4, 22 - 4, MAIN_GREEN); // 22 - ширина полоски
  if (with_text) {
    int16_t p = round(percent);
    if (p > 100) {
      p = 100;
    }
    String msg = String(p) + "%";
    uint8_t len = msg.length();
    int16_t text_x = tft.width() / 2 - len * TEXT_W / 2;
    tft.setTextSize(TEXTSIZE);
    tft.setCursor(text_x, y + 3);
    for (int i = 0; i < len; i++) {
      if (text_x + TEXT_W * i > w) {
        tft.setTextColor(MAIN_GREEN, BLACK);
      } else {
        tft.setTextColor(BLACK);
      }
      tft.print(msg[i]);
    }
  }
}

void drawText(String msg, int x = tft.width() + 1, int y = tft.height() + 1)
{
  Serial.println(__func__);
  Serial.println(msg);
  tft.setTextSize(TEXTSIZE);
  tft.setTextColor(MAIN_GREEN, BLACK);
  if (x != tft.width() + 1 and y != tft.height() + 1) {
    tft.setCursor(x, y);
  }
  tft.print(utf8rus(msg));
}

void drawTextCentered(String msg, int16_t vertical_offset, size_t custom_len = 0)
{
  size_t len;
  if (custom_len > 0) {
    len = custom_len;
  } else {
    len = utf8rus(msg).length();
  }
  return drawText(msg, tft.width() / 2 - len * TEXT_W / 2,  tft.height() / 2 + vertical_offset * TEXT_H);
}

void drawHead()
{
  drawText(F("Фьючер-Тек Г.Э.К.К."), tft.width() / 2 - 19 * TEXT_W / 2, 0); // 19 - количество букв
  tft.drawFastHLine(0, TEXT_H * 2, tft.width(), MAIN_GREEN);
}

void drawFooter(String msg)
{
  drawText(msg, tft.width() / 2 - utf8rus(msg).length() * TEXT_W / 2, tft.height() - TEXT_H * 2 + 3);
  tft.drawFastHLine(0, tft.height() - TEXT_H * 2, tft.width(), MAIN_GREEN);
}

void drawAdminHead()
{
  drawText(F("Админка"), tft.width() / 2 - 7 * TEXT_W / 2, 0); // 7 - количество букв
  tft.drawFastHLine(0, TEXT_H * 2, tft.width(), RED);
}

void drawAdminFooter(String msg)
{
  drawText(msg, tft.width() / 2 - utf8rus(msg).length() * TEXT_W / 2, tft.height() - TEXT_H * 2 + 3);
  tft.drawFastHLine(0, tft.height() - TEXT_H * 2, tft.width(), RED);
}

void drawHello()
{
  drawBackground();
  showBMP(F("/bitmaps/vault_logo.bmp"), tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H);
  drawTextCentered(F("Фьючер-Тек"), 3);
  drawTextCentered(F("Г.Э.К.К."), 5);
}

void drawPasswordRequired(Password *pass)
{
  drawBackground();
  drawHead();
  drawTextCentered(F("Требуется пароль"), -3);
  drawPasswordRequiredText(pass);
  drawFooter(F("A) Удалить"));
}

void drawPasswordRequiredText(Password *pass)
{
  String input;
  for (int i = 0; i < pass->guessLength(); i++) {
    input += "*";
  }
  for (int i = 0; i < pass->length() - pass->guessLength(); i++) {
    input += "_";
  }
  drawTextCentered(input, 3);
}

void drawWrongPass(uint8_t tries)
{
  drawBackground();
  drawHead();
  showBMP(F("/bitmaps/vault_logo_upset.bmp"), tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H);
  drawTextCentered(F("Вход запрещён"), 3);
  drawTextCentered(F("Осталось попыток: "), 5, 19);
  drawText(String(tries));
}

void drawBlocked(uint16_t sec)
{
  drawBackground();
  drawHead();
  showBMP(F("/bitmaps/vault_logo_angry.bmp"), tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H);
  drawTextCentered(F("Устройство заблокировано"), 3);
  drawBlockedText(sec);
}

void drawBlockedText(uint16_t sec)
{
  drawTextCentered(formatTime(sec), 5);
}

void drawRightPass()
{
  drawBackground();
  drawHead();
  showBMP(F("/bitmaps/vault_logo_thumbs_up.bmp"), tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H);
  drawTextCentered(F("Пароль принят"), 3);
  drawTextCentered(F("Добро пожаловать"), 5);
}

void drawMainMenu()
{
  drawBackground();
  drawHead();
  drawText(F("1) Диагностика системы"), 26, 50);
  drawText(F("2) Коды разблокировки"), 26, 70);
  drawText(F("доп. возможностей"), 62, 90);
  drawText(F("Убежища"), 62, 110);
  drawText(F("3) Запустить генерацию"), 26, 130);
  drawText(F("Эдемских Кущ"), 62, 150);
}

void drawDiagnostic()
{
  drawBackground();
  drawHead();
  drawTextCentered(F("> Запуск диагностики"), 0, 23);
  delay(1000);
  for (int i = 0; i < 3; i++) {
    drawText(".");
    delay(1000);
  }
  drawBackground();
  drawHead();
  drawText(F("Базовый репликатор:"), 20, 30);
  drawText(F("Реактор:"), 20, 80);
  drawText(F("Установщик силового поля:"), 20, 130);
  drawText(F("Библиотека Конгресса:"), 20, 180);
  delay(500);

  drawText(F("> сбор данных"), 20, 50);
  drawText(F("> сбор данных"), 20, 100);
  drawText(F("> сбор данных"), 20, 150);
  drawText(F("> сбор данных"), 20, 200);
  delay(1000);
  const uint8_t coordsA[12][2] = {{176, 50},  {176, 100}, {188, 50}, {176, 150}, {200, 50}, {188, 100}, {176, 200}, {200, 100}, {188, 150}, {200, 150}, {188, 200}, {200, 200}};
  for (int i = 0; i < 12; i++) {
    drawText(".", coordsA[i][0], coordsA[i][1]);
    delay(500);
  }
  drawText(F("> анализ        "), 20, 50);
  drawText(F("> анализ        "), 20, 100);
  drawText(F("> анализ        "), 20, 150);
  drawText(F("> анализ        "), 20, 200);
  const uint8_t coordsB[12][2] = {{116, 50},  {116, 100}, {128, 50}, {116, 150}, {140, 50}, {128, 100}, {116, 200}, {140, 100}, {128, 150}, {140, 150}, {128, 200}, {140, 200}};
  for (int i = 0; i < 12; i++) {
    drawText(".", coordsB[i][0], coordsB[i][1]);
    delay(500);
  }
  delay(500);
  drawText(F("> Укомплектован на 98%"), 20, 50);
  drawText(F("> Ресурс 3%"), 20, 100);
  drawText(F("> Недостаточно энергии"), 20, 150);
  drawText(F("> Голодиски отсутствуют"), 20, 200);
  delay(1000);
  drawFooter(F("1) В меню"));
}

void drawVaultCodes()
{
  drawBackground();
  drawHead();
  String file = "Коды активации оборонных сист";
  for (int i = 0; i < 315; i++ ) {
    file += String(char(random(0, 256)));
  }
  drawText(file, 20, 30);
  delay(2000);
  drawBackground();
  drawHead();
  drawText(F("Системная ошибка:"), 20, 30);
  drawText(F("Буфер переполнен"), 20, 50);
  delay(1000);
  drawText(F("> Проверка целостности"), 20, 70);
  drawText(F("> данных"), 20, 90);
  delay(1000);
  for (int i = 0; i < 3; i++) {
    drawText(".");
    delay(2000);
  }
  drawText(F("Системная ошибка:"), 20, 110);
  drawText(F("Файл поврежден"), 20, 130);
  drawText(F("> Восстановление"), 20, 150);
  delay(1000);
  for (int i = 0; i < 3; i++) {
    drawText(".");
    delay(2000);
  }
  drawText(F("Восстановление невозможно"), 20, 170);
  delay(1000);
  drawFooter(F("1) В меню"));
}

void drawDots(int16_t x, int16_t y)
{
  drawText(dots, x, y);
  dots.trim();
  dots = String("  .") + dots;
  if (dots == "  ....") {
    dots = "     ";
  }
}

void drawGardenCreationFirstTime(int16_t *x, int16_t *y)
{
  drawBackground();
  drawHead();
  drawGardenCreation(0, x, y);
}


void drawGardenCreation(int16_t percent, int16_t *x, int16_t *y)
{
  Serial.print("percent: ");
  Serial.println(percent);
  Serial.print("last_p: ");
  Serial.println(last_p);
  if (percent - last_p > 0) {
    drawProgressBar(percent, 0, 215);
    for (last_p; last_p < percent; last_p++) {
      if (last_p % 2 == 0) {
        dots = "     ";
        drawGardenCreationText(last_p / 2, x, y);
      }
    }
  }
}

void drawGardenCreationText(uint8_t event, int16_t *x, int16_t *y)
{
  Serial.print("event: ");
  Serial.println(event);
  const uint16_t top = 30;
  const uint16_t bot = 215;
  *x = 20;

  switch (event)
  {
    case 0:
      drawText(F("> Взятие проб почвы"), 20, 30);
      drawText(F("> Взятие точечных проб"), 20, 50);
      *y = 70;
      break;
    case 1:
      drawText(F("  Готово"), 20, 70);
      drawText(F("> Составление"), 20, 90);
      drawText(F("> объединенных проб"), 20, 110);
      *y = 130;
      break;
    case 2:
      drawText(F("  Готово"), 20, 130);
      drawClearPart(top, bot);
      drawText(F("> Анализ состояния почвы"), 20, 30);
      drawText(F("> Проверка загрязнения"), 20, 50);
      drawText(F("> поверхности"), 20, 70);
      *y = 90;
      break;
    case 3:
      drawText(F(" Тяжелые металлы, кислоты"), 20, 90);
      drawText(F("> Радиологический анализ"), 20, 110);
      drawText(F("> Поиск радиоактивных"), 20, 130);
      drawText(F("> компонентов"), 20, 150);
      *y = 170;
      break;
    case 4:
      drawText(F("  Найдены йод-131"), 20, 170);
      drawText(F("          цезий-137"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("  Суммарная активность"), 20, 30);
      drawText(F("  веществ:"), 20, 50);
      drawText(F("  14*10^18 Бк"), 20, 70);
      drawClearPart(top, bot);
      drawText(F("> Гранулометрический"), 20, 30);
      drawText(F("> анализ"), 20, 50);
      drawText(F("> Подсчёт интегральныого"), 20, 70);
      drawText(F("> гранулометрического"), 20, 90);
      drawText(F("> состава"), 20, 110);
      *y = 130;
      break;
    case 5:
      drawClearPart(top, bot);

      drawText(F("  D(10)  19,18"), 20, 30);
      drawText(F("  D(25) 10,33"), 20, 50);
      drawText(F("  D(50) 11,46"), 20, 70);
      drawText(F("  D(75) 12,81"), 20, 90);
      drawText(F("  D(90) 13,92"), 20, 110);
      drawText(F("  Повышенное содержиние"), 20, 130);
      drawText(F("  песка и глины"), 20, 150);
      drawText(F("  Вид почвы:"), 20, 170);
      drawText(F("  пустошь"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Химический анализ"), 20, 30);
      drawText(F("> Показатель содержания"), 20, 50);
      drawText(F("> питательных веществ"), 20, 70);
      *y = 90;
      break;
    case 6:
      drawText(F("  20% от нормы"), 20, 90);
      drawText(F("> Концентрация тяж. мет."), 20, 110);
      *y = 130;
      break;
    case 7:
      drawText(F("  свинец 117  кадмий 78,8"), 20, 130);
      drawText(F("  цинк 18,75  медь 12"), 20, 150);
      drawText(F("  никель 7,6  мышьяк 6,31"), 20, 170);
      drawText(F("  ртуть 3,57"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Химический анализ"), 20, 30);
      drawText(F("> Кислотность почвы"), 20, 50);
      *y = 70;
      break;
    case 8:
      drawText(F("  2,7 pH"), 20, 70);
      drawText(F("> Содержания бензапирена"), 20, 90);
      drawText(F("> и нефтепродуктов"), 20, 110);
      *y = 130;
      break;
    case 9:
      drawText(F("  В норме"), 20, 130);
      drawClearPart(top, bot);

      drawText(F("> Агрохимический анализ"), 20, 30);
      drawText(F("> Показатель содержания"), 20, 50);
      drawText(F("> Нитритов"), 20, 70);
      *y = 90;
      break;
    case 10:
      drawText(F("  64% от нормы"), 20, 90);
      drawText(F("> Нитратов"), 20, 110);
      *y = 130;
      break;
    case 11:
      drawText(F("  112% от нормы"), 20, 130);
      drawText(F("> Аммония"), 20, 150);
      *y = 170;
      break;
    case 12:
      drawText(F("  4% от нормы"), 20, 170);
      drawClearPart(top, bot);

      drawText(F("> Минералогический анализ"), 20, 30);
      drawText(F("> Исследование грунта"), 20, 50);
      drawText(F("> Илистого"), 20, 70);
      *y = 90;
      break;
    case 13:
      drawText(F("  Илистый грунт не найден"), 20, 90);
      drawText(F("> Коллоидного"), 20, 110);
      *y = 130;
      break;
    case 14:
      drawText(F("  Пониженный коэффициент"), 20, 130);
      drawText(F("  водонасыщения"), 20, 150);
      drawText(F("  Степень плотности"), 20, 170);
      drawText(F("  низкая"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Минералогический анализ"), 20, 30);
      drawText(F(" Коэффициент выветрелости"), 20, 50);
      drawText(F("  близок к критическим"), 20, 70);
      drawText(F("  значениям"), 20, 90);
      drawText(F("> Глинистого"), 20, 110);
      *y = 130;
      break;
    case 15:
      drawText(F("  Степени засоленности"), 20, 130);
      drawText(F("  низкая"), 20, 150);
      drawText(F("  Температура в"), 20, 170);
      drawText(F("  пределах нормы"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Микробиологический"), 20, 30);
      drawText(F("> анализ"), 20, 50);
      drawText(F("> Проверка бактерий"), 20, 70);
      *y = 90;
      break;
    case 16:
      drawText(F("  Содержание полезных"), 20, 90);
      drawText(F("  бактерий в норме"), 20, 110);
      drawText(F("> Проверка грибов"), 20, 130);
      *y = 150;
      break;
    case 17:
      drawText(F("  Найден неизвестный вид"), 20, 150);
      drawClearPart(top, bot);

      drawText(F("> Микробиологический"), 20, 30);
      drawText(F("> анализ"), 20, 50);
      drawText(F("> Поиск почвенных"), 20, 70);
      drawText(F("> водорослей"), 20, 90);
      *y = 110;
      break;
    case 18:
      drawText(F("  Не найдено"), 20, 110);
      drawText(F("> Анализ наличия"), 20, 130);
      drawText(F("> простейших"), 20, 150);
      *y = 170;
      break;
    case 19:
      drawText(F("  Найдены кишечная"), 20, 170);
      drawText(F("  палочка и сальмонелла"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Подготовка к"), 20, 30);
      drawText(F("> терраформированию"), 20, 50);
      drawText(F("> Составление списка "), 20, 70);
      drawText(F("> операций"), 20, 90);
      *y = 110;
      break;
    case 20:
      drawText(F("  Готово"), 20, 110);
      drawText(F("> Рассчёт доз химикатов"), 20, 130);
      *y = 150;
      break;
    case 21:
      drawText(F("  Готово"), 20, 150);
      drawText(F("> Растворение  реагентов"), 20, 170);
      *y = 190;
      break;
    case 22:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Подготовка к"), 20, 30);
      drawText(F("> терраформированию"), 20, 50);
      drawText(F("> Гидрация реактора"), 20, 70);
      *y = 90;
      break;
    case 23:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Проращивание семян"), 20, 110);
      *y = 130;
      break;
    case 24:
      drawText(F("  Готово"), 20, 130);
      drawText(F("> Пробуждение образцов"), 20, 150);
      drawText(F("> бактерий"), 20, 170);
      *y = 190;
      break;
    case 25:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Подготовка к"), 20, 30);
      drawText(F("> терраформированию"), 20, 50);
      drawText(F("> Дезинфекция"), 20, 70);
      drawText(F("> инструментов"), 20, 90);
      *y = 110;
      break;
    case 26:
      drawText(F("  Готово"), 20, 110);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Уборка поверхностных "), 20, 50);
      drawText(F("> загрязнений"), 20, 70);
      *y = 90;
      break;
    case 27:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Распыление антирадина"), 20, 110);
      *y = 130;
      break;
    case 28:
      drawText(F("  Готово"), 20, 130);
      drawText(F("> Установка антирад."), 20, 150);
      drawText(F("> навеса"), 20, 170);
      *y = 190;
      break;
    case 29:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Нормализация состава "), 20, 50);
      drawText(F("> почвы"), 20, 70);
      *y = 90;
      break;
    case 30:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Черноземирование почвы"), 20, 110);
      *y = 130;
      break;
    case 31:
      drawText(F("  Готово"), 20, 130);
      drawText(F("> Инъекция питательных"), 20, 150);
      drawText(F("> веществ"), 20, 170);
      *y = 190;
      break;
    case 32:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Коррозирование"), 20, 50);
      drawText(F("> тяжёлых металлов"), 20, 70);
      *y = 90;
      break;
    case 33:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Нейтрализация"), 20, 110);
      drawText(F("> кислотности почвы"), 20, 130);
      *y = 150;
      break;
    case 34:
      drawText(F("  Готово"), 20, 150);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Экстракция"), 20, 50);
      drawText(F("> нефтепродуктов"), 20, 70);
      *y = 90;
      break;
    case 35:
      drawText(F("  Не требуется"), 20, 90);
      drawText(F("> Нормализация"), 20, 110);
      drawText(F("> азотистого обмена "), 20, 130);
      drawText(F("> почвы"), 20, 150);
      *y = 170;
      break;
    case 36:
      drawText(F("  Готово"), 20, 170);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Мелиорация земельного"), 20, 50);
      drawText(F("> ресурса"), 20, 70);
      *y = 90;
      break;
    case 37:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Укрепление насыпного"), 20, 110);
      drawText(F("> слоя"), 20, 130);
      *y = 150;
      break;
    case 38:
      drawText(F("  Готово"), 20, 150);
      drawText(F("> Минерализация почвы"), 20, 170);
      *y = 190;
      break;
    case 39:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Подготовка микробной"), 20, 50);
      drawText(F("> культуры"), 20, 70);
      *y = 90;
      break;
    case 40:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Высеивание микробной"), 20, 110);
      drawText(F("> культуры"), 20, 130);
      *y = 150;
      break;
    case 41:
      drawText(F("  Готово"), 20, 150);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Ликвидация неизвестных"), 20, 50);
      drawText(F("> грибковых"), 20, 70);
      *y = 90;
      break;
    case 42:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Внедрение почвенных"), 20, 110);
      drawText(F("> водорослей"), 20, 130);
      *y = 150;
      break;
    case 43:
      drawText(F("  Готово"), 20, 150);
      drawText(F("> Взрыхление почвы"), 20, 170);
      *y = 190;
      break;
    case 44:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Удобрение почвы"), 20, 50);
      *y = 70;
      break;
    case 45:
      drawText(F("  Готово"), 20, 70);
      drawText(F("> Высеивание зерновых"), 20, 90);
      drawText(F("> культур"), 20, 110);
      *y = 130;
      break;
    case 46:
      drawText(F("  Готово"), 20, 130);
      drawText(F("> Высеивание технических"), 20, 150);
      drawText(F("> культур"), 20, 170);
      *y = 190;
      break;
    case 47:
      drawText(F("  Готово"), 20, 190);
      drawClearPart(top, bot);

      drawText(F("> Терраформирование"), 20, 30);
      drawText(F("> Высеивание кормовых"), 20, 50);
      drawText(F("> культур"), 20, 70);
      *y = 90;
      break;
    case 48:
      drawText(F("  Готово"), 20, 90);
      drawText(F("> Высеивание бахчевых"), 20, 110);
      drawText(F("> культур"), 20, 130);
      *y = 150;
      break;
    case 49:
      drawText(F("  Готово"), 20, 150);
      drawText(F("> Распыление инсектицидов"), 20, 170);
      *y = 190;
      break;
    case 50:
      drawText(F("  Готово"), 20, 190);
      break;
    default:
      return;
  }
}

void drawGardenCreated()
{
  drawBackground();
  showBMP(F("/bitmaps/vault_logo_happy.bmp"), tft.width() / 2 - VAULT_LOGO_W / 2, tft.height() / 2 - VAULT_LOGO_H / 2 - 3 * TEXT_H);
  drawTextCentered(F("Вы создали новый оазис!"), 2);
  drawTextCentered(F("Г.Э.К.К. активирован,"), 4);
  drawTextCentered(F("Компания Фьючер-Тек"), 6);
  drawTextCentered(F("поздравляет Вас"), 8);
}

void drawAdminMenu()
{
  drawBackground();
  drawAdminHead();
  drawText(F("1) Вернуться обратно"), 26, 70);
  drawText(F("   к игре"), 26, 90);
  drawText(F("2) Сменить пароль"), 26, 110);
  drawText(F("3) Сменить состояние"), 26, 130);
  drawText(F("   игры"), 26, 150);

}

void drawPasswordSet(String *new_pass)
{
  drawBackground();
  drawAdminHead();
  drawTextCentered(F("Задать новый пароль"), -9);
  drawTextCentered(String(F("Старый пароль (длина: ")) + secret.length() + String(F(")")), -5);
  drawTextCentered(secret.getPassword(), -3);
  drawPasswordSetText(new_pass);
  drawAdminFooter(F("A) Гтв B) Удл D) Отм"));
}

void drawPasswordSetText(String *new_pass)
{
  drawTextCentered(String(F("Новый пароль (длина: ")) + new_pass->length() + String(F(")")), 1);
  drawClearPart(150, 170);
  drawTextCentered(*new_pass, 3);
}

void drawStateChange()
{
  drawBackground();
  drawAdminHead();
  drawTextCentered(F("1) ИСХОДНОЕ"), -7);
  drawTextCentered(F("2) ТРЕБУЕТСЯ ПАРОЛЬ"), -5);
  drawTextCentered(F("3) ЗАБЛОКИРОВАНО"), -3);
  drawTextCentered(F("4) ГЛАВНОЕ МЕНЮ"), -1);
  drawTextCentered(F("5) СОЗДАНИЕ КУЩ"), 1);
  drawTextCentered(F("6) КУЩИ СОЗДАНЫ"), 3);
  drawAdminFooter(F("D) Отмена"));
}
