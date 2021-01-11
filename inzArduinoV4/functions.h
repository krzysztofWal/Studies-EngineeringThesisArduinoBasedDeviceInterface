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
//#include "libraries/Adafruit-MCP23017-Arduino-Library-master/Adafruit_MCP23017.h"
#include "Adafruit_MCP23017.h"


/* ==== poczatkowe ustawienia peryferiow ==== */
/*
void przyciskTimerUstawienie()
Opis :
    Ustawia rejestry 16-to bitowego TIMER1 : TCCR1A, TCCR1B, OCR1A
    Wyb�r preskalera oraz warto�ci por�wnawczej
    Timer uzywany jest do usuwania drgan mechanicznych stykow przycisku
Argumenty :
    -
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przyciskTimerUstawienie();
/*
void czestOdswEkranuTimerUstawienie()
Opis :
    Ustawia rejestry 8-bitowego TIMER2 : TCCR1A, TCCR1B, OCR1A
    Wyb�r preskalera 1024 oraz warto�ci por�wnawczej 254
    Przy takich ustawieniach timer bedzie osiagal wartosc co  srednio 16.3 us
    Timer uzywany jest do cyklicznego odswiezania wartosci wyswietlanych na ekranie
Argumenty :
    -
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void czestOdswEkranuTimerUstawienie();

/*
void czestOdswEkranuTimerUstawienie()
Opis :
    Ustawia rejestry: PCMSK0, PCICR
    Wykorzystuje tzw. Pin Change Interrupt, generowany na grupie pin�w (PCI0), kiedy dowolny z nich zmieni stan. Wybrany do uruchomienia tego przerwania jest jednak tylko pin 53, do kt�rego pod��czony jest przycisk. Nacisniecie przycisku powoduje wystapienie przerwania PCI0
Argumenty :
    -
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przerwaniePrzyciskUstawienie();

/* ==== obsluga wyswietlacza ====*/
/*
void wyswietl()
Opis :
    Wyswietla opisy wy�wietlanych warto�ci dla danego trybu wy�wietlania, okre�lonego przez przekazywany parametr trybWyswietlacza
Argumenty :
    byte trybWyswietlacza - na jego podstawie wybierane s� wy�wietlane opisy
    uint8_t *font - wska�nik do pierwszego elementu tablicy z u�yt� do wy�wietlania czcionk� z pliku DefaultFonts.c znajduj�cego sie w folderzeLCD5110_Graph
    LCD5110& wyswietlacz - referencja do obiektu reprezentuj�cego wy�wietlacz
Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewn�trznej biblioteki LCD5110_Graph,
    opisane w dokumentacji biblioteki znajdujacej sie w folderze projektowym:
        clrScr(),
        setFont(),
        print(),
        update()
*/
void wyswietl(byte trybWyswietlacza, LCD5110& wyswietlacz, uint8_t* font);
/*
void przelaczTrybWyswietlacza()
Opis :
    Zmienia warto�� zmiennej trybWyswietlacza w zale�no�ci od przekazanej warto�ci zmiennej wyborZrodlaZasilania (1 albo 0)
    wyborZrodlaZasilania == 1 : tryby ..2->0->1->2->0...
    wyborZrodlaZasilania  == 0 : tryby ..0->2->0.. lub 1->0->2->0->2.. (w zale�no�ci od pocz�tkjowej warto�ci)
Argumenty :
    byte &trybWyswietlacza - modyfikowana zmienna
    byte wyborZrodlaZasilania - kryterium cyklu zmiany
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przelaczTrybWyswietlacza(byte& trybWyswietlacza, byte wyborZrodlaZasilania);
/*
void aktualizujWyswietlaneWartosci(),
Opis :
    w zale�no�ci od zmiennej trybWyswietlacza (pomi�dzy 0 - 2) odczytuje wartosci z tablicy, do kt�rej wska�nik jest przekazywanym parametrem i wy�wietla je na wy�wietlaczu, kt�rego reprezentuj�cy obiekt jest przekazany jako referencja
Argumenty :
    byte* odczytaneWartosci - wska�nik do pierwszego elementu tablicy, z kt�rej odczytywane s� warto�ci
    LCD5110& wyswietlacz - referencja do obiektu reprezentuj�cego wy�wietlacz
    uint8_t *font - wska�nik do pierwszego elementu tablicy z u�yt� do wy�wietlania czcionk� z pliku DefaultFonts.c znajduj�cego sie w folderzeLCD5110_Graph
    byte trybWyswietlacza - na jego podstawie wybierany jest wy�wietlnay zestaw warto�ci
    Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewn�trznej biblioteki LCD5110_Graph,
    opisane w dokumentacji biblioteki znajdujacej sie w folderze projektowym:
        setFont(),
        printNumI(),
        update()
*/
void aktualizujWyswietlaneWartosci(byte* odczytaneWartosci, LCD5110& wyswietlacz, byte trybWyswietlacza, uint8_t* font);

