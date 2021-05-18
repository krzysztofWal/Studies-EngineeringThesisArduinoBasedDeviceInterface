// typedefs.h

#ifndef _TYPEDEFS_h
#define _TYPEDEFS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define TERMITE 

// V_IN 4.94
// V_MAX 4.80


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
/* koljność odpowiada tej na płytce i w pracy*/
#define LED1_PIN 6 // PA6
#define LED2_PIN 7 // PA7
#define LED3_PIN 11 // PB3
#define LED4_PIN 10 // PB2
#define LED5_PIN 9 // PB1
#define LED6_PIN 8// PB0
#define LED7_PIN 12// PB4
#define LED_LASER_READY_PIN 13 //PB5 LED 8
#define LED_LAS_EMIT_GATE_ENABLE_PIN 14 //PB6 LED 9
#define LED_LASER_DISABLE_PIN 15//PB7 LED 10

/*wyswietlacz*/
#define WYBOR_ZRODLA_NAPIECIA_PIN A3
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

/*nie zmieniac bez modyfikacji funkcji odczytajWartosciADC - wykorzystuje ona tablice z szescioma wartosciami*/
#define ILOSC_CYFR_ADC 6  

/*piny sterujace laserem*/
#define LASER_EMITION_GATE_PIN 14
#define ALIGNMENT_LAS_ENABLE_PIN 15
#define GLOBAL_ENABLE_PIN 16
#define LASER_READY_PIN 17
#define LAS_DISABLE_PIN 18
#define MODE_SELECT_BIT 19

#define STATE_SEL_0_PIN 22
#define STATE_SEL_1_PIN 24
#define STATE_SEL_2_PIN 26
#define STATE_SEL_3_PIN 28
#define STATE_SEL_4_PIN 30
#define STATE_SEL_5_PIN 23
#define STATE_SEL_6_PIN 25
#define STATE_SEL_7_PIN 27

/*piny monitorujace*/
#define BASE_PLATE_TEMP_MON_PIN A0
#define PRE_AMP_CUR_MON_PIN A1
#define POW_AMP_CUR_MON_PIN A2
#define A_S_CUR_S_P_PIN A4
#define S_C_S_P_PIN A5
 
/*piny bledow*/
#define POW_SUP_FAULT_PIN 62
#define RESERVED_FAULT_PIN 63
#define BEAM_COLL_FAULT_PIN 64
#define POW_AMP_CUR_FAULT_PIN 65
#define PRE_AMP_CUR_FAULT_PIN 67
#define BASE_PLATE_TEMP_FAULT_PIN 66
/* zmiana celowa PRE_AMP i BASE_PLATE z powodu przylutowania odwrotnego*/
#define SEED_LAS_TEMP_FAULT_PIN 68

#define ILOSC_PINOW 26
/*ILOSC_PINOW_BLEDOW musi byc mniejsza lub rowna ILOSC_LEDOW
patrz: funkcja obslugaLedowBledow()*/
#define ILOSC_PINOW_CHARAKT 8
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
#define WIELKOSC_BUFORA_SERIAL 6
#define ODCZYTAJ_WSZYSTKIE 3
#define ODCZYTAJ_ANALOGOWY 2
#define ODCZYTAJ_CYFROWY 1
#define ZMIEN_STAN_CYFROWEG0 0
#define MAKSMALNY_MOZLIWY_NUMER_CHARAKT 30

/* skroty metod klasy Serial */
#define sSend Serial.print
#define sSendLn Serial.println
#define sSendLine Serial.println("====================================")

typedef struct {
    byte rzeczywistyNrPinu; //nr pinu Atmegi2560
    byte numerWyboru; //nr widoczny przez użytkownika, piny 'posegregowane' są kategoriami
    byte rodzajPinu; // ANALOG/DIGITAL
    byte stanPinu; //używany przy pinach cyfrowych - HIGH/LOW
    char opisPinu[DLUGOSC_OPISU_PINOW]; //opis wyświetlany użytkownikowi w terminalu
    byte inOut; // INPUT/OUTPUT
} Pin;

typedef struct {
    byte rodzajPolecenia;
    byte nrPinu;
    byte nowyStan;
} PolecenieInfo;



#endif
