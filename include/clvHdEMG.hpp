#ifndef CLV_HD_EMG_H
#define CLV_HD_EMG_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "clvHdMaster.hpp"
#include "registers.hpp"
namespace ClvHd
{
class Master;

class EMG
{
    enum Mode
    {
        START_CONV = 0x1,
        STANDBY = 0x2,
        POWER_DOWN = 0x4
    };

    enum CLK_SRC
    {
        EXTERN = 0,
        INTERN = 1
    };

    public:
    EMG(Master *master, int id) : m_master(master), m_id(id)
    {
        m_fast_value = (int16_t *)(m_regs + DATA_CH1_PACE_REG);
        m_precise_value = (int32_t *)(m_regs + DATA_CH1_ECG_REG);
    };
    ~EMG();

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

    int32_t
    read_precise_value(int ch);

    int16_t
    read_fast_value(int ch);

    int32_t
    precise_value(int ch)
    {
        return conv(m_precise_value[ch-1]);
    };

    int16_t
    fast_value(int ch)
    {
        return conv(m_fast_value[ch-1]);
    };

    int16_t
    conv(int16_t val)
    {
        return (val * 1. / 0x8000 - 0.5) * 4.8 / 3.5 * 1000;
    }

    int32_t
    conv(int32_t val)
    {
        return (val * 1. / 0x800000 - 0.5) * 4.8 / 3.5 * 1000;
    }

    private:
    Master *m_master;
    int m_id;
    Mode m_mode;
    uint8_t m_regs[0x50];
    int16_t *m_fast_value;
    int32_t *m_precise_value;
};
} // namespace ClvHd
#endif //CLV_HD_EMG_H
