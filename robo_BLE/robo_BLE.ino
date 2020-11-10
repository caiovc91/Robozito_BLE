#include <Wire.h>
#include "BluetoothSerial.h"
#include <ESP32Servo.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

/*

Pinout Ponte H / motores:
13 - ENA - motor da direita
12 - ENB - motor da esquerda

A
14 - IN1
27 - IN2
B
26 - IN3
25 - IN4

Pino Motor chute:

*/
Servo myservo;
//variaveis motor
int enable[2] = {13, 12};
int input[4] = {14, 27, 26, 25};
int servo = 4;

//variavel controle chute
int c = 0;

//variaveis pwm
const int channel_L = 0;
const int channel_R = 2;
const int channel_K = 3;
const int freq = 78125;
const int resol = 8;

//variaveis joystick
char message;
char incomingChar;

BluetoothSerial SerialBT;

void forward(){
  digitalWrite(input[0], HIGH);
  digitalWrite(input[1], LOW);  
  digitalWrite(input[2], HIGH);
  digitalWrite(input[3], LOW);
  
  for(int s = 200; s < 215; s++){
  ledcWrite(channel_L, s);
  ledcWrite(channel_R, s);
  }
  delay(500);
  ledcWrite(channel_L, 0);
  ledcWrite(channel_R, 0);

}

void left(){
  ledcWrite(channel_L, 0);
  for(int s = 200; s < 215; s++){
  ledcWrite(channel_R, s);
  delay(10);
  }
  ledcWrite(channel_L, 215);
  delay(10);
  ledcWrite(channel_L, 0);
  ledcWrite(channel_R, 0);
}

void right(){
  ledcWrite(channel_R, 0);
  for(int s = 200; s < 215; s++){
  ledcWrite(channel_L, s);
  delay(10);
  }
  ledcWrite(channel_R, 215);
  delay(10);
  ledcWrite(channel_L, 0);
  ledcWrite(channel_R, 0);
}

void reverse(){
  digitalWrite(input[0], LOW);
  digitalWrite(input[1], HIGH);  
  digitalWrite(input[2], LOW);
  digitalWrite(input[3], HIGH); 
  
  for(int s = 200; s < 215; s++){
  ledcWrite(channel_L, s);
  ledcWrite(channel_R, s);
  }
  delay(500);
  ledcWrite(channel_L, 0);
  ledcWrite(channel_R, 0);
  
   
}

void action(){

  for(c = 0; c <= 30; c++){
    ledcWrite(channel_K, c);
    //myservo.write(c);
    delay(10);
  }
  delay(100);
  for(c = 30; c >= 0; c--){
    ledcWrite(channel_K, c);
    //myservo.write(c);
    delay(10);
  }
}

void retract(){
  for(c = 30; c >= 0; c--){
    ledcWrite(channel_K, c);
    delay(10);
  }
}

void setupEngine(){
  for(int en = 0; en < 2; en++){
    pinMode(enable[en], OUTPUT);
  }
    for(int mot = 0; mot < 4; mot++){
    pinMode(input[mot], OUTPUT);
  }
  //habilita pwm Direita
  ledcAttachPin(enable[0], channel_R);

    //habilita pwm Esquerda
  ledcAttachPin(enable[1], channel_L);

  ledcSetup(channel_L, freq, resol);
  ledcSetup(channel_R, freq, resol);
}

void setupServo(){
  ledcAttachPin(servo, channel_K);
  ledcSetup(channel_K, freq, resol);
}

void set_motor_params_R(){
  digitalWrite(input[0], HIGH);
  digitalWrite(input[1], LOW);  
}
void set_motor_params_L(){
  digitalWrite(input[2], HIGH);
  digitalWrite(input[3], LOW);  
}

void test_speedLR(){
  ledcWrite(channel_L, 229);
  ledcWrite(channel_R, 229);
}

void test_pwm(){
    for (int i = 149; i < 229; i++){
    ledcWrite(channel_L, i);
    ledcWrite(channel_R, i);
    Serial.println(i);
    delay(150);
  }
}

void setup() {
  setupEngine();
  setupServo();
  c = 0;
  //myservo.attach(servo);
  Serial.begin(115200);
  SerialBT.begin("ESP32test"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
   if (SerialBT.available()){
    char incomingChar = SerialBT.read();
    if (incomingChar != '\n'){
      message = incomingChar;
      
      switch(message){
        case 'w':
        forward();
        break;
        
        case 's':
        reverse();
        break;
        
        case 'd':
        right();
        break;
        
        case 'a':
        left();
        break;
        
        case 'c':
        action();
        break;
      }
    }
    Serial.write(incomingChar);  
  }
  delay(20);
}
