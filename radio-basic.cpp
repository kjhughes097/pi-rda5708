// Ken Hughes
// July 2016



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>//Needed for I2C port
#include "rda5702e.h"


// set up the preset channels here
const int PRESET_COUNT = 4;
// these can be changed / added to etc as needed - you will want your own local radio stations and frequencies.
const char *channelNames[PRESET_COUNT] = {"Heart Berkshire 1","Heart Berkshire 2","BBC Radio Berkshire","Jazz FM"};
int channelFreqs[PRESET_COUNT] =         { 970,                1029,               1041,                 1023};

bool debug = false;
int channelIndex = 0;

void printUsage() {
    printf("Usage:  radio-basic -f 870..1080 | -p 1..n [-v 0..15]\n");
    printf("\t -f : set the desired frequency (in Mhz * 10, e.g. 102.3Mhz = 1023)\n");
    printf("\t -p : select the desired preset (1..n where n is the number of presets defined)\n\n");
    printf("Examples :\n");
    printf("\t radio-basic -f 1028\n\t Sets frequency to 102.8Mhz and volume to default (10)\n\n");
    printf("\t radio-basic -f 1028 -v 5\n\t Sets frequency to 102.8Mhz and volume to 5\n\n");
    printf("\t radio-basic -p 1\n\t Sets frequency to the first preset and volume to default (10)\n\n");
    printf("\t radio-basic -p 1 -v 8\n\t Sets frequency to first preset and volume to default (10)\n\n");
    printf("\t radio-basic -l\n\t Lists the frequencies and names of the presets\n\n");
}

void printPresets() {
    printf("Preset List :\n");
    for(int i=0;i<PRESET_COUNT;i++) {
        printf("Preset %i - %i : %s \n", i+1,channelFreqs[i], channelNames[i]);
    }
}

int main(int argc, char* argv[]) {

    int freq = -1;  // no default for freq as we need the user to set it (directly, or via chosing a preset)
    int vol = 10; // default to reasonable volume

    for(int i=1; i<argc; i++) {
        //printf("argv[%i] is %s\n", i, argv[i]);
        if(strncmp(argv[i], "-f", 100) == 0) {
            i++;
            freq = atoi(argv[i]);
        } else if(strncmp(argv[i], "-v", 100) == 0) {
            i++;
            vol = atoi(argv[i]);
        } else if(strncmp(argv[i], "-d", 100) == 0) {
            debug = true;
        } else if(strncmp(argv[i], "-p", 100) == 0) {
            i++;
            int chnlIdx = atoi(argv[i]);
            if((chnlIdx > 0) && (chnlIdx <= PRESET_COUNT)) {
                printf("Using preset %i (%s : %i)\n", chnlIdx, channelNames[chnlIdx -1], channelFreqs[chnlIdx -1]);
                freq = channelFreqs[chnlIdx -1];
            } else {
                printf("Preset is invalid, it should be between 1 and %i\n", PRESET_COUNT);
            }
        } else if(strncmp(argv[i], "-l", 100) == 0) {
            printPresets();
            return 0;
        }
    }

    if(freq == -1) {
        printf("No frequency (or preset) provided.\n\n");
        printUsage();
        return 0;
    }

    RDA5702E radio;
    radio.debug = debug;
    radio.init();
    radio.setFreq(freq);
    radio.setVolume(vol);

}

