// functions.h

#ifndef _FUNCTIONS_h
#define _FUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "typedefs.h"
#include "libraries/LCD5110_Graph/LCD5110_Graph.h"
#include "avr/pgmspace.h"

void wyswietl(byte trybWyswietlacza, LCD5110& wyswietlacz, uint8_t* font);

void setNext(byte& trybWyswietlacza);

/*  wyswietlStanyPinow()
    Wysyla serialem stany pinow zapisane w tablicy
    Przyjmuje wskaznik do tablicy z pinami, ktorej elementamim sa struktury
    oraz jej rozmiar
*/
void wyswietlStanyPinow(Pin stanyPinowCyfrowych[], size_t iloscPinow);

/* podajStanPinuCyfrowego()
     argumenty:
        adres tablicy pinow
        jej rozmiar
        nr pinu ktorego stan chcemy odczytac
    zwraca stan pinu wyjsciowego
*/
byte podajStanPinuCyfrowego(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu);

/* podajStanPinuAnalogowego
    argumenty:
        adres tablicy pinow
        jej rozmiar
        nr pinu ktorego stan chcemy odczytac
     zwraca wartoœæ odczytana z pinu funkcja analogRead() jako procent z 0-5V (uzywa funkcji map())
*/
long podajStanPinuAnalogowego(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu);

/*  sprawdzNumerPinu()
    Sprawdza czy dana liczba jest numerem jednego z pinow zapisanych w tablicyPinow i jesli tak to czy jest to pin cyfrowy czy analogowy
    zwraca PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE albo PIN_NIE_ISTNIEJE
*/
byte sprawdzNumerPinu(byte liczba, Pin stanyPinowCyfrowych[], size_t iloscPinow);

/* sprawdzPolecenie()
    zapisuje dane z ostatniego otrzymanego polecenia w  strukturze ktorej adres jest przekazany
    lista argumentow:
        adres struktury gdzie maja byc zapisane dane
        adres tablicy z poleceniem,
        dlugosc tablicy z poleceniem,
        tablica z danymi odnosnie pinow (typ PinCyfrowy),
        dlugosc tablicy z danymi pinow (inaczej ilosc pinow)
    zwraca:
        1 jesli polecenie bylo poprawne
        0 jesli polecenie nie bylo poprawne
    funkcja dziala dla 1 i 2 cyfrowych numerow pinow
*/
byte sprawdzPolecenie(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin stanyPinowCyfrowych[], size_t iloscPinow);

/* zmienStanPinu
    zmienia stan wyjscia na wybranym pinie cyfrowym
    argumenty:
        adres tablicy zawierajacej obecne stany pinow
        rozmiar tablicy (ilosc pinow)
        nr pinu na ktorym ma byc zmieniony stan
        nowy stan na wyjsciu
    zwraca:
        adres elementu tablicy z pinami ktorego stan zostal zmieniony
        !- nie aktualizuje tablicy stanow tylko zmienia stan wyjscia -!

*/
Pin* zmienStanPinu(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu, byte nowyStan);

/* aktualizujTabeleStanow
    aktualizuje element tablicy stanow
    argumenty:
        adres elementu tablicy do aktualizacji
        nowy stan pinu
    !- nie zmienia stanu wyjscia -!
*/
void aktualizujTabeleStanow(Pin* stanyPinowCyfrowych, byte nowyStan);

void wyswietlOpisPinu(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu);

void przyciskTimerUstawienie();
void czestOdswEkranuTimerUstawienie();
void przerwaniePrzyciskUstawienie();
void przerwanieBledyUstawienie();

#endif
