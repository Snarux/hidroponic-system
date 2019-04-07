#include <LCD5110_Basic.h>
#include "DHT.h"

#define min_v_adc 443
#define max_v_adc 625
#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11

//nokia
LCD5110 myGLCD(8, 9, 10, 11, 12);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

//dth
DHT dht(DHTPIN, DHTTYPE);

//Animation
int animation = 0;

//Bip
const int buzzer = 13;

//Time
long previousMillis = 0;
long interval = 60000;
int timer = 1;
long calctimer = 0;
int oldtimer = 0;
int minutes = 0;
String txt1;
String txt2;
int contA = 0;

//Battery
int batt = 0, batt_percentage = 0;
float batt_v = 0;
float calc_media;
int cont_calc_media;
long percent;

//Pump
int pump = 0;
int oldpump = 0;
void setup()
{

  Serial.begin(9600);
 
  dht.begin();
  /* pinar */
  pinMode(PIN2, OUTPUT);

  /* turn nokia on */
  myGLCD.InitLCD();

  /* bip if alive */
  int i = 100;
  pinMode(buzzer, OUTPUT);  while (i < 1901) {
    tone(buzzer, i);
    delay(50);
    i = i + 100;
  } noTone(buzzer);
}

// ########################################## START HERE ##########################################
void loop()
{
  //Get global time
  unsigned long currentMillis = millis();
  //Calc time passed
  calctimer = currentMillis - previousMillis;
  //Convert time to seconds
  timer = ((calctimer / 1000) % 60);

  //limit one cicle per second
  if (oldtimer != timer) {

    //refresh nokia clock
    nokia();

    // animation
    if (pump > 0) {
      animation++;
      animation_pump();
    } else {
      offanimation_pump();
    }

    //ShowTime
    if (timer == 2) {
      temp();
    }
    if (timer == 3)  {
      battery();
    }
    if (timer == 5)  {
      battery();
    }
    if (timer == 7)  {
      battery();
    }
    if (timer == 9)  {
      battery();
    }
    if (timer == 10) {
      battery();
    }
    if (timer == 12)  {
      battery();
    }
    if (timer == 13)  {
      battery();
    }    
    if (timer == 15) {
      battery();
    }
    if (timer == 18) {
      battery();
    }
    if (timer == 20) {
      battery();
    }

    if (timer == 50) {
      pumping();
      if (pump != oldpump) {
        tone(buzzer, 100);
      }
    }
    if (timer == 51) {
      noTone(buzzer);
      oldpump = pump;
    }

    //reset seconds
    if (timer > 60) {
      previousMillis = currentMillis;
    }

    //detect errors
    if ((timer < 60) && (timer > 60)) {
      asm volatile ("  jmp 0");
    }

  } oldtimer = timer;
  //.limit one cicle per second
}

// ###PUMPING
void pumping() {

  pump = 0;

  Serial.println("###");

  if (percent > 95) {
    myGLCD.setFont(SmallFont); myGLCD.print("Full---------", LEFT, 8);
    /**/ pump++;
  }

  if ((percent > 80) && (percent < 96)) {
    myGLCD.setFont(SmallFont); myGLCD.print("0-10-20-40-50", LEFT, 8);
    /**/ if ((minutes == 0) || (minutes == 10) || (minutes == 20) || (minutes == 40) || (minutes == 50)) {
      pump++;
    }
  }

  if ((percent > 70) && (percent < 81)) {
    myGLCD.setFont(SmallFont); myGLCD.print("--10-20-45---", LEFT, 8);
    /**/ if ( (minutes == 10) || (minutes == 30) || (minutes == 45)) {
      pump++;
    }
  }

  if ((percent > 60) && (percent < 71)) {
    myGLCD.setFont(SmallFont); myGLCD.print("-----30------", LEFT, 8);
    /**/ if (minutes == 30) {
      pump++;
    }
  }
  

  if (percent < 61) {
    myGLCD.setFont(SmallFont); myGLCD.print("-------------", LEFT, 8);
  }
/* 
  if ((percent > 40) && (percent < 51)) {
    myGLCD.setFont(SmallFont); myGLCD.print("0-30---------", LEFT, 8);
      if ((minutes == 0) || (minutes == 30)) {
      pump++;
    }
  }

  if ((percent > 30) && (percent < 41)) {
    myGLCD.setFont(SmallFont); myGLCD.print("0------------", LEFT, 8);
    
    if ((minutes == 0)) {
      pump++;
    }
   
  }
 */
 
  if (percent < 61) {
    myGLCD.setFont(SmallFont);
    myGLCD.print("EMPTY--------", LEFT, 8); /**/ pump = 0;
  }


  //can i pump?
  if (pump > 0) {
    digitalWrite(PIN2, HIGH);
  } else {
    digitalWrite(PIN2, LOW);
  }

  Serial.println(minutes);
  Serial.println(percent);
  Serial.println(pump);
}

