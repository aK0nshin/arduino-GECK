bool enter_admin_input()
{
  static uint32_t lastDebounceTime = 0;
  static int16_t lastState = HIGH;
  if (millis() - lastDebounceTime >= DEBOUNCE_DELAY) {
    int v = digitalRead(PIN_ENTER_ADMIN);
    Serial.println("Enter Admin signal: " + String(v));
    if (v != lastState) {
      lastState = v;
      lastDebounceTime = millis();
      if (v == HIGH) {
        transit(ADMIN_MENU);
        return true;
      }
    }
  }
  return false;
}

char get_input_key()
{
  if (Serial.available() > 0) {
    String s = Serial.readString();
    char key = s.charAt(0);
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        if (key == keys[i][j]) {
          return key;
        }
      }
    }
  }
  return keypad.getKey();
}

void password_required_input()
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    String deleteChars = "ABCD";
    if (deleteChars.indexOf(key) != -1) {
      secret.pop();
    } else if (!secret.enoughGuess()) {
      secret.append(key);
    }
    drawPasswordRequiredText(&secret);
    if (secret.enoughGuess()) {
      if (secret.evaluate()) {
        transit(PASSWORD_ACCEPTED);
      } else {
        transit(PASSWORD_DENIED);
      }
      secret.reset();
    }
  }
}

void main_menu_input()
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    switch (key)
    {
      case '1':
        transit(DIAGNOSTIC);
        break;
      case '2':
        transit(VAULT_CODES);
        break;
      case '3':
        transit(GARDEN_CREATION);
        break;
    }
  }
}

void to_main_menu_input()
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    if (key == '1') {
      transit(MAIN_MENU);
    }
  }
}

void admin_menu_input()
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    switch (key)
    {
      case '1':
        transit(prevS);
        break;
      case '2':
        transit(PASSWORD_SET);
        break;
      case '3':
        transit(STATE_CHANGE);
        break;
    }
  }
}

void password_set_input(String *new_pass)
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    String done_chars = "A";
    String delete_chars = "BC";
    String cancel_chars = "D";
    if (done_chars.indexOf(key) != -1) {
      writeEepromString(EEPROM_PASS_ADDR, *new_pass);
      secret = Password(*new_pass);
      *new_pass = "";
      drawClearPart(30, 215);
      drawTextCentered(F("Пароль успешно"), -1);
      drawTextCentered(F("Установлен"), 1);
      delay(1000);
      transit(ADMIN_MENU);
    } else if (delete_chars.indexOf(key) != -1) {
      int16_t len = new_pass->length();
      if (len > 0) {
        new_pass->remove(len - 1);
        drawPasswordSetText(new_pass);
      }
    } else if (cancel_chars.indexOf(key) != -1) {
      *new_pass = "";
      transit(ADMIN_MENU);
    } else {
      *new_pass += key;
      drawPasswordSetText(new_pass);
    }
  }
}

void state_change_input()
{
  char key = get_input_key();
  if (key) {
    Serial.println("Got char: " + String(key));
    switch (key)
    {
      case '1':
        transit(INITIAL);
        break;
      case '2':
        transit(PASSWORD_REQUIRED);
        break;
      case '3':
        transit(BLOCKED);
        break;
      case '4':
        transit(MAIN_MENU);
        break;
      case '5':
        transit(GARDEN_CREATION);
        break;
      case '6':
        transit(GARDEN_CREATED);
        break;
      case 'D':
        transit(ADMIN_MENU);
        break;
    }
  }
}
