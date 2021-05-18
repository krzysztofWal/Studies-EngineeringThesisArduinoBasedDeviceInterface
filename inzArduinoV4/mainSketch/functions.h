// functions.h

#ifndef _FUNCTIONS_h
#define _FUNCTIONS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "typedefs.h"
#include "LCD5110_Graph.h"

#include "avr/pgmspace.h"
//#include "libraries/Adafruit-MCP23017-Arduino-Library-master/Adafruit_MCP23017.h"
#include "Adafruit_MCP23017.h"


/* ==== poczatkowe ustawienia peryferiow ==== */
/* void przyciskTimerUstawienie()
Opis :
   voidUstawia rejestry 16-to bitowego TIMER1 : TCCR1A, TCCR1B, OCR1A
    Wybór preskalera 64 oraz wartości porównawczej
    Timer uzywany jest do usuwania drgan mechanicznych stykow przycisku
Argumenty :
    -
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przyciskTimerUstawienie();
/* void czestOdswEkranuTimerUstawienie()
Opis :
    Ustawia rejestry 8-bitowego TIMER2 : TCCR2A, TCCR2B, OCR2A
    Wybór preskalera 1024 oraz wartości porównawczej 254
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
/* void czestOdswEkranuTimerUstawienie()
Opis :
    Ustawia rejestry: PCMSK0, PCICR
    Wykorzystuje tzw. Pin Change Interrupt, generowany na grupie pinów (PCI0), kiedy dowolny z nich zmieni stan. 
    Wybrany do uruchomienia tego przerwania jest jednak tylko pin 53, do którego podłączony jest przycisk. Nacisniecie przycisku powoduje wystapienie przerwania PCINT0
Argumenty :
    -
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przerwaniePrzyciskUstawienie();

/* ==== obsluga wyswietlacza ====*/
/* void wyswietl()
Opis :
    Wyswietla opisy wyświetlanych wartości dla danego trybu wyświetlania, określonego przez przekazywany parametr trybWyswietlacza
Argumenty :
    byte trybWyswietlacza - na jego podstawie wybierane są wyświetlane opisy
    uint8_t *font - wskaźnik do pierwszego elementu tablicy użytej do wyświetlania czcionki z pliku DefaultFonts.c znajdującego sie w folderzeLCD5110_Graph
    LCD5110& wyswietlacz - referencja do obiektu reprezentującego wyświetlacz
Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewnętrznej biblioteki LCD5110_Graph,
    opisane w dokumentacji biblioteki znajdujacej sie w folderze projektowym:
        clrScr(),
        setFont(),
        print(),
        update()
*/
void wyswietl(byte trybWyswietlacza, LCD5110& wyswietlacz, uint8_t* font);
/* void przelaczTrybWyswietlacza()
Opis :
    Zmienia wartość zmiennej trybWyswietlacza w zale�no�ci od przekazanej warto�ci zmiennej wyborZrodlaZasilania (1 albo 0)
    wyborZrodlaZasilania == 1 : tryby ..2->0->1->2->0...
    wyborZrodlaZasilania  == 0 : tryby ..0->2->0.. lub 1->0->2->0->2.. (w zale�no�ci od początkowej wartości)
Argumenty :
    byte &trybWyswietlacza - modyfikowana zmienna
    byte wyborZrodlaZasilania - kryterium cyklu zmiany
Funkcja zwraca:
    -
Uzywane funkcje:
    -
*/
void przelaczTrybWyswietlacza(byte& trybWyswietlacza, byte wyborZrodlaZasilania);
/* void aktualizujWyswietlaneWartosci(),
Opis :
    w zależności od zmiennej trybWyswietlacza (pomiędzy 0 - 2) odczytuje wartosci z tablicy, do której wskaźnik jest przekazywanym parametrem i wyświetla je na wyświetlaczu. 
Argumenty :
    byte* odczytaneWartosci - wskaźnik do pierwszego elementu tablicy, z której odczytywane są wartości
    LCD5110& wyswietlacz - referencja do obiektu reprezentującego wyświetlacz
    uint8_t *font - wskaźnik do pierwszego elementu tablicy z użytą do wyświetlania czcionką z pliku DefaultFonts.c znajdująego sie w folderzeLCD5110_Graph
    byte trybWyswietlacza - na jego podstawie wybierany jest wyświetlnay zestaw wartości
    Funkcja zwraca:
    -
Uzywane funkcje:
    metody klasy LCD5110 z zewnętrznej biblioteki LCD5110_Graph,
    opisane w dokumentacji biblioteki znajdujacej sie w folderze projektowym:
        setFont(),
        printNumI(),
        update()
*/
void aktualizujWyswietlaneWartosci(byte* odczytaneWartosci, LCD5110& wyswietlacz, byte trybWyswietlacza, uint8_t* font);

