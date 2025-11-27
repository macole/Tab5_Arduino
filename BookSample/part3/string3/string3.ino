void setup() {
  Serial.begin(115200);

  String str = "abcdefgh";
  Serial.print("compareTo(\"abcdf\") = ");
  Serial.println(str.compareTo("abcdg"));
  Serial.print("str.endsWith(\"fgh\") = ");
  Serial.println(str.endsWith("fgh"));
  Serial.print("str.equals(\"abcde\") = ");
  Serial.println(str.equals("abcde"));
  Serial.print("str.equalsIgnoreCase(\"AbCdEfGh\") = ");
  Serial.println(str.equalsIgnoreCase("AbCdEfGh"));
  Serial.print("str.indexOf(\"def\") = ");
  Serial.println(str.indexOf("def"));
  Serial.print("str.lastIndexOf(\"def\") = ");
  Serial.println(str.lastIndexOf("def"));
}

void loop() {
}
