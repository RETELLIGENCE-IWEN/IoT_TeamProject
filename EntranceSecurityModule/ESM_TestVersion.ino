// Retelligence Iwen
// Isaac Kim
// Based on default BT example for esp32
// KOREA AEROSPACE UNIV.
// IoT Team Project
// Home BlackBox

// Entrance Security Module
// ESM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
esp_now_peer_info_t master[2] = {};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ESM_SLEEP      55
#define ESM_OFF        11
#define ESM_ON         22
#define ESM_ACTIVATE   99
#define ESM_DEACTIVATE 88
#define ESM_NOISSUE    77
#define ESM_S_ISSUE    44

#define ESM_index      10


int alert_count_x = 0;
int alert_count_y = 0;
int alert_count_z = 0;

char MASMAC[18] = "a4:cf:12:6c:3e:dc";// Retelligence Iwen
// Isaac Kim
// Based on default BT example for esp32
// KOREA AEROSPACE UNIV.
// IoT Team Project
// Home BlackBox

// Entrance Security Module
// ESM

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1
esp_now_peer_info_t master[2] = {};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ESM_SLEEP      55
#define ESM_OFF        11
#define ESM_ON         22
#define ESM_ACTIVATE   99
#define ESM_DEACTIVATE 88
#define ESM_NOISSUE    77
#define ESM_S_ISSUE    44

#define ESM_index      10


int alert_count_x = 0;
int alert_count_y = 0;
int alert_count_z = 0;

//char MASMAC[18] = "a4:cf:12:6c:3e:dc";
char MASMAC[18] = "80:7d:3a:ba:3a:88";

const int sLED = 33;
const int mLED = 32;

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

uint8_t Sdata = 0;

int MoniteringState = ESM_NOISSUE;
int SecurityState = ESM_DEACTIVATE;

float acc_ThreshHold = 8.4;

bool Security_State = false;  // true : security on
unsigned char Last_cmd = NULL;

String __ = "=";
String DeviceCodeName = "ESM9";

