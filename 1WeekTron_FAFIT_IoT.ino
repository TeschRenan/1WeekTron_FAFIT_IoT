
#include <stdio.h>
#include <WiFi.h>
#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient

#define LED 2
// Update these with values suitable for your network.

const char* ssid = "------";
const char* password = "------";
const char* mqtt_server = "mqtt.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);

int32_t lastMsg = 0;
int32_t cont = 0;
int8_t LedStatus = 0;

const char* clientID = "DeviceIoTFAFIT";
const char* subscribeTopic = "FAFIT/IoT/Sub";
const char* publishTopic = "FAFIT/IoT/Pub";

char subscribeContent[32];
char publishContent[64];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  strncpy(subscribeContent,"",sizeof(subscribeContent));// Cleaning the variable

  strncpy(subscribeContent,(char*)payload,sizeof(subscribeContent));
  
  Serial.print(subscribeContent);
  
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if (strstr(subscribeContent, "true")){

    Serial.print("Ativando LED");
    
    digitalWrite(LED, HIGH);   
    LedStatus = 1;
    // Turn the LED on 

  } 
  if (strstr(subscribeContent, "false")){

    Serial.print("Desativando LED");
    
    digitalWrite(LED, LOW);  
    LedStatus = 0;
    
    // Turn the LED off
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(subscribeTopic);
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);     // Initialize the LED pin as an output
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (millis() - lastMsg > 2000) {
    
    lastMsg = millis();

    cont++;

    if(LedStatus == 0){
      
        sprintf(publishContent, "Contagem: %ld, Status LED: %s", cont,"OFF");
      
    }
    if(LedStatus == 1){
      
        sprintf(publishContent, "Contagem: %ld, Status LED: %s", cont,"ON");
    
    }
    
    Serial.print("Publish message: ");
    Serial.println(publishContent);
    
    client.publish(publishTopic,publishContent);
  }
}
