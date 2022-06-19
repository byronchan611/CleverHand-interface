#include <SPI.h>

#define DEBUG(x) Serial.print(x)
#define DEBUGF(x,y) Serial.print(x,y)
#define DEBUGLN(x) Serial.println(x)

const byte READ = 0b10000000;
const byte WRITE = 0b00000000;
const byte REG_MASK = 0b01111111;


const int addPins[4] = {7, 8, 9, 10};
const int gpio1Pin = 6;
const int resetPin = 14;
char c;
byte id;
byte reg;
byte n;
byte n_board;
byte val = 0x00;
byte vals[255] = {0x00, 0x00, 0x00, 0x00};
uint8_t boardId = 15;

void(* resetFunc) (void) = 0;  // declare reset fuction at address 0

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
  for (int i = 0; i < n; i++)
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
  pinMode(resetPin, OUTPUT);
  digitalWrite(gpio1Pin, HIGH);
  digitalWrite(resetPin, HIGH);


  delay(1000);

  clean_start();

  n_board = nb_emg_connected();

  Serial.write(n_board);
  for (int j = 15; j > 15 - n_board; j--)
    for (int i = 0; i < 5; i++, delay(30))
      selectBrd(j * (i % 2));


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
    if (c == 'z')
    {
      for (int j = 16 - n_board; j < 16; j++)
      {
        writeRegister(0x00, 2, id);
        readRegister_(0x19, vals, 1, j);
      }
      for (int j = 16 - n_board; j < 16; j++)
        for (int i = 0; i < 5; i++, delay(30))
          selectBrd(j * (i % 2));
      setup();
    }

  }

}