/*==== obsługa komend i funkcje pomocnicze====*/
/* void obsluzKomende()
Opis:
	Sprawdza, czy komenda przechowywana w tablicy bufor jest prawdiłową komendą. Na początku sprawdza czy jest komendą "d", jeśli tak to ustawia w stan wysoki wyjście LASER_DISABLE_PIN i sygnalizuje to odpowiednią diodą. Jeśli nie, to sprawdza czy komenda jest prawidłową komendą za pomocą funkcji sprawdzKomende().
	Jesli tak to wykonuje odpowiednie kroki, czyli wyświetla stan pinów lub zmienia stan wybranego. Jeśli stan  na wyjściu jest zmieniany na pinie LASER_EMITION_GATE_PIN lub LAS_DISABLE_PIN to nowy stan jest sygnalizowany odpowiednim LEDem podłączonym do ekspandera mcp.
Argumenty:
	Pin *pinyLasera - wskaźnik na pierwszy element tablicy z danymi pinów
	byte iloscPinow - ilosc elementów w powyższej
	char *bufor - wskaźnik na pierwszy element tablicy w któej przechowywana jest komenda
	Adafruit_MCP23017& mcp - referencja obiektu reprezentującego ekspander
Funkcja zwraca:
	-
Używane funkcje:
	print(), println() - metody obiektu Serial (Arduino),
    digitalWrite() - metoda klasy mcp,
	sprawdzKomende(),
	wyswietlOpisPinu(),
	zwrocStanPinuCyfrowego(),
	wyswietlStanyWszystkichPinow(),
 */
void obsluzKomende(Pin *pinyLasera, byte iloscPinow, Pin *pinyCharakterystyk, byte iloscPinowCharakterystyk, char *bufor, Adafruit_MCP23017& mcp);
/*  byte sprawdzNumerPinu()
Opis :
    Sprawdza czy dana liczba jest numerem "opisowym" jednego z pinow zapisanych w tablicyPinow i jeśli tak, to zwraca typ tego pinu
Argumenty:
    byte liczba - "opisowy" numer pinu
    Pin pinyLasera[] - wskaźnik do tablicy zawieraj�cej sprawdzane piny
    size_t - ilość elementów w tablicy pinyLasera
Funkcja zwraca:
    (byte) wartość z tych określoneych makrami:
    PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE albo PIN_NIE_ISTNIEJE
Uzywane funkcje:
    -
*/
byte sprawdzNumerPinu(byte liczba, Pin pinyLasera[], size_t iloscPinow);

