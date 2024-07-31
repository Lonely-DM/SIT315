const byte LED_PIN = 13;
const byte METER_PIN = A4;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  pinMode(METER_PIN, INPUT);
  Serial.begin(9600);
  
  // 以默认频率启动计时器
  startTimer(0.5); // 0.5 Hz (2秒周期)
}

void loop()
{
  // 读取电位计的值
  int sensorValue = analogRead(METER_PIN);

  // 将传感器值映射到频率范围（例如：0.1 Hz 到 2 Hz）
  double timerFrequency = map(sensorValue, 0, 1023, 1, 20) / 10.0;

  // 启动具有新频率的计时器
  startTimer(timerFrequency);

  delay(1000); // 每秒调整一次频率
}

void startTimer(double timerFrequency)
{
  noInterrupts(); // 禁用中断

  // 计算 OCR1A 寄存器的值
  // 计时器1是一个16位计时器，计时器时钟的频率为16 MHz
  // OCR1A = (16*10^6) / (2*预分频器*timerFrequency) - 1
  // 这里我们使用预分频器1024
  const int prescaler = 1024;
  int ocrValue = (16000000 / (2 * prescaler * timerFrequency)) - 1;

  // 重置计时器1控制寄存器
  TCCR1A = 0;
  TCCR1B = 0;

  // 设置CTC模式（在比较匹配时清除计时器）
  TCCR1B |= (1 << WGM12);

  // 设置预分频器为1024
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // 设置OCR1A值
  OCR1A = ocrValue;

  // 启用计时器1比较中断
  TIMSK1 |= (1 << OCIE1A);

  interrupts(); // 启用中断
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1); // 切换LED状态
}
