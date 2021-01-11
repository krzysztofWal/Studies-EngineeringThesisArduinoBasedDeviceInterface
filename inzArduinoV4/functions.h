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
#include "libraries/Adafruit-MCP23017-Arduino-Library-master/Adafruit_MCP23017.h"



/* ==== poczatkowe ustawienia peryferiow ==== */
/*
void przyciskTimerUstawienie()
Opis :
    Ustawia rejestry 16-to bitowego TIMER1 : TCCR1A, TCCR1B, OCR1A
    Wybór preskalera oraz wartoœci porównawczej
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
    Wybór preskalera 1024 oraz wartoœci porównawczej 254
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
    Wykorzystuje tzw. Pin Change Interrupt, generowany na grupie pinów (PCI0), kiedy dowolny z nich zmieni stan. Wybrany do uruchomienia tego przerwania jest jednak tylko pin 53, do którego pod³¹czony jest przycisk. Nacisniecie przycisku powoduje wystapienie przerwania PCI0
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
    Wyswietla opisy wyœwietlanych wartoœci dla danego trybu wyœwietlania, okreœlonego przez przekazywany parametr trybWyswietlacza
Argumenty :
    byte trybWyswietlacza - na jego podstawie wybierane s¹ wyœwietlane opisy
    uint8_t *font - wskaŸnik do pierwszego elementu tablicy z u¿yt¹ do wyœwietlania czcionk¹ z pliku DefaultFonts.c znajduj¹cego sie w folderzeLCD5110_Graph
    LCD5110& wyswietlacz - referencja do obiektu reprezentuj¹cego wyœwietlacz
Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewnêtrznej biblioteki LCD5110_Graph,
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
    Zmienia wartoœæ zmiennej trybWyswietlacza w zale¿noœci od przekazanej wartoœci zmiennej wyborZrodlaZasilania (1 albo 0)
    wyborZrodlaZasilania == 1 : tryby ..2->0->1->2->0...
    wyborZrodlaZasilania  == 0 : tryby ..0->2->0.. lub 1->0->2->0->2.. (w zale¿noœci od pocz¹tkjowej wartoœci)
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
    w zale¿noœci od zmiennej trybWyswietlacza (pomiêdzy 0 - 2) odczytuje wartosci z tablicy, do której wskaŸnik jest przekazywanym parametrem i wyœwietla je na wyœwietlaczu, którego reprezentuj¹cy obiekt jest przekazany jako referencja
Argumenty :
    byte* odczytaneWartosci - wskaŸnik do pierwszego elementu tablicy, z której odczytywane s¹ wartoœci
    LCD5110& wyswietlacz - referencja do obiektu reprezentuj¹cego wyœwietlacz
    uint8_t *font - wskaŸnik do pierwszego elementu tablicy z u¿yt¹ do wyœwietlania czcionk¹ z pliku DefaultFonts.c znajduj¹cego sie w folderzeLCD5110_Graph
    byte trybWyswietlacza - na jego podstawie wybierany jest wyœwietlnay zestaw wartoœci
    Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewnêtrznej biblioteki LCD5110_Graph,
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
    Sprawdza czy dana liczba jest numerem "opisowym" jednego z pinow zapisanych w tablicyPinow i jeœli tak, to zwraca typ tego pinu
Argumenty:
    byte liczba - potencjalny "opsiowy" numer pinu
    Pin wszystkiePiny[] - wskaŸnik do tablicy zawieraj¹cej sprawdzane piny
    size_t - iloœæ elementów w tablicy wszystkiePiny
Funkcja zwraca:
    (bute) wartoœæ z tych okreœlone makrami:
    PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE albo PIN_NIE_ISTNIEJE
Uzywane funkcje:
    -
*/
byte sprawdzNumerPinu(byte liczba, Pin wszystkiePiny[], size_t iloscPinow);

