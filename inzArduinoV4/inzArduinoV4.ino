
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
        //{2, PIN_CYFROWY, 0, "led1"},
        //{3, PIN_CYFROWY, 0, "led2"},
        //{4, PIN_CYFROWY, 0, "led3"},
        //{5, PIN_CYFROWY, 0, "led4"},
        //{6, PIN_CYFROWY, 0, "led5"},
        //{A13, PIN_ANALOGOWY, 0, "A13"},
        //{A14, PIN_ANALOGOWY, 0, "A14"},
        //{A15, PIN_ANALOGOWY, 0, "A15"}

        {19, PIN_CYFROWY, 0, "Pulsed/CW mode select", OUTPUT},
        {18, PIN_CYFROWY, 0, "Laser disable", OUTPUT},
        {16, PIN_CYFROWY, 0, "Global Enable", OUTPUT},
        {15, PIN_CYFROWY, 0, "Alignment laser enable", OUTPUT},
        {14, PIN_CYFROWY, 0, "Laser emission gate", OUTPUT},

        {17, PIN_CYFROWY, 0, "Laser ready", INPUT},

        {2, PIN_CYFROWY, 0, "Power supply fault", INPUT},
        {3, PIN_CYFROWY, 0, "Reserved (GND?) fault", INPUT},
        {4, PIN_CYFROWY, 0, "Beam collimator fault", INPUT},
        {5, PIN_CYFROWY, 0, "Power-amp current fault", INPUT},
        {6, PIN_CYFROWY, 0, "Pre-amp currernt fault", INPUT},
        {7, PIN_CYFROWY, 0, "Base plate temp fault", INPUT},
        {8, PIN_CYFROWY, 0, "Seed laser temp fault", INPUT},

        {A0, PIN_CYFROWY, 0, "State select: 0", OUTPUT},
        {A1, PIN_CYFROWY, 0, "State select: 1", OUTPUT},
        {A2, PIN_CYFROWY, 0, "State select: 2", OUTPUT},
        {A3, PIN_CYFROWY, 0, "State select: 3", OUTPUT},
        {A4, PIN_CYFROWY, 0, "State select: 4", OUTPUT},
        {A5, PIN_CYFROWY, 0, "State select: 5", OUTPUT},
        {A6, PIN_CYFROWY, 0, "State select: 6", OUTPUT},
        {A7, PIN_CYFROWY, 0, "State select: 7", OUTPUT},

        {A8, PIN_ANALOGOWY, 0, "Base plate temp", INPUT},
        {A9, PIN_ANALOGOWY, 0, "Pre amp diode current", INPUT},
        {A10, PIN_ANALOGOWY, 0, "Power amp diode current", INPUT},

        {A11, PIN_ANALOGOWY, 0, "Active-State current set point", INPUT},
        {A12, PIN_ANALOGOWY, 0, "Simmer current set point", INPUT}
    
    };

    for (size_t i = 0; i < ILOSC_PINOW; i++) {
        if (stanyPinowCyfrowych[i].rodzajPinu == PIN_CYFROWY) {
            pinMode(stanyPinowCyfrowych[i].nrPinu, stanyPinowCyfrowych[i].inOut);
        }
   }

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
