# План реализации: Симулятор броска костей

## Этап 1: Настройка окружения

### 1.1 Обновление platformio.ini

Добавить зависимости библиотек:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 115200
lib_deps = 
    adafruit/Adafruit GFX Library@^1.11.9
    adafruit/Adafruit ST7735 and ST7789 Library@^1.10.3
```

**Статус**: ⏳ Ожидает выполнения

---

## Этап 2: Разработка кода

### 2.1 Определение констант и глобальных переменных

```cpp
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Пины подключения дисплея
#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2
#define TFT_SCLK 18
#define TFT_MOSI 23

// Параметры кубиков
#define DICE_SIZE 50
#define DICE_RADIUS 8
#define DOT_RADIUS 4

// Создание объекта дисплея
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
```

### 2.2 Функция drawDice()

**Алгоритм отрисовки кубика:**

```
FUNCTION drawDice(x, y, value)
│
├─> Нарисовать белый квадрат с закругленными углами
│   fillRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, WHITE)
│
├─> Нарисовать черную рамку
│   drawRoundRect(x, y, DICE_SIZE, DICE_SIZE, DICE_RADIUS, BLACK)
│
├─> SWITCH (value)
│   │
│   ├─> CASE 1: Одна точка в центре
│   │   fillCircle(x+25, y+25, DOT_RADIUS, BLACK)
│   │
│   ├─> CASE 2: Две точки по диагонали
│   │   fillCircle(x+15, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+35, DOT_RADIUS, BLACK)
│   │
│   ├─> CASE 3: Три точки (диагональ + центр)
│   │   fillCircle(x+15, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+25, y+25, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+35, DOT_RADIUS, BLACK)
│   │
│   ├─> CASE 4: Четыре точки по углам
│   │   fillCircle(x+15, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+15, y+35, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+35, DOT_RADIUS, BLACK)
│   │
│   ├─> CASE 5: Пять точек (углы + центр)
│   │   fillCircle(x+15, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+15, DOT_RADIUS, BLACK)
│   │   fillCircle(x+25, y+25, DOT_RADIUS, BLACK)
│   │   fillCircle(x+15, y+35, DOT_RADIUS, BLACK)
│   │   fillCircle(x+35, y+35, DOT_RADIUS, BLACK)
│   │
│   └─> CASE 6: Шесть точек (два столбца)
│       fillCircle(x+15, y+13, DOT_RADIUS, BLACK)
│       fillCircle(x+15, y+25, DOT_RADIUS, BLACK)
│       fillCircle(x+15, y+37, DOT_RADIUS, BLACK)
│       fillCircle(x+35, y+13, DOT_RADIUS, BLACK)
│       fillCircle(x+35, y+25, DOT_RADIUS, BLACK)
│       fillCircle(x+35, y+37, DOT_RADIUS, BLACK)
│
└─> END FUNCTION
```

### 2.3 Функция setup()

```
FUNCTION setup()
│
├─> Инициализация Serial (115200)
│   Serial.begin(115200)
│
├─> Инициализация дисплея
│   tft.initR(INITR_REDTAB)  // или INITR_BLACKTAB
│   tft.setRotation(2)       // поворот на 180° (опционально)
│
├─> Инициализация генератора случайных чисел
│   randomSeed(analogRead(0))
│
├─> Очистка экрана
│   tft.fillScreen(ST7735_BLACK)
│
└─> END FUNCTION
```

### 2.4 Функция loop()

```
FUNCTION loop()
│
├─> Генерация случайных значений
│   dice1 = random(1, 7)  // 1-6
│   dice2 = random(1, 7)  // 1-6
│
├─> Вывод в Serial для отладки
│   Serial.print("Dice 1: "), Serial.print(dice1)
│   Serial.print(" Dice 2: "), Serial.println(dice2)
│
├─> Очистка экрана
│   tft.fillScreen(ST7735_BLACK)
│
├─> Отрисовка первого кубика
│   drawDice(14, 39, dice1)
│
├─> Отрисовка второго кубика
│   drawDice(64, 39, dice2)
│
├─> Задержка 10 секунд
│   delay(10000)
│
└─> RETURN (повторение цикла)
```

---

## Этап 3: Компиляция и загрузка

### 3.1 Компиляция проекта

1. Нажать кнопку компиляции (галочка) в PlatformIO
2. Дождаться успешной сборки

### 3.2 Загрузка прошивки

1. Подключить ESP32 к компьютеру через USB
2. Нажать кнопку загрузки (стрелка вправо)
3. При появлении "Connecting..." нажать кнопку BOOT на плате
4. Дождаться успешной загрузки

---

## Этап 4: Тестирование

### 4.1 Функциональное тестирование

- [ ] Проверка инициализации дисплея
- [ ] Проверка отображения значения 1
- [ ] Проверка отображения значения 2
- [ ] Проверка отображения значения 3
- [ ] Проверка отображения значения 4
- [ ] Проверка отображения значения 5
- [ ] Проверка отображения значения 6
- [ ] Проверка интервала обновления (10 секунд)
- [ ] Проверка случайности генерации

### 4.2 Возможные проблемы и решения

| Проблема | Причина | Решение |
|----------|---------|---------|
| Смещение изображения влево/вправо | Неправильная инициализация | Изменить INITR_REDTAB на INITR_BLACKTAB или наоборот |
| Изображение перевернуто | Неправильная ориентация | Изменить параметр setRotation(0-3) |
| Не загружается прошивка | Не нажата кнопка BOOT | Зажать BOOT при появлении "Connecting..." |
| Библиотеки не найдены | Не установлены зависимости | Проверить lib_deps в platformio.ini |

---

## Этап 5: Отладка через Serial Monitor

Для мониторинга работы программы:

```bash
# В PlatformIO открыть Serial Monitor
# Или выполнить команду:
pio device monitor --baud 115200
```

Ожидаемый вывод:
```
Dice 1: 3 Dice 2: 5
Dice 1: 1 Dice 2: 6
Dice 1: 4 Dice 2: 2
...
```

---

## Блок-схема программы

```
         ┌─────────────┐
         │   START     │
         └──────┬──────┘
                │
         ┌──────▼──────────────────┐
         │  setup()                │
         │  • Serial.begin(115200) │
         │  • tft.initR()          │
         │  • randomSeed()         │
         │  • tft.fillScreen()     │
         └──────┬──────────────────┘
                │
         ┌──────▼──────────────────┐
         │  loop()                 │
         │                         │
         │  ┌──────────────────┐   │
         │  │ dice1=random(1,7)│   │
         │  │ dice2=random(1,7)│   │
         │  └────────┬─────────┘   │
         │           │             │
         │  ┌────────▼─────────┐   │
         │  │ Serial.print()   │   │
         │  └────────┬─────────┘   │
         │           │             │
         │  ┌────────▼─────────┐   │
         │  │ fillScreen(BLACK)│   │
         │  └────────┬─────────┘   │
         │           │             │
         │  ┌────────▼─────────┐   │
         │  │ drawDice(14,39,  │   │
         │  │          dice1)  │   │
         │  └────────┬─────────┘   │
         │           │             │
         │  ┌────────▼─────────┐   │
         │  │ drawDice(64,39,  │   │
         │  │          dice2)  │   │
         │  └────────┬─────────┘   │
         │           │             │
         │  ┌────────▼─────────┐   │
         │  │  delay(10000)    │   │
         │  └────────┬─────────┘   │
         │           │             │
         └───────────┼─────────────┘
                     │
                     └───────┐
                             │
                ┌────────────▼──────────┐
                │  Повтор цикла loop()  │
                └───────────────────────┘
```

---

## Контрольный список выполнения

- [ ] **Этап 1**: Обновлен platformio.ini с зависимостями
- [ ] **Этап 2**: Написан код main.cpp
  - [ ] Функция drawDice() реализована
  - [ ] Функция setup() реализована
  - [ ] Функция loop() реализована
- [ ] **Этап 3**: Прошивка загружена на ESP32
- [ ] **Этап 4**: Функциональное тестирование пройдено
- [ ] **Этап 5**: Отладка через Serial Monitor выполнена

---

## Ожидаемый результат

На TFT-дисплее каждые 10 секунд отображаются два игральных кубика с случайными значениями от 1 до 6. Кубики представлены в виде белых квадратов с закругленными углами и черными точками, расположенными согласно стандартным паттернам игральных костей.
