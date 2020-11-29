
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

    Pin stanyPinowCyfrowych[ILOSC_PINOW] =
    {
        {2, PIN_CYFROWY, 0, "led1"},
        {3, PIN_CYFROWY, 0, "led2"},
        {4, PIN_CYFROWY, 0, "led3"},
        {5, PIN_CYFROWY, 0, "led4"},
        {6, PIN_CYFROWY, 0, "led5"},
        {A13, PIN_ANALOGOWY, 0, "A13"},
        {A14, PIN_ANALOGOWY, 0, "A14"},
        {A15, PIN_ANALOGOWY, 0, "A15"}
    };

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);

    /* wyswietl status poczatkowy */
    uSendLn("===\r\nStatus poczatkowy:");
    wyswietlStanyPinow(stanyPinowCyfrowych, ILOSC_PINOW);
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
        if (sprawdzPolecenie(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, stanyPinowCyfrowych, ILOSC_PINOW)) {
            /*
            uSendLn("=============================================");
            uSendLn("while(1): Informacje o otrzymanym poleceniu:");
            uSend("Rodzaj:");uSendLn(sprawdzonePolecenie.rodzaj);
            uSend("Pin:");uSendLn(sprawdzonePolecenie.nrPinu);
            uSend("Wartosc:");uSendLn(sprawdzonePolecenie.nowyStan);
            uSendLn("=============================================");
            */
            if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
                //uSend("while(1): pin cyfrowy zapisz nowyStan");
                //uSendLn(sprawdzonePolecenie.nowyStan);
                aktualizujTabeleStanow(zmienStanPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);
               // for (size_t i = 0; i < ILOSC_PINOW; i++) {
               //     uSend(i);uSend(" ");uSendLn(stanyPinowCyfrowych[i].stanPinu);
               // }
            } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_CYFROWY) {
                uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu);uSend(" ");
                wyswietlOpisPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
                uSend(" ");uSend(" Stan: ");
                uSendLn(podajStanPinuCyfrowego(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));
            } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_ANALOGOWY){
                uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu); uSend(" ");
                wyswietlOpisPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
                uSend(" "); uSend(" Wartosc: ");
                uSendLn(podajStanPinuAnalogowego(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));
            } else {
                wyswietlStanyPinow(stanyPinowCyfrowych, ILOSC_PINOW);
            }

        } else {
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
