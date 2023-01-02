#include "clvHdMaster.hpp"
#include <chrono>
namespace ClvHd
{

Master::~Master()
{
    uint8_t b[6] = {'z', 0, 0, 0};
    this->writeS(b, 4, true);
    this->close_connection();
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
    char msg[6] = {'r', (char)(0x0f - id), (char)reg, (char)size};
    writeS(msg, 4, true);
    return readS((uint8_t *)buff, size + 2, true); //+2 for CRC
};

int8_t
Master::readReg8(uint8_t id, uint8_t reg)
{
    int8_t val;
    readReg(id, reg, 1, &val);
    return val;
};

int16_t
Master::readReg16(uint8_t id, uint8_t reg)
{
    int16_t val;
    readReg(id, reg, 2, &val);
    return val;
};

int32_t
Master::readReg32(uint8_t id, uint8_t reg)
{
    int32_t val;
    readReg(id, reg, 4, &val);
    return val;
};

int64_t
Master::readReg64(uint8_t id, uint8_t reg)
{
    int64_t val;
    readReg(id, reg, 8, &val);
    return val;
};

float
Master::readRegFloat(uint8_t id, uint8_t reg)
{
    float val;
    readReg(id, reg, 4, &val);
    return val;
};

double
Master::readRegDouble(uint8_t id, uint8_t reg)
{
    double val;
    readReg(id, reg, 8, &val);
    return val;
};

int
Master::writeReg(uint8_t id, uint8_t reg, char val)
{
    char msg[6] = {'w', (char)(0x0f - id), (char)reg, (char)val};
    writeS(msg, 4, true);
    int n = readS((uint8_t *)m_buffer, 4, true); //+2 for CRC
    if(m_buffer[0] == 'w' && m_buffer[1] == val)
        return 1;
    return -1;
};

int
Master::setupEMG(int n_board,
                 int route_table[3][2],
                 bool chx_enable[3],
                 bool chx_high_res[3],
                 bool chx_high_freq[3],
                 int R1[3],
                 int R2,
                 int R3[3])
{
    return m_EMG[n_board]->setup(route_table, chx_enable, chx_high_res, chx_high_freq,
                         R1, R2, R3);
}

int
Master::setup()
{
    // //Communication::Client::clean_start();
    logln("Testing connection: " + ESC::fstr(((test_connection()) ? "Established" : "Error"),
                                             {ESC::FORMAT::FG_GREEN}), true);

    //blink(15, 10, 3);
    int8_t nb_emg = getNbModules();
    logln("Number of EMG modules: " + std::to_string((int)nb_emg), true);
    for(int i = 0; i < nb_emg; i++) m_EMG.push_back(new EMG(this, i, m_verbose));


    return nb_emg;
}

bool
Master::data_ready(int id, int channel, bool precise)
{
    uint8_t mask = 1 << (2 + precise * 3 + channel);
    return *(m_EMG[id]->get_regs() + DATA_STATUS_REG) & mask;
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

int
Master::start_acquisition()
{
    int n=0;
    for(int i = 0; i < m_EMG.size(); i++) n+=m_EMG[i]->set_mode(EMG::START_CONV);
    return n;
}

int
Master::stop_acquisition()
{
    int n =0;
    for(int i = 0; i < m_EMG.size(); i++) n+=m_EMG[i]->set_mode(EMG::STANDBY);
    return n;
}

int
Master::read_all_signal()
{
    char msg[6] = {'R', 0, (char)DATA_STATUS_REG, (char)16};//read (satue + 3*pace(2bytes) + 3*pace(3bytes))=16 for each connected  modules
    writeS(msg, 4, true);
    int n = readS((uint8_t *)m_buffer, 16 * m_EMG.size() + 2, true);
    for(int i = 0; i < m_EMG.size(); i++)
        std::copy(m_buffer + 16 * i, m_buffer + 16 * (i + 1),
                  m_EMG[i]->get_regs() + DATA_STATUS_REG);

    return 16 * m_EMG.size() + 2;
}

std::string
Master::get_error(int id, bool verbose)
{
    std::string str;
    m_EMG[id]->get_error();
    str += m_EMG[id]->error_status_str();
    if(verbose)
        str += m_EMG[id]->error_range_str();

    return str;
}

bool
Master::error_at(int id, int index)
{
    return (m_EMG[id]->get_regs() + ERROR_STATUS_REG)[index / 8] &
           (1 << (index % 8));
}

} // namespace ClvHd