/* byte sprawdzKomende()
 Opis:
    zapisuje dane odczytane z komendy zapisanej w tablicy polecenie[] w instancji struktury typu Polecenie Info, której adres jest przekazywany
    Aby polecenie było odczytane jako poprawne nie może zawierać żadnych innych znaków (spacji, tabulatury itp.)
    Możliwe polecenia:
        ar - odczytaj wartości wszystkich pinów
        NwX, gdzie N to numer pinu a X to '1' lub '0' - ustaw wyjście pinu cyfrowego
        Nr - odczytaj wartość N-tego pinu

Argumenty :
    PolecenieInfo* struktAdr - wskźnik na instancję struktury gdzie maja byc zapisane dane
    char polecenie[] - wskażnik na pierwszy element tablicy z poleceniem
    size_t dlugosc - dlugosc tablicy z poleceniem
    Pin pinyLasera[] - wskaźnik na pierwszy element tablicy z danymi pinów
    size_t iloscPinow - długość tablicy z danymi pinow (inaczej ilosc pinow)
Funkcja zwraca :
    1 - jeśli polecenie prawidłowe
    0 - jeśli polecenie było niewłaściwe
Używane funkcje:
    byte sprawdzNumerPinu(byte, Pin* , size_t),
	byte zwrocRzeczywistyNumerPinu(Pin *, size_t, byte)
Uwagi:
    Algorytm działania:
        Funkcja określa czy w podanym poleceniu na pierwszych pozycjach znajduje się  numer pinu (funkcja obsługuje numery 1 lub 2 cyfrowe !) lub znak 'a'
        Jeśli numer pinu istnieje (lub zamiast niego jest znak 'a') to w zależności od jego typu (PIN_ANALOGOWY, PIN_CYFROWY_WYJSCIE, PIN_CYFROWY_WEJSCIE, WSZYSTKIE_PINY) sprawdzana jest poprawność polecenia wdg kryteriów:
        - piny analogowe, wejścia cyfrowe i wszystkie piny jednocześnie można tylko odczytać (jako rodzajPolecenia zapisywany jest ODCZYTAJ_ANALOGOWY, ODCZYTAJ_CYFROWY lub ODCZYTAJ_WSZYSTKIE)
        - wyjścia cyfrowe można odczytać i zmienić (ODCZYTAJ_CYFROWY albo ZMIEN_STAN CYFROWEGO)
	Nr "rzeczywisty" pinu zapisywany jest w polu struktury nrPinu
    W przypadku zmiany stanu wyjścia cyfrowego, pożądany stan (1 lub 0) jt wpisywany w pole struktury nowyStan

*/
byte sprawdzKomende(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin pinyLasera[], size_t iloscPinow);

/* Pin* zmienStanPinu()
Opis:
    Zmienia stan na wybranym wyjściu cyfrowym (nie dokonuje aktualizacji jego stanu w tablicy pinyLasera). Zakłada, że zostało sprawdzone, czy pin taki istnieje i jest wyjściem cyfrowym.
 Argumenty:
    Pin pinyLasera[] - wskaźnik na pierwszy element tablicy, w której znajduje się pin, którego stan ma ulec zmianie
    size_t iloscPinow - rozmiar powyższej 
    byte rzeczywistyNrPinu - "rzeczywisty" nr pinu, na którym ma być zmieniony stan
    byte nowyStan - pożądany stan na określonym wyjściu
 Funkcja zwraca:
    (Pin *) wskaźnik na element tablicy z pinami, którego stan został zmieniony
Używane funkcje:
    digitalWrite() - funkcja Arduino
*/
Pin* zmienStanPinu(Pin pinyLasera[], size_t iloscPinow, byte rzeczywistyNrPinu, byte nowyStan);

/*byte zwrocRzeczywistyNumerPinu()
Opis:
	Zwraca "rzeczywisty" nr pinu, którego "opisowy" nr przyjmuje.
	Jeśli taki pin nie istnieje zwraca 255.
Argumenty:
	Pin pinyLasera[] - wskaźnik na pierwszy element tablicy, w której znajduje się pin
	size_t iloscPinow - rozmiar powyższejm albo nr indeksu znaku następującego po ostatnim, jaki chcemy brać pod uwagę
	byte numerWyboru - "opisowy" nr pinu
Funkcja zwraca:
	(byte) - "rzeczywisty" nr pinu
Używane funkcje:
	-
*/
byte zwrocRzeczywistyNumerPinu(Pin pinyLasera[], size_t iloscPinow, byte numerWyboru);

