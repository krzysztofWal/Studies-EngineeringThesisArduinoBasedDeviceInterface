// functions.h

#ifndef _FUNCTIONS_h
#define _FUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ILOSC_PINOW_CYFROWYCH 5
#define WIELKOSC_BUFORA_SERIAL 5
#define uSend Serial.print
#define uSendLn Serial.println

#include "typedefs.h"

/*  wyswietlStanyPinowCyfrowych()
    Wysyla serialem stany pinow zapisane w tablicy
    Przyjmuje wskaznik do tablicy z pinami, ktorej elementamim sa struktury
    oraz jej rozmiar
*/
void wyswietlStanyPinowCyfrowych(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow);

void wyswietlStanPinu(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu);
#endif

/*  sprawdzPrzedzial()
    Sprawdza czy dana liczba jest numerem jednego z pinow zapisanych w tablicy stanyPinowCyfrowych
    zwraca 1 jesli tak
    zwraca 0 jesli nie
*/
byte sprawdzPrzedzial(byte liczba, PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow);

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
byte sprawdzPolecenie(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow);

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
PinCyfrowy* zmienStanPinu(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu, byte nowyStan);

/* aktualizujTabeleStanow
    aktualizuje element tablicy stanow
    argumenty:
        adres elementu tablicy do aktualizacji
        nowy stan pinu
    !- nie zmienia stanu wyjscia -!
*/
void aktualizujTabeleStanow(PinCyfrowy* stanyPinowCyfrowych, byte nowyStan);