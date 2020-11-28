#include "functions.h"

void wyswietlStanyPinow(Pin stanyPinowCyfrowych[], size_t iloscPinow) {
    for (size_t i = 0; i < iloscPinow; i++) {
        uSend((stanyPinowCyfrowych + i)->nrPinu);
        uSend(" ");
        if ((stanyPinowCyfrowych + i)->rodzajPinu == PIN_ANALOGOWY) {
            uSendLn("A ");
        } else {
            uSend("D "); uSendLn((stanyPinowCyfrowych + i)->stanPinu);
        }
    }
}

void wyswietlStanPinuAnalogowego(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (nrPinu == (stanyPinowCyfrowych + i)->nrPinu) {
            uSend("Pin:"), uSend(nrPinu), uSend(" Stan: "), uSendLn(stanyPinowCyfrowych->stanPinu);
            break;
        }
    }
}

byte sprawdzNumerPinu(byte liczba, Pin stanyPinowCyfrowych[], size_t iloscPinow) {
    byte temp = 0;
    for (size_t i = 0; i < iloscPinow; i++) {
        if (liczba == (stanyPinowCyfrowych + i)->nrPinu) {
            temp = (stanyPinowCyfrowych + i)->rodzajPinu;
            /*
            uSend("sprawdzPrzedzial(): liczba iteracji: "); uSendLn(i + 1);
            */
            break;
            /* przerwij petle jesli znalazles */
        }
    }
    return temp;
}

byte sprawdzPolecenie(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin stanyPinowCyfrowych[], size_t iloscPinow) {
    /*
    polecenieInfo tempInfo = {e, 0, 0}; // w przypadku zwracania struktury
    */

    size_t tempIndeks = 0;
    byte tempPinNr = 0;

    /* sprawdz ile cyfr ma pierwszy numer */
    while (*(polecenie + tempIndeks) > 47 && *(polecenie + tempIndeks) < 58) {
        tempIndeks++;
    }

    /*
    uSend("sprawdzPolecenie(): dlugosc pierwszego numeru: "); uSendLn(tempIndeks);
    */

    /*policz jaki to numer, najwiekszy mozliwy to 2-cyfrowy */
    if (tempIndeks == 2) {
        tempPinNr += *(polecenie + 1) - 48 + (*polecenie - 48) * 10;
        /*
        uSend("sprawdzPolecenie(): numer pinu"); uSend(tempPinNr);uSendLn();
        */
    }
    else if (tempIndeks == 1) {
        /*
        uSend("sprawdzPolecenie(): "); uSend(*polecenie - 48);uSendLn();
        */
        tempPinNr += (*polecenie - 48);
    }
    else { /* temp indeks > 2 lub temp indeks == 0*/
        /* podany pin jest nieodpowiedni - za duzy numer lub brak numeru*/
        /*
        uSendLn("sprawdzPolecenie(): zla instrukcja");
        /*
        return tempInfo;
        */
        /*
        if (tempIndeks == 0 && ) {
            tempPinNr = 255;
        } else {
        */
        return 0; /* numer pinu 3-cyfrowy lub wiekszy - nieodpowiedni */
    }

    byte analogowyCzyCyfrowy = sprawdzNumerPinu(tempPinNr, stanyPinowCyfrowych, iloscPinow);

    switch (analogowyCzyCyfrowy) {
    case 0:
        /* nie ma takiego pinu */
        return 0;
    case PIN_CYFROWY:
        /*pin jest cyfrowy, mozna wiec dokonac operacji zapisu i odczytu */
        /* nastepny znak to 0 lub 1 za ktorym nastepuje NULL - prawidlowa operacja zapisu*/
        if (polecenie[tempIndeks] == 'w') {
            if (((polecenie[tempIndeks + 1] == '1') ||
                    (polecenie[tempIndeks + 1] == '0')) &&
                    polecenie[tempIndeks + 2] == 0) {
                struktAdr->rodzajPolecenia = ZMIEN_STAN_CYFROWEG0;
                struktAdr->nowyStan = polecenie[tempIndeks + 1] - 48;
                struktAdr->nrPinu = tempPinNr;
                return 1;
            }
        }
    case PIN_ANALOGOWY:
        /*pin jest analogowy, mozna wiec dokonac tylko operacji odczytu*/
        /*nastepny znak to null, prawidlowa instrukcja odczytu*/

        //uSend("sprawdzPolecenie(): pin analogowy, zmienna analogowyCzyCyfrowy: ");
        //uSendLn(analogowyCzyCyfrowy);

        if ((polecenie[tempIndeks] == 'r')) {
            if (polecenie[tempIndeks + 1] == 0) {
                struktAdr->rodzajPolecenia = (analogowyCzyCyfrowy == PIN_ANALOGOWY) ? ODCZYTAJ_ANALOGOWY : ODCZYTAJ_CYFROWY;
                struktAdr->nowyStan = 0;
                struktAdr->nrPinu = tempPinNr;
                return 1;
            }
        } else {
            return 0;
        }
    }

}

Pin* zmienStanPinu(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu, byte nowyStan) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (nrPinu == (stanyPinowCyfrowych + i)->nrPinu) {
            digitalWrite(nrPinu, nowyStan);
            return (stanyPinowCyfrowych + i);
        }
    }
}

void aktualizujTabeleStanow(Pin* stanyPinowCyfrowych, byte nowyStan) {
    stanyPinowCyfrowych->stanPinu = nowyStan;
}