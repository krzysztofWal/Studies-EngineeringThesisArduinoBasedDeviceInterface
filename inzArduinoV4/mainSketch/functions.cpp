#include "functions.h"

/*
 * TO-DO:
 * wyswietlacz
 * dodać możliwość wpisania zera przy instukcji waveformowej - dun, trzeba sprawdzić
 * wyświetlanie na termicie
 * (?) zerowanie global enable i emition gate przy laser disable - dun
 * 
 */

/* ==== poczatkowe ustawienia peryferiow ==== */
void przyciskTimerUstawienie() {
    TCCR1A = 0; // timer 1 control register A - wyzerowanie , Arduino podobno lubi ustawiać
    TCCR1B |= 3u; TCCR1B &= ~(4u); // ustawienie 011 na bitach 2:0 - wybór preskalera /64
    OCR1A = 62500; //wartośc porównawcza (do 65 535)
    /*
    preskaler 64
    1 cykl licznika -> 64/16MHz = 4000ns
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


/*==== obsluga komend ====*/
void obsluzKomende(Pin *pinyLasera, byte iloscPinow, Pin *pinyCharakterystyk, byte iloscPinowCharakterystyk, char *bufor, Adafruit_MCP23017& mcp) {

    size_t i = 0; // w tej zmiennej ilosc znakow b polecenie bufor, ktora nie jest 13
    char polecenie[WIELKOSC_BUFORA_SERIAL];
    PolecenieInfo sprawdzonePolecenie;
	
	byte iloscZnakowPolecenie = 0;
	
    for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
        polecenie[i] = 0;
    }

    while (i < WIELKOSC_BUFORA_SERIAL) {
        if (bufor[i] != 13) {
            polecenie[i] = bufor[i];
            i++;
        }
        else {
			break;
            //i = 6;
        }
    }
	
    /*
     *  czy polecenie to sama litera 'd', jesli tak, wysoki stan na 'LASER DISABLE', takie - "szybsze traktowanie", bez przechodzenia przez nastepne warunki
     */
     
    if(polecenie[0] == 'd') {
        byte temp = 0;
        for (size_t i = 1; i < WIELKOSC_BUFORA_SERIAL; i++) {
            if (polecenie[i] != 0) {
                temp = 1;
            }
        }
        if (temp == 0) {
            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, LAS_DISABLE_PIN, HIGH), HIGH);
            mcp.digitalWrite(LED_LASER_DISABLE_PIN, HIGH);
            // zmien stan emmition gate i global enable pin na niski (razem z dioda infomujaca o stanie emition gate)
            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, LASER_EMITION_GATE_PIN, LOW), LOW);
            mcp.digitalWrite(LED_LAS_EMIT_GATE_ENABLE_PIN, LOW);
            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, GLOBAL_ENABLE_PIN, LOW), LOW);
        }
	
	/*
	* czy polecenie zaczyna się od litery 'b' - to oznacza, że użytkownik chce zmieniać stany StateSelect pinów, cyli zmieniać wybór charakterystyki,
	* po b wtedy pojawia się numer charakterystykiw postaci dziesiętnej
	*/	
	
    } else if(polecenie[0] == 'b') {

        /*
         * funkcja konwersja sprawdza, czy wielkosc jest w odpowiednim zakresie,
		 * zwraca -1 jesli nie jest w odpowiednim zakresie lub nie jest liczbą
         */

        int8_t nr = konwersjaCharInt(bufor, i, MAKSMALNY_MOZLIWY_NUMER_CHARAKT); /* do i-tego wyrazu (bez niego), i-1 to ostatni wyraz  zapełniony w tablicy polecenie*/
        //sSendLn(nr);
        if (nr >= 0) {
            ustawCharakt(nr, pinyCharakterystyk, ILOSC_PINOW_CHARAKT, pinyLasera, ILOSC_PINOW);
        } else {
            sSendLn("Nieprawidlowa komenda");
        }
        
    } else {
  
        /* wpisana komenda jest prawidlowa (ale nie jest 'd' ani 'b' )*/
        if (sprawdzKomende(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, pinyLasera, iloscPinow)) {
    
            if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
                /* zmien stan pinu cyfrowego */
                aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);
    
                /* zasygnalizuj zmiane pinow laser disable i laser emittion gate enable 
                jesli laser disable pinjest wlaczany to wylacz global enable i emittion gate*/
   
                if (sprawdzonePolecenie.nrPinu == LASER_EMITION_GATE_PIN) {
                    mcp.digitalWrite(LED_LAS_EMIT_GATE_ENABLE_PIN, sprawdzonePolecenie.nowyStan);
                }
                else if (sprawdzonePolecenie.nrPinu == LAS_DISABLE_PIN) {
                    mcp.digitalWrite(LED_LASER_DISABLE_PIN, sprawdzonePolecenie.nowyStan);
                        if (sprawdzonePolecenie.nowyStan == HIGH) {
                            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, LASER_EMITION_GATE_PIN, LOW), LOW);
                            mcp.digitalWrite(LED_LAS_EMIT_GATE_ENABLE_PIN, LOW);
                            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinow, GLOBAL_ENABLE_PIN, LOW), LOW);
                        }
                }

            

            }
            else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_CYFROWY) {
                sSendLine;
                sSend("Pin nr: ");sSend(zwrocNumerWyboru(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu));sSend(" ");
                wyswietlOpisPinu(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu);
                sSend(" ");sSend(" Stan: ");
                if (zwrocStanPinuCyfrowego(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu)) {
                    sSendLn("HIGH");
                }
                else {
                    sSendLn("LOW");
                };
                sSendLine;
            }
            else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_ANALOGOWY) {
                sSendLine;
                sSend("Pin nr: ");sSend(zwrocNumerWyboru(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu)); sSend(" ");
                wyswietlOpisPinu(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu);
                sSend(" "); sSend(" Wartosc: ");
                sSend(zwrocWartPinuAnalogowego(pinyLasera, iloscPinow, sprawdzonePolecenie.nrPinu));
                sSendLn("%");
                sSendLine;
            }
            else {
                wyswietlStanyWszystkichPinow(pinyLasera, iloscPinow);
            }
            
        }
        else {
            /* Wpisana komenda jest nieprawidlowa */
            sSendLn("Nieprawidłowa komenda");
        }
    }
}

