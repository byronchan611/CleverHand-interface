#define VERSION_MAJOR 3
#define VERSION_MINOR 0
#define PACKAGE_SIZE 6
#include "clvHd_util.hpp"

uint8_t recv_buff[PACKAGE_SIZE];
uint8_t send_buff[255];
uint64_t *timestamp = (uint64_t *)send_buff;
uint8_t *size_buff = send_buff + 8;
uint8_t *vals_buff = send_buff + 9;
int i, n, reg, nb, id, val;

ClvHdEMG clvHdEMG;

void
setup()
{

    Serial.begin(500000);
    clvHdEMG.begin();
    nb = clvHdEMG.nbModules();
    delay(100);
}

void
loop()
{
    if(Serial.available() >= PACKAGE_SIZE)
    {
        Serial.readBytes((char *)recv_buff, PACKAGE_SIZE);
        switch(recv_buff[0])
        {
        case 'r': // Reading cmd > 'r' | id | reg | n : read n bytes starting from reg
        {
            id = recv_buff[1];
            reg = recv_buff[2];
            n = recv_buff[3];
            *timestamp = micros();
            if(id == 0xff) //broadcast: read all modules
            {
                for(i = 0; i < nb; i++)
                    clvHdEMG.readRegister(reg, vals_buff + n * i, n, i);
                *size_buff = n * nb;
            }
            else
            {
                clvHdEMG.readRegister(reg, vals_buff, n, id);
                *size_buff = n;
            }
            Serial.write(send_buff, 9+*size_buff);
            break;
        }
        case 'w': // Reading cmd > 'w' | id | reg | val
        {
            id = recv_buff[1];
            reg = recv_buff[2];
            val = recv_buff[3];
            if(id == 0xff) //broadcast: write to all modules
                for(i = 0; i < nb; i++) clvHdEMG.writeRegister(reg, val, i);
            else
                clvHdEMG.writeRegister(reg, val, id);
            break;
        }
        case 'n': // Nb module cmd > 'n' | 0 | 0 | 0
        {
            nb = clvHdEMG.nbModules();
            *timestamp = micros();
            *vals_buff = nb;
            *size_buff = 1;

            Serial.write(send_buff, 9+*size_buff);
            break;
        }
        case 'b': // Blink cmd > 'b' | id | time_cs | nb_repeat
        {
            break;
        }
        case 'm': // Mirror cmd > 'm' | b1 | b2 | b3
        {
            *timestamp = micros();
            vals_buff[0] = recv_buff[2];
            vals_buff[1] = recv_buff[3];
            vals_buff[2] = recv_buff[4];
            *size_buff = 3;
            Serial.write(send_buff, 9+*size_buff);
            break;
        }
        case 'v': // Version cmd > 'v' | 0 | 0 | 0
        {
            *timestamp = micros();
            *(uint8_t *)vals_buff = VERSION_MAJOR;
			*(uint8_t *)(vals_buff + 1) = VERSION_MINOR;
            *size_buff = 2;
            Serial.write(send_buff, 9+*size_buff);
            break;
        }
        }
    }
}
