#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Пины подключения дисплея ST7735
#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2
#define TFT_SCLK 18
#define TFT_MOSI 23

// Пин кнопки
#define BUTTON_PIN 15

// Параметры экрана 160x128 (ландшафтный режим)
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

// Параметры кубиков (увеличены под большой экран)
#define DICE_SIZE 70       // Размер кубика в пикселях
#define DICE_RADIUS 10     // Радиус закругления углов
#define DOT_RADIUS 7       // Радиус точек на кубике

// Позиции кубиков на экране (160x128, центрированы)
#define DICE1_X 10         // X: (160 - 70*2) / 2 = 10
#define DICE1_Y 29         // Y: (128 - 70) / 2 = 29
#define DICE2_X 80         // X: 10 + 70 = 80
#define DICE2_Y 29         // Y: (128 - 70) / 2 = 29

// Создание объекта дисплея
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Переменные для антидребезга кнопки
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // 50мс антидребезг

// Переменные для управления состоянием и таймером
bool isDiceMode = true;       // true = бросок костей, false = запуск таймера
bool timerActive = false;     // true = таймер запущен
unsigned long timerStartTime = 0;
unsigned long lastSecondUpdate = 0;
const int TIMER_DURATION = 10; // Длительность таймера в секундах

// Переменные для мигающего оповещения
bool alertActive = false;
unsigned long lastBlinkTime = 0;
bool alertVisible = true;
const int BLINK_INTERVAL = 500; // Интервал мигания в мс

int lastRemainingSeconds = -1; // Для частичного обновления таймера
int lastDice1 = 0;             // Для частичного обновления кубиков
int lastDice2 = 0;

/**
 * Функция отрисовки кубика с заданным значением
 */