/*byte zwrocNumer Wyboru()
Opis:
	Zwraca "opisowy" nr pinu, którego "rzeczywisty" nr przyjmuje.
	Jeśli taki pin nie istnieje zwraca 255.
Argumenty:
	Pin pinyLasera[] - wskaźnik na pierwszy element tablicy, w której znajduje się pin
	size_t iloscPinow - rozmiar powyższej
	byte rzeczywistyNrPinu - "rzeczywisty" nr pinu
Funkcja zwraca:
	(byte) - "opisowy" nr pinu
Używane funkcje:
	-
*/
byte zwrocNumerWyboru(Pin pinyLasera[], size_t iloscPinow, byte rzeczywistyNrPinu);

/*int konwersjaCharToInt()
Opis: konwertuje znaki w przekazanym buforze (argument - wskaźnik do bufora) na integer
zwraca wartość liczby, kiedy znajduje się ona w zakresie [0 , maksNumber)
!- rozpatruje znaki od PIERWSZEGO indeksu, pomija zerowy -!

Argumenty:
	char *bufor - wskaźnik na zerowy wyraz bufora,
	byte rozmiarBufora
	byte maksNumber - maksymalna wartość liczbyw buforze która jest dopuszczalna (przy przekroczeniu funkcja zwraca -1)
Fukcja zwraca
	(int) liczbę zapisaną w buforze
Używane funkcje:
	-
*/
int8_t konwersjaCharInt(char *bufor, byte rozmiarBufora, byte maksNumer);

/*void ustawCharakt()
Opis: Ustawia stany pinow zawartych w tablicy pinyCharakterystyk przekazywanej do funkcji jako wskaźnik, rowniez aktualizując ich stany w tablicy pinyLaser[],
!- Zakłada odpowiednie ułożenie pinów w tablicy pinyCharakterystyk, tak aby liczba była odpowiednio "przetłumaczona" na binarną na wyjściach -!
Argumenty: 
	int number - numer jaki ma zostać ustawiony "binarnie"na pinach
	Pin pinyCharakterystyk - wskaźnik do pierweszego (właściwie zerowego) elementu tablicy z pinami, które ulegną zmianie
	byte iloscPinowCharakt - rozmiar powyższej
	Pin pinyLasera - wskaźnik do tablicy z wszystkimi  pinami
	byte iloscPinowLasera - rozmiar powyższej
Funkcja zwraca:
	-
Używane funkcje:
	aktualizujStanPinu(),
	zmienStanPinu()
*/
void ustawCharakt(int number, Pin pinyCharakterystyk[], byte iloscPinowCharakt, Pin pinyLasera[], byte iloscPinowLasera);

/*==== obsluga tablicy pinow (przechowującej stany pinów cyfrowych) ====*/
/*void aktualizujTabeleStanow()
Opis:
    Zmienia wartość stanu pinu zapisanego w tablicy pinyLasera (tj. zmienia wartość zapisaną w tablicy, a nie tą "rzeczywistą" - patrz: funkcja zmienStanPinu()); zakłada, że pin taki istnieje i jest cyfrowym wyjściem
Argumenty:
    Pin* pinyLasera - wskaźnik na pin (element w tablicy), którego zapisany w niej stan ma być zmieniony
	byte nowyStan - pożądany stan
Funkcja zwraca:
    -
Używane funkcje:
    -
*/
void aktualizujStanPinu(Pin* pinyLasera, byte nowyStan);

/* ===== odcztywanie stanow pinow ==== */
/* byte zwrocStanPinuCyfrowego()
 Opis:
    Zwraca stan logiczny pinu odczytany funkcją digitalRead() - w przypadku wejścia - lub z tablicy pinyLasera - w przypadku wyjścia; zakłada, że pin jest cyfrowy i znajduje się w  tablicy
 Argumenty:
        Pin pinyLasera[] - wskaźnik na pierwszy element tablicy pinów
        size_t iloscPinow - rozmiar powyższej
        byte nrPinu - nr pinu którego stan logiczny ma być zwrócony
Funkcja zwraca:
    (byte) odczytany stan (HIGH albo LOW)
Używane funkcje:
    (w zależności od tego, czy pin jest wejściem czy wyjściem)
    digtalRead() - funkcja Arduino 
*/
byte zwrocStanPinuCyfrowego(Pin pinyLasera[], size_t iloscPinow, byte nrPinu);