/* ==== obslugaPrzycisku ====*/
/* !- modyfikuje zmienne globalne stanPrzycisku i przerwaniePrzycisku !- */
byte obsluzPrzycisk(byte& pierwszeWykrycieNacisnieciaPrzycisku);

/*==== obsluga komend ====*/

void obsluzKomende(Pin* wszystkiePiny, byte iloscPinow, char* bufor, Adafruit_MCP23017& mcp);

/*  byte sprawdzNumerPinu()
Opis :
    Sprawdza czy dana liczba jest numerem "opisowym" jednego z pinow zapisanych w tablicyPinow i je�li tak, to zwraca typ tego pinu
Argumenty:
    byte liczba - potencjalny "opsiowy" numer pinu
    Pin wszystkiePiny[] - wska�nik do tablicy zawieraj�cej sprawdzane piny
    size_t - ilo�� element�w w tablicy wszystkiePiny
Funkcja zwraca:
    (bute) warto�� z tych okre�lone makrami:
    PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE albo PIN_NIE_ISTNIEJE
Uzywane funkcje:
    -
*/
byte sprawdzNumerPinu(byte liczba, Pin wszystkiePiny[], size_t iloscPinow);

/* byte sprawdzPolecenie()
 Opis:
    zapisuje dane odczytane z komendy zapisanej w tablicy polecenie[] w instancji struktury typu Polecenie Info, kt�rej adres jest przekazywany
    Aby polecenie by�o odczytane jako poprawne nie mo�e zawiera� �adnych innych znak�w (spacji, tabulatury itp.)
    Mo�liwe polecenia:
        ar - odczytaj warto�ci wszystkich pin�w
        NwX, gdzie N to numer pinu a X to '1' lub '0' - ustaw wyj�cie pinu cyfrowego
        Nr - odczytaj warto�� N-tego pinu

Argumenty :
    PolecenieInfo* struktAdr - wsk�nik na instancj� struktury gdzie maja byc zapisane dane
    char polecenie[] - wska�nik na pierwszy element tablicy z poleceniem
    size_t dlugosc - dlugosc tablicy z poleceniem
    Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy z danymi pin�w
    size_t iloscPinow - d�ugo�� tablicy z danymi pinow (inaczej ilosc pinow)
Funkcja zwraca :
    1 - je�li polecenie by�o nieprawid�owe
    0 - je�li komenda by�a niew�a�ciwa
U�ywane funkcje:
    byte sprawdzNumerPinu(byte, Pin* , size_t),
Uwagi:
    Algorytm dzia�ania:
        Funkcja okre�la czy w podanym poleceniu na pierwszych pozycjach znajduje si�  numer pinu (funkcja obs�uguje numery 1 lub 2 cufrowe !) lub znak 'a'
        Je�li jest to 'a' to w polu instancji struktury rodzajPolecenia zapisywana jest warto�� okre�lona makrem ODCZYTAJ WSZYSTKIE
        Je�li numer pinu istnieje to w zale�no�ci od jego typu (PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE) sprawdzana jest poprawno�� polecenia wdg kryteri�w:
        - piny analogowe i wej�cia cyfrowe mo�na tylko odczyta� (jako rodzajPolecenia zapisywany jest ODCZYTAJ_ANALOGOWY lub ODCZYTAJ_CYFROWY)
        - wyj�cia cyfrowe mo�na odczyta� i zmieni� (ODCZYTAJ_CYFROWY albo ZMIEN_STAN CYFROWEGO)
    W przypadku zmiany stanu wyj�cia cyfrowego, po��dany stan (1 lub 0) jesgt wpisywany w pole struktury nowyStan

*/
byte sprawdzKomende(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin wszystkiePiny[], size_t iloscPinow);

