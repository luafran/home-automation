#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h> //Se importan las librerías
#include <DallasTemperature.h>

#define I2C_ADDR    0x27 // Find it from I2C Scanner

#define RS_PIN  0
#define RW_PIN  1
#define EN_PIN  2
#define D4_PIN  4
#define D5_PIN  5
#define D6_PIN  6
#define D7_PIN  7
#define BACKLIGHT_PIN     3
#define BACKLIGHT_POL POSITIVE
 
#define ONE_WIRE_DATA_PIN 2 //Se declara el pin donde se conectará la DATA
 
OneWire ourWire(ONE_WIRE_DATA_PIN); //Se establece el pin declarado como bus para la comunicación OneWire
 
DallasTemperature sensors(&ourWire); //Se instancia la librería DallasTemperature

LiquidCrystal_I2C  lcd(I2C_ADDR,EN_PIN,RW_PIN,RS_PIN,
                       D4_PIN,D5_PIN,D6_PIN,D7_PIN,
                       BACKLIGHT_PIN, BACKLIGHT_POL);

void setup()
{
  Serial.begin(9600);
  delay(1000);
  sensors.begin();
  
  lcd.begin (16,2); // LCD 16x2
  lcd.setBacklight(LOW);
  delay(1000);
  lcd.setBacklight(HIGH);
  lcd.home (); // go home
  lcd.clear();
  lcd.print("Lorenzo");
  lcd.setCursor (0,1);
  lcd.print("Afranllie");
}

void loop()
{
  sensors.requestTemperatures(); //Prepara el sensor para la lectura
  float tempC = sensors.getTempCByIndex(0);

  lcd.home (); // go home
  lcd.clear();
  lcd.print("TEMP: ");
  Serial.print("TEMP: ");
  Serial.println(tempC);
  lcd.print(tempC);
  
  delay(1000);
}
