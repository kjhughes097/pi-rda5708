#include <unistd.h>//Needed for I2C port
#include <fcntl.h>//Needed for I2C port
#include <sys/ioctl.h>//Needed for I2C port
#include <linux/i2c-dev.h>//Needed for I2C port
#include <stdio.h>
#include <string.h>
#include "rda5702e.h"
#include <stdlib.h>

#define u8 unsigned char
#define u16 unsigned short
#define debug_print(fmt, ...) do { if (debug) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

const unsigned short regInit[] =  {0xC002, 0x0000, 0x0400, 0xC3A7, 0x6000}; // only need the 5 writable registers

void RDA5702E::writeOneRegI2C(int reg, u16 word) {
    int i2cHandle;

    char *deviceName = (char*)"/dev/i2c-1";
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0) {
        printf("Error opening I2C\n");
    }
    else {
        if (ioctl(i2cHandle, I2C_SLAVE, i2cAddressRnd) < 0) {
            printf("Error at ioctl\n");
        }
        else {
            //
            debug_print("Writing %04x to register %02x at I2C addr %02x\n", word, reg, i2cAddressRnd);
            unsigned char buffer[3];
            buffer[0] = (unsigned char)reg;
            buffer[1] = (unsigned char)(word>>8);
            buffer[2] = (unsigned char)(word &0xFF);
            write(i2cHandle, buffer, 3);
        }
            
        // Close the i2c device bus
        close(*deviceName);
    }
}


void RDA5702E::writeAllRegsI2C() {
    int i2cHandle;

    char *deviceName = (char*)"/dev/i2c-1";
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0) {
        printf("Error opening I2C\n");
    }
    else {
        if (ioctl(i2cHandle, I2C_SLAVE, i2cAddressSeq) < 0) {
            printf("Error at ioctl\n");
        }
        else {
            //
            debug_print("Writing 5 words to registers at I2C addr %02x:\n", i2cAddressSeq);
            for (int i=2;i<7;i++) {
                debug_print("0x%02x = 0x%04x\n", i, registers[i]);
            }
            debug_print("\n", NULL);
            unsigned char buffer[10];
            int wordindex = 2;
            for(int i=0;i<10;i++) {
                buffer[i] = (unsigned char)(registers[wordindex]>>8);
                i++;
                buffer[i] = (unsigned char)(registers[wordindex] & 0xFF);
                wordindex++;
            }
            write(i2cHandle, buffer, 10);
        }
            
        // Close the i2c device bus
        close(*deviceName);
    }
}

/*
void RDA5702E::readOneRegI2C(int reg, unsigned short* word) {
    int i2cHandle;

    char *deviceName = (char*)"/dev/i2c-1";
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0) {
        printf("Error opening I2C\n");
    }
    else {
        if (ioctl(i2cHandle, I2C_SLAVE, i2cAddressRnd) < 0) {
            printf("Error at ioctl\n");
        }
        else {
            unsigned char cmdBuffer[1] = {(unsigned char)reg};
            unsigned char readBuffer[2];
            write(i2cHandle, cmdBuffer, 1);
            int bytesRead = read(i2cHandle, readBuffer, 2);
            if (2 != bytesRead) {
                printf("Error reading from I2C, expected 2 bytes, but got %i\n", bytesRead);
            }
            else {
                *word = (readBuffer[0]<<8);
                *word += readBuffer[1];
                printf("Read 0x%04x from register 0x%02x at I2C address 0x%02x\n", *word, reg, i2cAddressRnd);
            }
        }
            
        // Close the i2c device bus
        close(*deviceName);
    }
}
*/

void RDA5702E::readAllRegsI2C() {
    int i2cHandle;
    int wordcount = 128;
    unsigned char byteBuffer[wordcount * 2];
    int bytecount = wordcount * 2;

    char *deviceName = (char*)"/dev/i2c-1";
    if ((i2cHandle = open(deviceName, O_RDWR)) < 0) {
        printf("Error opening I2C\n");
    }
    else {
        if (ioctl(i2cHandle, I2C_SLAVE, i2cAddressSeq) < 0) {
            printf("Error at ioctl\n");
        }
        else {
            int bytesRead = read(i2cHandle, byteBuffer, bytecount);
            if (bytecount != bytesRead) {
                printf("Error reading from I2C, expected %i bytes, but got %i\n", bytecount, bytesRead);
            }
            else {
                //printf("Read %i bytes from device at I2C address 0x%02x\n", bytecount, i2cAddressSeq);
                //for(int i=0;i<bytecount;i++) { printf("Byte %03i = %02x\n", i, byteBuffer[i]); }
                int wordindex = 0;
                for(int i=108;i<140;i++) {
                    unsigned short word = (byteBuffer[i]<<8);
                    i++;
                    word += byteBuffer[i];
                    registers[wordindex] = word;
                    wordindex++;
                }
            }
        }
            
        // Close the i2c device bus
        close(*deviceName);
    }

}

