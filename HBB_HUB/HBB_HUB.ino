// Retelligence Iwen
// Isaac Kim
// Based on AWS & ESP-now
// KOREA AEROSPACE UNIV.
// IoT Team Project
// Home BlackBox

// HUB
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <WiFi.h>
#include <esp_now.h>
#include <AWS_IOT.h>
#include <ArduinoJson.h>
#include <LinkedList.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LinkedList<int> ESM_Reports = LinkedList<int>();

bool C_W_E_D_C_A = false;

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
int SecurityState = ESM_ACTIVATE;

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

char HOST_ADDRESS[] = "a1ewasgidh0jna-ats.iot.ap-northeast-2.amazonaws.com";
char CLIENT_ID[] = "HBB_sensor1";
char TOPIC_NAME_update[] = "$aws/things/HBB_sensor1/shadow/update";
char TOPIC_NAME_delta[] = "$aws/things/HBB_sensor1/shadow/update/delta";


int publishCount = 0;
int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
        slaves[SlaveCnt].channel = CHANNEL; // pick a channel
        slaves[SlaveCnt].encrypt = 0; // no encryption
        SlaveCnt++;
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
    esp_err_t result = esp_now_send(peer_addr, &data, sizeof(data));
    //    Serial.print("Send Status: ");
    if (result == ESP_OK) {
      //      Serial.println("Success");
    } else if (result == ESP_ERR_ESPNOW_NOT_INIT) {
      // How did we get so far!!
      //      Serial.println("ESPNOW not Init.");
    } else if (result == ESP_ERR_ESPNOW_ARG) {
      //      Serial.println("Invalid Argument");
    } else if (result == ESP_ERR_ESPNOW_INTERNAL) {
      //      Serial.println("Internal Error");
    } else if (result == ESP_ERR_ESPNOW_NO_MEM) {
      //      Serial.println("ESP_ERR_ESPNOW_NO_MEM");
    } else if (result == ESP_ERR_ESPNOW_NOT_FOUND) {
      //      Serial.println("Peer not found.");
    } else {
      //      Serial.println("Not sure what happened");
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
  //  Serial.print("Last Packet Sent to: "); Serial.println(macStr);
  Serial.print("Last Packet Send Status: "); Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// callback when data is recv from Slave
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
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
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  Serial.println("HBB_HUB Module Online");
  Serial.print("STA MAC: "); Serial.println(WiFi.macAddress());
  InitESPNow();
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);



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


  if (HBB_AWS.connect(HOST_ADDRESS, CLIENT_ID) == 0) { // Connect to AWS
    Serial.println("Connected to AWS");
    delay(1000);
    if (0 == HBB_AWS.subscribe(TOPIC_NAME_delta, callBackDelta))
      Serial.println("Subscribe Successfull");
    else {
      Serial.println("Subscribe Failed");
      ESP.restart();
      while (1);
    }

  }
  else {
    Serial.println("AWS connection failed");
    ESP.restart();
  }

}



void loop() {




  StaticJsonDocument<200> msgRECV;

  if (msgReceived == 1) {
    msgReceived = 0;
    Serial.print("$$ Received Message : ");
    Serial.println(rcvdPayload);
    // Deserialize the JSON document
    if (deserializeJson(msgRECV, rcvdPayload)) { // if error
      Serial.print(F("deserializeJson() failed.. \n"));
    }

    JsonObject state = msgRECV["state"];

    String crl_Security_ctrl = state["security"];
    String crl_Camera_ctrl = state["cameraOn"];
    String crl_Security_state = state["security"];
    String crl_Monitering_state = state["cameraOn"];


    Serial.print("\nSubscribed data security : ");
    Serial.print(crl_Security_ctrl);
    Serial.print("\n");
    Serial.print("\nSubscribed data camera : ");
    Serial.print(crl_Camera_ctrl);
    Serial.print("\n");

    Serial.print("\nSubscribed data Monitering State : ");
    Serial.print(crl_Monitering_state);
    Serial.print("\n");
    Serial.print("\nSubscribed data Security State : ");
    Serial.print(crl_Security_state);
    Serial.print("\n");
  }






  ///////////////////////////////////////
  ScanForSlave();
  if (SlaveCnt > 0) {
    manageSlave();
    sendData();
    // automaticly recieves data
  }
  ///////////////////////////////////////




  //  StaticJsonDocument<200> msg1;
  //  StaticJsonDocument<200> msg2;
  //
  //  String SSJP = msg1["SecurityState"];      // 감시 여부
  //  String MSJP = msg2["MoniteringState"];    // 침입 여부

  String SSJP;      // 감시 여부
  String MSJP;    // 침입 여부

  if (SecurityState == 99) {
    SSJP = "true";
  } else {
    SSJP = "false";
  }


  noissue = true;
  // GET MONITERING STATES
  int e = ESM_Reports.size();
  for (int h = 0; h < e; h++) {
    if (ESM_Reports.get(h) == 44) {
      MSJP = "true";
      noissue = false;
      break;
    }
  }
  if (noissue) {
    MSJP = "false";
  }
  ESM_Reports.clear();


  //  char SecurityState_JP[100];
  //  char MoniteringState_JP[100];
  //
  //  serializeJson(msg1, SecurityState_JP);
  //  serializeJson(msg2, MoniteringState_JP);
  //
  //  sprintf(SecurityState_JP, "{\"SecurityState\":\"%s\"}", SSJP);
  //  sprintf(MoniteringState_JP, "{\"MoniteringState\":\"%s\"}", MSJP);


  //  // Publish [SSJP]
  //  if (HBB_AWS.publish(TOPIC_NAME_update, SecurityState_JP) == 0) {
  //    Serial.print("[SSJP]publish Message:");
  //    Serial.println(SecurityState_JP);
  //  }
  //  else {
  //    Serial.println("[SSJP]Publish failed: ");
  //    Serial.println(SecurityState_JP);
  //  }
  //
  //  // Publish [MSJP]
  //  if (HBB_AWS.publish(TOPIC_NAME_update, MoniteringState_JP) == 0) {
  //    Serial.print("[MSJP]publish Message:");
  //    Serial.println(MoniteringState_JP);
  //  }
  //  else {
  //    Serial.println("[MSJP]Publish failed: ");
  //    Serial.println(MoniteringState_JP);
  //  }



  sprintf(payload, "{\"state\":{\"reported\":{\"MoniteringState\":\"%s\"}}}", MSJP);
  if (HBB_AWS.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
    Serial.print("Publish Monitering State : ");
    Serial.println(payload);
  }

  else {
    Serial.print("Publish Monitering State failed: ");
    Serial.println(payload);
    publishCount += 1;
  }



  sprintf(payload, "{\"state\":{\"reported\":{\"SecurityState\":\"%s\"}}}", SSJP);
  if (HBB_AWS.publish(TOPIC_NAME_update, payload) == 0) { // Publish the message
    Serial.print("Publish Security State : ");
    Serial.println(payload);
  }

  else {
    Serial.print("Publish Security State failed: ");
    Serial.println(payload);
    publishCount += 1;
  }


  if (publishCount > 6) {
    Serial.print("+-----Publish failed 7 Times = Reboot in 3 sec-----+");
    delay(3000);
    ESP.restart();
  }

  vTaskDelay(500 / portTICK_RATE_MS);

}
