#pragma once

#include <stdint.h>

// Все настраиваемые параметры приложения вынесены сюда.
// ВАЖНО: этот файл предполагает, что до него уже подключены
// заголовки Adafruit (для макросов ST7735_* и INITR_*).

namespace Config {

namespace Hardware {
  inline constexpr uint8_t TFT_CS   = 5;
  inline constexpr uint8_t TFT_RST  = 4;
  inline constexpr uint8_t TFT_DC   = 2;
  inline constexpr uint8_t TFT_SCLK = 18;
  inline constexpr uint8_t TFT_MOSI = 23;

  inline constexpr uint8_t BUTTON_PIN = 15;
  inline constexpr uint8_t BUZZER_PIN = 13;
}

namespace Display {
  // Параметры экрана 160x128 (ландшафтный режим)
  inline constexpr uint16_t WIDTH  = 160;
  inline constexpr uint16_t HEIGHT = 128;

  // Режим инициализации контроллера ST7735
  inline constexpr uint8_t INITR_MODE = INITR_BLACKTAB;

  // Ориентация экрана (0..3)
  inline constexpr uint8_t ROTATION   = 1;
}

namespace Colors {
  inline constexpr uint16_t BACKGROUND  = ST7735_BLACK;

  inline constexpr uint16_t DICE_FILL   = ST7735_WHITE;
  inline constexpr uint16_t DICE_BORDER = ST7735_BLACK;
  inline constexpr uint16_t DICE_PIP    = ST7735_BLACK;

  inline constexpr uint16_t TIMER_TEXT  = ST7735_YELLOW;
  inline constexpr uint16_t ALERT       = ST7735_YELLOW;

  inline constexpr uint16_t TITLE_TEXT  = ST7735_CYAN;
  inline constexpr uint16_t HINT_TEXT   = ST7735_WHITE;
  namespace DiceSum {
    inline constexpr uint16_t SUM_2_12 = 0x781F; // Пурпурный (Magenta)
    inline constexpr uint16_t SUM_3_11 = ST7735_BLUE;
    inline constexpr uint16_t SUM_4_10 = ST7735_YELLOW;
    inline constexpr uint16_t SUM_5_9  = 0xFC00; // Оранжевый (Orange)
    inline constexpr uint16_t SUM_6_8  = ST7735_GREEN;
    inline constexpr uint16_t SUM_7    = ST7735_RED;
  }

  namespace TimerColor {
    inline constexpr uint16_t LEVEL_OK       = ST7735_GREEN;
    inline constexpr uint16_t LEVEL_WARN     = ST7735_YELLOW;
    inline constexpr uint16_t LEVEL_URGENT   = 0xFC00; // Оранжевый (Orange)
    inline constexpr uint16_t LEVEL_CRITICAL = ST7735_RED;
  }
}

namespace Dice {
  // Геометрия кубиков
  inline constexpr uint16_t SIZE       = 70;  // размер кубика в пикселях
  inline constexpr uint16_t RADIUS     = 10;  // радиус закругления углов
  inline constexpr uint16_t DOT_RADIUS = 7;   // радиус точек на кубике

  // Базовый "равномерный" отступ, исходя из требования:
  // [margin][cube][margin][cube][margin] по ширине экрана.
  // 2*SIZE + 3*margin ≈ Display::WIDTH
  inline constexpr int16_t BASE_MARGIN =
      static_cast<int16_t>((static_cast<int16_t>(Config::Display::WIDTH)
                           - static_cast<int16_t>(2 * SIZE)) / 3);

  // Остаток от деления (для равномерного распределения погрешности, максимум ±1 пиксель)
  inline constexpr int16_t REMAINDER =
      static_cast<int16_t>(
        static_cast<int16_t>(Config::Display::WIDTH)
        - static_cast<int16_t>(2 * SIZE)
        - static_cast<int16_t>(3 * BASE_MARGIN)
      );

  // Левый отступ
  inline constexpr int16_t MARGIN_LEFT =
      static_cast<int16_t>(BASE_MARGIN + (REMAINDER > 0 ? 1 : 0));

  // Расстояние между кубиками
  inline constexpr int16_t SPACING =
      static_cast<int16_t>(BASE_MARGIN + (REMAINDER > 1 ? 1 : 0));

  // Правый отступ (для контроля, чтобы сумма сходилась)
  inline constexpr int16_t MARGIN_RIGHT =
      static_cast<int16_t>(
        static_cast<int16_t>(Config::Display::WIDTH)
        - static_cast<int16_t>(2 * SIZE)
        - MARGIN_LEFT
        - SPACING
      );

  // Координаты кубиков с учетом равномерных отступов
  inline constexpr int16_t DICE1_X = MARGIN_LEFT;
  inline constexpr int16_t DICE1_Y =
      static_cast<int16_t>(
        (static_cast<int16_t>(Config::Display::HEIGHT)
         - static_cast<int16_t>(SIZE)) / 2
      );

