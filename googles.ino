#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIN 1
#define IN_0 0
#define IN_1 2
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, NEOPIN);

uint8_t mode;        //=0;
uint8_t brillo;      //=15;
uint32_t color;      //  = 0x00FF00; // Start red
uint8_t  offset = 0; // Position of spinny eyes



int color_state=0;
int previous_1=HIGH;
int reading_1;
int previous_2=HIGH;
int reading_2;
int option=0;
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers



//This function will write a 3 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt32(int p_address, uint32_t p_value)
      {
      byte point1 = ((p_value >> 0) & 0xFF);
      byte point2 = ((p_value >> 8) & 0xFF);
      byte point3 = ((p_value >> 16) & 0xFF);
      EEPROM.write(p_address, point1);
      EEPROM.write(p_address + 1, point2);
      EEPROM.write(p_address + 2, point3);
      }
      
//This function will read a 3 byte integer from the eeprom at the specified address and address + 1
uint32_t EEPROMReadInt32(int p_address)
      {
      byte point1 = EEPROM.read(p_address);
      byte point2 = EEPROM.read(p_address + 1);
      byte point3 = EEPROM.read(p_address + 2);
      return ((point1 << 0) & 0xFF) + ((point2 << 8) & 0xFF00)+ ((point3<<16) & 0xFF0000);
      }


void setup() {
//  EEPROMWriteInt32(0,color);
//  EEPROM.write(3,mode);
//  EEPROM.write(4,brillo);
  mode=EEPROM.read(3);
  brillo=EEPROM.read(4);
  color=EEPROMReadInt32(0);
  pixels.begin();
  pixels.setBrightness(brillo); // 1/3 brightness
  pinMode(IN_0,INPUT);
  pinMode(IN_1,INPUT);
  digitalWrite(IN_0,HIGH);
  digitalWrite(IN_1,HIGH);
}
 
void loop() {
  uint8_t  i;
  reading_1 = digitalRead(IN_0);
  reading_2 = digitalRead(IN_1);
  if (reading_1 == LOW && previous_1 == HIGH && millis() - time > debounce) {
    switch (option){
      case 0: mode=(mode+1)&1;
              EEPROM.write(3,mode);
              break;
      case 1: color_state++;
              if (color_state==6) color_state=0;
              switch (color_state){
                case 1: color=0x00FFFF; break;
                case 2: color=0x0000FF; break;
                case 3: color=0xFF00FF; break;
                case 4: color=0xFF0000; break;
                case 5: color=0xFFFF00; break;
                default: color=0x00FF00; break;
              }
              EEPROMWriteInt32(0,color);
              break;
      case 2: brillo+=20;
              EEPROM.write(4,brillo);
              break;
    }
    time = millis();    
  }
  //Lectura de opciones
  if (reading_2 == LOW && previous_2 == HIGH && millis() - time > debounce) {
    //brillo+=32;
    option++;
    if (option==3) option=0;
    time = millis();    
  }
  previous_2 = reading_2;
  previous_1 = reading_1;

  switch(mode){
    case 0: 
      for (i=0;i<32;i++)
          pixels.setPixelColor(i,0);    
      pixels.setPixelColor(offset,color); 
      pixels.setPixelColor(offset-1,color);
      pixels.setPixelColor(15-offset,color);
      pixels.setPixelColor(15-offset+1,color);
      
      pixels.setPixelColor(offset+16,color); 
      pixels.setPixelColor(offset-1+16,color);
      pixels.setPixelColor(15-offset+16,color);
      pixels.setPixelColor(15-offset+1+16,color);
//      for (i=0;i<16;i++){
//        if(offset==i || i==(~(offset)&15)){
//          pixels.setPixelColor(i,color);
//          //pixels.setPixelColor(15-i,color);
//         }
//      }
      offset=(offset+1)& 15;
      break;
    case 1:
       // Spinny wheels (8 LEDs on at a time)
      for(i=0; i<16; i++) {
          uint32_t c = 0;
          if(((offset + i) & 7) < 2)
              c = color; // 4 pixels on...
          pixels.setPixelColor(   i, c); // First eye
          pixels.setPixelColor(31-i, c); // Second eye (flipped)
      }
      pixels.setBrightness(brillo); // 1/3 brightness
      pixels.show();
      offset++;
      break;
  }
  pixels.show();
  delay(50);   
}
