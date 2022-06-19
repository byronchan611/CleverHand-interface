#ifndef CLV_HD_EMG_H
#define CLV_HD_EMG_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "clvHdMaster.hpp"
#include "registers.hpp"
#include "strANSIseq.hpp"

namespace ClvHd
{
class Master;

class EMG
{
    protected:
    typedef struct
    {
        uint8_t error_lod;
        uint8_t error_status;
        uint8_t error_range1;
        uint8_t error_range2;
        uint8_t error_range3;
        uint8_t error_sync;
        uint8_t error_misc;
        uint8_t padding;
    } Error;

    enum CLK_SRC
    {
        EXTERN = 1,
        INTERN = 0
    };

    public:
    enum Mode
    {
        START_CONV = 0x1,
        STANDBY = 0x2,
        POWER_DOWN = 0x4
    };

    public:
    EMG(Master *master, int id);
    ~EMG();

    void
    setup(int route_table[3][2],
          bool chx_enable[3],
          bool chx_high_res[3],
          bool chx_high_freq[3],
          int R1[3],
          int R2,
          int R3[3]);

    void
    route_channel(uint8_t channel, uint8_t pos_in, uint8_t neg_in);

    int
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
    config_R3(int ch, uint8_t R3);

    double
    read_precise_value(int ch);

    double
    read_fast_value(int ch);

    double
    precise_value(int ch);

    double
    fast_value(int ch);

    double
    conv(uint16_t val);

    double
    conv(int ch, int32_t val);

    Error *
    get_error();

    std::string
    error_range_str();

    std::string
    error_status_str();

    uint8_t *
    get_regs()
    {
        return m_regs;
    };

    private:
    Master *m_master;
    int m_id;
    Mode m_mode;
    uint8_t m_regs[0x50];
    int16_t *m_fast_value;
    int32_t *m_precise_value[3];
    int32_t m_fast_adc_max;
    int32_t m_precise_adc_max[3];
    bool m_verbose = true;
};
} // namespace ClvHd
#endif //CLV_HD_EMG_H