byte sprawdzNumerPinu(byte liczba, Pin pinyLasera[], size_t iloscPinow) {
    byte temp = PIN_NIE_ISTNIEJE;
    for (size_t i = 0; i < iloscPinow; i++) {
        if (liczba == (pinyLasera + i)->numerWyboru) {
            if ((pinyLasera + i)->rodzajPinu == PIN_CYFROWY) {
                if ((pinyLasera + i)->inOut == INPUT) {
                    temp = PIN_CYFROWY_WEJSCIE;
                } else {
                    temp = PIN_CYFROWY_WYJSCIE;
                }
            } else {
                temp = (pinyLasera + i)->rodzajPinu;
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
                 struktAdr->nrPinu = zwrocRzeczywistyNumerPinu(stanyPinowCyfrowych, iloscPinow, tempPinNr);
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

Pin* zmienStanPinu(Pin pinyLasera[], size_t iloscPinow, byte rzeczywistyNrPinu, byte nowyStan) {
    for (size_t i = 0; i < iloscPinow; i++) {
      /* 
       *  wprowadzona zmiana nrPinu -> rzeczywistyNrPinu, bardzo możliwe, że to w związku z niezmienieniem wczesniejszym (za to zmienieniem wyzej)
       */
        if (rzeczywistyNrPinu == (pinyLasera + i)->rzeczywistyNrPinu) {
            digitalWrite(rzeczywistyNrPinu, nowyStan);
            return (pinyLasera + i);
        }
    }
}

byte zwrocRzeczywistyNumerPinu(Pin pinyLasera[], size_t iloscPinow, byte numerWyboru) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (pinyLasera[i].numerWyboru == numerWyboru) {
            return pinyLasera[i].rzeczywistyNrPinu;
        }
    }
	return 255;
}

byte zwrocNumerWyboru(Pin pinyLasera[], size_t iloscPinow, byte rzeczywistyNrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (pinyLasera[i].rzeczywistyNrPinu == rzeczywistyNrPinu) {
            return pinyLasera[i].numerWyboru;
        }
    }
	return 255;
}

/*
 *  to co dostajemy to tablica maksymalnie piec znakow
 *  zwraca -1 jesli niechciane wejscie
 *  przekazujemy b + jakis numer czyli numer zaczynamy od pierwszego ineksu
 *  jesli o nie bedzie numer to funkcja zwroci blad wiec nie trzeba sprawdzac przy wywolaniu
 *  
 */
int8_t konwersjaCharInt(char *bufor, byte rozmiarBufora, byte maksNumer) { // returns -1 if invalid input
    int8_t result = 0;
  //  int32_t tWP;
  //  int32_t number;
  // theoretically od tylu bylo by ciut wydajniej bo bez tego odejmowania za kazdym razem ale to tam szczegoly
    for (byte i = 1; i < rozmiarBufora ; i++) {
          if (bufor[i] < 58 && bufor[i] > 47) {
              //  number = arr[i] - 48;
              //  tWP = size - i - 1;
              result += (bufor[i] - 48) * (int32_t)pow(10,rozmiarBufora - i - 1);
          } else {
              result = -1;
              break;
          }
    }
//    sSend("konwersjaCharInt: ");
//    sSendLn(result);
    if (result > maksNumer) {result = -1;}
//    sSend("konwersjaCharInt: ");
//    sSendLn(result);
    return result;
}
/*
 * zakladamy ze numer miesci sie w osmiu bitach ofkors
 * iteracja po numerach w tablicy pinyCharakterystyk, ale uaktualniana jest tablica pinyLasera
 */
void ustawCharakt(int number, Pin pinyCharakterystyk[], byte iloscPinowCharakt, Pin pinyLasera[], byte iloscPinowLasera) {  
    for (uint8_t i = 0; i < iloscPinowCharakt; i++)
        if ((int)(number & (1u << i))) {
        /* na wyjsciu 1 */
            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinowLasera, pinyCharakterystyk[i].rzeczywistyNrPinu, HIGH), HIGH);
        } else {
            aktualizujStanPinu(zmienStanPinu(pinyLasera, iloscPinowLasera, pinyCharakterystyk[i].rzeczywistyNrPinu, LOW), LOW);
        }
}

