#include <ESP8266WiFi.h>

IPAddress ip(192, 168, 1, 211);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress DNS(192, 168, 1, 1);

const char* ssid = "MySSID";
const char* password = "MyPassword";

int pin_labels[6] = {4, 5, 12, 13, 14, 16};
bool pin_states[6] = {true, true, true, true, true, true};
bool authed = false;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.config(ip, gateway, subnet, DNS);
  delay(100);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  digitalWrite(14, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
 
  // Connect to WiFi network
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
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }
  authed = false;
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  if(request.indexOf("favicon.ico") == -1)
    Serial.println(request);
  client.flush();

  // Match auth key
  if(request.indexOf("/auth=9kBYD18zezE9U6qeQKlqKSpR") != -1){
    authed = true;
  }
  // Match the request
  if(authed==true && request.indexOf("pin") != -1){
    int current_pin = request.substring(39, 41).toInt();
    
    if (current_pin==4 || current_pin==5 || current_pin==12 || current_pin==13 || current_pin==14 || current_pin==16){
      if(request.indexOf("state=on") != -1){
        pin_states[map_pins(current_pin)] = true;
        Serial.print("Turning on : ");
        Serial.println(current_pin);
        if (current_pin==16)
          digitalWrite(LED_BUILTIN, LOW);
        else
          digitalWrite(current_pin, HIGH);
      }
      else{
        pin_states[map_pins(current_pin)] = false;
        Serial.print("Turning off : ");
        Serial.println(current_pin);
        if (current_pin==16)
          digitalWrite(LED_BUILTIN, HIGH);
        else
          digitalWrite(current_pin, LOW);
      }
    }
      
  }
  if (authed == true){
    String json = "{";
    for (int i = 0; i < 6; i = i + 1) {
      if (i == 5){
        json =  json + "'"+map_pins_invert(i)+"\':\'"+pin_states[i]+"\'";
      }
      else{
        json =  json + "'"+map_pins_invert(i)+"\':\'"+pin_states[i]+"\',";
      }
    }
    json = json + "\}";
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json;charset=utf-8");
    client.println("Server: Arduino");
    client.println("Connnection: close");
    client.println();
    client.println(json);
  }
}

int map_pins(int current_pin){
    switch (current_pin) {
    case 4:
      return 0;
    case 5:
      return 1;
    case 12:
      return 2;
    case 13:
      return 3;
    case 14:
      return 4;
    case 16:
      return 5;
  }
}

int map_pins_invert(int pin){
    switch (pin) {
    case 0:
      return 4;
    case 1:
      return 5;
    case 2:
      return 12;
    case 3:
      return 13;
    case 4:
      return 14;
    case 5:
      return 16;
    
  }
}
