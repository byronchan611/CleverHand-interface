#include "clvHdController.hpp"
#include <chrono>
namespace ClvHd
{

Controller::~Controller()
{
    sendCmd('z');
    this->close_connection();
};

int
Controller::readReg(uint8_t id, uint8_t reg, uint8_t size, const void *buff, uint64_t *timestamp)
{
    uint8_t msg[2] = {reg, size};
    sendCmd('r', id, msg, 2);
    return readReply((uint8_t *)buff, timestamp);
};

int
Controller::writeReg(uint8_t id, uint8_t reg, uint8_t val)
{
    uint8_t msg[2] = {reg, val};
    return sendCmd('w', id, msg, 2);
};

int
Controller::setupEMG(int n_board,
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
Controller::setup()
{
    logln("Testing connection: " + ESC::fstr(((test_connection()) ? "Established" : "Error"),
                                             {ESC::FORMAT::FG_GREEN}), true);
    logln("Version: " + getVersion(), true);
    int8_t nb_emg = getNbModules();
    logln("Number of EMG modules: " + std::to_string((int)nb_emg), true);
    for(int i = 0; i < nb_emg; i++) m_EMG.push_back(new EMG(this, i, m_verbose));
    return nb_emg;
}

bool
Controller::data_ready(int id, int channel, bool precise)
{
    uint8_t mask = 1 << (2 + precise * 3 + channel);
    if(!(*(m_EMG[id]->get_regs() + DATA_STATUS_REG)&0x02))
        return false;
    return *(m_EMG[id]->get_regs() + DATA_STATUS_REG) & mask;
}

double
Controller::read_precise_EMG(int id, int channel)
{
    return m_EMG[id]->read_precise_value(channel);
}

double
Controller::read_fast_EMG(int id, int channel)
{
    return m_EMG[id]->read_fast_value(channel);
}

double
Controller::precise_EMG(int id, int channel)
{
    return m_EMG[id]->precise_value(channel);
}

double
Controller::fast_EMG(int id, int channel)
{
    return m_EMG[id]->fast_value(channel);
}

int
Controller::start_acquisition()
{
    int n=0;
    for(int i = 0; i < m_EMG.size(); i++) n+=m_EMG[i]->set_mode(EMG::START_CONV);
    return n;
}

int
Controller::stop_acquisition()
{
    int n =0;
    for(int i = 0; i < m_EMG.size(); i++) n+=m_EMG[i]->set_mode(EMG::STANDBY);
    return n;
}

int
Controller::read_all_signal(uint64_t *timestamp)
{
    uint8_t msg[2] = {DATA_STATUS_REG, 16};//read (satue + 3*pace(2bytes) + 3*pace(3bytes))=16 for each connected  modules
    sendCmd('r', 0, msg, 2);
    int n = readReply((uint8_t *)m_buffer, timestamp); 
    if(n==16 * m_EMG.size())
        for(int i = 0; i < m_EMG.size(); i++)
            std::copy(m_buffer + 16 * i, m_buffer + 16 * (i + 1),
                      m_EMG[i]->get_regs() + DATA_STATUS_REG);
    return n;
}

std::string
Controller::get_error(int id, bool verbose)
{
    std::string str;
    m_EMG[id]->get_error();
    str += m_EMG[id]->error_status_str();
    if(verbose)
        str += m_EMG[id]->error_range_str();

    return str;
}

bool
Controller::error_at(int id, int index)
{
    return (m_EMG[id]->get_regs() + ERROR_STATUS_REG)[index / 8] &
           (1 << (index % 8));
}

} // namespace ClvHd
