#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <stdio.h>

#include "clvHdMaster.hpp"
#include <lsl_cpp.h>

int
main()
{
    std::cout << "CleverHand Serial Interface:" << std::endl;

    ClvHd::Master master = ClvHd::Master("/dev/ttyACM0");
    master.setup();

    try
    {
        int nb_ch = 2;
        lsl::stream_info info_sample("ECG", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<float> sample(nb_ch);
        std::cout << "[INFOS] Now sending data... " << std::endl;
        master.start_streaming(ClvHd::DATA_CH1_PACE_REG, 2);
        for(int t = 0;; t++)
        {
            master.read_stream();

            sample[0] = master.fast_EMG(15, 1);
            sample[1] = master.precise_EMG(15, 1);

            outlet_sample.push_sample(sample);
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }

    return 0; // success
}