void RDA5702E::init() {
    for (int i=2;i<7;i++)  {
        registers[i] = regInit[i-2];
    }
    writeAllRegsI2C();
    registers[2] = 0xC001;
    writeOneRegI2C(2, registers[2]);
    readAllRegsI2C();
}

void RDA5702E::setBand(int band) {
    // sets the band to 0 = 87-108Mhz US/EU, 1 = 76-91Mhz Japan, 2 = 76-108Mhz Worldwide, 3 = 65-76Mhz East Europe
    // Register 0x03 bits 2 and 3
    if((band >= 0) && (band < 4)) {
        printf("Setting band %i\n", band);
        readAllRegsI2C();
        registers[03] &= 0xFFF3;
        registers[03] |= (band<<2);
        writeAllRegsI2C(); 
    } else {
        printf("Band %i is invalid, band should be in the range 0 - 3\n", band);
    }
}

int RDA5702E::getBand() {
    readAllRegsI2C();
    int retVal = ((registers[03]>>2) & 0x03);
    printf("Got band (=%i)\n", retVal);
    return retVal;
}

void RDA5702E::setFreq(int freq) {
    // sets the frequency (specified in 100Khz units, e.g. 1023 = 102.3Mhz)
    readAllRegsI2C();
    printf("SetFreq = %i\n", freq);
    int band = getBand();
    int bandMinFreqs[] = {870, 760, 760, 650};
    int bandMaxFreqs[] = {1080, 910, 1080, 760};
    if((freq < bandMinFreqs[band]) || (freq > bandMaxFreqs[band])) {
        printf("Freq %i is invalid, freq should be >= %i and <= %i\n", bandMinFreqs[band], bandMaxFreqs[band]);
    } else {
        int freqVal = freq - bandMinFreqs[band];
        printf("Setting freq to %i (* 100Khz)\n", freq);
        readAllRegsI2C();
        unsigned short freqValShifted = freqVal<<6;
        registers[03] &= 0x003F;
        registers[03] |= freqValShifted;
        registers[03] |= 0x0010; // set tune bit
        writeOneRegI2C(3, registers[03]); 
    }
}

int RDA5702E::getFreq() {
    readAllRegsI2C();
    int band = getBand();
    int baseFreqs[] = {870, 760, 760, 650};
    int freqVal = (registers[3] & 0xFFC0)>>6;
    freqVal += baseFreqs[band];
    printf("Got freq (=%i using band %i)\n", freqVal, band);
    return freqVal;
}

void RDA5702E::setVolume(int vol) {
    if ((vol < 0) || (vol > 15)) {
        printf("Volume is invalid, vol should be between 0 (min) and 15 (max)\n");        
    } else {
        volume = vol;
        readAllRegsI2C();
        printf("Setting volume to %i (0 min - 15 max)\n", vol);
        registers[5] &= 0xFFF0;
        registers[5] |= vol;
        writeOneRegI2C(5, registers[05]); 
    }
}


int RDA5702E::getVolume() {
    return (muted) ? 0 : volume;
}

void RDA5702E::mute() {
    muted = true;
    int oldVol = volume;
    setVolume(0);
    volume = oldVol;
}

void RDA5702E::unMute() {
    muted = false;
    setVolume(volume);
}

void RDA5702E::toggleMute() {
    if(muted) {
        unMute();
    } else {
        mute();
    }
}

bool RDA5702E::isMuted() {
    return muted;
}

void RDA5702E::printRegs() {
    readAllRegsI2C();
    debug_print("Printing Reg Values:\n", NULL);
    for(int i=0;i<16;i++) {
        debug_print("0x%02x = %04x\n", i, registers[i]);
    }
}