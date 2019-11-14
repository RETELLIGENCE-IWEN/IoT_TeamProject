// Retelligence Iwen
// Isaac Kim
// Based on default BT example for esp32 provided by Arduino.cc
// KOREA AEROSPACE UNIV.
// IoT Team Project
// Home BlackBox


#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT; // BT Handler

const int xpin = 36; // x-axis of the accelerometer
const int ypin = 39; // y-axis
const int zpin = 34; // z-axis

float acc_X = 0;
float acc_Y = 0;
float acc_Z = 0;
int   _A_   = 0;

float acc_ThreshHold = 0;

bool Security_State = false;  // true : security on
unsigned char Last_cmd = NULL;

// Device Info
String DeviceCodeName = "ESM0";
String Device_BT_ADDR;
String __ = "=";

// SET UP
void setup()
{
  Serial.begin(115200);

  SerialBT.begin(DeviceCodeName); //Bluetooth device name
  Serial.print("Entrance Security Module [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] Online");

  pinMode(3, OUTPUT); // LED OUTPUT
  pinMode(5, OUTPUT); // LED OUTPUT


  // Get Device BT ADDR
  Device_BT_ADDR = "00:11:22:33:FF:EE";


}


// Check Movement Occurrence for 1 Time
int CheckMovement() {

  int x = analogRead(xpin); //read from xpin
  delay(1); //
  int y = analogRead(ypin); //read from ypin
  delay(1);
  int z = analogRead(zpin); //read from zpin

  float zero_G = 512.0; //ADC is 0~1023 the zero g output equal to Vs/2
  float scale = 102.3; //ADXL335330 Sensitivity is 330mv/g
  //330 * 1024/3.3/1000

  acc_X = ((float)x - 331.5) / 65 * 9.8;
  acc_Y = ((float)y - 329.5) / 68.5 * 9.8;
  acc_Z = ((float)z - 340) / 68 * 9.8;

  //  Serial.print(((float)x - 331.5) / 65 * 9.8); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(((float)y - 329.5) / 68.5 * 9.8); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(((float)z - 340) / 68 * 9.8); //print z value on serial monitor
  //  Serial.print("\n");

  if (acc_X > acc_ThreshHold) return 1;
  if (acc_Y > acc_ThreshHold) return 2;
  if (acc_Z > acc_ThreshHold) return 3;
  return 0;
}




int ReadBT() {
  if (SerialBT.available()) {
    char BTcmd = (SerialBT.read());
    delay(50);
  }

  // Send Ack
  return 1;
}


void SecurityAlert(int AlertAxis) {
  while (!SerialBT.available()) {
    delay(50);
    Serial.print("BT Connection not available - ");
    Serial.println(DeviceCodeName);
  }
  String State  = "E";
  String S_Code = "0130"; // *** Requires refactoring
  String StateMSG = String(DeviceCodeName + __ + Device_BT_ADDR + __ + State + __ + S_Code);
  //  SerialBT.write(StateMSG);
}





void loop()
{



  // CHECK FOR COMMANDS //////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch (ReadBT()) {

    case 0:
      // No Issue or Command
      break;

    case 1:
      // Unable To Recieve BT Signals
      Serial.print("Unable To Recieve BT Signals - ");
      Serial.println(DeviceCodeName);
      break;

    case 2:
      // Turn On Security Mode
      Serial.print("Turning On Security Mode - ");
      Serial.println(DeviceCodeName);
      Security_State = true;

    case 3:
      // Turn Off Security Mode
      Serial.print("Turning Off Security Mode -");
      Serial.println(DeviceCodeName);
      Security_State = false;

    default:
      break;
  }



  if (Security_State) {
    
    // CHECK FOR INVASION //////////////////////////////////////////////////////////////////////////////////////////////////////////
    _A_ = CheckMovement();
    switch (_A_) {
      case 0:
        delay(1000);
        break;

      default:
        SecurityAlert(_A_);
        break;
    }



  }



}
