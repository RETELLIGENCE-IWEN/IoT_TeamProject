const int xpin = A0; // x-axis of the accelerometer
const int ypin = A3; // y-axis
const int zpin = A4; // z-axis

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  int x = analogRead(xpin); //read from xpin
  delay(1); //
  int y = analogRead(ypin); //read from ypin
  delay(1);
  int z = analogRead(zpin); //read from zpin

  float zero_G = 512.0; //ADC is 0~1023 the zero g output equal to Vs/2
  float scale = 102.3; //ADXL335330 Sensitivity is 330mv/g
  //330 * 1024/3.3/1000
  Serial.print(((float)x - 331.5) / 65 * 9.8); //print x value on serial monitor
  Serial.print("\t");
  Serial.print(((float)y - 329.5) / 68.5 * 9.8); //print y value on serial monitor
  Serial.print("\t");
  Serial.print(((float)z - 340) / 68 * 9.8); //print z value on serial monitor
  Serial.print("\n");
  delay(1000); //wait for 1 second
}