  inline constexpr int16_t DICE2_X =
      static_cast<int16_t>(DICE1_X + static_cast<int16_t>(SIZE) + SPACING);
  inline constexpr int16_t DICE2_Y = DICE1_Y;
}

namespace Input {
  // Антидребезг кнопки
  inline constexpr uint32_t DEBOUNCE_MS     = 50;
  
  // Время для регистрации долгого нажатия (в мс)
  inline constexpr uint32_t LONG_PRESS_MS   = 1500;

  // Небольшая задержка в конце loop() для разгрузки CPU
  inline constexpr uint32_t LOOP_IDLE_DELAY = 10;
}

namespace Timer {
  // Длительность таймера в секундах
  inline constexpr uint32_t DURATION_SEC = 45;

  // Длительность показа результата броска кубиков (в секундах)
  inline constexpr uint32_t RESULT_DISPLAY_SEC = 5;

  // Размер шрифта таймера
  inline constexpr uint8_t  TEXT_SIZE   = 13;

  // Дополнительный вертикальный сдвиг при центрировании (в пикселях)
  inline constexpr int16_t  CENTER_Y_OFFSET = 5;
}

namespace Alert {
  // Интервал мигания в миллисекундах
  inline constexpr uint32_t BLINK_INTERVAL_MS = 500;

  // Геометрия треугольника (по отношению к экрану)
  inline constexpr int16_t TRI_TOP_Y             = 10;
  inline constexpr int16_t TRI_BOTTOM_MARGIN     = 10;
  inline constexpr int16_t TRI_HORIZONTAL_MARGIN = 20;
}

namespace Animation {
  // Количество кадров анимации броска
  inline constexpr uint8_t  ROLL_FRAMES    = 15;

  // Задержка между кадрами (мс)
  inline constexpr uint32_t FRAME_DELAY_MS = 50;

  // Очищать ли экран перед началом анимации
  inline constexpr bool     CLEAR_SCREEN_ON_START = true;
}

namespace Intro {
  // Задержки в setup()
  inline constexpr uint32_t SERIAL_START_DELAY_MS  = 1000;
  inline constexpr uint32_t DISPLAY_INIT_DELAY_MS  = 100;
  inline constexpr uint32_t DISPLAY_CLEAR_DELAY_MS = 100;
  inline constexpr uint32_t INTRO_PAUSE_MS         = 500;

  // Размеры текста
  inline constexpr uint8_t TITLE_TEXT_SIZE = 3;
  inline constexpr uint8_t HINT_TEXT_SIZE  = 1;

  // Позиции заголовка и подсказок
  inline constexpr int16_t TITLE_DICE_X   = 45;
  inline constexpr int16_t TITLE_DICE_Y   = 30;
  inline constexpr int16_t TITLE_ROLLER_X = 25;
  inline constexpr int16_t TITLE_ROLLER_Y = 65;

  inline constexpr int16_t HINT_LINE1_X   = 45;
  inline constexpr int16_t HINT_LINE1_Y   = 105;
  inline constexpr int16_t HINT_LINE2_X   = 60;
  inline constexpr int16_t HINT_LINE2_Y   = 115;
}

namespace Sound {
  // Звук "клика" для каждого кадра анимации
  inline constexpr int ANIM_TICK_FREQ = 2200;
  inline constexpr int ANIM_TICK_DURATION = 20; // мс

  // Звук тревоги
  inline constexpr int ALERT_FREQ = 2000;
  inline constexpr int ALERT_TONE_DURATION = 250;

  // Звук нажатия кнопки
  inline constexpr int BUTTON_CLICK_FREQ = 1500;
  inline constexpr int BUTTON_CLICK_DURATION = 50;

#include "rock_1.h"

  // --- Управление набором мелодий ---
  // 1. Массив указателей на все мелодии
  inline const int* const MELODIES[] = {
    OMNI_STYLE_LOOP
  };

  // 2. Массив с количеством нот для каждой мелодии
  inline constexpr int MELODY_NOTE_COUNTS[] = {
    OMNI_STYLE_LOOP_SIZE
  };
  
  // 3. Общее количество доступных мелодий
  inline constexpr int TOTAL_MELODIES = sizeof(MELODIES) / sizeof(MELODIES[0]);

  // Звук подтверждения смены мелодии
  inline constexpr int CHANGE_MELODY_FREQ = 2500;
  inline constexpr int CHANGE_MELODY_DURATION = 100;

  inline constexpr int NOTE_PAUSE_BETWEEN = 50; // Пауза между нотами

  // Коэффициент масштабирования темпа. 1.0 = оригинал, > 1.0 = медленнее, < 1.0 = быстрее.
  inline constexpr float TEMPO_SCALE = 1.5f;
}

} // namespace Config