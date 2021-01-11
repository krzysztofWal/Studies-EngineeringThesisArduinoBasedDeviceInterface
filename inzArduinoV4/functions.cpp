#include "functions.h"

/* ==== poczatkowe ustawienia peryferiow ==== */
void przyciskTimerUstawienie() {
    TCCR1A = 0; // timer 1 control register A - wyzerowanie , Arduino podobno lubi ustawia�
    TCCR1B |= 3u; TCCR1B &= ~(4u); // ustawienie 011 na bitach 2:0 - wyb�r preskalera /64
    OCR1A = 62500; //warto�c por�wnawcza (do 65 535)
    /*
    preskaler 64
    1 cykl licznika -> 64/16MHz = 4000ns
    4000ns x 16383 ~= 0,066 s
    */
}

void czestOdswEkranuTimerUstawienie() {
    TCCR2A = 0;
    TCCR2B |= (7u); // 111 na bitach 2:0 - preskaler 1024
    OCR2A = 0b11111110; //254 - compare value
}

void przerwaniePrzyciskUstawienie() {

    PCMSK0 = (1 << PCINT0); //przerwanie PCINT0 na pinie 53
    PCICR |= (1 << PCIE0); //pin change interrupt enable dla PCIE0
}

/* ==== obsluga wyswietlacza ====*/
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

void przelaczTrybWyswietlacza(byte& trybWyswietlacza, byte wyborZrodlaZasilania) {
    /* wyborZrodlaZasilania HIGH - tryby 0, 1, 2*/
    /* stan niski - tryby 0, 2*/
    sSend(trybWyswietlacza);
    if (wyborZrodlaZasilania) {
        if (trybWyswietlacza < 2) {
            trybWyswietlacza++;
        } else {
            trybWyswietlacza = 0;
        }
    } else {
        if (trybWyswietlacza == 0) {
            trybWyswietlacza = 2;
        } else {
            trybWyswietlacza = 0;
        }
    } 
    sSendLn(trybWyswietlacza);
}

void aktualizujWyswietlaneWartosci(byte* odczytaneWartosci, LCD5110& wyswietlacz, byte trybWyswietlacza, uint8_t* font) {
    // wyswietl(trybWyswietlacza, wyswietlacz, fontNapisy);
    wyswietlacz.setFont(font);
    switch (trybWyswietlacza) {
    case 0:
    case 1:

        wyswietlacz.printNumI(odczytaneWartosci[0], WARTOSC_GORA_X, WARTOSC_GORA_Y);
        wyswietlacz.printNumI(odczytaneWartosci[1], WARTOSC_GORA_X + 15, WARTOSC_GORA_Y);
        wyswietlacz.printNumI(odczytaneWartosci[2], WARTOSC_GORA_X + 28, WARTOSC_GORA_Y);

        wyswietlacz.printNumI(odczytaneWartosci[3], WARTOSC_DOL_X, WARTOSC_DOL_Y);
        wyswietlacz.printNumI(odczytaneWartosci[4], WARTOSC_DOL_X + 14, WARTOSC_DOL_Y);
        wyswietlacz.printNumI(odczytaneWartosci[5], WARTOSC_DOL_X + 27, WARTOSC_DOL_Y);
    case 2:
        wyswietlacz.printNumI(odczytaneWartosci[0], WARTOSC_GORA_X, WARTOSC_GORA_Y);
        wyswietlacz.printNumI(odczytaneWartosci[1], WARTOSC_GORA_X + 14, WARTOSC_GORA_Y);
        wyswietlacz.printNumI(odczytaneWartosci[2], WARTOSC_GORA_X + 27, WARTOSC_GORA_Y);
        break;
    }
    wyswietlacz.update();

}

/* ==== obslugaPrzycisku ====*/
/* !- modyfikuje zmienne globalne stanPrzycisku i przerwaniePrzycisku !- */
//byte obsluzPrzycisk(byte&  pierwszeWykrycieNacisnieciaPrzycisku) {
//
//    if (przerwaniePrzycisk) {
//
//        if (pierwszeWykrycieNacisnieciaPrzycisku) {
//            /* wykrycie nacisniecia przycisku - wlaczanie pierwszego timera*/
//            PRZERWANIE_PRZYCISK_OFF;
//            TCNT1 = WARTOSC_WCZYTYWANA; //warto�� wczytywana przez timer na pocz�tku odliczania
//            PRZERWANIE_TIMER1_ON;
//            pierwszeWykrycieNacisnieciaPrzycisku = 0;
//        }
//
//        if (stanPrzycisku != NIE_SPRAWDZAJ_STANU_PRZYCISKU) {
//            PRZERWANIE_TIMER2_OFF;
//            PRZERWANIE_TIMER1_OFF;
//            if (stanPrzycisku == PRZYCISK_WCIAZ_WCISNIETY) {
//                stanPrzycisku = NIE_SPRAWDZAJ_STANU_PRZYCISKU;
//                pierwszeWykrycieNacisnieciaPrzycisku = 1;
//                PRZERWANIE_PRZYCISK_ON;
//                przerwaniePrzycisk = 0;
//                return 1;
//            }
//            stanPrzycisku = NIE_SPRAWDZAJ_STANU_PRZYCISKU;
//            pierwszeWykrycieNacisnieciaPrzycisku = 1;
//            PRZERWANIE_PRZYCISK_ON;
//            przerwaniePrzycisk = 0;
//        }
//        return 0;
//    }
//
//}

