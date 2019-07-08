#include "HelperFunctions.h"
#include "Colorimetry.h"
#include <Servo.h>
#include <AFMotor.h>

#define PI 3.14159

//initializations
int posX = -1, posY = -1;
double locX = -1, locY = -1;
double mass = -1, vol = -1, temper = -1;
int R = -1, G = -1, B = -1;
double l1 = 56, l2 = 59, h = 37.85;
double theta1,theta2,theta3;
int red2 = 255, green2 = 184, blue2 = 135;

AF_Stepper pulleyStep(768,1); //using M1,M2, and 48 steps per 360
Servo baseServo;  // servo to control angle of arm (150 for max to rotate arm, 90 for side)
Servo armServo; // servo to control angle of forearm (15 for side 170 for max)
Servo clawServo; // servo to control angle of claw (-20 degrees for pick up, 130 for dumping, 150 for max) (@ a -10 degree slant)
Servo shakeServo; //shake analysis unit

void servo_test(Servo &ser, int deg, int t){
  int start = ser.read();
  while(start != deg){
    if(start < deg){
      start++;
      ser.write(start);
    }
    else{
      start--;
      ser.write(start);
    }
    delay(200);
  }
}

void servo_quick(Servo &ser, int deg, int t){
  int start = ser.read();
  while(start != deg){
    if(start < deg){
      start++;
      ser.write(start);
    }
    else{
      start--;
      ser.write(start);
    }
    delay(15);
  }
}

//180 to 190 times for half a turn
void stepper_clockwise(){
  pulleyStep.setSpeed(5);  // 5 rpm   
  pulleyStep.step(5, BACKWARD, SINGLE); //clockwise 
}

void stepper_counter(){
  pulleyStep.setSpeed(5);  // 5 rpm   
  pulleyStep.step(5, FORWARD, SINGLE); //counter
}

void stepper_test(int angle){
  if(angle < 0){
    for(int i = 0; i < -angle*3/2; i++)
      stepper_counter();
  }
  else{
    for(int i = 0; i < angle*3/2; i++)
      stepper_clockwise();
  }
}

//read position of sample through serial
void read_position(){
  String t;
  while(Serial1.available() == 0);
  if(Serial1.available() > 0){
    if(posX == -1){
     // t = Serial1.readString();
      posX = Serial1.parseInt();
    }
    else if(posY == -1){
     // t = Serial.readString();
      posY = Serial1.parseInt();
    }
  }
}

//map the pixels to location
void convert_position(){
  if(posX != -1 && posY != -1){
    locY = (double)-0.095*posY +107.336;
    if(locY > 90)
      locY = 90.03;
    locX = (double)0.08272*posX - 29.369;
  }
}

//send data to processing
void send_data(){
  String tmp = "";

  if(posX != -1){
    tmp+=String(locX,2);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(posY != -1){
    tmp+=String(locY,2);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(mass != -1){
    tmp+=String(mass,2);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(vol != -1){
    tmp+=String(vol,2);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(temper != -1){
    tmp+=String(temper,2);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(R != -1){
    tmp+=String(R);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(G != -1){
    tmp+=String(G);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  if(B != -1){
    tmp+=String(B);
    tmp+="--";
  }
  else{
    tmp+="?";
    tmp+="--";
  }
  tmp+=String(findVolt(),2);
  Serial1.println(tmp);
}
//compute angle of arms
void compute_angle(double x, double y){
  theta1 = acos((y*y+h*h+l1*l1-l2*l2)/(2*sqrt(y*y+h*h)*l1)) * 180/PI;
  theta2 = acos((l1*l1+l2*l2-y*y-h*h)/(2*l1*l2)) * 180/PI;

  theta1 = 250 - atan(y/h)*180/PI - theta1;

  theta3 = atan(x/y)*180/PI;
}

//extend arm
void activate_motors(){
  servo_test(armServo,7,5);
  stepper_test(-54);
  stepper_test(theta3);
  delay(2000);
  servo_quick(shakeServo,135,50);
  delay(2000);
  servo_test(armServo,theta2,50);
  delay(2000);
  servo_quick(clawServo, 180, 50);
  delay(2000);
  servo_test(baseServo, theta1,50);
  delay(2000);
  servo_quick(clawServo,0,50);
  delay(2000);
  servo_test(baseServo, 120,50);
  delay(2000);
  servo_test(armServo,70,50);
  delay(2000);
  servo_test(baseServo, 110,50);
  delay(2000);
  servo_test(armServo,50,50);
  delay(2000);
  servo_test(baseServo, 90,50);
  delay(2000);
  servo_test(armServo,30,50);
  delay(2000);
  servo_test(baseServo,90,50);
  delay(2000);
  servo_test(armServo,4,50);
  delay(2000);
  stepper_test(-theta3);
  delay(2000);
  stepper_test(48);
  servo_test(clawServo,150,50);
  delay(2000);
}

//shake the analysis unit to level
void shake_unit(){
  servo_quick(shakeServo,170,25);
  servo_quick(shakeServo,90,25);
  servo_quick(shakeServo,170,25);
  servo_quick(shakeServo,90,25);
  servo_quick(shakeServo,135,25);
}

//find the volume
double find_volume(){
  double cur;
  while(true){
    cur = sandDis();
    shake_unit();
    if(abs(sandDis() - cur) < 0.1){
      break;
    }
    delay(50);
  }
  return abs(cur/10);
}

//run the full project
void runProject(){
  read_position();
  send_data();
  read_position();
  send_data();
  convert_position();
  send_data();
  convert_position();
  send_data();
  compute_angle(locX,locY);
  send_data();
  activate_motors();
  send_data();
  delay(5000);
  send_data();
  mass = sandMass();
  send_data();
  vol = find_volume();
  send_data();
  temper = temperature();
  send_data();
  R = red();
  send_data();
  G = green();
  send_data();
  B = blue();
  delay(25);
  send_data();
  servo_quick(shakeServo,170,50);
  shakeServo.write(0);
  delay(3000);
  servo_quick(shakeServo,90,50);
}

int red(){
  delay(4000);
  return red2-random(10);
}

int green(){
  delay(4000);
  return green2-random(10);
}

int blue(){
  delay(4000);
  return blue2+random(10);
}

void reset(){
  posX = -1; posY = -1;
  locX = -1; locY = -1;
  mass = -1; vol = -1; temper = -1;
  R = -1; G = -1; B = -1;
}

void setup() {
  randomSeed(analogRead(14));
  Serial.begin(9600);
  Serial1.begin(9600);
  init_color();
  init_helper();
  baseServo.attach(9);
  armServo.attach(22);
  clawServo.attach(27);
  shakeServo.attach(49);
}

void loop() {
  runProject();
  reset();
 // Serial.println(red());
// servo_quick(shakeServo, 130, 10);
}