/* Pin* zmienStanPinu()
    Zmienia stan na wybranym wyj�ciu cyfrowym (nie dokonuje aktualizacji jego stanu w tablicy wszystkiePiny). Zak�ada, �e zosta�o sprawdzone, czy pin taki istnieje i jest wyj�ciem cyfrowym.
    Argumenty:
        Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy, w kt�ej znajduje si� pin, kt�rego stan ma ulec zmianie
        size_t iloscPinow - rozmiar powy�szej 
        byte nrPinu - nr pinu, na kt�rym ma by� zmieniony stan
        byte nowyStan - po��dany stan na okre�lonym wyj�ciu
    Funkcja zwraca:
        (Pin *) wska�nik na element tablicy z pinami, kt�rego stan zosta� zmieniony
    U�ywane funkcje:
        digitalWrite() - funkcja Arduino
*/
Pin* zmienStanPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu, byte nowyStan);

byte zwrocRzeczywistyNumerPinu(Pin wszystkiePiny[], size_t iloscPinow, byte numerWyboru);

byte zwrocNumerWyboru(Pin wszystkiePiny[], size_t iloscPinow, byte rzeczywistyNrPinu);

/*==== obsluga tablicy pinow (stany pinow cyfrowych) ====*/
/*void aktualizujTabeleStanow()
Opis:
    Zmienia warto�� stanu pinu zapisanego w tablicy wszystkiePiny (tj. zmienia warto�� zapisan� w tablicy, a nie t� "rzeczywist�" - patrz: funkcja zmienStanPinu()); zak�ada, �e pin taki istnieje i jest cyfrowym wyj�ciem
Argumenty:
    Pin* wszystkiePiny - wska�nik na pin (element w tablicy), kt�rego zapisany w niej stan ma by� zmieniony
Funkcja zwraca:
    -
U�ywane funkcje:
    -
*/
void aktualizujStanPinu(Pin* wszystkiePiny, byte nowyStan);

