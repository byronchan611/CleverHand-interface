#include "clvHdMaster.hpp"
#include <iomanip>
#include <iostream>
#include <lsl_cpp.h>

int
main()
{
    std::cout << "CleverHand Serial Interface:" << std::endl;
    try
    {
        ClvHd::Master master;
        master.open_connection("/dev/ttyACM0");
        master.setup();

        int nb_ch = 2;
        lsl::stream_info info_sample("EMG", "sample", nb_ch, 0,
                                     lsl::cf_float32);
        lsl::stream_outlet outlet_sample(info_sample);
        std::vector<float> sample(nb_ch);
        std::cout << "[INFOS] Now sending data... " << std::endl;
        for(int t = 0;; t++)
        {
            master.read_all_signal();
            //   int val = master.readReg<uint8_t>(15, 0x40);
            // std::cout << "[INFOS]  " << val << " " ;
            sample[0] = master.fast_EMG(0, 1);
            sample[1] = master.precise_EMG(0, 1);

            //std::cout << sample[0] << ";" << sample[1] << std::endl;

            outlet_sample.push_sample(sample);
            //usleep(1000);
        }
    }
    catch(std::exception &e)
    {
        std::cerr << "[ERROR] Got an exception: " << e.what() << std::endl;
    }
    catch(std::string str)
    {
        std::cerr << "[ERROR] Got an exception: " << str << std::endl;
    }

    return 0; // success
}