/*
Pin* wskaznikNaPin(Pin* tablicaPinow, byte iloscPinow, byte rzeczywistyNrPinu) {
   for (size_t i = 0; i < iloscPinow; i++) {
      if (tablicaPinow[i].rzeczywistyNrPinu == rzeczywistyNrPinu) {
          return &tablicaPinow[i];
      }
   }
}
*/
/*==== obsluga tablicy pinow (stany pinow cyfrowych) ====*/
void aktualizujStanPinu(Pin* pinyLasera, byte nowyStan) {
    pinyLasera->stanPinu = nowyStan;
}

/* ===== odcztywanie stanow pinow ==== */

byte zwrocStanPinuCyfrowego(Pin pinyLasera[], size_t iloscPinow, byte podanyNrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if (podanyNrPinu == (pinyLasera + i)->rzeczywistyNrPinu) {
            /*
            uSend("podajStanPinuCyfrowego() ");uSend(" ");uSend(podanyNrPinu); uSendLn((stanyPinowCyfrowych + i)->nrPinu);
            uSendLn((stanyPinowCyfrowych-+ 1)->stanPinu);
            */
            if ((pinyLasera + i)->inOut == INPUT) {
                return digitalRead((pinyLasera + i)->rzeczywistyNrPinu);
            }
            else {
                return (pinyLasera + i)->stanPinu;
            }
        }
    }
}

byte zwrocWartPinuAnalogowego(Pin pinyLasera[], size_t iloscPinow, byte nrPinu) {
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

void wyswietlOpisPinu(Pin pinyLasera[], size_t iloscPinow, byte nrPinu) {
    for (size_t i = 0; i < iloscPinow; i++) {
        if ((pinyLasera + i)->rzeczywistyNrPinu == nrPinu) {
            sSend((pinyLasera + i)->opisPinu);
        }
    }
}

void wyswietlStanyWszystkichPinow(Pin pinyLasera[], size_t iloscPinow) {
    sSendLine;
    for (size_t i = 0; i < iloscPinow; i++) {
        sSend((pinyLasera + i)->numerWyboru);
        sSend(" | ");
        if ((pinyLasera + i)->rodzajPinu == PIN_ANALOGOWY) {
            sSend(zwrocWartPinuAnalogowego(pinyLasera, iloscPinow, (pinyLasera + i)->rzeczywistyNrPinu));
            sSend("%");
        } 
        else {
            if ((zwrocStanPinuCyfrowego(pinyLasera, iloscPinow, (pinyLasera + i)->rzeczywistyNrPinu))) {
                sSend("HIGH");
            }
            else {
                sSend("LOW ");
            }
        }
        sSend(" | ");
        sSend((pinyLasera + i)->opisPinu);
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