/*==== obsluga komend ====*/
void obsluzKomende(Pin *wszystkiePiny, byte iloscPinow, char *bufor, Adafruit_MCP23017& mcp) {

    size_t i = 0;
    char polecenie[WIELKOSC_BUFORA_SERIAL];
    PolecenieInfo sprawdzonePolecenie;

    for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
        polecenie[i] = 0;
    }

    while (i < WIELKOSC_BUFORA_SERIAL) {
        if (bufor[i] != 13) {
            polecenie[i] = bufor[i];
            i++;
        }
        else {
            i = 6;
        }
    }
  
    if(polecenie[0] == 'd') {
        byte temp = 0;
        for (size_t i = 1; i < WIELKOSC_BUFORA_SERIAL; i++) {
            if (polecenie[i] != 0) {
                temp = 1;
            }
        }
        if (temp == 0) {
           aktualizujStanPinu(zmienStanPinu(wszystkiePiny, iloscPinow, LAS_DISABLE_PIN, HIGH), HIGH);
           mcp.digitalWrite(LED_LASER_DISABLE_PIN, HIGH);
        }
    } else {
  
        /* wpisana komenda jest prawidlowa */
        if (sprawdzKomende(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, wszystkiePiny, iloscPinow)) {
    
            if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
                /* zmien stan pinu cyfrowego */
                aktualizujStanPinu(zmienStanPinu(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);
    
                /* zasygnalizuj zmiane pinow laser disable i laser emittion gate enable */
    
                if (sprawdzonePolecenie.nrPinu == LASER_EMITION_GATE_PIN) {
                    mcp.digitalWrite(LED_LAS_EMIT_GATE_ENABLE_PIN, sprawdzonePolecenie.nowyStan);
                  
                }
                else if (sprawdzonePolecenie.nrPinu == LAS_DISABLE_PIN) {
                    mcp.digitalWrite(LED_LASER_DISABLE_PIN, sprawdzonePolecenie.nowyStan);
                }
    
            }
            else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_CYFROWY) {
                sSendLine;
                sSend("Pin nr: ");sSend(zwrocNumerWyboru(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu));sSend(" ");
                wyswietlOpisPinu(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu);
                sSend(" ");sSend(" Stan: ");
                if (zwrocStanPinuCyfrowego(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu)) {
                    sSendLn("HIGH");
                }
                else {
                    sSendLn("LOW");
                };
                sSendLine;
            }
            else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_ANALOGOWY) {
                sSendLine;
                sSend("Pin nr: ");sSend(zwrocNumerWyboru(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu)); sSend(" ");
                wyswietlOpisPinu(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu);
                sSend(" "); sSend(" Wartosc: ");
                sSend(zwrocWartPinuAnalogowego(wszystkiePiny, iloscPinow, sprawdzonePolecenie.nrPinu));
                sSendLn("%");
                sSendLine;
            }
            else {
                wyswietlStanyWszystkichPinow(wszystkiePiny, iloscPinow);
            }
            
        }
        else {
            /* Wpisana komenda jest nieprawidlowa */
            sSendLn("Nieprawidlowa komenda");
        }
    }
}

byte sprawdzNumerPinu(byte liczba, Pin wszystkiePiny[], size_t iloscPinow) {
    byte temp = PIN_NIE_ISTNIEJE;
    for (size_t i = 0; i < iloscPinow; i++) {
        if (liczba == (wszystkiePiny + i)->numerWyboru) {
            if ((wszystkiePiny + i)->rodzajPinu == PIN_CYFROWY) {
                if ((wszystkiePiny + i)->inOut == INPUT) {
                    temp = PIN_CYFROWY_WEJSCIE;
                } else {
                    temp = PIN_CYFROWY_WYJSCIE;
                }
            } else {
                temp = (wszystkiePiny + i)->rodzajPinu;
            }

            break;
            /* przerwanie petli jesli znaleziono */
        }
    }
    return temp;
}

