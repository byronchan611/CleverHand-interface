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
Master::readReg(uint8_t id, uint8_t reg, size_t size, const void *buff)
{
    char msg[4] = {'r', (char)id, (char)reg, (char)size};
    writeS(msg, 4);
    return readS((uint8_t *)buff, size);
};

int
Master::writeReg(uint8_t id, uint8_t reg, char val)
{
    char msg[4] = {'w', (char)id, (char)reg, (char)val};
    return writeS(msg, 4);
};

void
Master::setupEMG(int n_board,
                 int route_table[3][2],
                 bool chx_enable[3],
                 bool chx_high_res[3],
                 bool chx_high_freq[3],
                 int R1[3],
                 int R2,
                 int R3[3])
{
    m_EMG[n_board]->setup(route_table, chx_enable, chx_high_res, chx_high_freq,
                          R1, R2, R3);
}

int
Master::setup()
{
    Communication::Client::clean_start();
    uint8_t nb_emg = 0;
    this->readS(&nb_emg, 1);
    std::cout << "hey " << (int)nb_emg << std::endl;
    for(int i = 0; i < nb_emg; i++) m_EMG.push_back(new EMG(this, 0x0f - i));
    return nb_emg;
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

void
Master::start_streaming(std::vector<std::pair<int, uint8_t>> active_channels)
{
    uint8_t *b = new uint8_t[active_channels.size() * 2 + 2];
    b[0] = 'S';
    b[1] = (uint8_t)active_channels.size();

    m_streaming_channels.clear();

    for(int i = 0; i < active_channels.size(); i++)
    {
        b[2 + i * 2] = active_channels[i].first;
        b[2 + i * 2 + 1] = active_channels[i].second;
        m_streaming_channels.push_back(std::make_pair(
            active_channels[i].first, active_channels[i].second));
    }
    this->writeS(b, active_channels.size() * 2 + 2);
    delete[] b;
}

int
Master::read_stream()
{

    for(int i = 0; i < m_streaming_channels.size(); i++)
    {
        this->readS(m_EMG[m_streaming_channels[i].first]->get_regs() +
                        m_streaming_channels[i].second,
                    (m_streaming_channels[i].second < DATA_CH1_ECG_REG) ? 2
                                                                        : 3);
    }
    return 1;
}

void
Master::stop_streaming()
{
    uint8_t b[3] = {'n'};
    this->writeS(b, 1);
};

} // namespace ClvHd
