#include <DHT.h>            // Include the DHT library for the DHT sensor
#include <LiquidCrystal.h>  // Include the LiquidCrystal library for the LCD display
#include <ESP8266WiFi.h>    // Include the ESP8266WiFi library for WiFi connectivity

// ThingSpeak Settings
String apiKey = "OZAYN8FLZDEDAQIJ";  // Enter your Write API key from ThingSpeak

// WiFi Settings
const char *ssid = "KAGIMBURA_Home";   //  WiFi SSID
const char *pass = "ERIC@2023";       //  WiFi password
const char *server = "api.thingspeak.com"; // ThingSpeak API server

// Sensor and LCD Pins
#define DHTPIN D2           // DHT sensor is connected to pin D2
#define SOILMOISTUREPIN A0  // Soil moisture sensor is connected to analog pin A0

#define RS D3  // LCD pin for register select
#define EN D4  // LCD pin for enable
#define D4 D5  // LCD data pin 4
#define D5 D6  // LCD data pin 5
#define D6 D7  // LCD data pin 6
#define D7 D8  // LCD data pin 7

// Initialize DHT sensor and LCD display
DHT dht(DHTPIN, DHT11);                // DHT sensor type DHT11
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // Initialize the LCD

WiFiClient client;  // Create a WiFiClient object

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud rate
  delay(2000);          // Wait for 2 seconds

  lcd.begin(16, 2);     // Set up the LCD's number of columns and rows
  dht.begin();          // Start the DHT sensor

  Serial.println("Connecting to WiFi");
  Serial.println(ssid);

  WiFi.begin(ssid, pass); // Connect to WiFi network

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  // Read humidity, temperature, and soil moisture values
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int soilMoisture = analogRead(SOILMOISTUREPIN);

  // Display the readings on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(t); lcd.print(" H:"); lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("Moist:"); lcd.print(soilMoisture);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Connect to ThingSpeak
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=" + String(t);
    postStr += "&field2=" + String(h);
    postStr += "&field3=" + String(soilMoisture);
    postStr += "\r\n\r\n";

    // Create data string and send it to ThingSpeak
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: " + String(postStr.length()) + "\n\n");
    client.print(postStr);

    Serial.println("Data sent to ThingSpeak");
  }
  client.stop(); // Stop the client

  Serial.println("Waiting...");
  delay(2000); // ThingSpeak requires a minimum 15 seconds delay between updates, using 2 seconds for demonstration
}
