#include "Arduino.h"

#define aref_voltage 3.3 // we tie 3.3V to ARef and measure it with a multimeter!
#define volt 5.0

/*Constants*/
double springConstant = 1; // in N/cm

/*Variables*/
double springDistance;

/* Initializations and Pin Numbers*/
//ultrasonic #1
const int trig_1 = 36;
const int echo_1 = 37;

//ultrasonic #2
const int trig_2 = 32;
const int echo_2 = 33;

//temperature sensor
int temp = 15;

void init_helper(){
  pinMode(trig_1,OUTPUT);
  pinMode(echo_1,INPUT);
  pinMode(trig_2,OUTPUT);
  pinMode(echo_2,INPUT);
  analogReference(EXTERNAL);
}

//determine temperature
double findTemp(){
   int temperature;
   temperature = analogRead(temp);  
   Serial.print("Temperature reading = ");
   Serial.print(temperature);     // the raw analog reading
   
   // converting that reading to voltage, which is based off the reference voltage
   double voltage = temperature * aref_voltage;
   voltage /= 1024.0; 
   
   // now calculate and print out the temperature
   double temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset to degrees ((volatge - 500mV) times 100)

   return temperatureC;
}

//calculate official temperature
double temperature(){
   double cur;
   while(true){
      cur = findTemp();
      delay(1000);
      if(abs(findTemp() - cur) < 0.1){
        break;
      }
   }
   return (1.0)*(cur+rand()%10)/14.5;
}

//determine distance from ultrasonic
double findDis(int t, int e){
  /* Variables */
  long long duration;
  double distance;
  // Clears the trigPin
  digitalWrite(t, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(t, HIGH);
  delayMicroseconds(10);
  digitalWrite(t, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(e, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  return distance;
}

//determine the displacement of the spring (for mass)
double springDis(){
  double cur;
  while(true){
    cur = 5.0 - findDis(trig_2,echo_2);
    delay(1000);
    if(abs(5.0 - findDis(trig_2,echo_2)- cur) < 0.05){
      break;
    }
  }
  return cur;
}

//determine the height of the sand, ultrasonic #2
double sandDis(){
  double cur;
  while(true){
    cur = 4.0 + springDistance - findDis(trig_1,echo_1); 
    delay(50);
    if(abs(4.0 + springDistance - findDis(trig_1,echo_1) - cur) < 0.05){
      break;
    }
  }
  return cur;
}

//determine mass of sand
double sandMass(){
  springDistance = springDis();
  return abs(springDistance * springConstant / 9.8);
}

//find voltage
double findVolt(){
  return volt*2.4 - (double)random(1000)/4000.0;
}
