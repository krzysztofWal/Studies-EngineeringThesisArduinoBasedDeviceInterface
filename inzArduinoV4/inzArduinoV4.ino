
#include "typedefs.h"
#include "functions.h"


void setup()
{
    Serial.begin(9600);

    pinMode(12, OUTPUT);
    Serial.println("Hello world!");

    char polecenie[WIELKOSC_BUFORA_SERIAL];
    //unsigned int indeks = 0;
    PolecenieInfo sprawdzonePolecenie;

    PinCyfrowy stanyPinowCyfrowych[ILOSC_PINOW_CYFROWYCH] =
    {
        {2, 0},
        {3, 0},
        {4, 0},
        {5, 0},
        {6, 0}
    };

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    /* wyswietl status poczatkowy */
    uSendLn("===\nStatus poczatkowy:");
    wyswietlStanyPinowCyfrowych(stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH);
    uSendLn("===");

    /*
        uSendLn(sprawdzPrzedzial(0, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(1, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(2, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(3, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(4, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(5, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn(sprawdzPrzedzial(6, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH));
        uSendLn();
    */

    while (1) {


        uSendLn("Wpisz komende: ");

        /* czekaj na dane z bufora */
        while (Serial.available() < 1) {
        }

        Serial.readBytesUntil('\r', polecenie, WIELKOSC_BUFORA_SERIAL);

        for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
            uSend(polecenie[i]);
        }
        uSendLn();

        /* wpisana instrukcja jest prawidlowa */
        if (sprawdzPolecenie(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH)) {
            /*
            uSendLn("=============================================");
            uSendLn("while(1): Informacje o otrzymanym poleceniu:");
            uSend("Rodzaj:");uSendLn(sprawdzonePolecenie.rodzaj);
            uSend("Pin:");uSendLn(sprawdzonePolecenie.nrPinu);
            uSend("Wartosc:");uSendLn(sprawdzonePolecenie.nowyStan);
            uSendLn("=============================================");
            */
            if (sprawdzonePolecenie.rodzaj == w) {
                aktualizujTabeleStanow(zmienStanPinu(stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);
            }
            else if (sprawdzonePolecenie.rodzaj == r) {
                wyswietlStanPinu(stanyPinowCyfrowych, ILOSC_PINOW_CYFROWYCH, sprawdzonePolecenie.nrPinu);
            }

        }
        else {
            uSendLn("Nieprawidlowa instrukcja");
        }

        /* wyczysc tablice z poleceniem */
        for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
            polecenie[i] = 0;
        }
    }

}

void loop()
{
}
