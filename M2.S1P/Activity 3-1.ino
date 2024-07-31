const byte LED_PIN = 13;

void setup()
{
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  startTimer();
}

void loop()
{
}

void startTimer()
{
  noInterrupts(); // 禁用中断

  // 设置比较匹配值，使计时器每2秒中断一次
  // 16 MHz / 1024 (预分频器) / 2 (2秒的频率) - 1
  OCR1A = 15624; 

  // 重置计时器控制寄存器
  TCCR1A = 0;
  TCCR1B = 0;

  // 设置CTC模式 (清除计时器在比较匹配时)
  TCCR1B |= (1 << WGM12);

  // 设置预分频器为1024
  TCCR1B |= (1 << CS12) | (1 << CS10);

  // 启用计时器比较中断
  TIMSK1 |= (1 << OCIE1A);

  interrupts(); // 启用中断
}

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1); // 切换LED状态
}
