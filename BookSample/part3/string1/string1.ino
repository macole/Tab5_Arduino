void setup() {
  Serial.begin(115200);

  String str = "abcdefgh", subs;
  char ch;
  int len;

  ch = str.charAt(3);           // strの3文字目を得る
  subs = str.substring(2, 5);   // strの2文字目から(5-1)文字目を得る
  len = str.length();           // strの長さを得る
  Serial.print("charAt(3) = ");
  Serial.println(ch);
  Serial.print("substring(2, 5) = ");
  Serial.println(subs);
  Serial.print("length() = ");
  Serial.println(len);
}

void loop() {
}
