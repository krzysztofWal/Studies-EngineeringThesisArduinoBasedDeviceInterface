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
#define PRZYCISK_PIN 53
#define SCK_PIN 51
#define MOSI_PIN 9
#define DC_PIN 10
#define RST_PIN 12
#define CS_PIN 11

/*piny "laserowe"*/
#define LASER_EMITION_GATE_PIN 14
#define ALIGNMENT_LAS_ENABLE_PIN 15
#define GLOBAL_ENABLE_PIN 16
#define LASER_READY_PIN 17
#define LAS_DISABLE_PIN 18
#define MODE_SELECT_BIT 19

#define STATE_SEL_0_PIN 23
#define STATE_SEL_1_PIN 25
#define STATE_SEL_2_PIN 27
#define STATE_SEL_3_PIN 29
#define STATE_SEL_4_PIN 31
#define STATE_SEL_5_PIN 33
#define STATE_SEL_6_PIN 35
#define STATE_SEL_7_PIN 37

#define BASE_PLATE_TEMP_PIN 1
#define PRE_AMP_CUR_MON_PIN 1
#define PO_AMP_CUR_MON_PIN 1
#define A_S_CUR_S_P_PIN 1
#define S_C_S_P_PIN 1
 
#define ILOSC_PINOW 21
#define ILOSC_PINOW_BLEDOW 7
#define WIELKOSC_BUFORA_SERIAL 5

#define WSZYSTKIE_PINY 255
#define PIN_NIE_ISTNIEJE 0
#define PIN_CYFROWY_WEJSCIE 3
#define PIN_CYFROWY_WYJSCIE 2
#define PIN_ANALOGOWY 1
#define PIN_CYFROWY 4

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