byte sprawdzKomende(PolecenieInfo* struktAdr, char polecenie[], size_t dlugosc, Pin stanyPinowCyfrowych[], size_t iloscPinow) {
    size_t tempIndeks = 0;
    byte tempPinNr = 0; //255 - wszystkie piny
    byte analogowyCzyCyfrowy = 3;

    /* sprawdz ile cyfr ma pierwszy numer */
    while (*(polecenie + tempIndeks) > 47 && *(polecenie + tempIndeks) < 58) {
        tempIndeks++;
    }
   
    /*policz numer, najwiekszy mozliwy to 2-cyfrowy */
    if (tempIndeks == 2) {
        tempPinNr += *(polecenie + 1) - 48 + (*polecenie - 48) * 10;
    }
    else if (tempIndeks == 1) {
        tempPinNr += (*polecenie - 48);
    }
    else { /* temp indeks > 2 lub temp indeks == 0*/
        if (tempIndeks == 0 && polecenie[0] == 'a') { /* pierwszy wyraz to 'a'*/
            tempPinNr = WSZYSTKIE_PINY;
            tempIndeks++;
        } else {
           return 0; /* numer pinu 3-cyfrowy lub wiekszy - nieodpowiedni lub brak numeru a pierwszy znak to nie 'a'*/
        }
        
    }

    if (tempPinNr != WSZYSTKIE_PINY)
        analogowyCzyCyfrowy = sprawdzNumerPinu(tempPinNr, stanyPinowCyfrowych, iloscPinow);
    else
        analogowyCzyCyfrowy = WSZYSTKIE_PINY;

    switch (analogowyCzyCyfrowy) {
    case PIN_NIE_ISTNIEJE:
        /* nie ma takiego pinu */
        return 0;
    case PIN_CYFROWY_WYJSCIE:
        /*pin jest cyfrowy, mozna wiec dokonac operacji zapisu i odczytu */
        /* nastepny znak to 0 lub 1 za ktorym nastepuje NULL - prawidlowa operacja zapisu*/
        if (polecenie[tempIndeks] == 'w') {
            if (((polecenie[tempIndeks + 1] == '1') ||
                    (polecenie[tempIndeks + 1] == '0')) &&
                    polecenie[tempIndeks + 2] == 0) {
                struktAdr->rodzajPolecenia = ZMIEN_STAN_CYFROWEG0;
                struktAdr->nowyStan = polecenie[tempIndeks + 1] - 48;
                struktAdr->nrPinu = struktAdr->nrPinu = zwrocRzeczywistyNumerPinu(stanyPinowCyfrowych, iloscPinow, tempPinNr);
                return 1;
            } else {
                return 0;
            }
        }
    case PIN_CYFROWY_WEJSCIE:
    case PIN_ANALOGOWY:
    case WSZYSTKIE_PINY:
        /*pin jest analogowy lub sa to wszystkie piny, mozna wiec dokonac tylko operacji odczytu*/
        /*nastepny znak to null, prawidlowa instrukcja odczytu*/

        if ((polecenie[tempIndeks] == 'r')) {
            if (polecenie[tempIndeks + 1] == 0) {
                switch (analogowyCzyCyfrowy) {
                case PIN_ANALOGOWY:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_ANALOGOWY;
                    break;
                case PIN_CYFROWY_WEJSCIE:
                case PIN_CYFROWY_WYJSCIE:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_CYFROWY;
                    break;
                case WSZYSTKIE_PINY:
                    struktAdr->rodzajPolecenia = ODCZYTAJ_WSZYSTKIE;
                    break;
                }
              //  sSendLn(struktAdr->rodzajPolecenia);
                struktAdr->nowyStan = 0;
                struktAdr->nrPinu = zwrocRzeczywistyNumerPinu(stanyPinowCyfrowych, iloscPinow ,tempPinNr);
                return 1;
            } else {
                return 0;
            }
        } else {
            return 0;
        }

        
    }

}

Pin* zmienStanPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu, byte nowyStan) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (nrPinu == (wszystkiePiny + i)->rzeczywistyNrPinu) {
            digitalWrite(nrPinu, nowyStan);
            return (wszystkiePiny + i);
        }
    }
}

byte zwrocRzeczywistyNumerPinu(Pin wszystkiePiny[], size_t iloscPinow, byte numerWyboru) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (wszystkiePiny[i].numerWyboru == numerWyboru) {
            return wszystkiePiny[i].rzeczywistyNrPinu;
        }
    }
}

byte zwrocNumerWyboru(Pin wszystkiePiny[], size_t iloscPinow, byte rzeczywistyNrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (wszystkiePiny[i].rzeczywistyNrPinu == rzeczywistyNrPinu) {
            return wszystkiePiny[i].numerWyboru;
        }
    }
}

/*==== obsluga tablicy pinow (stany pinow cyfrowych) ====*/
void aktualizujStanPinu(Pin* wszystkiePiny, byte nowyStan) {
    wszystkiePiny->stanPinu = nowyStan;
}

