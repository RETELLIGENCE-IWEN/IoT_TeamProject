// Retelligence Iwen
// Isaac Kim
// Based on AWS & ESP-now
// KOREA AEROSPACE UNIV.
// IoT Team Project
// Home BlackBox

// HUB
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <WiFi.h>
//#include <String.h>
#include <esp_now.h>
#include <AWS_IOT.h>
#include <ArduinoJson.h>
#include <LinkedList.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LinkedList<int> ESM_Reports = LinkedList<int>();

bool Initialized = false;
bool C_W_E_D_C_A = false;
bool firstPubSuc = false;
bool DoPublishTo = false;
int ESPNOW_BROADCAST_SLEEP_COUNT = 0;
// Global copy of slave
#define NUMSLAVES 20
esp_now_peer_info_t slaves[NUMSLAVES] = {};
int SlaveCnt = 0;

#define CHANNEL 1
#define PRINTSCANRESULTS 0


#define ESM_SLEEP      55
#define ESM_OFF        11
#define ESM_ON         22
#define ESM_ACTIVATE   99
#define ESM_DEACTIVATE 88
#define ESM_NOISSUE    77
#define ESM_S_ISSUE    44

#define ESM_index      10

int MoniteringState = ESM_NOISSUE;
int SecurityState = ESM_DEACTIVATE;

bool souldPUB_MS = true;
bool souldPUB_SS = true;


float acc_ThreshHold = 2.6;
bool noissue = true;
bool Security_State = true;  // true : security on
unsigned char Last_cmd = NULL;

String __ = "=";
String DeviceCodeName = "HUB0";
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AWS_IOT HBB_AWS; // AWS_IOT instance

//char WIFI_SSID[] = "Yoni";
//char WIFI_PASSWORD[] = "88008800";

char WIFI_SSID[] = "KAU-Guest";
char WIFI_PASSWORD[] = "";

//const char *WIFI_SSID = "AndroidHotspot3642";
//const char *WIFI_PASSWORD = "88998899";

char HOST_ADDRESS[] = "a1ewasgidh0jna-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[] = "HBB_sensor1";
char TOPIC_NAME_update[] = "$aws/things/HBB_sensor1/shadow/update";
char TOPIC_NAME_delta[] = "$aws/things/HBB_App/shadow/update/delta";

//uint8_t Slave1[6] = {3c, 71, bf, ff, 64, 6c};
//uint8_t Slave2[6] = {3c, 71, bf, f1, d0, 8c};
//uint8_t Slave3[6] = {80, 7d, 3a, ba, 3a, 88};

int publishCount = 0;
int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];
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



