// Ken Hughes
// July 2016



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>//Needed for I2C port
#include "rda5702e.h"
#include <wiringPi.h>
#include "ssd1306.h"

#define SWITCH_DELAY 5000
#define SHUTDOWN_DELAY 20


// set up the preset channels here
const int PRESET_COUNT = 4;
const char *channelNames[PRESET_COUNT] = {"Heart Berkshire 1","Heart Berkshire 2","BBC Radio Berkshire","Jazz FM"};
int channelFreqs[PRESET_COUNT] =         { 970,                1029,               1041,                 1023};


void setupButtons();
void setupDisplay(SSD1306*);
void setupAnalogVolume();
int getAnalogVolume();
bool isUpPressed();
bool isDownPressed();
bool isStandByPressed();
void channelUp(RDA5702E*);
void channelDown(RDA5702E*);
void startShutdown();

const int RADIO_ADDR = 0x10;
bool debug = false;
int channelIndex = 0;


int main(int argc, char* argv[]) {

    int freq = 1029; // default to heart.co.uk
    int vol = 10;

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
                channelIndex = chnlIdx;
                printf("Using preset %i (%s : %i)\n", chnlIdx, channelNames[channelIndex -1], channelFreqs[channelIndex -1]);
                freq = channelFreqs[channelIndex -1];
            } else {
                printf("Preset is invalid, it should be between 1 and %i\n", PRESET_COUNT);
            }
        }   
    }

    // set up wiringPi
    wiringPiSetupGpio();

    // start and setup the radio with defaults
    RDA5702E radio;
    radio.debug = debug;
    radio.init();
    radio.setFreq(freq);
    radio.setVolume(vol);

    // set up the buttons and volume ADC
    setupButtons();
    setupAnalogVolume();
    
    // set up the OLED display
    SSD1306 oled;
    setupDisplay(&oled);


    // main loop - do forever
    while(true) {

        bool isDirty = false;

        if(isUpPressed()) {
            channelUp(&radio);
            delay(SWITCH_DELAY); // so we debounce
            isDirty = true; // so we update the display later
        }
        if(isDownPressed()) {
            channelDown(&radio);
            delay(SWITCH_DELAY);
            isDirty = true;
        }
        if(isStandByPressed()){
            startShutdown();
        }

        int vol = getAnalogVolume();
        if(vol != radio.getVolume()) {
            //radio.setVolume(vol);
            isDirty = true;
        }


        if(isDirty) {
            // update the OLED display
        }

        delay(100);

    }

}

#define BTN_UP 5
#define BTN_DN 6
#define BTN_SB 13
#define BTN_PW 19
void setupButtons() {
    // configure the buttons for up/down/standby/reset
    pinMode(BTN_UP, INPUT);
    pinMode(BTN_DN, INPUT);
    pinMode(BTN_SB, INPUT);
    pinMode(BTN_PW, INPUT);
    pullUpDnControl(BTN_UP, PUD_UP);
    pullUpDnControl(BTN_DN, PUD_UP);
    pullUpDnControl(BTN_SB, PUD_UP);
    pullUpDnControl(BTN_PW, PUD_UP);
}

void setupAnalogVolume() {
    // set up the PCF8571 ADC to monitor the volume pot.
}

void setupDisplay(SSD1306 *pOLED) {
    // set up the 128x64 OLED I2C display
	pOLED->initDisplay();
    pOLED->setWordWrap(FALSE);
    pOLED->setDisplayMode(SSD1306::Mode::WRAP);
    pOLED->textDisplay("UP  DN  STBY  PWR");   // top line showing button functions 
}

void startShutdown() {
    // start the shutdown sequence
    for(int i = 0; i<SHUTDOWN_DELAY; i++) {
        delay(1000);
        if(!isStandByPressed()) {
            return;
        } else {
            printf("Shutdown in %i seconds\n", SHUTDOWN_DELAY -i);
        }
    }
    printf("Shutting down now !!\n");
    system ("sudo shutdown -h now");
}

void channelUp(RDA5702E *radio) {
    // select the next preset channel
    channelIndex++;
    if(channelIndex == PRESET_COUNT) { channelIndex = 0; } // roll back round to 0
    int freq = channelFreqs[channelIndex];
    printf("Moving channel UP to preset %i [%s (%i)]\n", channelIndex+1,channelNames[channelIndex], freq);
    radio->setFreq(freq);
}

void channelDown(RDA5702E *radio) {
    // select the previous preset channel
    channelIndex--;
    if(channelIndex < 0) { channelIndex = (PRESET_COUNT -1); } // roll back round to 'PRESET_COUNT'
    int freq = channelFreqs[channelIndex];
    printf("Moving channel DOWN to preset %i [%s (%i)]\n", channelIndex+1,channelNames[channelIndex], freq);
    radio->setFreq(freq);
}

int getAnalogVolume() {
    // read the volume pot and convert to an int between 0 and 15
    return 1;
}

bool isDownPressed() {
    // read the down button
    return (digitalRead(BTN_DN) == 0);
}

bool isUpPressed() {
    // read the up button
    return (digitalRead(BTN_UP) == 0);
}

bool isStandByPressed() {
    // read the up button
    return (digitalRead(BTN_SB) == 0);
}