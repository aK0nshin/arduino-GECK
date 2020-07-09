void writeEepromString(char address, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + _size, '\0'); // Add termination null character for String Data
}

String readEepromString(char address)
{
  int i;
  char data[100]; // Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(address);
  while (k != '\0' && len < 500) // Read until null character
  {
    k = EEPROM.read(address + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}
