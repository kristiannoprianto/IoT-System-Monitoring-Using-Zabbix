//************************************************
//Author  : Kristian Noprianto (Telkom University)
//Purpose : (Arduino side)
//************************************************


#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "GravityTDS.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 

//DS18B20
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//TDS
#define TdsSensorPin A1
GravityTDS gravityTds;
int tdsValue;

//Turbidity
#define turbPin A2
int rawturbidity;
int buffer_ar[50], tempO;
float volt;
float totaL;
float ntu;
 
//Ph
#define phPin A3
int pH_Value; 
int buffer_arr[50], tempo;
float Voltage;
float total;
float pH;

//ultrasonic
const int TRIG_PIN = 4;
const int maiN = 7;
const int ab = 5;
const int ph = 6;
int fst;
int sec;
int trd;

//packet identity
#define startMarker '<'
#define endMarker '\n'

byte pH1, pH2, pH3, pH4;
byte Turb1, Turb2, Turb3, Turb4;
byte temp1, temp2, temp3, temp4;
byte TDS1, TDS2, TDS3, TDS4;
byte Vol1, Vol2, Vol3, Vol4;
byte vOl1, vOl2, vOl3, vOl4;
byte voL1, voL2, voL3, voL4;
byte sts1, sts2, sts3, sts4;

//Status code
int sts;
//0 = Kondisi hidroponik dalam keadaan normal
//1 = pH air pada bak hidroponik tidak normal
//2 = suhu air pada bak hidroponik tidak normal
//3 = konsentrasi AB Mix di bawah batas normal
//4 = air pada bak hidroponik dalam keadaan keruh
//5 = proses pengosongan bak hidroponik sedang berlangsung
//6 = bak hidroponik belum terisi
//7 = bak AB Mix belum terisi
//8 = bak pH buffer belum terisi

unsigned long now=0;

SoftwareSerial zabb(10,11);
 
void setup() 
{
  Serial.begin(9600);
  zabb.begin(115200);
  pinMode(maiN, INPUT);
  pinMode(ab, INPUT);
  pinMode(ph, INPUT);
  pinMode(TdsSensorPin, INPUT);
}
void loop(){
  wrapData();
  delay(50);
}

float pHval(){
  pH_Value = analogRead(phPin); 

  for(int i=0;i<50;i++){
    buffer_arr[i]=pH_Value;
    delay(30);
  }
  for(int i=0;i<49;i++){
    for(int j=i+1;j<50;j++){
      if(buffer_arr[i]!=buffer_arr[j]) {
        tempo=buffer_arr[i];
        buffer_arr[i]=buffer_arr[j];
        buffer_arr[j]=tempo;
      }
    }
  }
  total=0;
  for(int i=2;i<48;i++){
    total+=buffer_arr[i];
  }
  Voltage=total*(5.0/1024.0)/46;
  pH= ((-2.95*(Voltage-0.05))+14);
  return pH;
}

int Turbval(){
  rawturbidity = analogRead(turbPin);
  for(int i=0;i<50;i++){
    buffer_ar[i]=rawturbidity;
    delay(30);
  }
  for(int i=0;i<49;i++){
    for(int j=i+1;j<50;j++){
      if(buffer_ar[i]!=buffer_ar[j]) {
        tempO=buffer_ar[i];
        buffer_ar[i]=buffer_ar[j];
        buffer_ar[j]=tempO;
      }
    }
  }
  totaL=0;
  for(int i=2;i<48;i++){
    totaL+=buffer_ar[i];
  }
  volt = totaL/46*(5.0/1024.0)+0.5;
  ntu = -2857.142857*((volt*volt)-(6.7*volt)+10.5);
  return ntu;
}

float temp(){
  sensors.requestTemperatures();
  float tmp = sensors.getTempCByIndex(0);
  return tmp;
}

float TDSval(){
  gravityTds.setTemperature(temp());  // set the temperature and execute temperature compensation
  gravityTds.update();  //sample and calculate
  tdsValue = gravityTds.getTdsValue();  // then get the value
  return tdsValue;
}

