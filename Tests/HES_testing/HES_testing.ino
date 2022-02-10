/*****************************************************************************
Title: HES_testing

Description:
Simple program to read the analouge values of the Hall effect sensors on the 
PCB. This is done by accessing the mcp3004 on the pcb and selecing 
corresponding channel for the desired sensor. 


Author: Jerzy Aleksander Gorak
******************************************************************************/

#include <Adafruit_MCP3008.h>
#define CS1 13 // CS1 because there are more than one mcp3004 to deal with.(1 per PCB aka face) 
Adafruit_MCP3008 adc;
void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("MCP3008 simple test.");

  adc.begin(SCK,MOSI,MISO,CS1);

  //         (sck, mosi, miso, cs);
  //adc.begin(13,    11,   12, 10);
}

void loop() {
  
  Serial.print(adc.readADC(0)); Serial.print("\t");
  Serial.print("["); Serial.print("D1"); Serial.println("]");
  Serial.print(adc.readADC(1)); Serial.print("\t");
  Serial.print("["); Serial.print("D2"); Serial.println("]");
  Serial.print(adc.readADC(2)); Serial.print("\t");
  Serial.print("["); Serial.print("D3"); Serial.println("]");
  Serial.print(adc.readADC(3)); Serial.print("\t");
  Serial.print("["); Serial.print("D4"); Serial.println("]");
  
  delay(100);
}
