#ifndef CLVHDMASTER_H
#define CLVHDMASTER_H

#include <cmath>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include <stdio.h>

#include "clvHdEMG.hpp"

#define GLOBAL_VERBOSE_LEVEL 0

class ClvHdMaster
{
    public:
    ClvHdMaster(const char *path);
    ~ClvHdMaster()
    {
        close(m_fd);
    };

    void
    printBit(int8_t val)
    {
        std::cout << " " << std::flush;
        for(int i = 0; i < 8; i++)
            if(1 & (val >> i))
                std::cout << "[" + std::to_string(i) + "] " << std::flush;
    }

    template <typename T>
    T
    readReg(uint8_t id, uint8_t reg)
    {
        size_t size = sizeof(T);
        char msg[3] = {'r', (char)reg, (char)size};
        writeS( msg, 3);
        m_vbuff = 0;
        readS( (uint8_t *)&m_vbuff, size);
        return *(T *)&m_vbuff;
    }

    template <typename T>
    T
    readReg(uint8_t id, uint8_t reg, size_t size)
    {
        char msg[3] = {'r', (char)reg, (char)size};
        writeS( msg, 3);
        readS( (uint8_t *)&m_vbuff, size);
        return *(T *)&m_vbuff;
    }

    int
    writeReg(uint8_t id, uint8_t reg, char val)
    {
        char msg[3] = {'w', (char)reg, (char)val};
        return writeS(msg, 3);
    }

    template <typename T>
    T
    readSerial(size_t size)
    {
      readS( (uint8_t *)&m_vbuff, size);
        // std::cout << std::hex << n << " " << (int)(((uint8_t *)&m_vbuff)[0])
        //           << " " << (int)(((uint8_t *)&m_vbuff)[1]) << std::endl;
        return *(T *)&m_vbuff;
    }

    int
    readS(uint8_t *buffer, size_t size)
    {
        return read(m_fd, buffer, size);
    };

    int
    writeS(const void *arr, size_t size)
    {
        return write(m_fd, arr, size);
    }

    void
    setup()
    {
    }

    private:
    int m_fd;
    uint64_t m_vbuff;
};

#endif
