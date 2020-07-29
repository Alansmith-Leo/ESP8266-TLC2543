/*
 * MCU：ESP8266
 * ADC:TLC2543
 * Communication：Wifi
 * Author:Jiaming Luo
 * Date:2020/7/27
*/
#include "stdlib.h"
#include <Wire.h>
float Vx;
const int EOC=D7,CLK=D4,DIN=D3,DOUT=D2,CS=D1;        //ADC Wiring

void setup() {
  Serial.begin(115200);
  analogWriteFreq(50);
  analogWrite(D0, 500);

  //Pin INIT
  pinMode(EOC,INPUT);        //The pin connected to EOC
  pinMode(CLK,OUTPUT);       //The pin connected to I/O CLOCK
  pinMode(DIN,OUTPUT);       //The pin connected to DATA INPUT
  pinMode(DOUT,INPUT);       //The pin connected to DATA OUT
  pinMode(CS,OUTPUT);        //The pin connected to _CS
  digitalWrite(CS,LOW);      //_CS=0
  digitalWrite(CS,HIGH);     //_CS=1
  digitalWrite(CLK,LOW);     //CLK=0

  //Print boot information
  Serial.println();
  Serial.println("Welcome to use this product");
  Serial.println("This product was developed and created by SZTU TEAM");
  Serial.println();
}

void loop() {
  calculate();
  Serial.print("Vx:");
  Serial.println(Vx);
  delay(2000);
}

//Data acquisition
void calculate()
{
  Vx=Analoginput_read(0)/819.0;       //Get the output in all directions
}


//Use "Analoginput_read()" to read the output data of ADC_TLC2543CN
unsigned int Analoginput_read(unsigned char channel)
{
  //INIT
  unsigned int valueread=0,i;    //valueread: Data output from an ADC
  unsigned char Datainput;       //For DATA INPUT which connect to pin AD_Pin17
  Datainput=channel<<4;
  Datainput|=0x02;
  while(digitalRead(EOC)==0)     //Wait for data conversion to complete
  { 
  }
  
  //Initialization port
  digitalWrite(CLK,LOW);     //CLK=0 
  digitalWrite(CS,LOW);      //_CS=0

  for(i=0;i<8;i++)
  {
    valueread>>=1;
    
    //Get the first eight output data for Dataout
    if(digitalRead(DOUT)==1)
    {
      valueread|=0x0800;
    }
    else
    {
      valueread|=0x0000;  
    }

    //Enter the data of Datainput 
    if((Datainput&0x80)==0x80)
    {
      digitalWrite(DIN,HIGH);   //Data IN=1
    }
    else
    {
      digitalWrite(DIN,LOW);    //Data IN=0
    }

    digitalWrite(CLK,HIGH);     //CLK=1   Enter the data 
    delayMicroseconds(3);       //Hold on
    digitalWrite(CLK,LOW);      //CLK=0   Output the next data
    delayMicroseconds(3);       //Hold on
    Datainput<<=1;    
  }  

  //Get the last four output data for Dataout
  for(i=8;i<12;i++)
  {
    valueread>>=1;
    
    if(digitalRead(DOUT)==1)
    {
      valueread|=0x800; 
    }
    else
    {
      valueread|=0x000;  
    }

    digitalWrite(CLK,HIGH);     //CLK=1   
    delayMicroseconds(3);       //Hold on
    digitalWrite(CLK,LOW);      //CLK=0   Output the next data 
    delayMicroseconds(3);       //Hold on
  }
  
  digitalWrite(CS,HIGH);    //CS=1,the ADC goes to sleep
  
  return valueread; 
}
