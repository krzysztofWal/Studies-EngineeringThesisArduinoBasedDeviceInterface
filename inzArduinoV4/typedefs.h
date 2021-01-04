// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

/*timer interrupt*/
#define WARTOSC_WCZYTYWANA 0
#define PRZYCISK_WCIAZ_WCISNIETY 2
#define PRZYCISK_ZOSTAL_PUSZCZONY 1
#define NIE_SPRAWDZAJ_STANU_PRZYCISKU 0

#define PRZERWANIE_TIMER1_ON TIMSK1 = (1 << OCIE1A)
#define PRZERWANIE_TIMER1_OFF TIMSK1 &= ~(1 << OCIE1A)
#define PRZERWANIE_TIMER2_OFF TIMSK2 &= ~(1 << OCIE2A)
#define PRZERWANIE_TIMER2_ON TIMSK2 |= (1 << OCIE2A)
#define PRZERWANIE_PRZYCISK_ON PCICR |= (1 << PCIE2)
#define PRZERWANIE_PRZYCISK_OFF PCICR &= ~(1 << PCIE2)


/*ekran LCD*/
//#define BUTTON_PIN PK6
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