/* byte zwrocWartoscPinuAnalogowego()
 Opis:
      zwraca wartość odczytaną funkcją analogRead() jako procent z 0-5V (uzywa funkcji map()); zakłada, że pin znajduje się w tablicy
 Argumenty:
        Pin pinyLasera[] - wskaźnik na pierwszy element tablicy pinów, w której znajduje się pożądany pin
        size_t iloscPinow - ilość elementów w tablicy
        nrPinu - nr pinu którego wartość ma być odczytana
Funkcja zwraca:
    (byte) odczyt z pinu analogowego przeskalowany pomiędzy wartości 0-100
Używane funkcje:
    funkcje Arduino:
    analogRead(),
    map()
*/
byte zwrocWartPinuAnalogowego(Pin pinyLasera[], size_t iloscPinow, byte nrPinu);

/* void odczytajWartosciPinowAnalogowych
Opis:
    !- zakłada, że tablica odczytaneWartosci ma minimum 6 elementów -!
    Odczytuje wybrane wartości odczytane z: POW_AMP_CUR_MON_PIN, PRE_AMP_CUR_MON_PIN, A_S_CUR_S_P_PIN, S_C_S_P_PIN, BASE_PLATE_TEMP_MON_PIN (w zależności od wartości zmiennej trybWyswietlacza - w zakresie 0-2), skaluje je do zakresu 0-100 i ich cyfry zapisuje w elementach tablicy odczytaneWartości
		[0] - setki
 		[1] - dziesiątki
		[2] - jedności
		[3] - setki
		[4] - dziesiątki
		[5] - jedności
Argumenty:
     byte trybWyswietlacza - od jej wartości (0-2) zależy, które wartości zostaną wpisane do tablicy
     byte *odczytaneWartosci - wskaźnik na pierwszy element tablicy odczytaneWartosci
Funkcja zwraca:
    -
Używane funkcje:
    funkcje Arduino:
        analogRead(),
        map()
*/
void odczytajWartosciMonitorow(byte trybWyswietlacza, byte* odczytaneWartosci);

/* ==== wyswietlanie tekstowe ==== */

/* void wyswietlOpisPinu() 
Opis:
    Wysyła połączeniem szeregowym opis wybranego pinu zawarty w tablicy wszytkiePiny. Zakłada, że komunnikacja szeregowa została zainicjowana oraz, że pin znajduje się w tej tablicy
Argumenty:
    Pin pinyLasera[] - wska�nik na pierwszy element tablicy z zawartym opisem pinu
    size_t iloscPinow - ilość elementów w tablicy
    byte nrPinu - "rzeczywsuty" numer pinu, którego opis ma być przes�any
Funkcja zwraca:
    -
Używane funkcje:
    print() - metoda klasy Serial (Arduino)
*/
void wyswietlOpisPinu(Pin pinyLasera[], size_t iloscPinow, byte nrPinu);


/*  void wyswietlOpisAnalogowego()
 Opis:
    Wysyła połączeniem szeregowym sformatowany prompt o wartosci odczytanej na pinie analogowym
    Pin pinyLasera[] - wskaźnik na pierwszy element tablicy z zawaratymi informacjami o pinach
    size_t iloscPinow - rozmiar tej tablicy
    const PolecenieInfo &sprawdzonePolecenie - referencja do struktury zawierającej informacje o pinie do wyświetlenia
Funkcja zwraca:
    -
Używane funkcje:
    zwrocWartPinuAnalogowego(),
    zwrocNumerWyboru(),
    wyswietlOpisPinu(),
    print(), println() - metody obiektu Serial (Arduino)
*/
void wyswietlOpisAnalogowego(Pin pinyLasera[], size_t iloscPinow, const PolecenieInfo &sprawdzonePolecenie);

