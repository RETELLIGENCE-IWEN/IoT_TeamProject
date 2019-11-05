// Retelligence Iwen
// Isaac Kim


#include <SoftwareSerial.h>

SoftwareSerial bluetooth(8, 9); // 8번 TX, 9번 RX

void setup()
{
  Serial.begin(9600);
  bluetooth.begin(9600);

  pinMode(3, OUTPUT); // LED OUTPUT
  pinMode(5, OUTPUT); // LED OUTPUT
}

void loop()
{
  char val = bluetooth.read();

  if (bluetooth.available())
  {
    Serial.write(bluetooth.read());
  }

  if(val == 'a')
  {
    digitalWrite(3,HIGH);
  }

  if(val == 'b')
  {
    digitalWrite(5, HIGH);
  }

  else if(val == 'c')
  {
    digitalWrite(3, LOW);
    digitalWrite(5, LOW);
  }
  
}
