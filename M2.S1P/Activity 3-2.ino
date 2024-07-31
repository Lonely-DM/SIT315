const byte LED_PIN = 13;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  startTimer(0.5); // 以0.5 Hz（2秒周期）启动
}

void loop()
{
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