// ## Monitor
void nokia() {
  if (timer == 59) {
    minutes = minutes + 1;
  }
  if (minutes == 61) {
    minutes = 0;
  }

  myGLCD.setFont(SmallFont);
  
  myGLCD.print("      ", RIGHT, 0);
  txt1 = minutes;
  txt2 = timer;
  myGLCD.print(txt1 + "m" + txt2 + "s", RIGHT, 0);

}

// ## BATTERY
void battery() {

  batt = analogRead(0);

  //ADC counts -> Minimum: 443; Maximum: 625
  //voltage: Min:6.0; Max: 8.4

  batt_v = map(batt, min_v_adc, max_v_adc, 600, 840);

  float batt_voltage = batt_v / 100;
  batt_percentage = map(batt, min_v_adc, max_v_adc, 0, 100);

  calc_media = calc_media + batt_percentage;
  cont_calc_media++;

  //low batt bip disable
  noTone(buzzer);

  if (cont_calc_media == 10) {
    myGLCD.setFont(BigNumbers);
    percent = calc_media / 10; if (percent > 99) {
      percent = 99;
    }
    if (percent > 100) {
      percent = 99;
    }
    myGLCD.setFont(SmallFont);
    myGLCD.print("Battery:", LEFT, 24);
    myGLCD.setFont(MediumNumbers);
    myGLCD.printNumF(float(batt_voltage), 2, LEFT, 32);
    myGLCD.setFont(BigNumbers);
    myGLCD.printNumI(percent, RIGHT, 24);

    //reset
    cont_calc_media = 0;  calc_media = 0;

    //low batt bip
    if (percent < 20) {
      tone(buzzer, 100);
    }

  }

}

// ## TEMP
void temp() {

  //myGLCD.setFont(SmallFont);
  //myGLCD.print("Temp:", LEFT, 8);
  //myGLCD.print("Xc", RIGHT, 8);
  // A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    //Serial.println("Failed to read from DHT");
  } 
  else
  {
    myGLCD.setFont(SmallFont); 
    myGLCD.printNumF(float(t), 0, LEFT, 0);
    myGLCD.printNumF(float(h), 0, 24, 0);   
    myGLCD.print("C", 12, 0);  
    myGLCD.print("%", 37, 0);
  }
  
}

// ## ANIMATION 
void animation_pump() {

  myGLCD.setFont(SmallFont);

  if (animation == 1) {
    myGLCD.print("PUMPING:     ", LEFT, 16);
  }
  if (animation == 2) {
    myGLCD.print("PUMP:>       ", LEFT, 16);
  }
  if (animation == 3) {
    myGLCD.print("PUMP:=>      ", LEFT, 16);
  }
  if (animation == 4) {
    myGLCD.print("PUMP:==>     ", LEFT, 16);
  }
  if (animation == 5) {
    myGLCD.print("PUMP:===>    ", LEFT, 16);
  }
  if (animation == 6) {
    myGLCD.print("PUMP:====>   ", LEFT, 16);
  }
  if (animation == 7) {
    myGLCD.print("PUMP:=====>  ", LEFT, 16);
  }
  if (animation == 8) {
    myGLCD.print("PUMP:======> ", LEFT, 16);
  }
  if (animation == 9) {
    myGLCD.print("PUMP:=======>", LEFT, 16);
  }
  if (animation == 10) {
    myGLCD.print("PUMP: =======", LEFT, 16);
  }
  if (animation == 11) {
    myGLCD.print("PUMP:  ======", LEFT, 16);
  }
  if (animation == 12) {
    myGLCD.print("PUMP:   =====", LEFT, 16);
  }
  if (animation == 13) {
    myGLCD.print("PUMP:    ====", LEFT, 16);
  }
  if (animation == 14) {
    myGLCD.print("PUMPI:    ===", LEFT, 16);
  }
  if (animation == 15) {
    myGLCD.print("PUMPIN:    ==", LEFT, 16);
  }
  if (animation == 16) {
    myGLCD.print("PUMPING:    =", LEFT, 16);
    animation = 0;
  }

}
void offanimation_pump() {

  myGLCD.setFont(SmallFont);
  myGLCD.print("PUMPING: OFF  ", LEFT, 16);

}
