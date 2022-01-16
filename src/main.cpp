#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include <stdio.h>

#include <lsl_cpp.h>

#define GLOBAL_VERBOSE_LEVEL 0

#define CONFIG_REG 0x00

#define FLEX_CH1_CN_REG 0x01
#define FLEX_CH2_CN_REG 0x02
#define FLEX_CH3_CN_REG 0x03
#define FLEX_PACE_CN_REG 0x04
#define FLEX_VBAT_CN_REG 0x05

#define OSC_CN_REG 0x12

#define AFE_RES_REG 0x13
#define AFE_SHDN_CN_REG 0x14
#define AFE_PACE_CN_REG 0x17

#define ERROR_STATUS_REG 0x19

#define DATA_STATUS_REG 0x30

#define R1_RATE_REG 0x25
#define R2_RATE_REG 0x21
#define R3_RATE_CH1_REG 0x22
#define R3_RATE_CH2_REG 0x23
#define R3_RATE_CH3_REG 0x24
// #define _REG 0x
// #define _REG 0x
// #define _REG 0x
// #define _REG 0x

void
display(std::string s, int v_lvl = 1)
{
    if(v_lvl > GLOBAL_VERBOSE_LEVEL)
        std::cout << s << std::flush;
}
void
displayln(std::string s, int v_lvl = 1)
{
    if(v_lvl > GLOBAL_VERBOSE_LEVEL)
        std::cout << s << "\n";
}
void
displayxd(std::string s, int v_lvl = 1)
{
    if(v_lvl > GLOBAL_VERBOSE_LEVEL)
        std::cout << s << std::flush
                  << "\xd                                         \xd";
}

void
displayError(const char *s)
{
    printf("\"Error %i from %s: %s\"\n", errno, s, strerror(errno));
    exit(1);
}

void
printBit(int8_t val)
{
    display(" ");
    for(int i = 0; i < 8; i++)
        if(1 & (val >> i))
            display("[" + std::to_string(i) + "] ");
}

int
setSerialPort(const char *path)
{
    display("> Check connection: ");
    int serial_port = open(path, O_RDWR | O_NOCTTY);
    if(serial_port < 0)
        exit(-1);
    display("OK\n");

    struct termios tty;
    if(tcgetattr(serial_port, &tty) != 0)
        displayError("tcgetattr");

    tty.c_cflag &= ~PARENB;  // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB;  // Clear stop field, only 1 stop bit (most common)
    tty.c_cflag &= ~CSIZE;   // Clear all bits that set the data size
    tty.c_cflag |= CS8;      // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow ctrl (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrlline(CLOCAL = 1)
    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                     ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent spe. interp. of out bytes (newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conv of newline to car. ret/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10; // Wait for up to 1s, ret when any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if(tcsetattr(serial_port, TCSANOW, &tty) != 0)
        displayError("tcsetattr");
    return serial_port;
}

int
readReg(int fd, uint8_t reg, char *buff, size_t size)
{
    char msg[3] = {'r', (char)reg, (char)size};
    write(fd, msg, 3);
    return read(fd, buff, size);
}

int
writeReg(int fd, uint8_t reg, char val)
{
    char msg[3] = {'w', (char)reg, (char)val};
    return write(fd, msg, 3);
}

void
setup(int fd)
{
    uint8_t val = 0;
    display("> Checking ADS1293: ");
    while(val != 0x01)
    {
        readReg(fd, 0x40, (char *)&val, 1);
        display(".");
    }
    displayln("OK");

    display("> Seting up: ");
    writeReg(fd, CONFIG_REG, 0x02); //standby
    writeReg(fd, OSC_CN_REG, 0x05); //clk xtal output on CLK pin

    writeReg(fd, FLEX_CH1_CN_REG, 0x31); // route ch 1
    writeReg(fd, AFE_SHDN_CN_REG, 0x36); //dis ch 2 and 3

    writeReg(fd, AFE_RES_REG, 0x08);
    writeReg(fd, R1_RATE_REG, 0x01);
    writeReg(fd, R2_RATE_REG, 0x01);
    writeReg(fd, R3_RATE_CH1_REG, 0x01);

    writeReg(fd, CONFIG_REG, 0x01); //conversion ready
    displayln("OK");

    display("> Reading errors registers: ");

    readReg(fd, 0x19, (char *)&val, 0x01);
    printBit(val);

    readReg(fd, 0x1a, (char *)&val, 0x01);
    printBit(val);
    displayln("");
}

int
main()
{
    display("CleverHand Serial Interface:\n");

    int serial_port = setSerialPort("/dev/ttyACM0");
    int8_t val8;
    int16_t val16;
    int32_t val32;

    double fs = 256;
    double f0 = 50;
    double r = 0.99;
    double Xdata[3] = {0, 0, 0};
    double Ydata[3] = {0, 0, 0};
    double Acoef[3] = {0, -2 * r * std::cos(2 * 3.14159 * f0 / fs), r * r};
    double Bcoef[3] = {1, -2 * std::cos(2 * 3.14159 * f0 / fs), 1};
    for(int i = 0; i < 3; i++)
      std::cout << Acoef[i] << "\n";
    for(int i = 0; i < 3; i++)
      std::cout << Bcoef[i] << "\n";

    setup(serial_port);

    try
    {
        int nb_ch = 2;
        int n;
        lsl::stream_info info_sample("ECG", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<float> sample(nb_ch);
        std::cout << "[INFOS] Now sending data... " << std::endl;
        std::cin >> val32;
        for(int t = 0;; t++)
        {
            readReg(serial_port, 0x30, (char *)&val8, 1);
            if(val8 & 0b00100100)
            {
                readReg(serial_port, 0x31, (char *)&val16, 2);
                readReg(serial_port, 0x37, (char *)&val32, 3);

                sample[0] = (val32 * 1. / 0x800000 - 0.5) * 4.8 / 3.5 * 1000;

                Ydata[0] = 0;
                Xdata[0] = sample[0];//(val16 * 1. / 0x8000 - 0.5) * 4.8 / 3.5 * 1000;
                for(int i = 0; i < 3; i++)
                    Ydata[0] += Acoef[i] * Ydata[i] + Bcoef[i] * Xdata[i];
                for(int i = 2; i > 0; i--)
                {
                    Xdata[i] = Xdata[i - 1];
                    Ydata[i] = Ydata[i - 1];
                }
                sample[1] = Ydata[0];

                std::cout << std::hex << val16 << " " << sample[1];
                displayln("");
                outlet_sample.push_sample(sample);
            }
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }

    writeReg(serial_port, CONFIG_REG, 0x02);
    close(serial_port);

    return 0; // success
}
