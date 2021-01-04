#include "functions.h"


void wyswietl(byte trybWyswietlacza, LCD5110& wyswietlacz, uint8_t *font) {
    wyswietlacz.clrScr();
    wyswietlacz.setFont(font);
    switch (trybWyswietlacza) {
    case 0:
        wyswietlacz.print("POW-AMP MONIT", CENTER, 0);
        wyswietlacz.print("PRE-AMP MONIT", CENTER, 25);
        break;
    case 1:
        wyswietlacz.print("POW-AMP S. P.", CENTER, 0);
        wyswietlacz.print("PRE-AMP C. P.", CENTER, 25);
        break;
    case 2:
        wyswietlacz.print("B-TEMP MONIT", CENTER, 0);
        break;
    default:
        wyswietlacz.print("Display error", CENTER, 20);
        break;
    }
    wyswietlacz.update();
}

void setNext(byte& trybWyswietlacza) {
    if (trybWyswietlacza < 2) {
        trybWyswietlacza++;
    }
    else {
        trybWyswietlacza = 0;
    }
}

void wyswietlStanyPinow(Pin stanyPinowCyfrowych[], size_t iloscPinow) {
    for (size_t i = 0; i < iloscPinow; i++) {
        uSend((stanyPinowCyfrowych + i)->nrPinu);
        uSend(" ");
        if ((stanyPinowCyfrowych + i)->rodzajPinu == PIN_ANALOGOWY) {
            uSend("A "); uSend(podajStanPinuAnalogowego(stanyPinowCyfrowych, iloscPinow, (stanyPinowCyfrowych + i)->nrPinu)); uSend("/100 V ");
        } else {
            uSend("D "); uSend(podajStanPinuCyfrowego(stanyPinowCyfrowych, iloscPinow, (stanyPinowCyfrowych + i)->nrPinu));
        }
        uSend(" ");
        uSend((stanyPinowCyfrowych + i)->opisPinu);
        uSendLn();
    }
}

byte podajStanPinuCyfrowego(Pin *stanyPinowCyfrowych, size_t iloscPinow, byte podanyNrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (podanyNrPinu == (stanyPinowCyfrowych + i)->nrPinu) {
            /*
            uSend("podajStanPinuCyfrowego() ");uSend(" ");uSend(podanyNrPinu); uSendLn((stanyPinowCyfrowych + i)->nrPinu);
            uSendLn((stanyPinowCyfrowych-+ 1)->stanPinu);
            */
            if ((stanyPinowCyfrowych + i)->inOut == INPUT) {
                return digitalRead((stanyPinowCyfrowych + i)->nrPinu);
            }
            else {
                return (stanyPinowCyfrowych + i)->stanPinu;
            }
        }
    }
}

long podajStanPinuAnalogowego(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu) {
    return map(analogRead(nrPinu), 0, 1023, 0, 500);
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
    byte tempPinNr = 0; //255 - wszystkie piny
    byte analogowyCzyCyfrowy = 3;

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
        if (tempIndeks == 0 && polecenie[0] == 'a') { /* pierwszy wyraz to 'a'*/
            tempPinNr = WSZYSTKIE_PINY;
            tempIndeks++;
        } else {
           return 0; /* numer pinu 3-cyfrowy lub wiekszy - nieodpowiedni lub brak numeru a pierwszy znak to nie 'a'*/
        }
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
        
    }

    if (tempPinNr != WSZYSTKIE_PINY)
        analogowyCzyCyfrowy = sprawdzNumerPinu(tempPinNr, stanyPinowCyfrowych, iloscPinow);
    else
        analogowyCzyCyfrowy = WSZYSTKIE_PINY;

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
            } else {
                return 0;
            }
        }
    case PIN_ANALOGOWY:
    case WSZYSTKIE_PINY:
        /*pin jest analogowy lub sa to wszystkie piny, mozna wiec dokonac tylko operacji odczytu*/
        /*nastepny znak to null, prawidlowa instrukcja odczytu*/

        //uSend("sprawdzPolecenie(): pin analogowy, zmienna analogowyCzyCyfrowy: ");
        //uSendLn(analogowyCzyCyfrowy);

        if ((polecenie[tempIndeks] == 'r')) {
            if (polecenie[tempIndeks + 1] == 0) {
                switch (analogowyCzyCyfrowy) {
                case PIN_ANALOGOWY:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_ANALOGOWY;
                    break;
                case PIN_CYFROWY:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_CYFROWY;
                    break;
                case WSZYSTKIE_PINY:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_WSZYSTKIE;
                    break;
                }
                struktAdr->nowyStan = 0;
                struktAdr->nrPinu = tempPinNr;
                return 1;
            } else {
                return 0;
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
    /*
    uSend("aktualizujTabeleStanow(): ");uSendLn(stanyPinowCyfrowych->stanPinu);
    */
 }

void wyswietlOpisPinu(Pin stanyPinowCyfrowych[], size_t iloscPinow, byte nrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if ((stanyPinowCyfrowych + i)->nrPinu == nrPinu) {
            uSend((stanyPinowCyfrowych + i)->opisPinu);
        }
    }
}

void przyciskTimerUstawienie() {
    TCCR1A = 0; // timer 1 control register A - wyzerowanie , Arduino podobno lubi ustawiaæ
    TCCR1B |= 3u ; TCCR1B &= ~(4u); // ustawienie 011 na bitach 2:0 - wybór preskalera /64
    OCR1A = 16383; //wartoœc porównawcza (do 65 535)
    /*
    preskaler 64
    1 cykl licznika -> 64/16MHz = 4000ns
    4000ns x 16383 ~= 0,066 ms
    */
}

void czestOdswEkranuTimerUstawienie() {
    TCCR2A = 0;
    TCCR2B |= (7u); // 111 na bitach 2:0 - preskaler 1024
    OCR2A = 0b11111110; //254 - compare value
}

void przerwaniePrzyciskUstawienie() {
    pinMode(A14, INPUT_PULLUP);
    PCMSK2 = (1 << PCINT22); //interrupt for pin 22 - pk6 - A14
    PCICR |= (1 << PCIE2); //pin change interrupt enable dla PCIE2
}

