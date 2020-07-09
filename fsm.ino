state_t curS, prevS = INITIAL;
bool first = true;
uint32_t hello_start, password_denied_start, blocked_start, password_accepted_start, garden_creation_start;
uint32_t last_dots_time = 0, last_event_time = 0;
int16_t dots_x = 20, dots_y = 30;
String new_pass = "";

void run_machine()
{
  enter_admin_input();
  switch (curS)
  {
    case INITIAL:
      transit(HELLO);
      break;
    case HELLO:
      if (firstTime()) {
        drawHello();
        hello_start = millis();
      }
      if (millis() - hello_start >= HELLO_TIMEOUT) {
        transit(PASSWORD_REQUIRED);
      }
      break;
    case PASSWORD_REQUIRED:
      if (firstTime()) {
        digitalWrite(PIN_GARDEN_CREATED, HIGH);
        secret.reset();
        drawPasswordRequired(&secret);
      }
      password_required_input();
      break;
    case PASSWORD_DENIED:
      if (firstTime()) {
        if (tries < 1)  {
          transit(BLOCKED);
          break;
        }
        drawWrongPass(tries);
        tries -= 1;
        password_denied_start = millis();
      }
      if (millis() - password_denied_start >= PASSWORD_DENIED_TIMEOUT) {
        transit(PASSWORD_REQUIRED);
      }
      break;
    case BLOCKED:
      if (firstTime()) {
        drawBlocked(BLOCKED_TIMEOUT / 1000);
        blocked_start = millis() / 1000;
      }
      uint16_t sec;
      sec = BLOCKED_TIMEOUT / 1000 - (millis() / 1000 - blocked_start);
      drawBlockedText(sec);
      if (sec <= 0) {
        transit(PASSWORD_REQUIRED);
        tries = MAX_TRIES;
      }
      break;
    case PASSWORD_ACCEPTED:
      if (firstTime()) {
        drawRightPass();
        password_accepted_start = millis();
        tries = MAX_TRIES;
      }
      if (millis() - password_accepted_start >= PASSWORD_ACCEPTED_TIMEOUT) {
        transit(MAIN_MENU);
      }
      break;
    case MAIN_MENU:
      if (firstTime()) {
        drawMainMenu();
      }
      main_menu_input();
      break;
    case DIAGNOSTIC:
      if (firstTime()) {
        drawDiagnostic();
      }
      to_main_menu_input();
      break;
    case VAULT_CODES:
      if (firstTime()) {
        drawVaultCodes();
      }
      to_main_menu_input();
      break;
    case GARDEN_CREATION:
      if (firstTime()) {
        garden_creation_start = last_event_time = millis();
        last_dots_time = 0;
        dots_x = 20;
        dots_y = 30;
        drawGardenCreationFirstTime(&dots_x, &dots_y);
        dots = "     ";
        last_p = -1;
      }
      uint32_t cur_time;
      cur_time = millis();
      if (cur_time - last_event_time >= garden_creation_timeout / 100) {
        last_event_time = last_dots_time = cur_time;
        drawGardenCreation((cur_time - garden_creation_start) / garden_creation_timeout * 100, &dots_x, &dots_y);
      }
      if (cur_time - garden_creation_start >= garden_creation_timeout)  {
        drawGardenCreation(100, &dots_x, &dots_y);
        transit(GARDEN_CREATED);
        break;
      }
      if (cur_time - last_dots_time >= GARDEN_CREATION_DOTS_TIMEOUT) {
        last_dots_time = cur_time;
        drawDots(dots_x, dots_y);
      }
      break;
    case GARDEN_CREATED:
      if (firstTime()) {
        digitalWrite(PIN_GARDEN_CREATED, HIGH);
        drawGardenCreated();
      }
      break;
    case ADMIN_MENU:
      if (firstTime()) {
        drawAdminMenu();
      }
      admin_menu_input();
      break;
    case PASSWORD_SET:
      if (firstTime()) {
        new_pass = "";
        drawPasswordSet(&new_pass);
      }
      password_set_input(&new_pass);
      break;
    case STATE_CHANGE:
      if (firstTime()) {
        drawStateChange();
      }
      state_change_input();
      break;
  }
  Serial.println("Current state: " + String(curS));
}

bool firstTime() {
  bool f = first;
  first = false;
  return f;
}

void transit(state_t s) {
  Serial.print(__func__);
  Serial.print(" to ");
  Serial.println(s);
  digitalWrite(PIN_GARDEN_CREATED, LOW);
  prevS = curS;
  curS = s;
  first = true;
}