/* byte sprawdzPolecenie()
 Opis:
    zapisuje dane odczytane z komendy zapisanej w tablicy polecenie[] w instancji struktury typu Polecenie Info, której adres jest przekazywany
    Aby polecenie by³o odczytane jako poprawne nie mo¿e zawieraæ ¿adnych innych znaków (spacji, tabulatury itp.)
    Mo¿liwe polecenia:
        ar - odczytaj wartoœci wszystkich pinów
        NwX, gdzie N to numer pinu a X to '1' lub '0' - ustaw wyjœcie pinu cyfrowego
        Nr - odczytaj wartoœæ N-tego pinu

Argumenty :
    PolecenieInfo* struktAdr - wsk¿nik na instancjê struktury gdzie maja byc zapisane dane
    char polecenie[] - wskaŸnik na pierwszy element tablicy z poleceniem
    size_t dlugosc - dlugosc tablicy z poleceniem
    Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy z danymi pinów
    size_t iloscPinow - d³ugoœæ tablicy z danymi pinow (inaczej ilosc pinow)
Funkcja zwraca :
    1 - jeœli polecenie by³o nieprawid³owe
    0 - jeœli komenda by³a niew³aœciwa
U¿ywane funkcje:
    byte sprawdzNumerPinu(byte, Pin* , size_t),
Uwagi:
    Algorytm dzia³ania:
        Funkcja okreœla czy w podanym poleceniu na pierwszych pozycjach znajduje siê  numer pinu (funkcja obs³uguje numery 1 lub 2 cufrowe !) lub znak 'a'
        Jeœli jest to 'a' to w polu instancji struktury rodzajPolecenia zapisywana jest wartoœæ okreœlona makrem ODCZYTAJ WSZYSTKIE
        Jeœli numer pinu istnieje to w zale¿noœci od jego typu (PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE) sprawdzana jest poprawnoœæ polecenia wdg kryteriów:
        - piny analogowe i wejœcia cyfrowe mo¿na tylko odczytaæ (jako rodzajPolecenia zapisywany jest ODCZYTAJ_ANALOGOWY lub ODCZYTAJ_CYFROWY)
        - wyjœcia cyfrowe mo¿na odczytaæ i zmieniæ (ODCZYTAJ_CYFROWY albo ZMIEN_STAN CYFROWEGO)
    W przypadku zmiany stanu wyjœcia cyfrowego, po¿¹dany stan (1 lub 0) jesgt wpisywany w pole struktury nowyStan

*/
byte sprawdzKomende(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin wszystkiePiny[], size_t iloscPinow);

/* Pin* zmienStanPinu()
    Zmienia stan na wybranym wyjœciu cyfrowym (nie dokonuje aktualizacji jego stanu w tablicy wszystkiePiny). Zak³ada, ¿e zosta³o sprawdzone, czy pin taki istnieje i jest wyjœciem cyfrowym.
    Argumenty:
        Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy, w któej znajduje siê pin, którego stan ma ulec zmianie
        size_t iloscPinow - rozmiar powy¿szej 
        byte nrPinu - nr pinu, na którym ma byæ zmieniony stan
        byte nowyStan - po¿¹dany stan na okreœlonym wyjœciu
    Funkcja zwraca:
        (Pin *) wskaŸnik na element tablicy z pinami, którego stan zosta³ zmieniony
    U¿ywane funkcje:
        digitalWrite() - funkcja Arduino
*/
Pin* zmienStanPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu, byte nowyStan);

byte zwrocRzeczywistyNumerPinu(Pin wszystkiePiny[], size_t iloscPinow, byte numerWyboru);

byte zwrocNumerWyboru(Pin wszystkiePiny[], size_t iloscPinow, byte rzeczywistyNrPinu);

/*==== obsluga tablicy pinow (stany pinow cyfrowych) ====*/
/*void aktualizujTabeleStanow()
Opis:
    Zmienia wartoœæ stanu pinu zapisanego w tablicy wszystkiePiny (tj. zmienia wartoœæ zapisan¹ w tablicy, a nie t¹ "rzeczywist¹" - patrz: funkcja zmienStanPinu()); zak³ada, ¿e pin taki istnieje i jest cyfrowym wyjœciem
Argumenty:
    Pin* wszystkiePiny - wskaŸnik na pin (element w tablicy), którego zapisany w niej stan ma byæ zmieniony
Funkcja zwraca:
    -
U¿ywane funkcje:
    -
*/
void aktualizujStanPinu(Pin* wszystkiePiny, byte nowyStan);

