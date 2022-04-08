#include "def.h";

AsyncWebServer server(80);

const char *ssid = "Felipe-2G";
const char *password = "30051994f";

const char *PARAM_INPUT_TEMP = "temp";

int targetTemp = 25;
int currentTemp = 0;
bool connecting = true;
bool networkConfigured = false;
bool powerLedSet = false;
unsigned long previousMillis = 0;

IRAM_ATTR void addTemp()
{
  targetTemp += 1;
}

IRAM_ATTR void subTemp()
{
  targetTemp -= 1;
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

int getTemperature()
{
  sensors.requestTemperatures();
  currentTemp = round(sensors.getTempCByIndex(0));
  if (currentTemp < 100) return 30;
  return currentTemp;
}

void updatePowerLedStatus()
{
  if (networkConfigured == false)
  {
    if (LED_POWER_LEVEL == LOW)
    {
      LED_POWER_LEVEL = HIGH;
    }
    else
    {
      LED_POWER_LEVEL = LOW;
    }
    digitalWrite(LED_POWER, LED_POWER_LEVEL);
  }
  else
  {
    if (powerLedSet == false)
    {
      LED_POWER_LEVEL = HIGH;
      digitalWrite(LED_POWER, LED_POWER_LEVEL);
      powerLedSet = true;
    }
  }
}

void updateOutputStatus()
{
  int currentTemp = getTemperature();
  if (currentTemp >= (targetTemp + TEMP_THRESHOLD))
  {
    digitalWrite(COOLER, HIGH);
    digitalWrite(LED_COOL, HIGH);
    return;
  }
  digitalWrite(COOLER, LOW);
  digitalWrite(LED_COOL, LOW);
}

void configPinout()
{
  pinMode(LED_POWER, OUTPUT);
  pinMode(LED_COOL, OUTPUT);
  pinMode(COOLER, OUTPUT);
  pinMode(SENSOR, INPUT);

  attachInterrupt(digitalPinToInterrupt(BTN_PLUS), addTemp, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_MINUS), subTemp, RISING);
}

void configNetwork()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  networkConfigured = true;
  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/set-temp", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String inputMessage;
    String inputParam;
    if (request->hasParam(PARAM_INPUT_TEMP)) {
      inputMessage = request->getParam(PARAM_INPUT_TEMP)->value();
      inputParam = PARAM_INPUT_TEMP;
    }
    targetTemp = inputMessage.toInt();
    Serial.println("Setting target temp to " + inputMessage + "º C");
    request->send(200); });

  server.on("/get-temp", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String response = "Tempratura atual: ";
    response = response + getTemperature();
    response = response + "º C";
    response = response + "|";
    response = response + targetTemp;
    Serial.println(response);
    request->send(200, "text/plain", response); });
  server.onNotFound(notFound);
  server.begin();
}

void setup()
{
  Serial.begin(115200);
  configPinout();
  configNetwork();
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= STATUS_CHECK_INTERVAL)
  {
    previousMillis = currentMillis;
    updatePowerLedStatus();
    updateOutputStatus();
  }
}