/* ===== odcztywanie stanow pinow ==== */
/* byte zwrocStanPinuCyfrowego()
 Opis:
    Zwraca stan logiczny pinu odczytany funkcj� digitalRead() - w przypadku wej�cia - lub z tablicy wszystkiePiny - w przypadku wyj�cia; zak�ada, �e pin jest cyfrowy i znajduje si� w  tablicy
 Argumenty:
        Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy pin�w
        size_t iloscPinow - rozmiar powy�szej
        byte nrPinu - nr pinu kt�rego stan logiczny ma zwr�ci�
Funkcja zwraca:
    (byte) odczytany stan (HIGH albo LOW)
U�ywane funkcje:
    (w zale�no�ci od tego, czy pin jest wej�ciem czy wyj�ciem)
    digtalRead() - funkcja Arduino 
*/
byte zwrocStanPinuCyfrowego(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* byte zwrocWartoscPinuAnalogowego()
 Opis:
      zwraca warto�� odczytan� funkcj� analogRead() jako procent z 0-5V (uzywa funkcji map()); zak�ada, �e pin znajduje si� w tablicy
 Argumenty:
        Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy pin�w, w kt�rej znajduje si�
        size_t iloscPinow - ilo�� element�w w tablicy
        nrPinu - nr pinu kt�rego warto�� ma by� odczytana
Funkcja zwraca:
    (byte) odczyt z pinu analogowego przeskalowany pomi�dzy warto�ci 0-100
U�ywane funkcje:
    funkcje Arduino:
    analogRead(),
    map()
*/
byte zwrocWartPinuAnalogowego(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* void odczytajWartosciPinowAnalogowych
Opis:
    !- zak�ada, �e tablica odczytaneWartosci ma minimum 6 wyraz�w -!
    Odczytuje wybrane warto�ci odczytane z: POW_AMP_CUR_MON_PIN, PRE_AMP_CUR_MON_PIN, A_S_CUR_S_P_PIN, S_C_S_P_PIN, BASE_PLATE_TEMP_MON_PIN (w zale�no�ci od warto�ci zmiennej trybWyswietlacza - w zakresie 0-2), skaluje je do zakresu 0-100 i ich cyfry zapisuje w elementach tablicy odczytaneWarto�ci
Argumenty:
     byte trybWyswietlacza - od jej warto�ci (0-2) zale�y, kt�re warto�ci zostan� wpisane do tablicy
     byte *odczytaneWartosci - wska�nik na pierwszy element tablicy odczytaneWartosci
Funkcja zwraca:
    -
U�ywane funkcje:
    funkcje Arduino:
        analogRead(),
        map()
*/
void odczytajWartosciMonitorow(byte trybWyswietlacza, byte* odczytaneWartosci);

/* ==== wyswietlanie tekstowe ==== */

/*  void wyswietlStanyPinow()
 Opis:
    Wysy�a po��czeniem szeregowym stany wszystkich pin�w zawartych w tablicy wszytkiePiny. Do odczytania tych stan�w u�ywa funkcji zwrocWartoscPinuAnalogowego() oraz zwrocWartoscPinuCyfrowego(). Zak�ada, �e komunnikacja szeregowa zosta�a zainicjowana
Argumenty
    Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy z zawaratymi informacjami o pinach
    size_t iloscPinow - rozmiar tej tablicy
Funkcja zwraca:
    -
U�ywane funkcje:
    byte zwrocStanPinuAnalogowego(),
    byte zwrocStanPinuCyfrowego(),
    print(), println() - metody klasy Serial (Arduino)
*/
void wyswietlStanyWszystkichPinow(Pin wszystkiePiny[], size_t iloscPinow);

/* void wyswietlOpisPinu() 
Opis:
    Wysy�a po��czeniem szeregowym opis wybranego pinu zawarty w tablicy wszytkiePiny. . Zak�ada, �e komunnikacja szeregowa zosta�a zainicjowana oraz, �e pin znajduje si� w tej tablicy
Argumenty:
    Pin wszystkiePiny[] - wska�nik na pierwszy element tablicy z zawartym opisem pinu
    size_t iloscPinow - ilo�� element�w w tablicy
    byte nrPinu - numer pinu, kt�rego opis ma by� przes�any
Funkcja zwraca:
    -
U�ywane funkcje:
    print() - metoda klasy Serial (Arduino)
*/
void wyswietlOpisPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* ==== ledy bledow ==== */
/*void obslugaLedowBledow
Opis:
    !- w przypadku tej funkcji musi zachodzic liczba elementow tablicy pinyBledow <= liczba elemntow tablicy ledNrPin -!
    Sprawdza czy na pinach spo�r�d tych zapisanych w tablicy pinyBledow nast�pi�a zmiana od ostatniego sprawdzenia (warto�ci w tablicy poprzedniStanPinowBledow). Jesli tak by�o, to zmienia odpowiednio stan LED�w (o ilo�ci r�wnej wartosci iloscPinowB��dow) pod��czonych kolejno do  pocz�tkowych pin�w (znajdujacych sie w tablicy ledNrPin) ekspandera (mcp)
    Modyfikuje r�wnie� tablic� poprzedniStanPinowBledow przypisuj�c jej odczytane wartosci
    Zak�ada, �e obydwie tablice s� tych samych rozmiar�w
Argumenty:
    Pin* pinyBledow - wska�nik do pierwszego elementu tablicy ze sprawdzanymi pinami
    byte* poprzedniStanPinowBledow - wska�nik do pierwszego elementu tablicy, z kt�rej brane s� stany por�wnawcze 
    byte iloscPinowBledow - ilo�� element�w w obydwu tablicach
    byte* ledNrPin - wska�nik na pierwszy element tablicy, gdzie znajduj� si� nr pin�w ekspandera, do kt�rych pod��czone s� LEDy
    Adafruit_MCP23017& mcp - referencja na obiekt reprezentuj�cy ekspander
Funkcja zwraca:
    -
U�ywane funkcje:
    zwrocStanPinuCyfrowego(),
    digitalWrite() - metoda klasy Adafruit_MCP23017 wprowadzaj�ca pin ekspandera w okre�lony stan
*/

void obslugaLedowBledow(Pin* pinyBledow, byte* poprzedniStanPinowBledow, byte iloscPinowBledow, byte* ledNrPin, Adafruit_MCP23017& mcp);


#endif
