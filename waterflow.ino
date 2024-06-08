#define BLYNK_TEMPLATE_ID "TMPL6pUxVFWzg"
#define BLYNK_TEMPLATE_NAME "Quickstart Template"
#define BLYNK_AUTH_TOKEN "BAjPCRxuWASJAZ-iSEvReHZPQXzO8ATS"
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <Wire.h>


//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

BlynkTimer timer;
long millis_rumus;
long millis_current;
long millis_LCD;
long LCD_current;
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "HUAWEI-2008";
char pass[] = "1234567890";


//pin Sensor Ultrasonic
const int trigPin = 25;
const int echoPin = 33;
long duration;
float distance;

//pin Sensor Kelembaban Tanah
const int SoilSensor = 36;
float kelembabanTanah;
int hasilPembacaan;

//pin Sensor PH
const int PH_sensor = 39;
int sensorValue;
float outputValue;

//pin relay
int pompa = 12;
int lamp = 14;

int data_v0;
int data_v3;
int data_v8;
String lampstate = "MATI";
String modestate = "AUTO";
String pumpstate = "MATI";

char lcd_ultra[50];
char lcd_ph[50];
char lcd_kelembaban[50];
int count = 0;
int count2 = 0;
int cursor_text1;
int cursor_text2;
int start_cursor;
void senddata() {
  Blynk.virtualWrite(V5, distance);
  Blynk.virtualWrite(V2, outputValue);
  Blynk.virtualWrite(V1, kelembabanTanah);
  Blynk.virtualWrite(V6, lampstate);
  Blynk.virtualWrite(V7, modestate);
  Blynk.virtualWrite(V9, pumpstate);
}
void LCD()
{
  lcd.clear();
  sprintf(lcd_ph, "pH:%1.0f  Lembab:%1.0f%%", outputValue, kelembabanTanah);
  String data_atas = String(lcd_ph);

  sprintf(lcd_ultra, "Level Air:%1.0fcm", distance);
  String data_bawah = String(lcd_ultra);

  lcd.setCursor(0, 0);
  lcd.print(data_atas.substring(0, 16)); // Menampilkan 16 karakter pertama dari data atas
  lcd.setCursor(0, 1);
  lcd.print(data_bawah.substring(0, 16)); // Menampilkan 16 karakter pertama dari data bawah
//  
//  lcd.setCursor(count, 0);
//  lcd.print(lcd_ph);

//   if (count2 < data_bawah.length() - 15)
//  {
//    
//    count2++;
//  }
//  else count2 = 0;
}

void setup() {
  lcd.begin();      // initialize the lcd
  //lcd.init();
  lcd.backlight(); // open the backlight
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, senddata);
  timer.setInterval(700L, LCD);

  pinMode(lamp, OUTPUT);
  pinMode(pompa, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  String tes1 = "PENS SMART";
  String tes2 = "IRIGATION";

  lcd.setCursor(((16 - tes1.length()) / 2) + 1 , 0);
  lcd.print(tes1);
  lcd.setCursor(((16 - tes2.length()) / 2) + 1, 1);
  lcd.print(tes2);

  delay(1000);

  lcd.clear();
}
BLYNK_WRITE(V0) // Executes when the value of virtual pin 0 changes
{
  data_v0 = param.asInt();
}
BLYNK_WRITE(V3) // Executes when the value of virtual pin 0 changes
{
  data_v3 = param.asInt();
}
BLYNK_WRITE(V8) // Executes when the value of virtual pin 0 changes
{
  data_v8 = param.asInt();
}
void loop() {
  millis_current = millis();
  millis_LCD = millis();
  if (millis_current - millis_rumus >= 500)
  {
    millis_rumus = millis_current;
    //kelembabanTanah = random(0, 100);
   // outputValue = random(0, 14);
    //distance = random(0, 100);
    lamp_state();
          ultrasonic();
          rumus();
    mode_state();
  }
  //  if (millis_LCD - LCD_current >= 1000)
  //  {
  //    LCD_current = millis_LCD;
  //    LCD();
  //  }
  ultrasonic();

  Blynk.run();
  timer.run();
}
void ultrasonic()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn (echoPin, HIGH);
  distance = duration * 0.034 / 2 ;

  // //kontrol pompa
  // if(distance < 60){
  //   digitalWrite(pompa, HIGH);
  // } else {
  //   digitalWrite(pompa, LOW);
  // }
}
void rumus()
{
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  hasilPembacaan = analogRead(SoilSensor);
  kelembabanTanah = (100 - ((hasilPembacaan / 4095.00) * 100));
  sensorValue = analogRead(PH_sensor);
  delay(100);
  outputValue = (-0.0139 * sensorValue) + 7.7851;
}
void mode_state()
{
  if (data_v3 == 1)
  {
    modestate = "AUTO";
    if (kelembabanTanah <= 50 )
    {
      digitalWrite(pompa, HIGH);
      pumpstate = "HIDUP";
    }
    else
    {
      digitalWrite(pompa, LOW);
      pumpstate = "MATI";
    }
  }
  else
  {
    modestate = "MANUAL";
    if (data_v8 == 1)
    {
      digitalWrite(pompa, HIGH);
      pumpstate = "HIDUP";
    }
    else
    {
      digitalWrite(pompa, LOW);
      pumpstate = "MATI";
    }
  }
}

void lamp_state()
{
  if (data_v0 == 1)
  {
    digitalWrite(lamp, HIGH);
    lampstate = "HIDUP";
    }
  else
  {
    digitalWrite(lamp, LOW);
    lampstate = "MATI";
  }
}