// SCAN SLAVES
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Scan for slaves in AP mode
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  //reset slaves
  memset(slaves, 0, sizeof(slaves));
  SlaveCnt = 0;
  Serial.println("");
  if (scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    //    Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
    for (int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      if (PRINTSCANRESULTS) {
        //        Serial.print(i + 1); Serial.print(": "); Serial.print(SSID); Serial.print(" ["); Serial.print(BSSIDstr); Serial.print("]"); Serial.print(" ("); Serial.print(RSSI); Serial.print(")"); Serial.println("");
      }
      delay(10);
      // Check if the current device starts with `Slave`
      if (SSID.indexOf("Slave") == 0) {
        // SSID of interest
        // Get BSSID => Mac Address of the Slave
        int mac[6];

        if ( 6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5] ) ) {
          for (int ii = 0; ii < 6; ++ii ) {
            slaves[SlaveCnt].peer_addr[ii] = (uint8_t) mac[ii];
          }
        }
        //        if (memcmp(mac, Slave1, sizeof(mac)) == 0 || memcmp(mac, Slave2, sizeof(mac)) == 0 || memcmp(mac, Slave3, sizeof(mac)) == 0) {
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
        //        }
      }
    }
  }

  if (SlaveCnt > 0) {
    Serial.print(SlaveCnt); Serial.println(" Slave(s) found, processing..");
  } else {
    Serial.println("No Slave Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// CONFIG AP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// config AP SSID
void configDeviceAP() {
  String Prefix = "Master:";
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



// MANAGE DELTA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Check if the slave is already paired with the master.
// If not, pair the slave with master
void manageSlave() {
  if (SlaveCnt > 0) {
    for (int i = 0; i < SlaveCnt; i++) {
      //      Serial.print("Processing: ");
      for (int ii = 0; ii < 6; ++ii ) {
        //        Serial.print((uint8_t) slaves[i].peer_addr[ii], HEX);
        //        if (ii != 5) Serial.print(":");
      }
      //      Serial.print(" Status: ");
      // check if the peer exists
      bool exists = esp_now_is_peer_exist(slaves[i].peer_addr);
      if (exists) {
        // Slave already paired.
        Serial.println("Already Paired");
      } else {
        // Slave not paired, attempt pair
        esp_err_t addStatus = esp_now_add_peer(&slaves[i]);
        if (addStatus == ESP_OK) {
          // Pair success
          Serial.println("Pair success");
        } else if (addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
          // How did we get so far!!
          //          Serial.println("ESPNOW Not Init");
        } else if (addStatus == ESP_ERR_ESPNOW_ARG) {
          //          Serial.println("Add Peer - Invalid Argument");
        } else if (addStatus == ESP_ERR_ESPNOW_FULL) {
          //          Serial.println("Peer list full");
        } else if (addStatus == ESP_ERR_ESPNOW_NO_MEM) {
          //          Serial.println("Out of memory");
        } else if (addStatus == ESP_ERR_ESPNOW_EXIST) {
          //          Serial.println("Peer Exists");
        } else {
          Serial.println("Not sure what happened");
        }
        //        delay(100);
      }
    }
  } else {
    // No slave found to process
    Serial.println("No Slave found to process");
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




// SEND DATA
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint8_t data = 100;
// send data
void sendData() {
  data = SecurityState;
  for (int i = 0; i < SlaveCnt; i++) {
    const uint8_t *peer_addr = slaves[i].peer_addr;
    if (i == 0) { // print only for first slave
      Serial.print("Sending: ");
      Serial.println(data);
    }
    //////////////////////////////////
    //////////////////////////////////

    esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
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
    //    delay(100);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback when data is sent from Master to Slave
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //  C_W_E_D_C_A = true;

  // INITIALIZING ARRAY


  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback when data is recv from Slave
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  Serial.println("RECV");
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("!!! Last Packet Recv from ESM -[ ]- : "); Serial.println(macStr);
  Serial.print("!!! Last Packet Recv Data ESM -[ ]- : "); Serial.println(*data);
  Serial.println("");

  MoniteringState = *data;
  ESM_Reports.add(*data);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void callBackDelta(char *topicName, int payloadLen, char *payLoad) {
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
  //  Serial.println("call back function registered");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
  esp_err_t F = esp_now_register_recv_cb(OnDataRecv);
  if (F == ESP_OK) {
    Serial.println("register RECV cb function OK");
  }
  else {
    Serial.println("FUCK YOU  FUCK YOU  FUCK YOU  FUCK YOU  FUCK YOU");
  }

  //  ScanForSlave();
  //  if (SlaveCnt > 0) {
  //    manageSlave();
  //    sendData();
  //  }

  //  /*
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD, NULL);

    // wait 5 seconds for connection:
    delay(3000);
  }
  Serial.println("Connected to wifi");
  //  */
  //  /*

  if (HBB_AWS.connect(HOST_ADDRESS, CLIENT_ID) == 0) { // Connect to AWS
    Serial.println("Connected to AWS");

    //    /*
    delay(1000);
    if (0 == HBB_AWS.subscribe(TOPIC_NAME_delta, callBackDelta)) {
      Serial.println("Subscribe Successfull");
      DoPublishTo = true;
    } else {
      Serial.println("Subscribe Failed");
      ESP.restart();
      while (1);
    }
    //    */
  }
  else {
    Serial.println("AWS connection failed");
    while (1);
    ESP.restart();
  }
  //  */


  Initialized = true;
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("HBB_HUB Module Online");
}



void loop() {

  // ESP NOW
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (SecurityState == ESM_ACTIVATE || ESPNOW_BROADCAST_SLEEP_COUNT == 0) {
    if (Initialized) {
      ///////////////////////////////////////
      ScanForSlave();
      if (SlaveCnt > 0) {
        manageSlave();
        sendData();
        ESPNOW_BROADCAST_SLEEP_COUNT = 0;
        // automaticly recieves data
      }
      ///////////////////////////////////////
    }
  } else {
    ESPNOW_BROADCAST_SLEEP_COUNT += 1;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //  /*

  // Received
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  StaticJsonDocument<200> msgRECV;

  if (msgReceived == 1) {
    msgReceived = 0;
    Serial.print("$$ Received Message : ");
    //    Serial.println(rcvdPayload);
    // Deserialize the JSON document
    if (deserializeJson(msgRECV, rcvdPayload)) { // if error
      Serial.print(F("deserializeJson() failed.. \n"));
    }

    JsonObject state = msgRECV["state"];
    //    String cmd_Security_ctrl = state["Securitycmd"];
    String crl_Security_ctrl = state["security"];
    String crl_Camera_ctrl = state["cameraOn"];
    //    String crl_Security_state = state["SecurityState"];
    //    String crl_Monitering_state = state["MoniteringState"];


    Serial.print("\nSubscribed Security Command : ");
    Serial.print(crl_Security_ctrl);
    Serial.print("\n");
    Serial.print("\nSubscribed Camera Command : ");
    Serial.print(crl_Camera_ctrl);
    Serial.print("\n");


    if (crl_Security_ctrl == "null") {
      //
    }
    else if (crl_Security_ctrl == "true") {
      if (SecurityState == ESM_ACTIVATE) {
        Serial.println("---++--- Already ACTIVATEED ---++---");
        souldPUB_SS = false;
      } else {
        souldPUB_SS = true;
        SecurityState = ESM_ACTIVATE;
        Serial.println("------------------------------");
        Serial.println("-------- S ACTIVATEED --------");
        Serial.println("------------------------------");
      }
    } else if (crl_Security_ctrl == "false") {
      if (SecurityState == ESM_ACTIVATE) {
        souldPUB_SS = true;
        SecurityState = ESM_DEACTIVATE;
        Serial.println("------------------------------");
        Serial.println("------ S DE-ACTIVATEED -------");
        Serial.println("------------------------------");
      } else {
        Serial.println("--##-- Already DE ACTIVATEED --##--");
        souldPUB_SS = false;
      }
    } else {
      Serial.println("---Invalid command from APP---");
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






  // Set payload
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  String SSJP;      // 감시 여부
  String MSJP;    // 침입 여부
  String SCJP;    //

  if (SecurityState == 99) {
    SSJP = "true";
  } else {
    SSJP = "false";
  }
  SCJP = SSJP;

  noissue = true;
  // GET MONITERING STATES
  int e = ESM_Reports.size();
  for (int h = 0; h < e; h++) {
    if (ESM_Reports.get(h) == 44) {
      MSJP = "true";
      noissue = false;
      souldPUB_SS   = true;
      break;
    }
  }
  if (noissue) {
    MSJP = "false";
  }
  ESM_Reports.clear();
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // Publish
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (DoPublishTo) {

    sprintf(payload, "{\"state\":{\"reported\":{\"MoniteringState\":\"%s\"}}}", MSJP);
    if (HBB_AWS.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
      Serial.print("Publish Monitering State : ");
      Serial.println(payload);
      firstPubSuc = true;
    }
    else {
      Serial.print("! failed Publish Monitering State : ");
      Serial.println(payload);
      publishCount += 1;
      firstPubSuc = false;
    }

    if (souldPUB_SS) {
      if (firstPubSuc) {
        sprintf(payload, "{\"state\":{\"reported\":{\"SecurityState\":\"%s\"}}}", SSJP);
        if (HBB_AWS.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
          Serial.print("Publish Security State : ");
          Serial.println(payload);
          souldPUB_SS = false;
        }
        else {
          Serial.print("! failed Publish Security State : ");
          Serial.println(payload);
          publishCount += 1;
        }

        /*
              sprintf(payload, "{\"state\":{\"reported\":{\"SecurityControll\":\"%s\"}}}", SCJP);
              if (HBB_AWS.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
                Serial.print("Publish Security State : ");
                Serial.println(payload);
              }
              else {
                Serial.print("! failed Publish Security State : ");
                Serial.println(payload);
                publishCount += 1;
              }
        */
        if (publishCount > 11) {
          Serial.print("+——Publish failed 12 Times = Reboot in 3 sec——+");
          //          delay(3000);
          ESP.restart();
        }
      }
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  */

  //  vTaskDelay(1000 / portTICK_RATE_MS);



}

