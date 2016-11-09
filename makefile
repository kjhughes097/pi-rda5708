CC=gcc
CFLAGS= -std=c++11

radio-basic:
	$(CC) -o radio-basic radio-basic.cpp rda5702e.cpp $(CFLAGS)

radio: 
	$(CC) -o radio radio.cpp rda5702e.cpp -lwiringPi $(CFLAGS)

clean-radio:
	rm -f radio

clean-radio-basic:
	rm -f radio-basic
	
clean:
	rm -f radio
	rm -f radio-basic