/*  void wyswietlOpisCyfrowego()
 Opis:
    Wysyła połączeniem szeregowym sformatowany prompt o stanie pinu cyfrowego
    Pin pinyLasera[] - wskaźnik na pierwszy element tablicy z zawaratymi informacjami o pinach
    size_t iloscPinow - rozmiar tej tablicy
    const PolecenieInfo &sprawdzonePolecenie - referencja do struktury zawierającej informacje o pinie do wyświetlenia
Funkcja zwraca:
    -
Używane funkcje:
    byte zwrocStanPinuCyfrowego(),
    zwrocNumerWyboru(),
    wyswietlOpisPinu();
    print(), println() - metody obiektu Serial (Arduino)
*/
void wyswietlOpisCyfrowego(Pin pinyLasera[], size_t iloscPinow, const PolecenieInfo &sprawdzonePolecenie);

/*  void wyswietlStanyPinow()
 Opis:
    Wysyła połączeniem szeregowym stany wszystkich pinów zawartych w tablicy wszytkiePiny. Do odczytania tych stanów używa funkcji zwrocWartoscPinuAnalogowego() oraz zwrocWartoscPinuCyfrowego(). Zakłada, że komunikacja szeregowa została zainicjowana
Argumenty
    Pin pinyLasera[] - wskaźnik na pierwszy element tablicy z zawaratymi informacjami o pinach
    size_t iloscPinow - rozmiar tej tablicy
Funkcja zwraca:
    -
Używane funkcje:
    byte zwrocStanPinuAnalogowego(),
    byte zwrocStanPinuCyfrowego(),
    print(), println() - metody obiektu Serial (Arduino)
*/
void wyswietlStanyWszystkichPinow(Pin pinyLasera[], size_t iloscPinow);

/* ==== ledy bledow ==== */
/*void obslugaLedowBledow()
Opis:
    !- w przypadku tej funkcji musi zachodzic liczba elementów tablicy pinyBledow <= liczba elementów tablicy ledNrPin -!
    Sprawdza czy na pinach spośród tych zapisanych w tablicy pinyBledow nastąpiła zmiana od ostatniego sprawdzenia (wartości w tablicy poprzedniStanPinowBledow). Jesli tak było, to zmienia odpowiednio stan LEDów (októych jest tyle ile wynosi wartość iloscPinowBłędów) podłączonych kolejno do  początkowych pinów (znajdujacych sie w tablicy ledNrPin) ekspandera (mcp)
    Modyfikuje również tablice poprzedniStanPinowBledow przypisując jej odczytane wartosci
    Zakłada, że obydwie tablice są tych samych rozmiarów
Argumenty:
    Pin* pinyBledow - wskaźnik do pierwszego elementu tablicy ze sprawdzanymi pinami
    byte* poprzedniStanPinowBledow - wskaźnik do pierwszego elementu tablicy, z której brane są stany porównawcze 
    byte iloscPinowBledow - ilość elementów w obydwu tablicach
    byte* ledNrPin - wskaźnik na pierwszy element tablicy, gdzie znajdują się nry pinów ekspandera, do których podłączone są LEDy
    Adafruit_MCP23017& mcp - referencja na obiekt reprezentujący ekspander
Funkcja zwraca:
    -
Używane funkcje:
    zwrocStanPinuCyfrowego(),
    digitalWrite() - metoda klasy Adafruit_MCP23017 wprowadzająca pin ekspandera w określony stan
Uwagi:
	!- w przypadku tej funkcji musi zachodzic liczba elementów tablicy pinyBledow <= liczba elementów tablicy ledNrPin -!
	 Modyfikuje tablicę poprzedniStanPinowBledow przypisując jej odczytane wartosci
*/
void obslugaLedowBledow(Pin* pinyBledow, byte* poprzedniStanPinowBledow, byte iloscPinowBledow, byte* ledNrPin, Adafruit_MCP23017& mcp);


#endif
