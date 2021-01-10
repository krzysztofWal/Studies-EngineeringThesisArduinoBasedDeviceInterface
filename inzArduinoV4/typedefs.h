// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


/*obsluga przycisku*/
#define WARTOSC_WCZYTYWANA 0
#define PRZYCISK_WCIAZ_WCISNIETY 2
#define PRZYCISK_ZOSTAL_PUSZCZONY 1
#define NIE_SPRAWDZAJ_STANU_PRZYCISKU 0

/*wlaczanie i wylaczanie timerow*/
#define PRZERWANIE_TIMER1_ON TIMSK1 = (1 << OCIE1A)
#define PRZERWANIE_TIMER1_OFF TIMSK1 &= ~(1 << OCIE1A)
#define PRZERWANIE_TIMER2_OFF TIMSK2 &= ~(1 << OCIE2A)
#define PRZERWANIE_TIMER2_ON TIMSK2 |= (1 << OCIE2A)
#define PRZERWANIE_PRZYCISK_ON PCICR |= (1 << PCIE2)
#define PRZERWANIE_PRZYCISK_OFF PCICR &= ~(1 << PCIE2)

/*ekspander - numery pinow wdg dokumentacji Adafruit-MCP23017-Library*/
#define LED1_PIN 6 // PA6
#define LED2_PIN 7 // PA7
#define LED3_PIN 8 // PB0
#define LED4_PIN 9 //  .
#define LED5_PIN 10//  .
#define LED6_PIN 11//  .
#define LED7_PIN 12
#define LED_LASER_DISABLE_PIN 13
#define LED_LASER_READY_PIN 14
#define LED_LAS_EMIT_GATE_ENABLE_PIN 15//PB7

/*wyswietlacz*/
#define WYBOR_ZASILANIA_PIN 2
#define PRZYCISK_PIN 53
#define PRZYCISK_PORT PINB
#define PRZYCISK_REJ_POZYCJA PB0
#define SCK_PIN 51
#define MOSI_PIN 9
#define DC_PIN 10
#define RST_PIN 12
#define CS_PIN 11

#define WARTOSC_GORA_X 20
#define WARTOSC_GORA_Y 7
#define WARTOSC_DOL_X 20
#define WARTOSC_DOL_Y 33

/*nie zmieniac bez modyfikacji funkcji odczytajWartosciADC - wykorzystuje ona bufor z szescioma wartosciami*/
#define ILOSC_CYFR_ADC 6  

/*piny sterujace laserem*/
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

/*piny monitorujace*/
#define BASE_PLATE_TEMP_MON_PIN 54
#define PRE_AMP_CUR_MON_PIN 55
#define POW_AMP_CUR_MON_PIN 56
#define A_S_CUR_S_P_PIN 57
#define S_C_S_P_PIN 58
 
/*piny bledow*/
#define POW_SUP_FAULT_PIN 62
#define RESERVED_FAULT_PIN 63
#define BEAM_COLL_FAULT_PIN 64
#define POW_AMP_CUR_FAULT_PIN 65
#define PRE_AMP_CUR_FAULT_PIN 66
#define BASE_PLATE_TEMP_FAULT_PIN 67
#define SEED_LAS_TEMP_FAULT_PIN 68

#define ILOSC_PINOW 26
/*ILOSC_PINOW_BLEDOW musi byc mniejsza lub rowna ILOSC_LEDOW
patrz: funkcja obslugaLedowBledow()*/
#define ILOSC_PINOW_BLEDOW 7
#define ILOSC_LEDOW 10 
#define DLUGOSC_OPISU_PINOW 31

/* zwracane przez funkcje */

#define WSZYSTKIE_PINY 255
#define PIN_NIE_ISTNIEJE 0
#define PIN_CYFROWY_WEJSCIE 3
#define PIN_CYFROWY_WYJSCIE 2
#define PIN_ANALOGOWY 1
#define PIN_CYFROWY 4

/* obsluga komend */
#define WIELKOSC_BUFORA_SERIAL 5
#define ODCZYTAJ_WSZYSTKIE 3
#define ODCZYTAJ_ANALOGOWY 2
#define ODCZYTAJ_CYFROWY 1
#define ZMIEN_STAN_CYFROWEG0 0


/* === */
#define uSend Serial.print
#define uSendLn Serial.println


typedef struct {
   //char przypisanyOpisPinu[3];
    byte rzeczywistyNrPinu;
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

