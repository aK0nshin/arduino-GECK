// Перекодирует русские шрифты из UTF-8 в Windows-1251
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

// Представляет секунды в формате 00:00:00
String formatTime(uint16_t secCount)
{
  uint8_t minutes = secCount / 60;
  uint8_t hours = minutes / 60;
  minutes = minutes % 60;
  secCount = secCount % 60;
  return formatDyadicSection(hours) + ":" + formatDyadicSection(minutes) + ":" + formatDyadicSection(secCount);
}

// Представляет секунды в формате 00
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
