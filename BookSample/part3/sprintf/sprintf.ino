void setup() {
  Serial.begin(115200);

  char buf[50];
  int i = 12345, j = 123;
  unsigned int ui = 34567;
  long l = 1234567890, l2 = 1234567;
  unsigned long ul = 4000000000;
  char ch = 'a';
  char str[] = "abcde";

  // int型を変換
  sprintf(buf, "int %d", i);
  Serial.println(buf);
  // int型を6文字にして変換
  sprintf(buf, "int %6d", i);
  Serial.println(buf);
  // int型を6文字にして先頭に0を詰めて変換
  sprintf(buf, "int %06d", i);
  Serial.println(buf);
  // unsigned int型を変換
  sprintf(buf, "unsigned int %u", ui);
  Serial.println(buf);
  // unsigned int型を6文字にして変換
  sprintf(buf, "unsigned int %6u", ui);
  Serial.println(buf);
  // unsigned int型を6文字にして先頭に0を詰めて変換
  sprintf(buf, "unsigned int %06u", ui);
  Serial.println(buf);
  // long型を変換
  sprintf(buf, "long %ld", l);
  Serial.println(buf);
  // long型を12文字にして変換
  sprintf(buf, "long %12ld", l);
  Serial.println(buf);
  // long型を12文字にして先頭に0を詰めて変換
  sprintf(buf, "long %012ld", l);
  Serial.println(buf);
  // unsigned long型を変換
  sprintf(buf, "unsigned long %lu", ul);
  Serial.println(buf);
  // unsigned long型を12文字にして変換
  sprintf(buf, "unsigned long %12lu", ul);
  Serial.println(buf);
  // unsigned long型を12文字にして先頭に0を詰めて変換
  sprintf(buf, "unsigned long %012lu", ul);
  Serial.println(buf);
  // char型を変換
  sprintf(buf, "char %c", ch);
  Serial.println(buf);
  // unsigned int型を16進数に変換
  sprintf(buf, "unsigned int hex %X", j);
  Serial.println(buf);
  // unsigned int型を4文字の16進数に変換
  sprintf(buf, "unsigned int hex %4X", j);
  Serial.println(buf);
  // unsigned int型を4文字の16進数に変換して先頭に0を詰める
  sprintf(buf, "unsigned int hex %04X", j);
  Serial.println(buf);
  // unsigned long型を16進数に変換
  sprintf(buf, "unsigned long hex %lx", j);
  Serial.println(buf);
  // unsigned long型を8文字の16進数に変換
  sprintf(buf, "unsigned long hex %8lx", j);
  Serial.println(buf);
  // unsigned int型を8文字の16進数に変換して先頭に0を詰める
  sprintf(buf, "unsigned long hex %08lx", j);
  Serial.println(buf);
  // char型の配列（文字列）を変換
  sprintf(buf, "string %s", str);
  Serial.println(buf);
  // char型の配列（文字列）を10文字に変換
  sprintf(buf, "string %10s", str);
  Serial.println(buf);
}

void loop() {
  // put your main code here, to run repeatedly:

}