int volume(int bucket){
  long duration, duration2, duration3, distanceCm;

  if(bucket == 1){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(maiN, HIGH);
    distanceCm = duration/29.1/2;
    fst = ((18-(distanceCm-4))/18)*100;
    return fst;
  }
  else if(bucket == 2){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration2 = pulseIn(ab, HIGH);
    distanceCm = duration2/29.1/2;
    sec = ((11-(distanceCm-2))/11)*100;
    return sec;
  }
  else if(bucket == 3){
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    duration3 = pulseIn(ph, HIGH);
    distanceCm = 11-duration3/29.1/2;
    trd = ((11-(distanceCm-2))/11)*100;
    return trd;
  }
  else{
    Serial.println("No Index");
  }
  
}

typedef union{
  float number;
  uint8_t bytes[4];
}FLOATUNION_t;

void wrapData(){
  FLOATUNION_t myFloat;
  FLOATUNION_t myFloat1;
  FLOATUNION_t myFloat2;
  FLOATUNION_t myFloat3;
  FLOATUNION_t myFloat4;
  FLOATUNION_t myFloat5;
  FLOATUNION_t myFloat6;
  FLOATUNION_t myFloat7;
  
  myFloat.number=pHval();
  myFloat1.number=Turbval();
  myFloat2.number=temp();
  myFloat3.number=TDSval();
  myFloat4.number=volume(1);
  myFloat5.number=volume(2);
  myFloat6.number=volume(3);
  myFloat7.number=sts;
  
  pH1=myFloat.bytes[0];
  pH2=myFloat.bytes[1];
  pH3=myFloat.bytes[2];
  pH4=myFloat.bytes[3];
  
  Turb1=myFloat1.bytes[0];
  Turb2=myFloat1.bytes[1];
  Turb3=myFloat1.bytes[2];
  Turb4=myFloat1.bytes[3];

  temp1=myFloat2.bytes[0];
  temp2=myFloat2.bytes[1];
  temp3=myFloat2.bytes[2];
  temp4=myFloat2.bytes[3];
  
  TDS1=myFloat3.bytes[0];
  TDS2=myFloat3.bytes[1];
  TDS3=myFloat3.bytes[2];
  TDS4=myFloat3.bytes[3];
  
  Vol1=myFloat4.bytes[0];
  Vol2=myFloat4.bytes[1];
  Vol3=myFloat4.bytes[2];
  Vol4=myFloat4.bytes[3];
  
  vOl1=myFloat5.bytes[0];
  vOl2=myFloat5.bytes[1];
  vOl3=myFloat5.bytes[2];
  vOl4=myFloat5.bytes[3];
  
  voL1=myFloat6.bytes[0];
  voL2=myFloat6.bytes[1];
  voL3=myFloat6.bytes[2];
  voL4=myFloat6.bytes[3];
  
  sts1=myFloat7.bytes[0];
  sts2=myFloat7.bytes[1];
  sts3=myFloat7.bytes[2];
  sts4=myFloat7.bytes[3];
  
  if(millis()-now>=50){
    sendData(5);
    now = millis();
  }
}

void sendData(byte len){
  byte sensorRead[8][5]= {{0x10, pH1, pH2, pH3, pH4},
                          {0x11, Turb1, Turb2, Turb3, Turb4},
                          {0x12, temp1, temp2, temp3, temp4},
                          {0x13, TDS1, TDS2, TDS3, TDS4},
                          {0x14, Vol1, Vol2, Vol3, Vol4},
                          {0x15, vOl1, vOl2, vOl3, vOl4},
                          {0x16, voL1, voL2, voL3, voL4},
                          {0x17, sts1, sts2, sts3, sts4}};
  zabb.write(startMarker);
  for(int i=0;i<8;i++){
    for(int j=0;j<len;j++){
    zabb.write(sensorRead[i][j]);
  } 
  }
  zabb.write(endMarker);
}
