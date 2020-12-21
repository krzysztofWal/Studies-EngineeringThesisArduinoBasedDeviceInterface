// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*timer interrupt*/
#define LOAD 0
#define COMP 40000;
#define BUTTON_STILL_PRESSED 2;
#define BUTTON_NOT_PRESSED_ANYMORE 1;
#define DO_NOT_CHECK_BUTTON_YET 0;


/*ekran LCD*/
#define BUTTON_PIN 21
#define SCK_PIN 69
#define MOSI_PIN 9
#define DC_PIN 10
#define RST_PIN 12
#define CS_PIN 11

/*piny "laserowe"*/
#define ILOSC_PINOW 26 
#define WIELKOSC_BUFORA_SERIAL 5
#define WSZYSTKIE_PINY 255
#define PIN_ANALOGOWY 2
#define PIN_CYFROWY 1
#define ODCZYTAJ_WSZYSTKIE 3
#define ODCZYTAJ_ANALOGOWY 2
#define ODCZYTAJ_CYFROWY 1
#define ZMIEN_STAN_CYFROWEG0 0
#define DLUGOSC_OPISU_PINOW 31

#define uSend Serial.print
#define uSendLn Serial.println

typedef struct {
    byte nrPinu;
    byte rodzajPinu;
    byte stanPinu;
    char opisPinu[DLUGOSC_OPISU_PINOW];
    byte inOut;
} Pin;

typedef struct {
    byte rodzajPolecenia;
    byte nrPinu;
    byte nowyStan;
} PolecenieInfo;


#endif

