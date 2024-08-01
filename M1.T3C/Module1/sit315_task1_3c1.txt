// 定义引脚
const int buttonPin = 2;
const int tempSensor1Pin = A0;
const int tempSensor2Pin = A1;
const int ledPin = 13;

volatile bool buttonPressed = false;
bool checkFirstSensor = true; // 用于切换温度传感器的状态

void setup() {
  // 初始化串口通信
  Serial.begin(9600);

  // 初始化引脚
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(tempSensor1Pin, INPUT);
  pinMode(tempSensor2Pin, INPUT);
  pinMode(ledPin, OUTPUT);

  // 附加中断
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
}

void loop() {
  // 检查按钮按下
  if (buttonPressed) {
    buttonPressed = false; // 重置标志

    if (checkFirstSensor) {
      // 读取第一个温度传感器值
      int tempValue1 = analogRead(tempSensor1Pin);
      float voltage1 = tempValue1 * 5.0 / 1024;
      float temperatureC1 = (voltage1 - 0.5) * 100;
      Serial.print("Temperature 1: ");
      Serial.print(temperatureC1);
      Serial.println(" C");

      // 检查温度是否大于30度
      if (temperatureC1 > 30) {
        digitalWrite(ledPin, HIGH); // 打开LED
      } else {
        digitalWrite(ledPin, LOW); // 关闭LED
      }
    } else {
      // 读取第二个温度传感器值
      int tempValue2 = analogRead(tempSensor2Pin);
      float voltage2 = tempValue2 * 5.0 / 1024;
      float temperatureC2 = (voltage2 - 0.5) * 100;
      Serial.print("Temperature 2: ");
      Serial.print(temperatureC2);
      Serial.println(" C");

      // 检查温度是否大于30度
      if (temperatureC2 > 30) {
        digitalWrite(ledPin, HIGH); // 打开LED
      } else {
        digitalWrite(ledPin, LOW); // 关闭LED
      }
    }

    // 切换状态
    checkFirstSensor = !checkFirstSensor;
  }
}

// 按钮的中断服务程序
void buttonISR() {
  buttonPressed = true;
}
