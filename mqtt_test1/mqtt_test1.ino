

#include "WiFi.h"
#include <MQTTClient.h>
#include <WiFiClientSecure.h>
#include "cert_HBB_Core_test_1.h"



const char *WIFI_SSID = "KAU-Guest";
const char *WIFI_PASSWORD = NULL;

#define DEVICE_NAME "HBB_Core_t1"
#define AWS_IOT_ENDPOINT "a22u6t4dfbo39y-ats.iot.ap-northeast-2.amazonaws.com"
#define AWS_IOT_TOPIC "$aws/things/" DEVICE_NAME "/shadow/update"
#define AWS_MAX_RECONNECT_TRIES 50

WiFiClientSecure net = WiFiClientSecure();
//MQTTClient client = MQTTClient();
MQTTClient client = MQTTClient(512);





void connectToAWS()
{
    // Configure WiFiClientSecure to use the AWS certificates we generated
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Connect to the MQTT broker on the AWS endpoint we defined earlier
    client.begin(AWS_IOT_ENDPOINT, 8883, net);

    // Try to connect to AWS and count how many times we retried.
    int retries = 0;
    Serial.print("Connecting to AWS IOT");

    while (!client.connect(DEVICE_NAME) && retries < AWS_MAX_RECONNECT_TRIES) {
        Serial.print(".");
        delay(100);
        retries++;
    }

    // Make sure that we did indeed successfully connect to the MQTT broker
    // If not we just end the function and wait for the next loop.
    if(!client.connected()){
        Serial.println(" Timeout!");
        return;
    }

    // If we land here, we have successfully connected to AWS!
    // And we can subscribe to topics and send messages.
    Serial.println("Connected!");
}






void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Only try 15 times to connect to the WiFi
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 15){
    delay(500);
    Serial.print(".");
    retries++;
  }

  // If we still couldn't connect to the WiFi, go to deep sleep for a minute and try again.
  if(WiFi.status() != WL_CONNECTED){
    esp_sleep_enable_timer_wakeup(1 * 60L * 1000000L);
    esp_deep_sleep_start();
  }
}






void setup() {
  Serial.begin(9600);
  connectToWiFi();
  connectToAWS();

}





void loop() {
  // put your main code here, to run repeatedly:

}
