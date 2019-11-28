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

float G_acc_X = 0;
float G_acc_Y = 0;
float G_acc_Z = 0;

float acc_X = 0;
float acc_Y = 0;
float acc_Z = 0;
int   _A_   = 0;

float acc_ThreshHold = 2.6;

bool Security_State = true;  // true : security on
unsigned char Last_cmd = NULL;

// Device Info
String DeviceCodeName = "HB01";
String Device_BT_ADDR;
String __ = "=";



// Resset
void RessetESM() {


  delay(500);
}



// SET UP
void setup()
{
  Serial.begin(115200);

  SerialBT.begin(DeviceCodeName); //Bluetooth device name
  Serial.print("\n\nHBB Security HUB [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] Online");

  pinMode(3, OUTPUT); // LED OUTPUT
  pinMode(5, OUTPUT); // LED OUTPUT


  // Get Device BT ADDR
  Device_BT_ADDR = "00:11:22:33:FF:EE";

  delay(3000);
}






int ReadBT() {
  if (SerialBT.available()) {
    char BTcmd = (SerialBT.read());
    delay(50);
  }

  // Send Ack
  return 0;
}


void SecurityAlert(int AlertAxis) {
  Serial.println("Security Alert!  ");
  while (!SerialBT.available()) {
    delay(50);
    Serial.print("BT Connection not available - ");
    Serial.println(DeviceCodeName);
    break;
  }
  String State  = "E";
  String S_Code = "0130"; // *** Requires refactoring
  String StateMSG = String(DeviceCodeName + __ + Device_BT_ADDR + __ + State + __ + S_Code);
  //  SerialBT.write(StateMSG);
  Serial.println(StateMSG);
  Serial.println("\n");
}





void loop()
{


    
}