void drawDice(int x, int y, int value, int oldValue, bool isInitialDraw = false) {
  if (isInitialDraw) {
    tft.fillRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, ST7735_WHITE);
    tft.drawRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, ST7735_BLACK);
  }

  int center = x + DICE_SIZE / 2;
  int left = x + DICE_SIZE / 4;
  int right = x + DICE_SIZE * 3 / 4;
  int top = y + DICE_SIZE / 4;
  int middle = y + DICE_SIZE / 2;
  int bottom = y + DICE_SIZE * 3 / 4;

  // Стираем старые точки (рисуем их цветом фона)
  if (oldValue > 0) {
    switch(oldValue) {
      case 1: tft.fillCircle(center, middle, DOT_RADIUS, ST7735_WHITE); break;
      case 2: tft.fillCircle(left, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_WHITE); break;
      case 3: tft.fillCircle(left, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(center, middle, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_WHITE); break;
      case 4: tft.fillCircle(left, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_WHITE); break;
      case 5: tft.fillCircle(left, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(center, middle, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_WHITE); break;
      case 6: tft.fillCircle(left, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(left, middle, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, top, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, middle, DOT_RADIUS, ST7735_WHITE); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_WHITE); break;
    }
  }

  // Рисуем новые точки
  switch(value) {
    case 1: tft.fillCircle(center, middle, DOT_RADIUS, ST7735_BLACK); break;
    case 2: tft.fillCircle(left, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_BLACK); break;
    case 3: tft.fillCircle(left, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(center, middle, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_BLACK); break;
    case 4: tft.fillCircle(left, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_BLACK); break;
    case 5: tft.fillCircle(left, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(center, middle, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_BLACK); break;
    case 6: tft.fillCircle(left, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(left, middle, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(left, bottom, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, top, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, middle, DOT_RADIUS, ST7735_BLACK); tft.fillCircle(right, bottom, DOT_RADIUS, ST7735_BLACK); break;
  }
}

/**
 * Анимация броска с частичным обновлением.
 */
void animateRoll(int finalDice1, int finalDice2) {
  const int ANIMATION_FRAMES = 15; // Больше кадров для плавности
  const int FRAME_DELAY = 50;    // Задержка между кадрами
  
  // Полная перерисовка в начале
  tft.fillScreen(ST7735_BLACK);
  drawDice(DICE1_X, DICE1_Y, lastDice1, 0, true);
  drawDice(DICE2_X, DICE2_Y, lastDice2, 0, true);

  int currentDice1 = lastDice1;
  int currentDice2 = lastDice2;

  for(int frame = 0; frame < ANIMATION_FRAMES; frame++) {
    int nextDice1 = random(1, 7);
    int nextDice2 = random(1, 7);
    
    drawDice(DICE1_X, DICE1_Y, nextDice1, currentDice1);
    drawDice(DICE2_X, DICE2_Y, nextDice2, currentDice2);

    currentDice1 = nextDice1;
    currentDice2 = nextDice2;
    
    delay(FRAME_DELAY);
  }
  
  // Финальная отрисовка
  drawDice(DICE1_X, DICE1_Y, finalDice1, currentDice1);
  drawDice(DICE2_X, DICE2_Y, finalDice2, currentDice2);

  lastDice1 = finalDice1;
  lastDice2 = finalDice2;
}

/**
 * Функция отрисовки мигающего оповещения
 */
void drawAlert(bool visible) {
  if (visible) {
    tft.fillTriangle(
      SCREEN_WIDTH / 2, 10,
      20, SCREEN_HEIGHT - 10,
      SCREEN_WIDTH - 20, SCREEN_HEIGHT - 10,
      ST7735_YELLOW
    );
    tft.fillRect(SCREEN_WIDTH / 2 - 8, 30, 16, 50, ST7735_BLACK);
    tft.fillRect(SCREEN_WIDTH / 2 - 8, 90, 16, 16, ST7735_BLACK);
  } else {
    tft.fillScreen(ST7735_BLACK);
  }
}

/**
 * Функция отрисовки таймера
 */
void drawTimer(int remainingSeconds) {
  if (lastRemainingSeconds == remainingSeconds) return; // Ничего не изменилось

  // Устанавливаем цвет текста и ФОНА. Фон будет стирать старые цифры.
  tft.setTextSize(13);
  tft.setTextColor(ST7735_YELLOW, ST7735_BLACK);

  // При первом запуске таймера - очищаем весь экран
  if (lastRemainingSeconds == -1) {
    tft.fillScreen(ST7735_BLACK);
  }

  char timeStr[4];
  sprintf(timeStr, "%02d", remainingSeconds);
  
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((SCREEN_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2 + 5);
  
  // Просто печатаем новый текст. Фон сотрет старый.
  tft.print(timeStr);

  lastRemainingSeconds = remainingSeconds; // Запоминаем последнее значение
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Dice Simulator Starting...");
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  tft.initR(INITR_BLACKTAB);
  delay(100);
  
  tft.setRotation(1);
  randomSeed(analogRead(0));
  
  tft.fillScreen(ST7735_BLACK);
  delay(100);
  
  tft.setTextColor(ST7735_CYAN);
  tft.setTextSize(3);
  tft.setCursor(45, 30);
  tft.println("DICE");
  tft.setCursor(25, 65);
  tft.println("ROLLER");
  
  delay(500);
  
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(45, 105);
  tft.println("Press button");
  tft.setCursor(60, 115);
  tft.println("to roll!");
  
  Serial.println("Display initialized successfully!");
}

void loop() {
  unsigned long currentTime = millis();

  if (alertActive) {
    if ((currentTime - lastBlinkTime) > BLINK_INTERVAL) {
      lastBlinkTime = currentTime;
      alertVisible = !alertVisible;
      drawAlert(alertVisible);
    }
  }

  if (timerActive) {
    unsigned long elapsedTime = (currentTime - timerStartTime) / 1000;

    if (elapsedTime >= TIMER_DURATION) {
      timerActive = false;
      alertActive = true;
      isDiceMode = true;
      
      lastBlinkTime = currentTime;
      alertVisible = true;
      tft.fillScreen(ST7735_BLACK); // Очищаем экран перед алертом
      drawAlert(true);

      Serial.println("Timer finished. Alert mode activated.");
      
      while(digitalRead(BUTTON_PIN) == LOW) { delay(10); }
      return; 
    }

    if ((currentTime - lastSecondUpdate) >= 1000) {
      lastSecondUpdate = currentTime;
      int remainingSeconds = TIMER_DURATION - elapsedTime;
      drawTimer(remainingSeconds);
      Serial.print("Timer: ");
      Serial.println(remainingSeconds);
    }
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    if ((currentTime - lastDebounceTime) > debounceDelay) {
      lastDebounceTime = currentTime;

      if (alertActive) {
        alertActive = false;
        isDiceMode = true;
        Serial.println("Alert acknowledged. Rolling dice...");
      }
      
      else if (timerActive) {
        timerActive = false;
        isDiceMode = true;
        Serial.println("Timer interrupted. Rolling dice...");
      }
      
      if (isDiceMode) {
        int dice1 = random(1, 7);
        int dice2 = random(1, 7);
        
        Serial.print("Dice 1: "); Serial.print(dice1);
        Serial.print("  Dice 2: "); Serial.println(dice2);
        
        animateRoll(dice1, dice2);
        
        isDiceMode = false;
        Serial.println("Next press will start the timer.");

      } else {
        Serial.println("Starting timer...");
        timerActive = true;
        timerStartTime = currentTime;
        lastSecondUpdate = currentTime;
        lastRemainingSeconds = -1; // Сбрасываем для полной перерисовки
        
        drawTimer(TIMER_DURATION);
        
        isDiceMode = true;
        Serial.println("Timer started. Next press will roll dice.");
      }
      
      while(digitalRead(BUTTON_PIN) == LOW) { delay(10); }
      Serial.println("Button released. Ready for next action.");
    }
  }
  
  delay(10);
}
