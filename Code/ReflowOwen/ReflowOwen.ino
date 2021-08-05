
#include "max6675.h"
#include <LiquidCrystal.h>


int ktcSO = 12;
int ktcCS = 10;
int ktcCLK = 13;

MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

#define RAMP_TO_SOAK  2
#define PRE_HEAT      3
#define RAMP_TO_PEAK  4
#define REFLOW        5
#define COOLING       6

struct setting
{
  float TempUpInOneSecond;
  uint16_t MaxTempInMode;
  uint8_t TimeRun; // Sencond
  float Kp;
  float Ki;
  float Kd;
};

uint16_t set_temp;


void LoadSetting()
{
  setting RampToSoak;
  setting PreHeat;
  setting RampToPeak;
  setting Reflow;
  setting Cooling;

  RampToSoak.TempUpInOneSecond = 2;
  RampToSoak.MaxTempInMode = 150;
  RampToSoak.TimeRun = 180;
  RampToSoak.Kp = 50;
  RampToSoak.Ki = 2;
  RampToSoak.Kd = 5;

  PreHeat.TempUpInOneSecond = 0.5;
  PreHeat.MaxTempInMode = 180;
  PreHeat.TimeRun = 90;
  PreHeat.Kp = 10;
  PreHeat.Ki =0;
  PreHeat.Kd=0;

  

}


void ControlSSR(uint8_t duty)
{
    digitalWrite(3,1);
    delay(10*duty);
    digitalWrite(3,0);
    delay(1000-10*duty);
}

float read_temp;

void setup() {
  // put your setup code here, to run once:
 
 Serial.begin(9600);
 read_temp = ktc.readCelsius();
 lcd.begin(16,2);
 lcd.setCursor(0,0);
 lcd.print("Reflow Owen");
 pinMode(3,OUTPUT);
 pinMode(9,OUTPUT);
 digitalWrite(3,0);
 pinMode(6,INPUT_PULLUP);
 pinMode(7,INPUT_PULLUP);
 pinMode(8,INPUT_PULLUP);
 LoadSetting();
 delay(1000);
 
}

uint32_t count;
uint8_t state;

void loop() {
  // put your main code here, to run repeatedly:

  if(state!=5)
  {
  read_temp = ktc.readCelsius();
  }
  Serial.println(read_temp);

  
  if(digitalRead(6) == 0) 
  {
    
    while(digitalRead(6) == 0) {digitalWrite(9,1);};
    digitalWrite(9,0);
    state++;
    if(state>2) state=0;
  }

  if(digitalRead(7) == 0) 
  {
    
    while(digitalRead(7) == 0) {digitalWrite(9,1);};
    if(state==5) state=0;
    else state = 5;

   digitalWrite(9,0);
  }


  
  
  if(state==0)
  {
    lcd.setCursor(0,0);
    lcd.print("WAIT AND BEGIN");
    lcd.setCursor(0,1);
    lcd.print(read_temp);
    lcd.print("        ");
    digitalWrite(3,0);
    delay(1000);
  }
  else if(state == 1)
  {
    if(read_temp>110) state=2;
    lcd.setCursor(0,0);
    lcd.print("RAMP TO SOAK  ");
    lcd.setCursor(0,1);
    lcd.print(read_temp);
    lcd.print("    ");
    lcd.print(set_temp);
    if(read_temp >100) ControlSSR(60);
    else ControlSSR(60);
    
  }
  else if(state == 2)
  {
    int i=70;
    while(i>1)
    {
     read_temp = ktc.readCelsius();
     Serial.println(read_temp);
     if(read_temp<130)
     {
      if (read_temp<120) digitalWrite(3,1);
      else digitalWrite(3,0); 
     }
     else digitalWrite(3,0); 
     delay(1000);
     lcd.setCursor(0,0);
     lcd.print("PRE HEAT    ");
     lcd.setCursor(0,1);
     lcd.print(read_temp);
     lcd.print("    ");
     lcd.print(set_temp);
     i--;
    }
    state=3;
  }
  else if(state == 3)
  {
    
    lcd.setCursor(0,0);
    lcd.print("REFLOW ZONE     ");
    lcd.setCursor(0,1);
    lcd.print(read_temp);
    lcd.print("    ");
    lcd.print(set_temp);
    digitalWrite(3,1);
    delay(1000);
    if(read_temp>230)
    {
      digitalWrite(9,1);
      delay(5000);
      state=4;
      digitalWrite(3,0);
      digitalWrite(9,0);
    }
  }
  else if(state == 4)
  {
    lcd.setCursor(0,0);
    lcd.print("OPEN DOOR     ");
    lcd.setCursor(0,1);
    lcd.print(read_temp);
    lcd.print("    ");
    lcd.print(set_temp);
    digitalWrite(3,0);
    delay(1000);
    
  }
  else if (state==5)
  {
    lcd.setCursor(0,0);
    lcd.print("HAND MODE       ");
    lcd.setCursor(0,1);
    lcd.print(read_temp);
    
    count++;
    delay(10);
    if(count>150) 
    {
      count=0;
      read_temp = ktc.readCelsius();
    }
    if(digitalRead(8) == 0) 
    {
      while(digitalRead(8) == 0){};
      if(digitalRead(3)==0) 
      {
        digitalWrite(3,1);
        lcd.print(" HEAT ON ");
      }
      else 
      {
        digitalWrite(3,0);
        lcd.print(" HEAT OFF");
      }
    }
  }
  
  
}
