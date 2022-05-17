#include "clvHdMaster.hpp"
namespace ClvHd
{

Master::~Master()
{
    uint8_t b[1] = {'n'};
    this->writeS(b, 1);
};

void
Master::printBit(int8_t val)
{
    std::cout << " " << std::flush;
    for(int i = 0; i < 8; i++)
        if(1 & (val >> i))
            std::cout << "[" + std::to_string(i) + "] " << std::flush;
};

int
Master::readReg(uint8_t id, uint8_t reg, size_t size,  const void *buff)
{
    char msg[4] = {'r', (char)id, (char)reg, (char)size};
    writeS(msg, 4);
    return readS((uint8_t*)buff, size);
};

int
Master::writeReg(uint8_t id, uint8_t reg, char val)
{
    char msg[4] = {'w', (char)id, (char)reg, (char)val};
    return writeS(msg, 4);
};


void
Master::setup()
{
    Communication::Client::clean_start();

    uint8_t nb_emg=0;
    this->readS(&nb_emg,1);
    std::cout << (int)nb_emg << " EMG electrodes." << std::endl;
    for(int i = 0; i < nb_emg; i++)
    {
        m_EMG.push_back(new EMG(this, 0x0f - i));
        m_EMG[i]->setup();
    }
}

double
Master::read_precise_EMG(int id, int channel)
{
    return m_EMG[id]->read_precise_value(channel);
}

double
Master::read_fast_EMG(int id, int channel)
{
    return m_EMG[id]->read_fast_value(channel);
}

double
Master::precise_EMG(int id, int channel)
{
    return m_EMG[id]->precise_value(channel);
}

double
Master::fast_EMG(int id, int channel)
{
    return m_EMG[id]->fast_value(channel);
}

void
Master::start_streaming(ADS1293_Reg reg, uint8_t size)
{
    uint8_t b[3] = {'s', (uint8_t)reg, size};
    m_streaming_reg = reg;
    m_streaming = true;
    m_streaming_size = size;
    this->writeS(b, 3);
}

int
Master::read_stream()
{
    return this->readS(m_EMG[0]->get_regs() + m_streaming_reg,
                       m_streaming_size);
}
} // namespace ClvHd
