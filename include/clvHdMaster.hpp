#ifndef CLVHDMASTER_H
#define CLVHDMASTER_H

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

#include "clvHdEMG.hpp"
#include "com_client.hpp"
#include "registers.hpp"

#define GLOBAL_VERBOSE_LEVEL 0

namespace ClvHd
{
  class EMG;
  class Master : public Communication::Client
  {
  public:
    Master(){};
    ~Master();

    void
    printBit(int8_t val);

    int
    readReg(uint8_t id, uint8_t reg, size_t size, const void *buff);

    int
    writeReg(uint8_t id, uint8_t reg, char val);

    int 
    setup();

    void
    setupEMG(int n_board,
             int route_table[3][2],
             bool chx_enable[3],
             bool chx_high_res[3],
             bool chx_high_freq[3],
             int R1[3],
             int R2,
             int R3[3]);

    double
    read_fast_EMG(int id, int channel);

    double
    read_precise_EMG(int id, int channel);

    double
    fast_EMG(int id, int channel);

    double
    precise_EMG(int id, int channel);

    void
    start_streaming(ADS1293_Reg reg, uint8_t size);

    void
    start_streaming(std::vector<std::pair<int,uint8_t>> active_channels);
  
    int
    read_stream();

    void stop_streaming();

    std::vector<EMG *> m_EMG;

  private:
    bool m_streaming = false;
    ADS1293_Reg m_streaming_reg;
    size_t m_streaming_size;
    std::vector<std::pair<int,uint8_t>> m_streaming_channels;
  };
} // namespace ClvHd
#endif
