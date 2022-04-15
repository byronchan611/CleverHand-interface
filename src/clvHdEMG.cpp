#include "clvHdEMG.hpp"

ClvHdEMG::~ClvHdEMG() { m_master->writeReg(m_id, CONFIG_REG, 0x02); }

void
ClvHdEMG::setup()
{
    uint8_t val = 0;
    std::cout << "> Checking ADS1293: " << std::flush;
    while(val != 0x01)
    {
        val = m_master->readReg<uint8_t>(m_id, 0x40);
        std::cout << "." << std::flush;
    }
    std::cout << "OK" << std::endl;

    std::cout << "> Seting up: " << std::endl;

    // standby mode (0x02)
    this->set_mode(STANDBY);

    //Start clk and output on CLK pin (0x05);
    this->config_clock(true, INTERN, true);

    //route channel1 (+)in1 & (-)in6 (0x31)
    this->route_channel(1, 1, 6);

    //enable ch1 and disaable ch2 and ch3 (0x36)
    this->enable_channels(true, false, false);

    //no high resolution & Clock frequency for Channel 1 :204800 (0x08)
    this->config_resolution(false, false, false);
    this->config_frequence(true, false, false);

    //R1 ch1:2 ch2:4 ch3:4 (0x1)
    this->config_R1(2, 4, 4);
    //R2 = 4 (0x01)
    this->config_R2(4);

    //R3_ch1 = 4 (0x01)
    this->config_R3_ch1(4);
    
    // check DIS_EFILTER: 0x26

    //Start conversion (0x01)
    this->set_mode(START_CONV);
    std::cout << "OK" << std::endl;

    std::cout << "> Reading errors registers: " << std::flush;

    val = m_master->readReg<uint8_t>(m_id, 0x19);
    m_master->printBit(val);

    val = m_master->readReg<uint8_t>(m_id, 0x1a);
    m_master->printBit(val);
    std::cout << std::endl;
}

void
ClvHdEMG::route_channel(uint8_t channel, uint8_t pos_in, uint8_t neg_in)
{
    pos_in = (pos_in < 0) ? 0 : (pos_in > 6) ? 6 : pos_in;
    neg_in = (neg_in < 0) ? 0 : (neg_in > 6) ? 6 : neg_in;

    channel = ((channel < 1) ? 1 : (channel > 3) ? 3 : channel) - 1;
    uint8_t val = pos_in | (neg_in << 3);
    m_regs[FLEX_CH1_CN_REG + channel] = val;
    m_master->writeReg(m_id, FLEX_CH1_CN_REG + channel, val);
}

void
ClvHdEMG::set_mode(Mode mode)
{
    m_mode = mode;
    m_regs[CONFIG_REG] = mode;
    m_master->writeReg(m_id, CONFIG_REG, mode);
}

void
ClvHdEMG::config_clock(bool start, CLK_SRC src, bool en_output)
{
    uint8_t val = (start ? 0x4 : 0x0) | (src << 1) | (en_output ? 0x1 : 0x0);
    m_regs[OSC_CN_REG] = val;
    m_master->writeReg(m_id, OSC_CN_REG, val);
}

void
ClvHdEMG::enable_channels(bool ch1, bool ch2, bool ch3)
{
    uint8_t val =
        (ch1 ? 0 : 0b001001) | (ch2 ? 0 : 0b010010) | (ch3 ? 0 : 0x0b100100);
    m_regs[AFE_SHDN_CN_REG] = val;
    m_master->writeReg(m_id, AFE_SHDN_CN_REG, val);
}

void
ClvHdEMG::enable_SDM(bool ch1, bool ch2, bool ch3)
{
    uint8_t val = (m_regs[AFE_SHDN_CN_REG] & 0b111) | (ch1 ? 0 : 0b1000) |
                  (ch2 ? 0 : 0b10000) | (ch3 ? 0 : 0b100000);
    m_regs[AFE_SHDN_CN_REG] = val;
    m_master->writeReg(m_id, AFE_SHDN_CN_REG, val);
}

void
ClvHdEMG::enable_INA(bool ch1, bool ch2, bool ch3)
{
    uint8_t val = (m_regs[AFE_SHDN_CN_REG] & 0b111000) | (ch1 ? 0 : 0b1) |
                  (ch2 ? 0 : 0b10) | (ch3 ? 0 : 0b100);
    m_regs[AFE_SHDN_CN_REG] = val;
    m_master->writeReg(m_id, AFE_SHDN_CN_REG, val);
}

