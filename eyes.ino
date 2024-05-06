#include <Servo.h>

// Пины для подключения сервомоторов
const int servoLeftRightPin = 5;         // пин для серво поворота влево-вправо
const int servoUpDownPin = 4;            // пин для серво поворота вверх-вниз
const int servoRightUpperEyelidPin = 3;  // пин для верхнего века правого глаза
const int servoRightLowerEyelidPin = 2;  // пин для нижнего века правого глаза
const int servoLeftUpperEyelidPin = 6;   // пин для верхнего века левого глаза
const int servoLeftLowerEyelidPin = 7;   // пин для нижнего века левого глаза
const int servoHeadLeftRightPin = 8;     // пин поворота головы

// Объекты для управления сервомоторами
Servo servoLeftRight;
Servo servoUpDown;
Servo servoRightUpperEyelid;
Servo servoRightLowerEyelid;
Servo servoLeftUpperEyelid;
Servo servoLeftLowerEyelid;
Servo servoHeadLeftRight;

unsigned long lastDataTime = 0;   // Переменная для отслеживания времени последнего обновления данных
unsigned long lastBlinkTime = 0;  // Время последнего моргания
int blinkInterval = 3000;         // Интервал между морганиями (в миллисекундах)

// Переменные поворота серв
float valSmoothed;
float valPrev;

void setup() {
  // Устанавливаем пины сервомоторов как выходы
  pinMode(servoLeftRightPin, OUTPUT);
  pinMode(servoUpDownPin, OUTPUT);
  pinMode(servoRightUpperEyelidPin, OUTPUT);
  pinMode(servoRightLowerEyelidPin, OUTPUT);
  pinMode(servoLeftUpperEyelidPin, OUTPUT);
  pinMode(servoLeftLowerEyelidPin, OUTPUT);
  pinMode(servoHeadLeftRightPin, OUTPUT);

  // Прикрепляем объекты к пинам
  servoLeftRight.attach(servoLeftRightPin);
  servoUpDown.attach(servoUpDownPin);
  servoRightUpperEyelid.attach(servoRightUpperEyelidPin);
  servoRightLowerEyelid.attach(servoRightLowerEyelidPin);
  servoLeftUpperEyelid.attach(servoLeftUpperEyelidPin);
  servoLeftLowerEyelid.attach(servoLeftLowerEyelidPin);
  servoHeadLeftRight.attach(servoHeadLeftRightPin);

  // Инициализируем Serial порт
  Serial.begin(9600);

  // Поворачиваем голову прямо
  servoHeadLeftRight.write(90);

  // Установка начального положения глаз
  moveEyes(90, 90);  // Положение по умолчанию

  // Опускаем верхние и поднимаем нижние веки
  servoRightUpperEyelid.write(30);
  servoLeftUpperEyelid.write(30);
  servoRightLowerEyelid.write(150);
  servoLeftLowerEyelid.write(150);
  delay(200);  // Задержка для создания эффекта моргания
  // Поднимаем верхние и опускаем нижние веки
  servoRightUpperEyelid.write(90);
  servoLeftUpperEyelid.write(90);
  servoRightLowerEyelid.write(90);
  servoLeftLowerEyelid.write(90);
  delay(10000);
}

void loop() {
  // Проверяем, есть ли данные в Serial порте
  if (Serial.available() >= 7) {
    // Если данные есть, читаем их и обновляем переменную lastDataTime
    int delta_x = Serial.parseInt();
    int delta_y = Serial.parseInt();
    Serial.read();            // Пропускаем запятую
    lastDataTime = millis();  // Обновляем время последнего обновления данных

    // Переводим значения в углы поворота для сервомоторов
    int servoAngleX = map(delta_x, -320, 320, 160, 20);  // Для оси X
    int servoAngleY = map(delta_y, -240, 240, 160, 20);  // Для оси Y

    // Поворачиваем сервомоторы в соответствии с углами
    moveEyes(servoAngleX, servoAngleY);
    servoHeadLeftRight.write(turningServoSmoothed(servoAngleX));
  } else {
    // Если данные не поступают в течение определенного времени, возвращаем глаза в положение 90
    if (millis() - lastDataTime > 1000) {  // Если прошло более 1 секунды с последнего обновления данных
      moveEyes(90, 90);                    // Возвращаем в исходное положение
      servoHeadLeftRight.write(turningServoSmoothed(90));
    }
  }

  // Проверяем бездействие для моргания глазами
  if (millis() - lastDataTime > 10000) {  // Если прошло более 10 секунд бездействия
    autoBlink();
  }

  delay(50);  // Небольшая задержка между итерациями цикла
}

// Функция для поворота глаз
void moveEyes(int x, int y) {
  // Ограничиваем углы поворота глаз
  x = constrain(x, 20, 160);
  y = constrain(y, 20, 160);

  // Поворот глаз
  servoLeftRight.write(x);
  servoUpDown.write(y);

  servoHeadLeftRight.write(50);


}

int turningServoSmoothed(int goal) {
  // Переменные поворота головы

  valSmoothed = map(((goal * 0.02) + (valPrev * 0.98)), 20, 160, 65, 120);
  valPrev = valSmoothed;
  return valSmoothed;
}

// Функция для автоматического моргания глаз:
void autoBlink() {
  unsigned long currentTime = millis();

  if (currentTime - lastBlinkTime >= blinkInterval) {
    servoRightUpperEyelid.write(110);
    servoLeftUpperEyelid.write(30);
    servoRightLowerEyelid.write(60);
    servoLeftLowerEyelid.write(150);

    delay(100);  // Задержка для создания эффекта моргания

    servoRightUpperEyelid.write(90);
    servoLeftUpperEyelid.write(90);
    servoRightLowerEyelid.write(90);
    servoLeftLowerEyelid.write(90);

    lastBlinkTime = currentTime;  // Обновляем время последнего моргания
  }
}
