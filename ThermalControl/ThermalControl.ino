#include "def.h";
 
AsyncWebServer server(80);

const char* ssid = "Felipe-2G";
const char* password = "30051994f";

const char* PARAM_INPUT_TEMP = "temp";

int targetTemp = 25;
int currentTemp = 0;
bool connecting = true;
bool networkConfigured = false;
bool powerLedSet = false;
unsigned long previousMillis = 0;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

int getTemperature(){ 
  sensors.requestTemperatures();
  currentTemp = round(sensors.getTempCByIndex(0));
  return currentTemp;
}

void updatePowerLedStatus(){
  if (networkConfigured == false){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= STATUS_CHECK_INTERVAL){
      previousMillis = currentMillis;
      if(LED_POWER_LEVEL == LOW){
        LED_POWER_LEVEL = HIGH;
      }
      else{
        LED_POWER_LEVEL = LOW;
      }
      digitalWrite(LED_POWER, LED_POWER_LEVEL);
    }
  }
  else{
    if (powerLedSet == false){      
      digitalWrite(LED_POWER, HIGH);
      powerLedSet = true;
    }
  }
}

void addTemp(){
  targetTemp += 1;
}

void subTemp(){
  targetTemp -= 1;
}

void updateOutputStatus(){
  if (getTemperature() > targetTemp + TEMP_THRESHOLD){
    digitalWrite(RELAY, HIGH);
    return;
  }
  digitalWrite(RELAY, LOW);
}

void configPinout(){
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_COOL, OUTPUT);
  pinMode(LED_SENSOR, OUTPUT);
  pinMode(RELAY, OUTPUT);
  
 attachInterrupt(digitalPinToInterrupt(BTN_PLUS), addTemp, RISING);
 attachInterrupt(digitalPinToInterrupt(BTN_MINUS), addTemp, RISING);
}

void configNetwork(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/set-temp", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_TEMP)) {
      inputMessage = request->getParam(PARAM_INPUT_TEMP)->value();
      inputParam = PARAM_INPUT_TEMP;
    }
    targetTemp = inputMessage.toInt();
    Serial.println("Setting target temp to " + inputMessage + "º C");
    request->send(200);
  });

  server.on("/get-temp", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String response = "Tempratura atual: ";
    response = response + getTemperature();
    response = response + "º C";
    Serial.println(response);
    request->send(200, "text/plain", response);
  });
  server.onNotFound(notFound);
  server.begin();  
}


void setup() {
  Serial.begin(115200);
  configPinout();
  configNetwork();
}

void loop() {
  updatePowerLedStatus();
}
