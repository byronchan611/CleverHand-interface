//#define TEENSY_41
#define VERSION 0x0200
#include "clvHd_util.hpp"
#include "com.hpp"

#define DEBUG(x) Serial.print(x)
#define DEBUGF(x,y) Serial.print(x,y)
#define DEBUGLN(x) Serial.println(x)


byte buff[255];
int pkgSize = 6;
byte vals[255] = {0x00, 0x00, 0x00, 0x00};

ClvHdEMG clvHdEMG;
Com com_interface;

void setup()
{
  clvHdEMG.begin();
  com_interface.begin(500000, 192, 168, 127, 253, 5000);
}


void loop()
{
  if (com_interface.available() >= pkgSize)
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
      case 'w':// Reading cmd > 'w' | id | reg | val
        {
          clvHdEMG.writeRegister(buff[2], buff[3], buff[1]);
          break;
        }
      case 'n':// Nb module cmd > 'n' | 0 | 0 | 0
        {
          buff[0] = clvHdEMG.get_nbBoard();
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
  else
    delay(10);


}
