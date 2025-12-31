#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include "config.h"

// ----------------------------------------------------------
// Типы и глобальные объекты
// ----------------------------------------------------------

enum class AppState : uint8_t {
  DiceRollNext,   // Следующее нажатие - бросок кубиков
  DiceTimerNext,  // Следующее нажатие - запуск таймера
  DiceAnimating,  // Идёт анимация броска
  TimerRunning,   // Работает таймер обратного отсчёта
  AlertActive     // Мигающий алерт
};

// Объект дисплея с использованием конфигурации пинов
Adafruit_ST7735 tft(
  Config::Hardware::TFT_CS,
  Config::Hardware::TFT_DC,
  Config::Hardware::TFT_MOSI,
  Config::Hardware::TFT_SCLK,
  Config::Hardware::TFT_RST
);

// Текущее состояние приложения
AppState appState = AppState::DiceRollNext;

// Состояние кубиков
int lastDice1 = 0;
int lastDice2 = 0;

// Анимация броска
int animationCurrentDice1 = 0;
int animationCurrentDice2 = 0;
int animationTargetDice1  = 0;
int animationTargetDice2  = 0;
uint8_t animationFrame    = 0;
unsigned long lastFrameTime = 0;

// Таймер
unsigned long timerStartTime   = 0;
unsigned long lastSecondUpdate = 0;
int lastRemainingSeconds       = -1; // для частичного обновления таймера

// Алерт
bool alertVisible        = true;
unsigned long lastBlinkTime = 0;

// Кнопка (антидребезг)
int buttonStableState    = HIGH;
int lastButtonReading    = HIGH;
unsigned long lastDebounceTime = 0;
bool buttonPressedEvent  = false;

// ----------------------------------------------------------
// Прототипы функций
// ----------------------------------------------------------

void drawDice(int x, int y, int value, int oldValue, bool isInitialDraw = false);
void drawAlert(bool visible);
void drawTimer(int remainingSeconds);

void updateButton(unsigned long now);
void handleButtonPress(unsigned long now);
void handleDiceAnimation(unsigned long now);
void handleTimer(unsigned long now);
void handleAlert(unsigned long now);

void startDiceRoll(unsigned long now);
void startTimer(unsigned long now);
void showIntro();

// ----------------------------------------------------------
// Отрисовка кубика
// ----------------------------------------------------------

