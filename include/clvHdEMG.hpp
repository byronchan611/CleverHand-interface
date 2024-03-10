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
#include <stdint.h> // uint8_t, uint16_t, uint32_t, uint64_t

namespace ClvHd
{
class Master;

using namespace std;

/**
 * @brief The EMG class
 *
 * This class is used to read EMG data from the ClvHd board.
 */
class EMG : virtual public ESC::CLI
{
    public:
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
    EMG(Master *master, int id, int verbose=-1);
    ~EMG();

    /**
     * @brief setup Write all setup parameters to the board.
     * 
     * @param route_table Routing configuration of the EMG channels.
     * @param chx_enable Select which INA channels are enabled.
     * @param chx_high_res Select if the INA channels are high resolution.
     * @param chx_high_freq Select if the INA channels are high frequency.
     * @param R1 Gain R1 of the INA channels.
     * @param R2 Gain R2 of the INA channels.
     * @param R3 Gain R3 of the INA channels.
     */
    int
    setup(int route_table[3][2],
          bool chx_enable[3],
          bool chx_high_res[3],
          bool chx_high_freq[3],
          int R1[3],
          int R2,
          int R3[3]);

    int
    route_channel(uint8_t channel, uint8_t pos_in, uint8_t neg_in);
    int
    get_route_neg(int ch);
    int
    get_route_pos(int ch);

    int
    set_mode(Mode mode);
    Mode
    get_mode();

    int
    config_clock(bool start, CLK_SRC src, bool en_output);

    bool
    is_clock_started();
    bool
    is_clock_ext();
    bool
    is_clock_output_enabled();

    int
    enable_ADC(bool ch1, bool ch2, bool ch3);
    bool
    is_ADC_enabled(int ch);

    int
    enable_SDM(bool ch1, bool ch2, bool ch3);
    bool
    is_SDM_enabled(int ch);

    int
    enable_INA(bool ch1, bool ch2, bool ch3);
    bool
    is_INA_enabled(int ch);

    //config a frequency of 1024000Hz or 204800Hz
    int
    config_frequence(bool ch1_freq_double,
                     bool ch2_freq_double,
                     bool ch3_freq_double);
    bool
    is_high_freq_enabled(int ch);

    int
    config_resolution(bool ch1_high_res, bool ch2_high_res, bool ch3_high_res);
    bool
    is_high_res_enabled(int ch);

    //2 or 4
    int
    config_R1(uint8_t R1_ch1, uint8_t R1_ch2, uint8_t R1_ch3);
    int get_R1(int ch);

    //4, 5, 6 or 8
    int
    config_R2(uint8_t R2);
    int get_R2(int ch=0);

    //4, 6, 8, 12, 16, 32, 64, 128
    int
    config_R3(int ch, uint8_t R3);
    int get_R3(int ch);

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

    int
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
    int16_t *m_fast_value;


private:
    Master *m_master;
    int m_module_id;
    Mode m_mode;
    uint8_t m_regs[0x50];

    int32_t *m_precise_value[3];
    int32_t m_fast_adc_max;
    int32_t m_precise_adc_max[3];
};
} // namespace ClvHd
#endif //CLV_HD_EMG_H
