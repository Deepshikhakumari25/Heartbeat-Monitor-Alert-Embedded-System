#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// =====================================================
// HEARTBEAT MONITOR WITH ALERT SYSTEM
// WOKWI SIMULATION VERSION
// Educational Prototype - NOT a medical device
// =====================================================

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ---------------- PIN CONFIGURATION ----------------
const int BPM_CONTROL_PIN = A0;

const int GREEN_LED_PIN = 6;
const int RED_LED_PIN   = 7;
const int BUZZER_PIN    = 8;

// ---------------- EDUCATIONAL THRESHOLDS ----------------
const int LOW_THRESHOLD = 60;
const int HIGH_THRESHOLD = 100;

// ---------------- VARIABLES ----------------
int simulatedBPM = 75;

bool pulseDetected = true;

String status = "NORMAL";

unsigned long lastBeatTime = 0;

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);

  pinMode(BPM_CONTROL_PIN, INPUT);

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);

  // Start OLED
  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        0x3C))
  {
    Serial.println("OLED initialization failed!");

    while (true)
    {
      // Stop program
    }
  }

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(8, 10);
  display.println("HEART");

  display.setCursor(8, 35);
  display.println("MONITOR");

  display.display();

  delay(1500);

  display.clearDisplay();
  display.display();

  Serial.println("--------------------------------");
  Serial.println(" HEARTBEAT MONITOR SIMULATION");
  Serial.println("--------------------------------");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Read potentiometer
  int potValue = analogRead(BPM_CONTROL_PIN);

  // ---------------------------------------------------
  // NO PULSE MODE
  // Potentiometer at extreme left
  // ---------------------------------------------------

  if (potValue < 50)
  {
    pulseDetected = false;
  }
  else
  {
    pulseDetected = true;

    simulatedBPM = map(
      potValue,
      50,
      1023,
      40,
      140
    );
  }

  // Update status
  updateStatus();

  // Update LEDs and buzzer
  updateAlert();

  // Update OLED
  updateDisplay();

  // Generate heartbeat only when pulse exists
  generateHeartbeat();

  // Serial output
  printSerialData();

  delay(50);
}

// =====================================================
// STATUS LOGIC
// =====================================================

void updateStatus()
{
  // No pulse
  if (!pulseDetected)
  {
    status = "NO PULSE";
  }

  // Low BPM
  else if (simulatedBPM < LOW_THRESHOLD)
  {
    status = "LOW";
  }

  // Normal BPM
  else if (simulatedBPM <= HIGH_THRESHOLD)
  {
    status = "NORMAL";
  }

  // High BPM
  else
  {
    status = "HIGH";
  }
}

// =====================================================
// ALERT SYSTEM
// =====================================================

void updateAlert()
{
  // ---------------------------------------------------
  // NO PULSE
  // ---------------------------------------------------

  if (!pulseDetected)
  {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);

    noTone(BUZZER_PIN);

    return;
  }

  // ---------------------------------------------------
  // NORMAL
  // ---------------------------------------------------

  if (status == "NORMAL")
  {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);

    noTone(BUZZER_PIN);
  }

  // ---------------------------------------------------
  // LOW / HIGH ALERT
  // ---------------------------------------------------

  else
  {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);

    tone(BUZZER_PIN, 2000);
  }
}

// =====================================================
// SIMULATED HEARTBEAT
// =====================================================

void generateHeartbeat()
{
  // Don't generate heartbeat if there is no pulse
  if (!pulseDetected)
  {
    return;
  }

  unsigned long currentTime = millis();

  unsigned long beatInterval =
    60000UL / simulatedBPM;

  if (currentTime - lastBeatTime >= beatInterval)
  {
    lastBeatTime = currentTime;

    Serial.print("Heartbeat detected | BPM = ");
    Serial.println(simulatedBPM);
  }
}

// =====================================================
// OLED DISPLAY
// =====================================================

void updateDisplay()
{
  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.println("HEARTBEAT MONITOR");

  // BPM
  display.setTextSize(2);
  display.setCursor(0, 18);

  display.print("BPM:");

  if (pulseDetected)
  {
    display.println(simulatedBPM);
  }
  else
  {
    display.println("--");
  }

  // Status
  display.setCursor(0, 43);

  display.println(status);

  display.display();
}

// =====================================================
// SERIAL MONITOR
// =====================================================

void printSerialData()
{
  static unsigned long lastPrintTime = 0;

  unsigned long currentTime = millis();

  if (currentTime - lastPrintTime >= 1000)
  {
    lastPrintTime = currentTime;

    if (!pulseDetected)
    {
      Serial.println("BPM: -- | STATUS: NO PULSE");
    }
    else
    {
      Serial.print("BPM: ");
      Serial.print(simulatedBPM);

      Serial.print(" | STATUS: ");
      Serial.println(status);
    }
  }
}