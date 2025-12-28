/*


██╗███╗░░██╗███╗░░██╗░█████╗░██╗░░░██╗░█████╗░░█████╗░██████╗░███████╗
██║████╗░██║████╗░██║██╔══██╗██║░░░██║██╔══██╗██╔══██╗██╔══██╗╚════██║
██║██╔██╗██║██╔██╗██║██║░░██║╚██╗░██╔╝╚█████╔╝██║░░██║██████╔╝░░███╔═╝
██║██║╚████║██║╚████║██║░░██║░╚████╔╝░██╔══██╗██║░░██║██╔══██╗██╔══╝░░
██║██║░╚███║██║░╚███║╚█████╔╝░░╚██╔╝░░╚█████╔╝╚█████╔╝██║░░██║███████╗
╚═╝╚═╝░░╚══╝╚═╝░░╚══╝░╚════╝░░░░╚═╝░░░░╚════╝░░╚════╝░╚═╝░░╚═╝╚══════╝

Instagram: https://www.instagram.com/sahadnisham.k/
LinkedIn : https://www.linkedin.com/in/sahad-nisham-k/

*/

#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <math.h>

// Constants for phone numbers and other settings
const String EMERGENCY_PHONE = "+919999999999"; // Replace with your emergency number
#define rxPin 2
#define txPin 3

SoftwareSerial sim800(rxPin, txPin); // GSM Module connected to D2 (TX) and D3 (RX)
SoftwareSerial neogps(10, 9); // GPS Module connected to D10 (TX) and D9 (RX)
TinyGPSPlus gps;

String sms_status, sender_number, received_date, msg;
String latitude, longitude;

#define BUZZER 12
#define BUTTON 11
#define xPin A1
#define yPin A2
#define zPin A3

byte updateflag = 0;

int xaxis = 0, yaxis = 0, zaxis = 0;
int deltx = 0, delty = 0, deltz = 0;
int vibration = 2;
int devibrate = 75;
int magnitude = 0;
int sensitivity = 20;

boolean impact_detected = false;
unsigned long time1;
unsigned long impact_time;
unsigned long alert_delay = 3000; // Delay for alert in milliseconds

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);
  neogps.begin(9600);

  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);

  sms_status = "";
  sender_number = "";
  received_date = "";
  msg = "";

  // Initialize GSM module
  sim800.println("AT");
  delay(1000);
  sim800.println("ATE1");
  delay(1000);
  sim800.println("AT+CPIN?");
  delay(1000);
  sim800.println("AT+CMGF=1"); // Set SMS to text mode
  delay(1000);
  sim800.println("AT+CNMI=1,1,0,0,0"); // Enable SMS reception notification
  delay(1000);

  // Initialize accelerometer readings
  time1 = micros();
  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);
}

void loop() {
  // Check for impact detection every 2 milliseconds
  if (micros() - time1 > 1999) {
    Impact();
  }

  // If impact is detected, trigger actions
  if (updateflag > 0) {
    updateflag = 0;
    Serial.println("Impact detected!!");
    Serial.print("Magnitude: ");
    Serial.print(magnitude);

    getGps(); // Get GPS data
    digitalWrite(BUZZER, HIGH); // Turn on the buzzer
    impact_detected = true;
    impact_time = millis();
  }

  // After impact is detected, wait for alert_delay before sending alert
  if (impact_detected == true) {
    if (millis() - impact_time >= alert_delay) {
      digitalWrite(BUZZER, LOW); // Turn off the buzzer
      makeCall(); // Call emergency contact
      delay(1000);
      sendAlert(); // Send SMS with GPS coordinates
      impact_detected = false;
      impact_time = 0;
    }
  }

  // Button to reset the system after an alert
  if (digitalRead(BUTTON) == LOW) {
    delay(200);
    digitalWrite(BUZZER, LOW);
    impact_detected = false;
    impact_time = 0;
  }

  // Read incoming data from GSM
  while (sim800.available()) {
    parseData(sim800.readString());
  }
  
  // Debug via serial monitor
  while (Serial.available()) {
    sim800.println(Serial.readString());
  }
}

// Impact detection function using accelerometer readings
void Impact() {
  time1 = micros();

  int oldx = xaxis;
  int oldy = yaxis;
  int oldz = zaxis;

  xaxis = analogRead(xPin);
  yaxis = analogRead(yPin);
  zaxis = analogRead(zPin);

  vibration--;
  Serial.print("Vibration = ");
  Serial.println(vibration);

  if (vibration < 0) vibration = 0;
  if (vibration > 0) return;

  deltx = xaxis - oldx;
  delty = yaxis - oldy;
  deltz = zaxis - oldz;

  magnitude = sqrt(sq(deltx) + sq(delty) + sq(deltz));

  if (magnitude >= sensitivity) { // Impact detected
    updateflag = 1;
    vibration = devibrate;
  } else {
    magnitude = 0;
  }
}

// Parse incoming SMS data from GSM
void parseData(String buff) {
  Serial.println(buff);

  unsigned int len, index;
  index = buff.indexOf(":");
  String cmd = buff.substring(0, index);
  cmd.trim();

  buff.remove(0, index + 2);
  if (cmd == "+CMGR") {
    if (buff.indexOf(EMERGENCY_PHONE) > 1) {
      buff.toLowerCase();
      if (buff.indexOf("get gps") > 1) {
        getGps();
        String sms_data;
        sms_data = "GPS Location Data\r";
        sms_data += "http://maps.google.com/maps?q=loc:";
        sms_data += latitude + "," + longitude;
        sendSms(sms_data);
      }
    }
  }
}

// Get GPS data from Neo-6M module
void getGps() {
  boolean newData = false;
  for (unsigned long start = millis(); millis() - start < 2000;) {
    while (neogps.available()) {
      if (gps.encode(neogps.read())) {
        newData = true;
        break;
      }
    }
  }

  if (newData) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
  } else {
    Serial.println("No GPS data available");
    latitude = "";
    longitude = "";
  }

  Serial.print("Latitude= ");
  Serial.println(latitude);
  Serial.print("Longitude= ");
  Serial.println(longitude);
}

// Send SMS alert with GPS location
void sendAlert() {
  String sms_data;
  sms_data = "Accident Alert!!\r";
  sms_data += "http://maps.google.com/maps?q=loc:";
  sms_data += latitude + "," + longitude;

  sendSms(sms_data);
}

// Make a phone call to emergency contact
void makeCall() {
  Serial.println("Calling...");
  sim800.println("ATD " + EMERGENCY_PHONE + ";");
  delay(30000); // 20 seconds call duration
  sim800.println("ATH");
  delay(1000); // 1 second delay after hanging up
}

// Send SMS via GSM module
void sendSms(String text) {
  sim800.print("AT+CMGF=1\r");
  delay(1000);
  sim800.print("AT+CMGS=\"" + EMERGENCY_PHONE + "\"\r");
  delay(1000);
  sim800.print(text);
  delay(100);
  sim800.write(0x1A);
  delay(1000);
  Serial.println("SMS Sent Successfully.");
}