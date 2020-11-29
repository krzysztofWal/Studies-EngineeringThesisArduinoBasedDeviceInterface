// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ILOSC_PINOW 8 
#define WIELKOSC_BUFORA_SERIAL 5
#define WSZYSTKIE_PINY 255
#define PIN_ANALOGOWY 2
#define PIN_CYFROWY 1
#define ODCZYTAJ_WSZYSTKIE 3
#define ODCZYTAJ_ANALOGOWY 2
#define ODCZYTAJ_CYFROWY 1
#define ZMIEN_STAN_CYFROWEG0 0
#define DLUGOSC_OPISU_PINOW 20
#define uSend Serial.print
#define uSendLn Serial.println

typedef struct {
    byte nrPinu;
    byte rodzajPinu;
    byte stanPinu;
    char opisPinu[DLUGOSC_OPISU_PINOW];
} Pin;

typedef struct {
    byte rodzajPolecenia;
    byte nrPinu;
    byte nowyStan;
} PolecenieInfo;


#endif

