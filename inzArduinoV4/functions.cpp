// 
// 
// 

#include "functions.h"


void wyswietlStanyPinowCyfrowych(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow) {
    for (size_t i = 0; i < iloscPinow; i++) {
        uSend((stanyPinowCyfrowych + i)->nrPinu);
        uSend(" ");
        uSendLn((stanyPinowCyfrowych + i)->stanPinu);
    }
}

/* wyswietlStanPinu
    wyswietla stan pinu
    argumenty:
        adres tablicy zawierajacej obecne stany pinow
        jej rozmiar
        nr pinu ktorego stan chcemy odczytac
*/
void wyswietlStanPinu(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (nrPinu == (stanyPinowCyfrowych + i)->nrPinu) {
            uSend("Pin:"), uSend(nrPinu), uSend(" Stan: "), uSendLn(stanyPinowCyfrowych->stanPinu);
            break;
        }
    }
}


byte sprawdzPrzedzial(byte liczba, PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow) {
    byte temp = 0;
    for (size_t i = 0; i < iloscPinow; i++) {
        if (liczba == (stanyPinowCyfrowych + i)->nrPinu) {
            temp = 1;
            /*
            uSend("sprawdzPrzedzial(): liczba iteracji: "); uSendLn(i + 1);
            */
            break;
            /* przerwij petle jesli znalazles */
        }
    }
    return temp;
}


byte sprawdzPolecenie(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow) {
    /*
    polecenieInfo tempInfo = {e, 0, 0}; // w przypadku zwracania struktury
    */
    byte type = 0; // 0 - read, 1 - write, 255 - error
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
    else {
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

    /* jesli nieodpowiedni numer pinu - nie ma go w tablicy */
    if (!sprawdzPrzedzial(tempPinNr, stanyPinowCyfrowych, iloscPinow)) {
        /*
        uSendLn("sprawdzPolecenie(): zla instrukcja");
        /*
        return tempInfo;
        */
        return 0;
    }
    /*
    uSend(tempIndeks); uSend(" "); uSendLn(tempIndeks < dlugosc);
    */

    /* read, write czy kolejny nie jest r albo w, lub nie ma dalej nic albo jest co innego wtedy error */

    if ((polecenie[tempIndeks] == 'r')) {
        type = 0;
        /*
        uSend("sprawdzPolecenie(): literka: "); uSendLn(polecenie[tempIndeks]);
        */
    }
    else if (polecenie[tempIndeks] == 'w') {
        type = 1;
        /*
        uSend("sprawdzPolecenie(): literka: "); uSendLn(polecenie[tempIndeks]);
        */
    }
    else {
        /*
        uSendLn("sprawdzPolecenie(): brak r lub w ");
        */
        /*
        uSendLn("sprawdzPolecenie(): zla instrukcja");
        */
        /*
        return tempInfo;
        */
        return 0;
    }


    tempIndeks++;

    /*jesli tryb write, nastepny znak to 0 lub 1 a kolejny to null to jest to prawidlowa instrukcja modyfikacji stanu pinu */
    if (((polecenie[tempIndeks] == '1') ||
        (polecenie[tempIndeks] == '0')) &&
        type == 1 && polecenie[tempIndeks + 1] == 0) {
        /*
        uSend("sprawdzPolecenie(): Nalezy zapisac "); uSend(polecenie[tempIndeks]); uSend(" na pinie nr "); uSendLn(tempPinNr);
        */
        struktAdr->rodzaj = w;
        struktAdr->nowyStan = polecenie[tempIndeks] - 48;
        struktAdr->nrPinu = tempPinNr;
        /* jesli tryb read, nastepny znak to null to jest to prawidlowa instrukcja odczytania stanu */
    }
    else if (type == 0 && polecenie[tempIndeks] == 0) {
        /*
        uSendLn("sprawdzPolecenie(): prawidlowe polecenie przeczytania ");
        */
        struktAdr->rodzaj = r;
        struktAdr->nowyStan = 0;
        struktAdr->nrPinu = tempPinNr;
    }
    else {
        /* w pozostalych przypadkach instrukcja jest nieprawidlowa */
            /*
            uSendLn("sprawdzPolecenie(): zla instrukcja");
            */
        return 0;
    }
    /*
    return tempInfo;
    */
    return 1;
}


PinCyfrowy* zmienStanPinu(PinCyfrowy stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu, byte nowyStan) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (nrPinu == (stanyPinowCyfrowych + i)->nrPinu) {
            digitalWrite(nrPinu, nowyStan);
            return (stanyPinowCyfrowych + i);
        }
    }
}


void aktualizujTabeleStanow(PinCyfrowy* stanyPinowCyfrowych, byte nowyStan) {
    stanyPinowCyfrowych->stanPinu = nowyStan;
}
