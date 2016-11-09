#Raspberry Pi code to drive a RDA5708 FM module

This project allows a RDA5708 FM Radio module to be driven by a Raspberry Pi.

The RDA5708 FM Radio module is a neat little radio receiver that can be driven via I2C. This code provides a library for some of the typical functionality required for the Raspberry Pi to use the module. Functions include setting frequency, band, volume etc.

The rda5702e.cpp and rda5702e.h files can be used as a library for your own apps, or you can use the sample apps (radio, and radio-basic) directly with your Raspberry Pi.

## radio-basic
This app provides the simpler functionality of just allowing the user to select the frequency, volume and/or a preset channel with command line options.

A makefile is included, so compiling the app is a simple case of `make radio-basic`
Running the app with no command line parameters will provide usage information and examples of how to use it.

## radio
This app goes a little further. In addition to setting the frequency, volume and/or preset it will also configure GPIO pins are inputs and monitor them for being pulled low (i.e. a button pressed). These 'buttons' are used for changing the preset up or down and for shutting the Raspberry Pi down (long press the Standby button)

A makefile is included, so compiling the app is a simple case of `make radio`
Running the app with no command line parameters will provide usage information and examples of how to use it.

*Note* This app uses the wiringPi library, so needs to be run with sudo (e.g. `sudo ./radio -f 1029 -v 10`


