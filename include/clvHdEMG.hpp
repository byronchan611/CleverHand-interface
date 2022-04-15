#ifndef CLV_HD_EMG_H
#define CLV_HD_EMG_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "clvHdMaster.hpp"

class ClvHdMaster;
class ClvHdEMG
{
    enum Mode
    {
        START_CONV = 0x1,
        STANDBY = 0x2,
        POWER_DOWN = 0x4
    };
    enum Reg
    {
        CONFIG_REG = 0x00,
        FLEX_CH1_CN_REG = 0x01,
        FLEX_CH2_CN_REG = 0x02,
        FLEX_CH3_CN_REG = 0x03,
        FLEX_PACE_CN_REG = 0x04,
        FLEX_VBAT_CN_REG = 0x05,
        OSC_CN_REG = 0x12,
        AFE_RES_REG = 0x13,
        AFE_SHDN_CN_REG = 0x14,
        AFE_PACE_CN_REG = 0x17,
        ERROR_STATUS_REG = 0x19,
        DATA_STATUS_REG = 0x30,
        R1_RATE_REG = 0x25,
        R2_RATE_REG = 0x21,
        R3_RATE_CH1_REG = 0x22,
        R3_RATE_CH2_REG = 0x23,
        R3_RATE_CH3_REG = 0x24
    };

    enum CLK_SRC
    {
        EXTERN = 0,
        INTERN = 1
    };

    public:
    ClvHdEMG(ClvHdMaster *master, int id) : m_master(master), m_id(id){};
    ~ClvHdEMG();

    void
    setup();

    void
    route_channel(uint8_t channel, uint8_t pos_in, uint8_t neg_in);

    void
    set_mode(Mode mode);
    Mode
    get_mode()
    {
        return m_mode;
    }

    void
    config_clock(bool start, CLK_SRC src, bool en_output);

    void
    enable_channels(bool ch1, bool ch2, bool ch3);

    void
    enable_SDM(bool ch1, bool ch2, bool ch3);

    void
    enable_INA(bool ch1, bool ch2, bool ch3);

    //config a frequency of 1024000Hz or 204800Hz
    void
    config_frequence(bool ch1_freq_double,
                     bool ch2_freq_double,
                     bool ch3_freq_double);
    void
    config_resolution(bool ch1_high_res, bool ch2_high_res, bool ch3_high_res);

    //2 or 4
    void
    config_R1(uint8_t R1_ch1, uint8_t R1_ch2, uint8_t R1_ch3);

    //4, 5, 6 or 8
    void
    config_R2(uint8_t R2);

    //4, 6, 8, 12, 16, 32, 64, 128
    void
    config_R3_ch1(uint8_t R3);

    //4, 6, 8, 12, 16, 32, 64, 128
    void
    config_R3_ch2(uint8_t R3);

    //4, 6, 8, 12, 16, 32, 64, 128
    void
    config_R3_ch3(uint8_t R3);

    private:
    ClvHdMaster *m_master;
    int m_id;
    Mode m_mode;
    uint8_t m_regs[0x50];
};

#endif