void
ClvHdEMG::config_resolution(bool ch1_high_res,
                            bool ch2_high_res,
                            bool ch3_high_res)
{
    uint8_t val = (m_regs[AFE_RES_REG] & 0b00111000) |
                  (ch1_high_res ? 0b1 : 0) | (ch2_high_res ? 0b010 : 0) |
                  (ch3_high_res ? 0b100 : 0);
    m_regs[AFE_RES_REG] = val;
    m_master->writeReg(m_id, AFE_RES_REG, val);
}

void
ClvHdEMG::config_frequence(bool ch1_freq_double,
                           bool ch2_freq_double,
                           bool ch3_freq_double)
{
    uint8_t val =
        (m_regs[AFE_RES_REG] & 0b111) | (ch1_freq_double ? 0b1000 : 0) |
        (ch2_freq_double ? 0b010000 : 0) | (ch3_freq_double ? 0b100000 : 0);
    m_regs[AFE_RES_REG] = val;
    m_master->writeReg(m_id, AFE_RES_REG, val);
}

void
ClvHdEMG::config_R1(uint8_t R1_ch1, uint8_t R1_ch2, uint8_t R1_ch3)
{
    R1_ch1 = (R1_ch1 < 3) ? 0b001 : 0; //2 or 4
    R1_ch2 = (R1_ch2 < 3) ? 0b010 : 0; //2 or 4
    R1_ch3 = (R1_ch3 < 3) ? 0b100 : 0; //2 or 4
    uint8_t val = R1_ch1 | R1_ch1 | R1_ch1;
    m_regs[R1_RATE_REG] = val;
    m_master->writeReg(m_id, R1_RATE_REG, 0x01);
}

void
ClvHdEMG::config_R2(uint8_t R2)
{
    if(R2 < 5)
        R2 = 0b0001; //4
    else if(R2 < 6)
        R2 = 0b0010; //5
    else if(R2 < 8)
        R2 = 0b0100; //6
    else
        R2 = 0b1000; //8
    m_regs[R2_RATE_REG] = R2;
    m_master->writeReg(m_id, R2_RATE_REG, R2);
}

void
ClvHdEMG::config_R3_ch1(uint8_t R3)
{
    if(R3 < 6)
        R3 = 0b00000001; //4
    else if(R3 < 8)
        R3 = 0b00000010; //6
    else if(R3 < 11)
        R3 = 0b00000100; //8
    else if(R3 < 15)
        R3 = 0b00001000; //12
    else if(R3 < 25)
        R3 = 0b00010000; //16
    else if(R3 < 49)
        R3 = 0b00100000; //32
    else if(R3 < 97)
        R3 = 0b01000000; //64
    else
        R3 = 0b10000000; //128
    m_regs[R3_RATE_CH1_REG] = R3;
    m_master->writeReg(m_id, R3_RATE_CH1_REG, R3);
}

void
ClvHdEMG::config_R3_ch2(uint8_t R3)
{
    if(R3 < 6)
        R3 = 0b00000001; //4
    else if(R3 < 8)
        R3 = 0b00000010; //6
    else if(R3 < 11)
        R3 = 0b00000100; //8
    else if(R3 < 15)
        R3 = 0b00001000; //12
    else if(R3 < 25)
        R3 = 0b00010000; //16
    else if(R3 < 49)
        R3 = 0b00100000; //32
    else if(R3 < 97)
        R3 = 0b01000000; //64
    else
        R3 = 0b10000000; //128
    m_regs[R3_RATE_CH2_REG] = R3;
    m_master->writeReg(m_id, R3_RATE_CH2_REG, R3);
}

void
ClvHdEMG::config_R3_ch3(uint8_t R3)
{
    if(R3 < 6)
        R3 = 0b00000001; //4
    else if(R3 < 8)
        R3 = 0b00000010; //6
    else if(R3 < 11)
        R3 = 0b00000100; //8
    else if(R3 < 15)
        R3 = 0b00001000; //12
    else if(R3 < 25)
        R3 = 0b00010000; //16
    else if(R3 < 49)
        R3 = 0b00100000; //32
    else if(R3 < 97)
        R3 = 0b01000000; //64
    else
        R3 = 0b10000000; //128
    m_regs[R3_RATE_CH3_REG] = R3;
    m_master->writeReg(m_id, R3_RATE_CH3_REG, R3);
}