/* ===== odcztywanie stanow pinow ==== */
/* byte zwrocStanPinuCyfrowego()
 Opis:
    Zwraca stan logiczny pinu odczytany funkcj¹ digitalRead() - w przypadku wejœcia - lub z tablicy wszystkiePiny - w przypadku wyjœcia; zak³ada, ¿e pin jest cyfrowy i znajduje siê w  tablicy
 Argumenty:
        Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy pinów
        size_t iloscPinow - rozmiar powy¿szej
        byte nrPinu - nr pinu którego stan logiczny ma zwróciæ
Funkcja zwraca:
    (byte) odczytany stan (HIGH albo LOW)
U¿ywane funkcje:
    (w zale¿noœci od tego, czy pin jest wejœciem czy wyjœciem)
    digtalRead() - funkcja Arduino 
*/
byte zwrocStanPinuCyfrowego(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* byte zwrocWartoscPinuAnalogowego()
 Opis:
      zwraca wartoœæ odczytan¹ funkcj¹ analogRead() jako procent z 0-5V (uzywa funkcji map()); zak³ada, ¿e pin znajduje siê w tablicy
 Argumenty:
        Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy pinów, w której znajduje siê
        size_t iloscPinow - iloœæ elementów w tablicy
        nrPinu - nr pinu którego wartoœæ ma byæ odczytana
Funkcja zwraca:
    (byte) odczyt z pinu analogowego przeskalowany pomiêdzy wartoœci 0-100
U¿ywane funkcje:
    funkcje Arduino:
    analogRead(),
    map()
*/
byte zwrocWartPinuAnalogowego(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* void odczytajWartosciPinowAnalogowych
Opis:
    !- zak³ada, ¿e tablica odczytaneWartosci ma minimum 6 wyrazów -!
    Odczytuje wybrane wartoœci odczytane z: POW_AMP_CUR_MON_PIN, PRE_AMP_CUR_MON_PIN, A_S_CUR_S_P_PIN, S_C_S_P_PIN, BASE_PLATE_TEMP_MON_PIN (w zale¿noœci od wartoœci zmiennej trybWyswietlacza - w zakresie 0-2), skaluje je do zakresu 0-100 i ich cyfry zapisuje w elementach tablicy odczytaneWartoœci
Argumenty:
     byte trybWyswietlacza - od jej wartoœci (0-2) zale¿y, które wartoœci zostan¹ wpisane do tablicy
     byte *odczytaneWartosci - wskaŸnik na pierwszy element tablicy odczytaneWartosci
Funkcja zwraca:
    -
U¿ywane funkcje:
    funkcje Arduino:
        analogRead(),
        map()
*/
void odczytajWartosciMonitorow(byte trybWyswietlacza, byte* odczytaneWartosci);

/* ==== wyswietlanie tekstowe ==== */

/*  void wyswietlStanyPinow()
 Opis:
    Wysy³a po³¹czeniem szeregowym stany wszystkich pinów zawartych w tablicy wszytkiePiny. Do odczytania tych stanów u¿ywa funkcji zwrocWartoscPinuAnalogowego() oraz zwrocWartoscPinuCyfrowego(). Zak³ada, ¿e komunnikacja szeregowa zosta³a zainicjowana
Argumenty
    Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy z zawaratymi informacjami o pinach
    size_t iloscPinow - rozmiar tej tablicy
Funkcja zwraca:
    -
U¿ywane funkcje:
    byte zwrocStanPinuAnalogowego(),
    byte zwrocStanPinuCyfrowego(),
    print(), println() - metody klasy Serial (Arduino)
*/
void wyswietlStanyWszystkichPinow(Pin wszystkiePiny[], size_t iloscPinow);

/* void wyswietlOpisPinu() 
Opis:
    Wysy³a po³¹czeniem szeregowym opis wybranego pinu zawarty w tablicy wszytkiePiny. . Zak³ada, ¿e komunnikacja szeregowa zosta³a zainicjowana oraz, ¿e pin znajduje siê w tej tablicy
Argumenty:
    Pin wszystkiePiny[] - wskaŸnik na pierwszy element tablicy z zawartym opisem pinu
    size_t iloscPinow - iloœæ elementów w tablicy
    byte nrPinu - numer pinu, którego opis ma byæ przes³any
Funkcja zwraca:
    -
U¿ywane funkcje:
    print() - metoda klasy Serial (Arduino)
*/
void wyswietlOpisPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu);

/* ==== ledy bledow ==== */
/*void obslugaLedowBledow
Opis:
    !- w przypadku tej funkcji musi zachodzic liczba elementow tablicy pinyBledow <= liczba elemntow tablicy ledNrPin -!
    Sprawdza czy na pinach spoœród tych zapisanych w tablicy pinyBledow nast¹pi³a zmiana od ostatniego sprawdzenia (wartoœci w tablicy poprzedniStanPinowBledow). Jesli tak by³o, to zmienia odpowiednio stan LEDów (o iloœci równej wartosci iloscPinowB³êdow) pod³¹czonych kolejno do  pocz¹tkowych pinów (znajdujacych sie w tablicy ledNrPin) ekspandera (mcp)
    Modyfikuje równie¿ tablicê poprzedniStanPinowBledow przypisuj¹c jej odczytane wartosci
    Zak³ada, ¿e obydwie tablice s¹ tych samych rozmiarów
Argumenty:
    Pin* pinyBledow - wskaŸnik do pierwszego elementu tablicy ze sprawdzanymi pinami
    byte* poprzedniStanPinowBledow - wskaŸnik do pierwszego elementu tablicy, z której brane s¹ stany porównawcze 
    byte iloscPinowBledow - iloœæ elementów w obydwu tablicach
    byte* ledNrPin - wskaŸnik na pierwszy element tablicy, gdzie znajduj¹ siê nr pinów ekspandera, do których pod³¹czone s¹ LEDy
    Adafruit_MCP23017& mcp - referencja na obiekt reprezentuj¹cy ekspander
Funkcja zwraca:
    -
U¿ywane funkcje:
    zwrocStanPinuCyfrowego(),
    digitalWrite() - metoda klasy Adafruit_MCP23017 wprowadzaj¹ca pin ekspandera w okreœlony stan
*/

void obslugaLedowBledow(Pin* pinyBledow, byte* poprzedniStanPinowBledow, byte iloscPinowBledow, byte* ledNrPin, Adafruit_MCP23017& mcp);


#endif