/* ===== odcztywanie stanow pinow ==== */

byte zwrocStanPinuCyfrowego(Pin wszystkiePiny[], size_t iloscPinow, byte podanyNrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (podanyNrPinu == (wszystkiePiny + i)->rzeczywistyNrPinu) {
            /*
            uSend("podajStanPinuCyfrowego() ");uSend(" ");uSend(podanyNrPinu); uSendLn((stanyPinowCyfrowych + i)->nrPinu);
            uSendLn((stanyPinowCyfrowych-+ 1)->stanPinu);
            */
            if ((wszystkiePiny + i)->inOut == INPUT) {
                return digitalRead((wszystkiePiny + i)->rzeczywistyNrPinu);
            }
            else {
                return (wszystkiePiny + i)->stanPinu;
            }
        }
    }
}

byte zwrocWartPinuAnalogowego(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu) {
    return map(analogRead(nrPinu), 0, 1023, 0, 100);
}

void odczytajWartosciMonitorow(byte trybWyswietlacza, byte* odczytaneWartosci) {
    uint16_t pierwszaWartosc;
    uint16_t drugaWartosc;
    switch (trybWyswietlacza) {
    case 0:
        pierwszaWartosc = map(analogRead(POW_AMP_CUR_MON_PIN), 0, 1023, 0, 100);
        drugaWartosc = map(analogRead(PRE_AMP_CUR_MON_PIN), 0, 1023, 0, 100);
        break;
    case 1:
        pierwszaWartosc = map(analogRead(A_S_CUR_S_P_PIN), 0, 1023, 0, 100);
        drugaWartosc = map(analogRead(S_C_S_P_PIN), 0, 1023, 0, 100);
        break;
    case 2:
        pierwszaWartosc = map(analogRead(BASE_PLATE_TEMP_MON_PIN), 0, 1023, 0, 100);
        break;
    }
    odczytaneWartosci[0] = pierwszaWartosc / 100;
    odczytaneWartosci[1] = pierwszaWartosc / 10 - odczytaneWartosci[0] * 10;
    odczytaneWartosci[2] = pierwszaWartosc % 10;
    odczytaneWartosci[3] = drugaWartosc / 100;
    odczytaneWartosci[4] = drugaWartosc / 10 - odczytaneWartosci[3] * 10;
    odczytaneWartosci[5] = drugaWartosc % 10;
}

/* ==== wyswietlanie tekstowe ==== */

void wyswietlOpisPinu(Pin wszystkiePiny[], size_t iloscPinow, byte nrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if ((wszystkiePiny + i)->rzeczywistyNrPinu == nrPinu) {
            sSend((wszystkiePiny + i)->opisPinu);
        }
    }
}

void wyswietlStanyWszystkichPinow(Pin wszystkiePiny[], size_t iloscPinow) {
    sSendLine;
    for (size_t i = 0; i < iloscPinow; i++) {
        sSend((wszystkiePiny + i)->numerWyboru);
        sSend(" | ");
        if ((wszystkiePiny + i)->rodzajPinu == PIN_ANALOGOWY) {
            sSend(zwrocWartPinuAnalogowego(wszystkiePiny, iloscPinow, (wszystkiePiny + i)->rzeczywistyNrPinu));
            sSend("%");
        } 
        else {
            if ((zwrocStanPinuCyfrowego(wszystkiePiny, iloscPinow, (wszystkiePiny + i)->rzeczywistyNrPinu))) {
                sSend("HIGH");
            }
            else {
                sSend("LOW ");
            }
        }
        sSend(" | ");
        sSend((wszystkiePiny + i)->opisPinu);
        sSendLn();

        switch (i) {
        case 4:
        case 5:
        case 13:
        case 20:
            sSendLine;
        }
        
    }
    sSendLine;
}

/* ==== ledy bledow ==== */
/*w przypadku tej funkcji musi zachodzic
ILOSC_PINOW_BLEDOW <= ilosci ledow */
void obslugaLedowBledow(Pin* pinyBledow, byte* poprzedniStanPinowBledow, byte iloscPinowBledow, byte* ledNrPin, Adafruit_MCP23017& mcp) {
    for (size_t i = 0; i < iloscPinowBledow; i++) {
        if (zwrocStanPinuCyfrowego(pinyBledow, iloscPinowBledow, pinyBledow[i].rzeczywistyNrPinu) != poprzedniStanPinowBledow[i]) {

            poprzedniStanPinowBledow[i] = zwrocStanPinuCyfrowego(pinyBledow, iloscPinowBledow, pinyBledow[i].rzeczywistyNrPinu);

            if (poprzedniStanPinowBledow[i] == HIGH)
                mcp.digitalWrite(ledNrPin[i], LOW);
            else
                mcp.digitalWrite(ledNrPin[i], HIGH);
        }
    }
}
