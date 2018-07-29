#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define USE_SERIAL Serial

//WiFI Configuration
  const char* ssid     = "Jinawi-gw-2";
  const char* password = "jinawi123";

//API Server Data
  //const char* host = "35.198.211.124:1801";//restapi
  const char * host = "172.24.1.1";//gateway
  const char * stringUrl = "/update.php?field1=1";
  

//declare Variable
  int batteryPin = A0;
  const int WifiLED = D6;
  const int relay = D2;

void setup() {
  // initialize the digital pin as an output.
  pinMode(batteryPin, INPUT);
  pinMode (WifiLED, OUTPUT);
  pinMode (relay, OUTPUT);
  digitalWrite (WifiLED, LOW);
  //digitalWrite (relay, LOW);
  
  Serial.begin(9600);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

}


void loop() {

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite (WifiLED, HIGH);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
   digitalWrite (WifiLED, LOW);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.print("connecting to ");
  Serial.println(host);

    HTTPClient http;

 USE_SERIAL.print("[HTTP] begin...\n");

        String url = "http://";
        url += host;
        url += stringUrl;
        // configure traged server and url
       http.begin(url); //HTTP
        //http.begin("http://10.5.5.26/update.php"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);  

                StaticJsonBuffer<300> JSONBuffer;   //Memory pool
                JsonObject& parsed = JSONBuffer.parseObject(payload); //Parse message
               
                if (!parsed.success()) {   //Check for errors in parsing
                  Serial.println("Parsing failed");
                  delay(5000);
                  return;
                }

                String lampu = parsed["gateway_status"];
                if(lampu == "on"){
                  digitalWrite (relay, HIGH);
                  Serial.println("Relay ON");
                }else{
                  digitalWrite (relay, LOW);
                  Serial.println("Relay OFF");
                }
              
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
        
  Serial.println();
  Serial.println("closing connection");
  delay(1000);
}
