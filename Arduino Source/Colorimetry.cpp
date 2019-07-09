#include "Arduino.h"

/*Constants*/
/* Initializations and Pin Numbers*/
#define s0 48
#define s1 49
#define s2 50
#define s3 51
#define out 44
int frequency = 0;
int freqSum = 0;

int red1 = 255, green1 = 184, blue1 = 135;

void init_color(){
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(out, INPUT);

  digitalWrite(s0, HIGH);
  digitalWrite(s1, LOW);
}

//determine temperature
int red_color(){
  // Setting red filtered photodiodes to be read
  freqSum = 0;
  for(int i = 0; i < 10; i++){
    digitalWrite(s2,LOW);
    digitalWrite(s3,LOW);
    // Reading the output frequency
    frequency = pulseIn(out, LOW);
    //Remaping the value of the frequency to the RGB Model of 0 to 255
    frequency = map(frequency, 184,487,255,0);
    frequency = constrain(frequency,0,255);
    freqSum += frequency;
    delay(100);
  }
  // Printing the value on the serial monitor
  //Serial.print("R= ");//printing name
  //Serial.print(freqSum/10);//printing RED color frequency
  //Serial.print("  ");
  delay(100);
  return freqSum/10 ;
}
int green_color(){
  // Setting Green filtered photodiodes to be read
  freqSum = 0;
  for(int i = 0; i < 10; i++){
    digitalWrite(s2,HIGH);
    digitalWrite(s3,HIGH);
    // Reading the output frequency
    frequency = pulseIn(out, LOW);
    //Remaping the value of the frequency to the RGB Model of 0 to 255
    frequency = map(frequency, 62, 330,255,0);
    frequency = constrain(frequency,0,255);
    freqSum += frequency;
    delay(100);
  }
  delay(100);


  return freqSum/10;
}

int blue_color(){
  // Setting Blue filtered photodiodes to be read
  freqSum = 0;
  for(int i = 0; i < 10; i++){
    digitalWrite(s2,LOW);
    digitalWrite(s3,HIGH);
    // Reading the output frequency
    frequency = pulseIn(out, LOW);
    //Remaping the value of the frequency to the RGB Model of 0 to 255
    frequency = map(frequency, 139, 499,255,0);
    frequency = constrain(frequency,0,255);
    freqSum += frequency;
    delay(100);
  }
  delay(100);
  return freqSum/10;
}