void drawDice(int x, int y, int value, int oldValue, bool isInitialDraw) {
  const uint16_t DICE_SIZE   = Config::Dice::SIZE;
  const uint16_t DICE_RADIUS = Config::Dice::RADIUS;
  const uint16_t DOT_RADIUS  = Config::Dice::DOT_RADIUS;

  if (isInitialDraw) {
    tft.fillRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, Config::Colors::DICE_FILL);
    tft.drawRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, Config::Colors::DICE_BORDER);
  }

  int center = x + DICE_SIZE / 2;
  int left   = x + DICE_SIZE / 4;
  int right  = x + (DICE_SIZE * 3) / 4;
  int top    = y + DICE_SIZE / 4;
  int middle = y + DICE_SIZE / 2;
  int bottom = y + (DICE_SIZE * 3) / 4;

  // Стираем старые точки (рисуем их цветом фона кубика)
  if (oldValue > 0) {
    switch (oldValue) {
      case 1:
        tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
      case 2:
        tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
      case 3:
        tft.fillCircle(left,   top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right,  bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
      case 4:
        tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(left,  bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
      case 5:
        tft.fillCircle(left,   top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right,  top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(left,   bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right,  bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
      case 6:
        tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(left,  middle, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(left,  bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, top,    DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, middle, DOT_RADIUS, Config::Colors::DICE_FILL);
        tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_FILL);
        break;
    }
  }

  // Рисуем новые точки
  switch (value) {
    case 1:
      tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
    case 2:
      tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
    case 3:
      tft.fillCircle(left,   top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right,  bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
    case 4:
      tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(left,  bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
    case 5:
      tft.fillCircle(left,   top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right,  top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(center, middle, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(left,   bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right,  bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
    case 6:
      tft.fillCircle(left,  top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(left,  middle, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(left,  bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, top,    DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, middle, DOT_RADIUS, Config::Colors::DICE_PIP);
      tft.fillCircle(right, bottom, DOT_RADIUS, Config::Colors::DICE_PIP);
      break;
  }
}

// ----------------------------------------------------------
// Отрисовка мигающего алерта
// ----------------------------------------------------------

void drawAlert(bool visible) {
  if (visible) {
    const int16_t centerX = Config::Display::WIDTH / 2;
    const int16_t topY    = Config::Alert::TRI_TOP_Y;
    const int16_t bottomY = Config::Display::HEIGHT - Config::Alert::TRI_BOTTOM_MARGIN;
    const int16_t leftX   = Config::Alert::TRI_HORIZONTAL_MARGIN;
    const int16_t rightX  = Config::Display::WIDTH - Config::Alert::TRI_HORIZONTAL_MARGIN;

    tft.fillTriangle(
      centerX, topY,
      leftX,   bottomY,
      rightX,  bottomY,
      Config::Colors::ALERT
    );

    // Знак "!" внутри треугольника
    tft.fillRect(centerX - 8, 30, 16, 50, Config::Colors::BACKGROUND);
    tft.fillRect(centerX - 8, 90, 16, 16, Config::Colors::BACKGROUND);
  } else {
    // В оригинале при скрытии полностью очищался экран
    tft.fillScreen(Config::Colors::BACKGROUND);
  }
}

// ----------------------------------------------------------
// Отрисовка таймера
// ----------------------------------------------------------

void drawTimer(int remainingSeconds) {
  if (lastRemainingSeconds == remainingSeconds) {
    return; // Ничего не изменилось
  }

  tft.setTextSize(Config::Timer::TEXT_SIZE);
  tft.setTextColor(Config::Colors::TIMER_TEXT, Config::Colors::BACKGROUND);

  // При первом запуске таймера - очищаем весь экран
  if (lastRemainingSeconds == -1) {
    tft.fillScreen(Config::Colors::BACKGROUND);
  }

  char timeStr[4];
  snprintf(timeStr, sizeof(timeStr), "%02d", remainingSeconds);

  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);

  int16_t x = (Config::Display::WIDTH  - static_cast<int16_t>(w)) / 2;
  int16_t y = (Config::Display::HEIGHT - static_cast<int16_t>(h)) / 2
              + Config::Timer::CENTER_Y_OFFSET;

  tft.setCursor(x, y);
  tft.print(timeStr);

  lastRemainingSeconds = remainingSeconds;
}

// ----------------------------------------------------------
// Обработка кнопки (антидребезг, событие нажатия)
// ----------------------------------------------------------

void updateButton(unsigned long now) {
  int reading = digitalRead(Config::Hardware::BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime   = now;
    lastButtonReading  = reading;
  }

  if ((now - lastDebounceTime) >= Config::Input::DEBOUNCE_MS) {
    if (reading != buttonStableState) {
      buttonStableState = reading;
      if (buttonStableState == LOW) {
        // Фиксируем одно событие нажатия
        buttonPressedEvent = true;
      }
    }
  }
}

// ----------------------------------------------------------
// Запуск анимации броска кубиков
// ----------------------------------------------------------

void startDiceRoll(unsigned long now) {
  int dice1 = random(1, 7);
  int dice2 = random(1, 7);

  Serial.print("Dice 1: ");
  Serial.print(dice1);
  Serial.print("  Dice 2: ");
  Serial.println(dice2);

  if (Config::Animation::CLEAR_SCREEN_ON_START) {
    tft.fillScreen(Config::Colors::BACKGROUND);
  }

  // Рисуем рамки кубиков с предыдущими значениями
  drawDice(Config::Dice::DICE1_X, Config::Dice::DICE1_Y, lastDice1, 0, true);
  drawDice(Config::Dice::DICE2_X, Config::Dice::DICE2_Y, lastDice2, 0, true);

  animationCurrentDice1 = lastDice1;
  animationCurrentDice2 = lastDice2;
  animationTargetDice1  = dice1;
  animationTargetDice2  = dice2;
  animationFrame        = 0;
  lastFrameTime         = now;

  appState = AppState::DiceAnimating;
}

// ----------------------------------------------------------
// Запуск таймера
// ----------------------------------------------------------

void startTimer(unsigned long now) {
  Serial.println("Starting timer...");

  timerStartTime     = now;
  lastSecondUpdate   = now;
  lastRemainingSeconds = -1; // сброс для полной перерисовки

  appState = AppState::TimerRunning;

  drawTimer(static_cast<int>(Config::Timer::DURATION_SEC));

  Serial.println("Timer started. Next press will roll dice.");
}

// ----------------------------------------------------------
// Обработка анимации броска (неблокирующая)
// ----------------------------------------------------------

void handleDiceAnimation(unsigned long now) {
  if (now - lastFrameTime < Config::Animation::FRAME_DELAY_MS) {
    return;
  }

  lastFrameTime = now;

  if (animationFrame < Config::Animation::ROLL_FRAMES) {
    int nextDice1 = random(1, 7);
    int nextDice2 = random(1, 7);

    drawDice(Config::Dice::DICE1_X, Config::Dice::DICE1_Y, nextDice1, animationCurrentDice1);
    drawDice(Config::Dice::DICE2_X, Config::Dice::DICE2_Y, nextDice2, animationCurrentDice2);

    animationCurrentDice1 = nextDice1;
    animationCurrentDice2 = nextDice2;
    ++animationFrame;
  } else {
    // Финальная отрисовка
    drawDice(Config::Dice::DICE1_X, Config::Dice::DICE1_Y, animationTargetDice1, animationCurrentDice1);
    drawDice(Config::Dice::DICE2_X, Config::Dice::DICE2_Y, animationTargetDice2, animationCurrentDice2);

    lastDice1 = animationTargetDice1;
    lastDice2 = animationTargetDice2;

    appState = AppState::DiceTimerNext;
    Serial.println("Next press will start the timer.");
  }
}

// ----------------------------------------------------------
// Обработка таймера
// ----------------------------------------------------------

void handleTimer(unsigned long now) {
  unsigned long elapsedTimeSec = (now - timerStartTime) / 1000UL;

  if (elapsedTimeSec >= Config::Timer::DURATION_SEC) {
    // Таймер закончился - включаем алерт
    appState      = AppState::AlertActive;
    alertVisible  = true;
    lastBlinkTime = now;

    tft.fillScreen(Config::Colors::BACKGROUND);
    drawAlert(true);

    Serial.println("Timer finished. Alert mode activated.");
    return;
  }

  if (now - lastSecondUpdate >= 1000UL) {
    lastSecondUpdate = now;
    int remainingSeconds = static_cast<int>(Config::Timer::DURATION_SEC - elapsedTimeSec);
    drawTimer(remainingSeconds);
    Serial.print("Timer: ");
    Serial.println(remainingSeconds);
  }
}

// ----------------------------------------------------------
// Обработка алерта (мигание)
// ----------------------------------------------------------

void handleAlert(unsigned long now) {
  if (now - lastBlinkTime > Config::Alert::BLINK_INTERVAL_MS) {
    lastBlinkTime = now;
    alertVisible  = !alertVisible;
    drawAlert(alertVisible);
  }
}

// ----------------------------------------------------------
// Обработка нажатия кнопки (по событиям)
// ----------------------------------------------------------

void handleButtonPress(unsigned long now) {
  switch (appState) {
    case AppState::DiceRollNext:
      Serial.println("Manual roll (button pressed)!");
      startDiceRoll(now);
      break;

    case AppState::DiceTimerNext:
      // Следующее нажатие после броска - запуск таймера
      startTimer(now);
      break;

    case AppState::DiceAnimating:
      // Во время анимации игнорируем новые нажатия (как и раньше из-за delay)
      break;

    case AppState::TimerRunning:
      Serial.println("Timer interrupted. Rolling dice...");
      startDiceRoll(now);
      break;

    case AppState::AlertActive:
      Serial.println("Alert acknowledged. Rolling dice...");
      startDiceRoll(now);
      break;
  }
}

// ----------------------------------------------------------
// Интро-экран
// ----------------------------------------------------------

void showIntro() {
  tft.fillScreen(Config::Colors::BACKGROUND);

  tft.setTextColor(Config::Colors::TITLE_TEXT);
  tft.setTextSize(Config::Intro::TITLE_TEXT_SIZE);
  tft.setCursor(Config::Intro::TITLE_DICE_X, Config::Intro::TITLE_DICE_Y);
  tft.println("DICE");
  tft.setCursor(Config::Intro::TITLE_ROLLER_X, Config::Intro::TITLE_ROLLER_Y);
  tft.println("ROLLER");

  tft.setTextColor(Config::Colors::HINT_TEXT);
  tft.setTextSize(Config::Intro::HINT_TEXT_SIZE);
  tft.setCursor(Config::Intro::HINT_LINE1_X, Config::Intro::HINT_LINE1_Y);
  tft.println("Press button");
  tft.setCursor(Config::Intro::HINT_LINE2_X, Config::Intro::HINT_LINE2_Y);
  tft.println("to roll!");
}

// ----------------------------------------------------------
// setup / loop
// ----------------------------------------------------------

void setup() {
  Serial.begin(115200);
  delay(Config::Intro::SERIAL_START_DELAY_MS);
  Serial.println("ESP32 Dice Simulator Starting...");

  pinMode(Config::Hardware::BUTTON_PIN, INPUT_PULLUP);

  tft.initR(Config::Display::INITR_MODE);
  delay(Config::Intro::DISPLAY_INIT_DELAY_MS);

  tft.setRotation(Config::Display::ROTATION);
  randomSeed(analogRead(0));

  tft.fillScreen(Config::Colors::BACKGROUND);
  delay(Config::Intro::DISPLAY_CLEAR_DELAY_MS);

  showIntro();
  delay(Config::Intro::INTRO_PAUSE_MS);

  Serial.println("Display initialized successfully!");

  // Начальное состояние: ожидаем бросок кубиков
  appState = AppState::DiceRollNext;
}

void loop() {
  unsigned long now = millis();

  // Обновление кнопки и генерация события нажатия
  updateButton(now);

  // Обработка периодических задач в зависимости от состояния
  switch (appState) {
    case AppState::DiceRollNext:
    case AppState::DiceTimerNext:
      // Ничего периодически не обновляем, ждём нажатия
      break;

    case AppState::DiceAnimating:
      handleDiceAnimation(now);
      break;

    case AppState::TimerRunning:
      handleTimer(now);
      break;

    case AppState::AlertActive:
      handleAlert(now);
      break;
  }

  // Обработка события нажатия (если было)
  if (buttonPressedEvent) {
    buttonPressedEvent = false;
    handleButtonPress(now);
  }

  delay(Config::Input::LOOP_IDLE_DELAY);
}
