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

// Параметры экрана 128x160
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160

// Параметры кубиков (увеличены под большой экран)
#define DICE_SIZE 60       // Размер кубика в пикселях
#define DICE_RADIUS 10     // Радиус закругления углов
#define DOT_RADIUS 5       // Радиус точек на кубике

// Позиции кубиков на экране (128x160, центрированы)
#define DICE1_X 34         // X координата первого кубика (центр по X: (128-60)/2 = 34)
#define DICE1_Y 10         // Y координата первого кубика (отступ сверху)
#define DICE2_X 34         // X координата второго кубика (центр по X)
#define DICE2_Y 90         // Y координата второго кубика (отступ снизу, чтобы оба влезли)

// Создание объекта дисплея
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Переменные для антидребезга кнопки
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // 50мс антидребезг

/**
 * Функция отрисовки кубика с заданным значением
 * 
 * @param x координата X верхнего левого угла
 * @param y координата Y верхнего левого угла
 * @param value значение кубика (1-6)
 */
void drawDice(int x, int y, int value) {
  // Рисуем белый квадрат с закругленными углами (тело кубика)
  tft.fillRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, ST7735_WHITE);
  
  // Рисуем черную рамку
  tft.drawRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, ST7735_BLACK);
  
  // Рисуем точки в зависимости от значения
  switch(value) {
    case 1:
      // Одна точка в центре
      tft.fillCircle(x + 30, y + 30, DOT_RADIUS, ST7735_BLACK);
      break;
      
    case 2:
      // Две точки по диагонали (верхний левый - нижний правый)
      tft.fillCircle(x + 18, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 42, DOT_RADIUS, ST7735_BLACK);
      break;
      
    case 3:
      // Три точки: диагональ + центр
      tft.fillCircle(x + 18, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 30, y + 30, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 42, DOT_RADIUS, ST7735_BLACK);
      break;
      
    case 4:
      // Четыре точки по углам
      tft.fillCircle(x + 18, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 18, y + 42, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 42, DOT_RADIUS, ST7735_BLACK);
      break;
      
    case 5:
      // Пять точек: углы + центр
      tft.fillCircle(x + 18, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 18, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 30, y + 30, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 18, y + 42, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 42, DOT_RADIUS, ST7735_BLACK);
      break;
      
    case 6:
      // Шесть точек: два столбца по три
      tft.fillCircle(x + 18, y + 15, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 18, y + 30, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 18, y + 45, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 15, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 30, DOT_RADIUS, ST7735_BLACK);
      tft.fillCircle(x + 42, y + 45, DOT_RADIUS, ST7735_BLACK);
      break;
  }
}

/**
 * Простая анимация броска кубиков
 * Быстрое обновление случайных значений
 * Длительность: ~1.5 секунды
 */
void animateRoll(int finalDice1, int finalDice2) {
  const int ANIMATION_FRAMES = 5;  // 5 кадров для быстрой анимации
  
  for(int frame = 0; frame < ANIMATION_FRAMES; frame++) {
    // Генерируем случайные значения для анимации
    int tempDice1 = random(1, 7);
    int tempDice2 = random(1, 7);
    
    // Очищаем области кубиков
    tft.fillRect(DICE1_X - 2, DICE1_Y - 2, DICE_SIZE + 4, DICE_SIZE + 4, ST7735_BLACK);
    tft.fillRect(DICE2_X - 2, DICE2_Y - 2, DICE_SIZE + 4, DICE_SIZE + 4, ST7735_BLACK);
    
    // Рисуем кубики с временными значениями
    drawDice(DICE1_X, DICE1_Y, tempDice1);
    drawDice(DICE2_X, DICE2_Y, tempDice2);
    
    // БЕЗ delay() - время отрисовки (~400-500мс/кадр) создает анимацию
    // Общее время: 5 кадров × ~500мс = ~2.5 секунды
  }
  
  // Финальная отрисовка с настоящими значениями
  tft.fillRect(DICE1_X - 2, DICE1_Y - 2, DICE_SIZE + 4, DICE_SIZE + 4, ST7735_BLACK);
  tft.fillRect(DICE2_X - 2, DICE2_Y - 2, DICE_SIZE + 4, DICE_SIZE + 4, ST7735_BLACK);
  drawDice(DICE1_X, DICE1_Y, finalDice1);
  drawDice(DICE2_X, DICE2_Y, finalDice2);
}


void setup() {
  // Инициализация последовательного порта для отладки
  Serial.begin(115200);
  delay(1000);  // Задержка для стабилизации Serial
  Serial.println("ESP32 Dice Simulator 128x160 with Animation Starting...");
  
  // Настройка кнопки
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Встроенный pull-up резистор
  Serial.print("Button on GPIO ");
  Serial.println(BUTTON_PIN);
  
  // Инициализация дисплея 1.8" 128x160
  Serial.println("Initializing display...");
  
  // Для 1.8" ST7735 128x160 используем INITR_BLACKTAB
  tft.initR(INITR_BLACKTAB);
  
  // Если экран обрезан или битые пиксели, попробуйте:
  // tft.initR(INITR_GREENTAB);
  // или
  // tft.initR(INITR_REDTAB);
  
  delay(100);
  Serial.println("Display init command sent");
  
  // Портретная ориентация для 128x160
  tft.setRotation(0);
  
  // Инициализация генератора случайных чисел
  randomSeed(analogRead(0));
  
  // Очистка экрана черным цветом
  Serial.println("Clearing screen...");
  tft.fillScreen(ST7735_BLACK);
  delay(100);
  
  // Приветственное сообщение (центрировано для 128x160)
  tft.setTextColor(ST7735_CYAN);
  tft.setTextSize(2);
  tft.setCursor(25, 60);  // Центрировано: "DICE" = 4 символа × 12px = 48px, (128-48)/2 ≈ 40
  tft.println("DICE");
  tft.setCursor(5, 80);   // "ROLLER" = 6 символов × 12px = 72px, (128-72)/2 ≈ 28
  tft.println("ROLLER");
  
  delay(500);
  
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.setCursor(20, 120);  // Опущено ниже
  tft.println("Press button");
  tft.setCursor(30, 135);  // Опущено ниже
  tft.println("to roll!");
  
  Serial.println("Display initialized successfully!");
  Serial.println("Ready! Press the button to roll dice.");
}

void loop() {
  // Чтение состояния кнопки (LOW = нажата при использовании INPUT_PULLUP)
  int buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW) {
    // Антидребезг
    if ((millis() - lastDebounceTime) > debounceDelay) {
      Serial.println("Button pressed! Rolling dice...");
      
      // Генерация случайных значений для двух кубиков (1-6)
      int dice1 = random(1, 7);
      int dice2 = random(1, 7);
      
      // Вывод значений в Serial Monitor для отладки
      Serial.print("Dice 1: ");
      Serial.print(dice1);
      Serial.print("  Dice 2: ");
      Serial.println(dice2);
      
      // Очистка экрана
      tft.fillScreen(ST7735_BLACK);
      
      // Анимация броска кубиков
      animateRoll(dice1, dice2);
      
      lastDebounceTime = millis();
      
      // Ждем отпускания кнопки
      while(digitalRead(BUTTON_PIN) == LOW) {
        delay(10);
      }
      
      Serial.println("Ready for next roll!");
    }
  }
  
  delay(10);  // Небольшая задержка для стабильности
}