bool wasAlert = false;
bool handwasAlert = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RessetESM() {

  delay(500);

  int x = analogRead(xpin); //read from xpin
  delay(1); //
  int y = analogRead(ypin); //read from ypin
  delay(1);
  int z = analogRead(zpin); //read from zpin

  float zero_G = 512.0; //ADC is 0~1023 the zero g output equal to Vs/2
  float scale = 102.3; //ADXL335330 Sensitivity is 330mv/g
  //330 * 1024/3.3/1000

  G_acc_X = ((float)x - 331.5) / 65 * 9.8;
  G_acc_Y = ((float)y - 329.5) / 68.5 * 9.8;
  G_acc_Z = ((float)z - 340) / 68 * 9.8;

  //  Serial.println("\n[  Base acc Value  ]");
  //  Serial.print(G_acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Z); //print z value on serial monitor
  //  Serial.print("\n\n");

  delay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check Movement Occurrence for 1 Time
int CheckMovement(int what) {

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

  //  Serial.print(G_acc_X - acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
  //  Serial.print("\n");

  //  Serial.print(acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(acc_Z); //print z value on serial monitor
  //  Serial.print("\n");
  delay(3);

  if (abs(G_acc_X - acc_X) > acc_ThreshHold && what == 1) {
    alert_count_x += 1;
    if (alert_count_x > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 1;
  }
  if (abs(G_acc_Y - acc_Y) > acc_ThreshHold && what == 2) {
    alert_count_y += 1;
    if (alert_count_y > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 2;
  }
  if (abs(G_acc_Z - acc_Z) > acc_ThreshHold && what == 3) {
    alert_count_z += 1;
    if (alert_count_z > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 3;
  }

  alert_count_x = 0;
  alert_count_y = 0;
  alert_count_z = 0;

  return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SecurityAlert(int AlertAxis) {

  if (alert_count_x > 2) MoniteringState = ESM_S_ISSUE;
  if (alert_count_y > 2) MoniteringState = ESM_S_ISSUE;
  if (alert_count_z > 2) MoniteringState = ESM_S_ISSUE;

  if (MoniteringState == ESM_S_ISSUE) {
    Serial.println("Security Alert!  ");
    wasAlert = true;
    digitalWrite(mLED, HIGH);
  } else {
    digitalWrite(mLED, LOW);
  }


}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// config AP SSID
void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_STA);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  Serial.print("\n\nEntrance Security Module [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] Online");
  pinMode(sLED, OUTPUT);
  pinMode(mLED, OUTPUT);

  digitalWrite(sLED, HIGH);
  delay(1000);
  digitalWrite(sLED, LOW);


  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();

  Serial.print("\n\nEntrance Security Module [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] ACTIVATED");



  digitalWrite(sLED, HIGH);
  delay(1000);
  digitalWrite(sLED, LOW);

  MoniteringState = ESM_NOISSUE;
  Sdata = 77;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  Serial.println("Data RECV callback function");
  char macStr[18];
//
//for(int k = 0; k < 6; k++){
//  Serial.print("mac_addr: ");
//  Serial.println(mac_addr[k]);
//}
//  
//  Serial.print("\n");


  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  //  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);


  //    Serial.println("FU");
  //  if ( strcmp(macStr, MASMAC) != 0) {
  //    //    Serial.println(macStr);
  //    //    Serial.println("msg From no master");
  //    //    Serial.println(MASMAC);
  //    return;
  //  } else {


  Serial.println("");
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);

  if (*data == 99) {
    SecurityState = ESM_ACTIVATE;
    Serial.println("------------------------------");
    Serial.println("-------- S ACTIVATEED --------");
    Serial.println("------------------------------");
  } else if (*data == 88) {
    SecurityState = ESM_DEACTIVATE;
    Serial.println("------------------------------");
    Serial.println("------ S DE-ACTIVATEED -------");
    Serial.println("------------------------------");
  }



  uint8_t MasterADDR[18];
  for (int ii = 0; ii < 6; ii++ ) {
    MasterADDR[ii] = (uint8_t) macStr[ii];
    master[0].peer_addr[ii] = (uint8_t) macStr[ii];
  }

  master[0].channel = 1;
  master[0].encrypt = 0;


  bool exists = esp_now_is_peer_exist(MasterADDR);
  if (exists) {
    // Slave already paired.
    Serial.println("Already Paired");
  } else {
    // Slave not paired, attempt pair
    esp_err_t addStatus = esp_now_add_peer(&master[0]);
    if (addStatus == ESP_OK) {
      // Pair success
      Serial.println("Pair success");
    } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW Not Init");
    } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Add Peer - Invalid Argument");
    } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
      Serial.println("Peer list full");
    } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("Out of memory");
    } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
      Serial.println("Peer Exists");
    } else {
      Serial.println("Not sure what happened");
    }
  }


  if (wasAlert) {
    Sdata = 44;
    wasAlert = false;
  }
  //  const char Sdata = "ESM Bidirectional Positive";
  Serial.print("Sending: ");
  Serial.println(Sdata);
  for (int j = 0; j < 6; j++) {
    Serial.print(MasterADDR[j]);
    Serial.print(" ");
  }
  Serial.println("\n");
  //  Serial.println(*MasterADDR);
  //  Serial.println("\n");
  esp_err_t result = esp_now_send(MasterADDR, &Sdata, sizeof(Sdata));
  //  esp_now_send(MasterADDR, &Sdata, sizeof(Sdata));
  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
  delay(100);
  //  }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  if (SecurityState == ESM_ACTIVATE) {
    digitalWrite(sLED, HIGH);

    _A_ = CheckMovement(3);
    switch (_A_) {
      case 0:
        MoniteringState = ESM_NOISSUE;
        // No Issue
        break;

      default:
        SecurityAlert(_A_);
        break;
    }
    Sdata = MoniteringState;
    // Chill
    delay(50);
  }



  else {
    digitalWrite(sLED, LOW);
    delay(1000);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//char MASMAC[18] = "80:7d:3a:ba:3a:88";

const int sLED = 33;
const int mLED = 32;

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

uint8_t Sdata = 0;

int MoniteringState = ESM_NOISSUE;
int SecurityState = ESM_DEACTIVATE;

float acc_ThreshHold = 8.4;

bool Security_State = false;  // true : security on
unsigned char Last_cmd = NULL;

String __ = "=";
String DeviceCodeName = "ESM9";

bool wasAlert = false;
bool handwasAlert = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RessetESM() {

  delay(500);

  int x = analogRead(xpin); //read from xpin
  delay(1); //
  int y = analogRead(ypin); //read from ypin
  delay(1);
  int z = analogRead(zpin); //read from zpin

  float zero_G = 512.0; //ADC is 0~1023 the zero g output equal to Vs/2
  float scale = 102.3; //ADXL335330 Sensitivity is 330mv/g
  //330 * 1024/3.3/1000

  G_acc_X = ((float)x - 331.5) / 65 * 9.8;
  G_acc_Y = ((float)y - 329.5) / 68.5 * 9.8;
  G_acc_Z = ((float)z - 340) / 68 * 9.8;

  //  Serial.println("\n[  Base acc Value  ]");
  //  Serial.print(G_acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Z); //print z value on serial monitor
  //  Serial.print("\n\n");

  delay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check Movement Occurrence for 1 Time
int CheckMovement(int what) {

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

  //  Serial.print(G_acc_X - acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
  //  Serial.print("\n");

  //  Serial.print(acc_X); //print x value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(acc_Y); //print y value on serial monitor
  //  Serial.print("\t");
  //  Serial.print(acc_Z); //print z value on serial monitor
  //  Serial.print("\n");
  delay(3);

  if (abs(G_acc_X - acc_X) > acc_ThreshHold && what == 1) {
    alert_count_x += 1;
    if (alert_count_x > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 1;
  }
  if (abs(G_acc_Y - acc_Y) > acc_ThreshHold && what == 2) {
    alert_count_y += 1;
    if (alert_count_y > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 2;
  }
  if (abs(G_acc_Z - acc_Z) > acc_ThreshHold && what == 3) {
    alert_count_z += 1;
    if (alert_count_z > 2) {
      Serial.print(G_acc_X - acc_X); //print x value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Y - acc_Y); //print y value on serial monitor
      Serial.print("\t");
      Serial.print(G_acc_Z - acc_Z); //print z value on serial monitor
      Serial.print("\n");
    }
    return 3;
  }

  alert_count_x = 0;
  alert_count_y = 0;
  alert_count_z = 0;

  return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SecurityAlert(int AlertAxis) {

  if (alert_count_x > 2) MoniteringState = ESM_S_ISSUE;
  if (alert_count_y > 2) MoniteringState = ESM_S_ISSUE;
  if (alert_count_z > 2) MoniteringState = ESM_S_ISSUE;

  if (MoniteringState == ESM_S_ISSUE) {
    Serial.println("Security Alert!  ");
    wasAlert = true;
    digitalWrite(mLED, HIGH);
  } else {
    digitalWrite(mLED, LOW);
  }


}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// config AP SSID
void configDeviceAP() {
  String Prefix = "Slave:";
  String Mac = WiFi.macAddress();
  String SSID = Prefix + Mac;
  String Password = "123456789";
  bool result = WiFi.softAP(SSID.c_str(), Password.c_str(), CHANNEL, 0);
  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);

  //Set device in AP mode to begin with
  WiFi.mode(WIFI_STA);
  // configure device AP mode
  configDeviceAP();
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: "); Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);

  Serial.print("\n\nEntrance Security Module [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] Online");
  pinMode(sLED, OUTPUT);
  pinMode(mLED, OUTPUT);

  digitalWrite(sLED, HIGH);
  delay(1000);
  digitalWrite(sLED, LOW);


  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();
  RessetESM();

  Serial.print("\n\nEntrance Security Module [ ");
  Serial.print(DeviceCodeName);
  Serial.println(" ] ACTIVATED");



  digitalWrite(sLED, HIGH);
  delay(1000);
  digitalWrite(sLED, LOW);

  MoniteringState = ESM_NOISSUE;
  Sdata = 77;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  Serial.println("Data RECV callback function");
  char macStr[18];
//
//for(int k = 0; k < 6; k++){
//  Serial.print("mac_addr: ");
//  Serial.println(mac_addr[k]);
//}
//  
//  Serial.print("\n");


  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  //  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);


  //    Serial.println("FU");
  //  if ( strcmp(macStr, MASMAC) != 0) {
  //    //    Serial.println(macStr);
  //    //    Serial.println("msg From no master");
  //    //    Serial.println(MASMAC);
  //    return;
  //  } else {


  Serial.println("");
  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);

  if (*data == 99) {
    SecurityState = ESM_ACTIVATE;
    Serial.println("------------------------------");
    Serial.println("-------- S ACTIVATEED --------");
    Serial.println("------------------------------");
  } else if (*data == 88) {
    SecurityState = ESM_DEACTIVATE;
    Serial.println("------------------------------");
    Serial.println("------ S DE-ACTIVATEED -------");
    Serial.println("------------------------------");
  }



  uint8_t MasterADDR[18];
  for (int ii = 0; ii < 6; ii++ ) {
    MasterADDR[ii] = (uint8_t) macStr[ii];
    master[0].peer_addr[ii] = (uint8_t) macStr[ii];
  }

  master[0].channel = 1;
  master[0].encrypt = 0;


  bool exists = esp_now_is_peer_exist(MasterADDR);
  if (exists) {
    // Slave already paired.
    Serial.println("Already Paired");
  } else {
    // Slave not paired, attempt pair
    esp_err_t addStatus = esp_now_add_peer(&master[0]);
    if (addStatus == ESP_OK) {
      // Pair success
      Serial.println("Pair success");
    } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      Serial.println("ESPNOW Not Init");
    } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Add Peer - Invalid Argument");
    } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
      Serial.println("Peer list full");
    } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("Out of memory");
    } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
      Serial.println("Peer Exists");
    } else {
      Serial.println("Not sure what happened");
    }
  }


  if (wasAlert) {
    Sdata = 44;
    wasAlert = false;
  }
  //  const char Sdata = "ESM Bidirectional Positive";
  Serial.print("Sending: ");
  Serial.println(Sdata);
  for (int j = 0; j < 6; j++) {
    Serial.print(MasterADDR[j]);
    Serial.print(" ");
  }
  Serial.println("\n");
  //  Serial.println(*MasterADDR);
  //  Serial.println("\n");
  esp_err_t result = esp_now_send(MasterADDR, &Sdata, sizeof(Sdata));
  //  esp_now_send(MasterADDR, &Sdata, sizeof(Sdata));
  Serial.print("Send Status: ");
  if (result == ESP_OK) {
    Serial.println("Success");
  }
  else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
    // How did we get so far!!
    Serial.println("ESPNOW not Init.");
  } else if (result == ESP_ERR_ESPNOW_ARG) {
    Serial.println("Invalid Argument");
  } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
    Serial.println("Internal Error");
  } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
    Serial.println("ESP_ERR_ESPNOW_NO_MEM");
  } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
    Serial.println("Peer not found.");
  } else {
    Serial.println("Not sure what happened");
  }
  delay(100);
  //  }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {

  if (SecurityState == ESM_ACTIVATE) {
    digitalWrite(sLED, HIGH);

    _A_ = CheckMovement(3);
    switch (_A_) {
      case 0:
        MoniteringState = ESM_NOISSUE;
        // No Issue
        break;

      default:
        SecurityAlert(_A_);
        break;
    }
    Sdata = MoniteringState;
    // Chill
    delay(50);
  }



  else {
    digitalWrite(sLED, LOW);
    delay(1000);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
