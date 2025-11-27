void setup() {
  Serial.begin(115200);

  String str = "abcdefgh";
  str.concat("ijk");  // strに「ijk」を連結
  Serial.print("concat(\"ijk\") = ");
  Serial.println(str);
  str = "abcdefgh";
  str.remove(2, 5);   // strの2文字目から5文字削除
  Serial.print("remove(2, 5) = ");
  Serial.println(str);
  str = "abcdefgh";   // strに含まれる「def」を「ijk」に置換
  str.replace("def", "ijk");
  Serial.print("replace(\"def\", \"ijk\") = ");
  Serial.println(str);
  str = "abcdefgh";   // strの4文字目を「i」に置換
  str.setCharAt(4, 'i');
  Serial.print("setCharAt(4, 'i') = ");
  Serial.println(str);
  str = "abcdefgh";   // strのアルファベット小文字を大文字に変換
  str.toUpperCase();
  Serial.print("toUpperCase() = ");
  Serial.println(str);
}

void loop() {
}
