#include <SPI.h>

#define DEBUG(x) Serial.print(x)
#define DEBUGF(x,y) Serial.print(x,y)
#define DEBUGLN(x) Serial.println(x)

const byte READ = 0b10000000;
const byte WRITE = 0b00000000;
const byte REG_MASK = 0b01111111;


const int addPins[4] = {7, 8, 9, 10};
const int gpio1Pin = 6;
char c;
byte id;
byte reg;
byte n;
byte val = 0x00;
byte vals[255] = {0x00, 0x00, 0x00, 0x00};
uint8_t boardId = 15;

byte s_reg;
byte s_n;
bool streaming = false;
uint16_t s_board = 0;


void selectBrd(uint8_t id)
{
  //digitalWrite(add1Pin , id );
  for (int i = 0; i < 4; i++)
    digitalWrite(addPins[ i] , (id >> i) & 1 );
}


void readRegister(byte reg, byte val[], unsigned n = 1, uint8_t id = 15)
{
  byte dataToSend = READ | (REG_MASK & reg);

  selectBrd(id);
  SPI.transfer(dataToSend);
  for (int i = n - 1; i >= 0; i--)
  {
    *(val + i) = 0;
    *(val + i) = SPI.transfer(0x00);
  }
  selectBrd(0);
}

void readRegister_(byte reg, byte val[], unsigned n = 1, uint8_t id = 15)
{
  byte dataToSend = READ | (REG_MASK & reg);

  selectBrd(id);
  SPI.transfer(dataToSend);
  for (int i = 0; i <n; i++)
  {
    *(val + i) = 0;
    *(val + i) = SPI.transfer(0x00);
  }
  selectBrd(0);
}

void writeRegister(byte reg, byte val, uint8_t id = 15)
{
  byte dataToSend = WRITE | (REG_MASK & reg);;
  selectBrd(id);
  SPI.transfer(dataToSend);
  SPI.transfer(val);
  selectBrd(0);
}

void clean_start()
{
  // ARDUINO SIDE
  Serial.write(0xaa);
  char c;
  while (c != 0xbb)
    c = Serial.read();
}

int nb_emg_connected()
{
  uint8_t nb = 0;
  for (int i = 0; i < 16; i++)
  {
    readRegister(0x40, vals, 1, i);
    if (vals[0] == 0x01)
      nb++;
  }
  return nb;

}

void setup()
{
  Serial.begin(115200);
  SPI.begin();
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  for (int i = 0; i < 4; i++)
    pinMode(addPins[i], OUTPUT);
  pinMode(gpio1Pin, OUTPUT);
  digitalWrite(gpio1Pin, HIGH);


  delay(1000);

  clean_start();

  int n = nb_emg_connected();

  Serial.write(n);
  for(int j=15; j>15-n; j--)
  {
    for (int i = 0; i < 5; i++)
    {
      selectBrd(j*(i % 2));
      delay(30);
    }
  }

}

void loop()
{
  if (Serial.available() > 0)
  {
    c = Serial.read();
    
    if (c == 'r')
    {
      id = Serial.read();
      reg = Serial.read();
      n = Serial.read();
      readRegister_(reg, vals, n, id);
      //vals[0]=id;
      Serial.write(vals, n);
    }
    if (c == 'w')
    {
      id = Serial.read();
      reg = Serial.read();
      val = Serial.read();
      writeRegister(reg, val, id);
    }
    if (c == 's')//stream
    {
      streaming = true;
      s_reg = Serial.read();
      s_n = Serial.read();
    }
    if (c == 'n')// stop stream
    {
      streaming = false;
    }

  }
  if (streaming == true)
  {
    readRegister(0x30, vals, 1);
    if (vals[0] & 0b00100000)
    {
      readRegister_(s_reg, vals, s_n);
//      vals[0]=vvv;
//      vals[1]=0x00;
//      vals[6]=0x12;
//      vals[7]=0x34;
//      vals[8]=0x56;
      Serial.write(vals, s_n);
    }
    //byte vv[4] = {0xa4, 0xb6, 0x00, 0x00};
    //Serial.write(vv, 2);
  }

  //delay(10);
}
