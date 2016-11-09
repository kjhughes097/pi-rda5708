




class RDA5702E {
        unsigned char i2cAddressSeq = 0x10;
        unsigned char i2cAddressRnd = 0x11;
        unsigned char i2cAddressTea = 0x60;
        unsigned short registers[16] = { 0x00 };
        int frequency= 870;
        int volume = 10;
        bool muted = false;

        void writeOneRegI2C(int reg, unsigned short word);
//        void readOneRegI2C(int reg, unsigned short* word);
        void readAllRegsI2C();
        void writeAllRegsI2C();

    public:
        bool debug = false;
        RDA5702E() {};
        void init();
        void printRegs();
        void getReg(unsigned short* outVal, int reg);
        //void powerOff(void);
        void setFreq(int freq /* in 100Khz */ );
        int getFreq(); // in 100Khz units
        void setBand(int band);
        int getBand();
        void setVolume(int vol);
        int getVolume();
        void mute();
        void unMute();
        bool isMuted();
        void toggleMute();
                
};


