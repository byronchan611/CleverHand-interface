//#define TEENSY_41
#define IOT33
//#define UDP_MODE
#define VERSION 0x0200
#include "clvHd_util.hpp"
#include "com.hpp"

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

#define DEBUG(x) Serial.print(x)
#define DEBUGF(x,y) Serial.print(x,y)
#define DEBUGLN(x) Serial.println(x)


byte buff[255];
int pkgSize = 6;
byte vals[255] = {0x00, 0x00, 0x00, 0x00};
int i, nb=0;

ClvHdEMG clvHdEMG;
Com com_interface;

int ii=0;

void setup()
{
  
  Serial.begin(9600);
  com_interface.begin(500000, 5000, 192, 168, 127, 253);
  clvHdEMG.begin();
  
}


void loop()
{
  //delay(10);
  int nn = com_interface.available();
  if (nn >= pkgSize)
  {
    com_interface.read(buff, pkgSize);
    switch (buff[0])
    {
      case 'r':// Reading cmd > 'r' | id | reg | n
        {
          clvHdEMG.readRegister(buff[2], vals, buff[3], buff[1]);
          com_interface.write((uint8_t*)vals, buff[3], true);
          break;
        }
        case 'R':// Reading cmd > 'R' | 0 | reg | n
        {
          for(i=0;i<nb;i++)
              clvHdEMG.readRegister(buff[2], vals+buff[3]*i, buff[3], 0xf-i);
          com_interface.write((uint8_t*)vals, buff[3]*nb, true);
          
          break;
        }
      case 'w':// Reading cmd > 'w' | id | reg | val
        {
          clvHdEMG.writeRegister(buff[2], buff[3], buff[1]);
           buff[1] = buff[3];
          com_interface.write((uint8_t*)buff, 2, true);
          break;
        }
      case 'n':// Nb module cmd > 'n' | 0 | 0 | 0
        {
          nb = clvHdEMG.get_nbBoard();
          buff[0] = nb;
          com_interface.write((uint8_t*)buff, 2, true);
          break;
        }
      case 'b':// Blink cmd > 'b' | id | time_cs | nb_repeat
        {
          clvHdEMG.blink(buff[1], buff[2], buff[3]);
          break;
        }
      case 'm':// Mirror cmd > 'm' | b1 | b2 | b3
        {
          com_interface.write((uint8_t*)buff, 4, true);
          Serial.println("hey");
          break;
        }
      case 'v':// Version cmd > 'v' | 0 | 0 | 0
        {
          *(uint16_t *)(buff) = VERSION;
          com_interface.write((uint8_t*)buff, 2, true);
          break;
        }
      case 'z':// Reset cmd > 'z' | 0 | 0 | 0
        {
          break;
        }
    }
  }


}
