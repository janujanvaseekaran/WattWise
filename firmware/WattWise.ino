#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

// ----------------------------
// OLED Settings
// ----------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_INA219 ina219;

// ----------------------------
// WattWise Variables
// ----------------------------
float energyWh = 0.0;
float peakCurrent = 0.0;

unsigned long previousTime = 0;
unsigned long startTime = 0;

void setup() {

  Serial.begin(115200);

  // ESP32 I2C pins
  Wire.begin(21, 22);

  // Start OLED at address 0x3C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed!");
    while (true);
  }

  // Start INA219 at address 0x40
  if (!ina219.begin()) {
    Serial.println("INA219 failed!");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Startup screen
  display.setTextSize(2);
  display.setCursor(15, 10);
  display.println("WattWise");

  display.setTextSize(1);
  display.setCursor(25, 40);
  display.println("Starting...");

  display.display();

  delay(1500);

  previousTime = millis();
  startTime = millis();

  Serial.println("WattWise started!");
}

void loop() {

  // ----------------------------
  // Read INA219
  // ----------------------------

  float voltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  float power = ina219.getPower_mW();

  // ----------------------------
  // Peak Current
  // ----------------------------

  if (current > peakCurrent) {
    peakCurrent = current;
  }

  // ----------------------------
  // Energy Calculation
  // E = P x t
  // ----------------------------

  unsigned long currentTime = millis();

  unsigned long elapsedTime =
    currentTime - previousTime;

  previousTime = currentTime;

  // Convert mW to W
  float powerW = power / 1000.0;

  // Convert milliseconds to hours
  float elapsedHours =
    elapsedTime / 3600000.0;

  // Accumulate energy in Wh
  energyWh += powerW * elapsedHours;

  // ----------------------------
  // Runtime
  // ----------------------------

  unsigned long runtimeSeconds =
    (currentTime - startTime) / 1000;

  unsigned long minutes =
    runtimeSeconds / 60;

  unsigned long seconds =
    runtimeSeconds % 60;

  // ----------------------------
  // Serial Monitor
  // ----------------------------

  Serial.println("------ WATTWISE ------");

  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  Serial.print("Current: ");
  Serial.print(current, 2);
  Serial.println(" mA");

  Serial.print("Power: ");
  Serial.print(power, 2);
  Serial.println(" mW");

  Serial.print("Energy: ");
  Serial.print(energyWh, 6);
  Serial.println(" Wh");

  Serial.print("Peak Current: ");
  Serial.print(peakCurrent, 2);
  Serial.println(" mA");

  Serial.print("Runtime: ");
  Serial.print(minutes);
  Serial.print("m ");
  Serial.print(seconds);
  Serial.println("s");

  Serial.println("----------------------");

  // ----------------------------
  // OLED Display
  // ----------------------------

  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("WATTWISE");

  display.setCursor(0, 9);
  display.print("V: ");
  display.print(voltage, 2);
  display.println(" V");

  display.setCursor(0, 18);
  display.print("I: ");
  display.print(current, 2);
  display.println(" mA");

  display.setCursor(0, 27);
  display.print("P: ");
  display.print(power, 2);
  display.println(" mW");

  display.setCursor(0, 36);
  display.print("E: ");
  display.print(energyWh, 6);
  display.println(" Wh");

  display.setCursor(0, 45);
  display.print("Peak: ");
  display.print(peakCurrent, 2);
  display.println(" mA");

  display.setCursor(0, 54);
  display.print("Time: ");
  display.print(minutes);
  display.print(":");

  if (seconds < 10) {
    display.print("0");
  }

  display.print(seconds);

  display.display();

  // Update approximately once per second
  delay(1000);
}
