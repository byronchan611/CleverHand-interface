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

    ClvHdMaster clvHd = ClvHdMaster("/dev/ttyACM0");
    clvHd.setup();
    int16_t val16;
    int32_t val32;

    try
    {
        int nb_ch = 2;
        lsl::stream_info info_sample("ECG", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<float> sample(nb_ch);
        std::cout << "[INFOS] Now sending data... " << std::endl;
        uint8_t b[3] = {'s', 0x31, 2};
        clvHd.writeS(b, 3);
        std::cout << " hoy " << std::endl;
        for(int t = 0;; t++)
        {
            //val8 = clvHd.readReg<uint8_t>(15, 0x30);

            if(true) //val8 & 0b00100100)
            {
                //std::cout << " hoy " << std::endl;

                val16 = clvHd.readSerial<int16_t>(2);
                //clvHd.readReg<int16_t>(15, 0x31);
                val32 = 0; //clvHd.readReg<int32_t>(15, 0x37, 3);

                sample[0] = (val32 * 1. / 0x800000 - 0.5) * 4.8 / 3.5 * 1000;
                sample[1] = (val16 * 1. / 0x8000 - 0.5) * 4.8 / 3.5 * 1000;

                //std::cout << std::hex << val16 << std::endl;
                outlet_sample.push_sample(sample);
            }
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
    uint8_t b[1] = {'n'};
    clvHd.writeS(b, 1);

    return 0; // success
}
