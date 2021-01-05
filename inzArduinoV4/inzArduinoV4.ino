
#include "functions.h"

volatile byte przerwaniePrzycisk = 0;
volatile byte stanPrzycisku = 0;
volatile byte naliczoneCykleTimerDrugi = 0;
volatile byte przerwanieBledy = 0;

ISR(TIMER1_COMPA_vect) {
    if (bit_is_clear(PINK, PK6)) { // bit jest 0 - przycisk przycisniety
        stanPrzycisku = PRZYCISK_WCIAZ_WCISNIETY;
    }
    else if (bit_is_set(PINK, PK6)) { //bit jest 1 - przycisk nieprzycisniety
        stanPrzycisku = PRZYCISK_ZOSTAL_PUSZCZONY;
    }
}

ISR(TIMER2_COMPA_vect) {
    naliczoneCykleTimerDrugi++;
}

ISR(PCINT0_vect) {
    przerwaniePrzycisk = 1;
}

ISR(PCINT2_vect) {
    przerwanieBledy = 1;
}


void aktualizujWyswietlaneWartosci(LCD5110 &wyswietlacz, int &counter, uint8_t* font) {
    wyswietlacz.setFont(font);
    wyswietlacz.printNumI(counter, 50, 7);
    wyswietlacz.update();
}



void setup()
{
    /*komunikacja z komputerem*/
    Serial.begin(9600);
    char bufor[WIELKOSC_BUFORA_SERIAL];
    bool buforPelny = false;
    byte index = 0;

    char polecenie[WIELKOSC_BUFORA_SERIAL];
    PolecenieInfo sprawdzonePolecenie;

    Pin pinyBledow[ILOSC_PINOW_BLEDOW] = {
        {A8, PIN_CYFROWY, 0, "Power supply fault", INPUT},
        {A9, PIN_CYFROWY, 0, "Reserved (GND?) fault", INPUT},
        {A10, PIN_CYFROWY, 0, "Beam collimator fault", INPUT},
        {A11, PIN_CYFROWY, 0, "Power-amp current fault", INPUT},
        {A12, PIN_CYFROWY, 0, "Pre-amp currernt fault", INPUT},
        {A13, PIN_CYFROWY, 0, "Base plate temp fault", INPUT},
        {A14, PIN_CYFROWY, 0, "Seed laser temp fault", INPUT},
    };

    byte poprzedniStanPinowBledow[ILOSC_PINOW_BLEDOW];

    Pin laserReady = { LASER_READY_PIN, PIN_CYFROWY, 0, "Laser ready", INPUT };

    byte poprzedniStanLaserReady;

    Pin laserDisable = { LAS_DISABLE_PIN, PIN_CYFROWY, 0, "Laser disable", OUTPUT };

    Pin stanyPinowCyfrowych[ILOSC_PINOW] =
    {

        {MODE_SELECT_BIT, PIN_CYFROWY, 0, "Pulsed/CW mode select", OUTPUT},
        laserDisable,
        {GLOBAL_ENABLE_PIN, PIN_CYFROWY, 0, "Global Enable", OUTPUT},
        {ALIGNMENT_LAS_ENABLE_PIN, PIN_CYFROWY, 0, "Alignment laser enable", OUTPUT},
        {LASER_EMITION_GATE_PIN, PIN_CYFROWY, 0, "Laser emission gate", OUTPUT},

        laserReady,

        pinyBledow[0],
        pinyBledow[1],
        pinyBledow[2],
        pinyBledow[3],
        pinyBledow[4],
        pinyBledow[5],
        pinyBledow[6],

        {STATE_SEL_0_PIN, PIN_CYFROWY, 0, "State select: 0", OUTPUT},
        {STATE_SEL_1_PIN, PIN_CYFROWY, 0, "State select: 1", OUTPUT},
        {STATE_SEL_2_PIN, PIN_CYFROWY, 0, "State select: 2", OUTPUT},
        {STATE_SEL_3_PIN, PIN_CYFROWY, 0, "State select: 3", OUTPUT},
        {STATE_SEL_4_PIN, PIN_CYFROWY, 0, "State select: 4", OUTPUT},
        {STATE_SEL_5_PIN, PIN_CYFROWY, 0, "State select: 5", OUTPUT},
        {STATE_SEL_6_PIN, PIN_CYFROWY, 0, "State select: 6", OUTPUT},
        {STATE_SEL_7_PIN, PIN_CYFROWY, 0, "State select: 7", OUTPUT},

        
       // {BASE_PLATE_TEMP_PIN, PIN_ANALOGOWY, 0, "Base plate temp", INPUT},
      //  {PRE_AMP_CUR_MON_PIN, PIN_ANALOGOWY, 0, "Pre amp diode current", INPUT},
      //  {PO_AMP_CUR_MON_PIN, PIN_ANALOGOWY, 0, "Power amp diode current", INPUT},

     //   {A_S_CUR_S_P_PIN, PIN_ANALOGOWY, 0, "Active-State current set point", INPUT},
      //  {S_C_S_P_PIN, PIN_ANALOGOWY, 0, "Simmer current set point", INPUT}
    
    };

    /*obs³uga wyœwietlacza*/
    extern uint8_t SmallFont[];
    extern uint8_t TinyFont[];
    LCD5110 wyswietlacz(SCK_PIN, MOSI_PIN, DC_PIN, RST_PIN, CS_PIN);
    byte trybWyswietlacza = 0;
    byte poprzedniTrybWyswietlacza = 0;
    byte zmienonyEkranWyswietlacza = 0;
    bool pierwszeWykrycieNacisnieciaPrzycisku = true;

    /*piny bledow */
    /* odcztywanie poczatkowych wartosci */
    for (size_t i = 0; i < ILOSC_PINOW_BLEDOW; i++) {
        poprzedniStanPinowBledow[i] = podajStanPinuCyfrowego(pinyBledow, ILOSC_PINOW_BLEDOW, pinyBledow[0].nrPinu);
    }

    /* piny "laserowe" */
    for (size_t i = 0; i < ILOSC_PINOW; i++) {
        if (stanyPinowCyfrowych[i].rodzajPinu == PIN_CYFROWY) {
            pinMode(stanyPinowCyfrowych[i].nrPinu, stanyPinowCyfrowych[i].inOut);
        }
    }

    /* pin laser ready */ 
    poprzedniStanLaserReady = podajStanPinuCyfrowego(&laserReady, 1, laserReady.nrPinu);

    /* przycisk prze³¹czaj¹cy tryby ekran */
    pinMode(PRZYCISK_PIN, INPUT_PULLUP);

    /*przerwanie od przycisku - Pin Change Interrupt*/
    przerwaniePrzyciskUstawienie();

    /* Przerwanie od zmiany wejsc bledow */
    przerwanieBledyUstawienie();

    /* timer u¿ywany do zniwelowania prze³¹czania styków */
    przyciskTimerUstawienie();
    
    /* timer - odœwie¿anie ekranu*/
    czestOdswEkranuTimerUstawienie();

    /* umo¿liwienie obs³ugi przerwañ*/
    sei(); 

  
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

    wyswietlacz.InitLCD(60);
    wyswietlacz.setFont(SmallFont);
    wyswietlacz.clrScr();
    wyswietlacz.print("HALKO", CENTER, 20);
    wyswietlacz.update();
    delay(500);
    wyswietlacz.clrScr();
    wyswietlacz.update();

    uSendLn(trybWyswietlacza);
    wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);

    int counter = 0;

    while (1) {

        /* ====== obsluga laser ready =======*/
        if (podajStanPinuCyfrowego(&laserReady, 1, laserReady.nrPinu) != poprzedniStanLaserReady) {
            poprzedniStanLaserReady = podajStanPinuCyfrowego(&laserReady, 1, laserReady.nrPinu);

            uSend("Zmiana stanu laser ready na: "); uSendLn(poprzedniStanLaserReady);
        }

        /* ======= obsluga bledow =======- */
        if (przerwanieBledy) {
            PRZERWANIE_PRZYCISK_OFF;
            PRZERWANIE_TIMER1_OFF;
            PRZERWANIE_TIMER2_OFF;

            for (size_t i = 0; i < ILOSC_PINOW_BLEDOW; i++) {
                if (podajStanPinuCyfrowego(stanyPinowCyfrowych, ILOSC_PINOW, pinyBledow[i].nrPinu) != poprzedniStanPinowBledow[i]) {
                    uSend("Zmiana na pinie: " );
                    uSend(pinyBledow[i].nrPinu);
                    uSend(" ");
                    uSend(pinyBledow[i].opisPinu);
                    uSend(" Poprzedni stan ");
                    uSend(poprzedniStanPinowBledow[i]);
                    uSend(" Obecny stan: ");
                    uSendLn(podajStanPinuCyfrowego(stanyPinowCyfrowych, ILOSC_PINOW, pinyBledow[i].nrPinu));
                    poprzedniStanPinowBledow[i] = podajStanPinuCyfrowego(stanyPinowCyfrowych, ILOSC_PINOW, pinyBledow[i].nrPinu);
                }
            }

            przerwanieBledy = 0;
            PRZERWANIE_PRZYCISK_ON;
            PRZERWANIE_TIMER1_ON;
            PRZERWANIE_TIMER2_ON;
        }

        /* ===== obsluga przycisku ======= */
        if (przerwaniePrzycisk) {
            
            if (pierwszeWykrycieNacisnieciaPrzycisku) {
                /* wykrycie nacisniecia przycisku - wlaczanie pierwszego timera*/
                PRZERWANIE_PRZYCISK_OFF;
                TCNT1 = WARTOSC_WCZYTYWANA; //wartoœæ wczytywana przez timer na pocz¹tku odliczania
                PRZERWANIE_TIMER1_ON;
                pierwszeWykrycieNacisnieciaPrzycisku = false;
            }

            if (stanPrzycisku != NIE_SPRAWDZAJ_STANU_PRZYCISKU) {
                PRZERWANIE_TIMER2_OFF;
                PRZERWANIE_TIMER1_OFF;
                //if (timerFlag == BUTTON_STILL_PRESSED) {
                    setNext(trybWyswietlacza);
                    zmienonyEkranWyswietlacza = 0; 
                //}
                stanPrzycisku = NIE_SPRAWDZAJ_STANU_PRZYCISKU;
                pierwszeWykrycieNacisnieciaPrzycisku = true;
                PRZERWANIE_PRZYCISK_ON;
                przerwaniePrzycisk = 0;
            }
        }
        
        /* ==== obsluga zmiany wyswietlanych danych ==== */
        if (!zmienonyEkranWyswietlacza) {
            wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);
            zmienonyEkranWyswietlacza = 1;
            PRZERWANIE_TIMER2_ON;
        }

        /* ==== obsluga odswiezania wartosci na wyswietlaczu ==== */
        if (naliczoneCykleTimerDrugi > 19) {
            PRZERWANIE_TIMER2_OFF;
            aktualizujWyswietlaneWartosci(wyswietlacz, ++counter, SmallFont);
            naliczoneCykleTimerDrugi = 0;
            PRZERWANIE_TIMER2_ON;
        }

        /* ==== odbieranie komend od uzytkownika ==== */
        if (Serial.available() > 0) {
            bufor[index] = Serial.read();
            uSend(bufor[index]);
            index++;
            if (index == WIELKOSC_BUFORA_SERIAL || bufor[index - 1] == 13 || bufor[index - 1] == 10) {
                index = 0;
                buforPelny = 1;
            }
        }
        
        /* ==== obsluga wpisanej komendy ==== */
        if (buforPelny) {
            PRZERWANIE_PRZYCISK_OFF;
            PRZERWANIE_TIMER1_OFF;
            PRZERWANIE_TIMER2_OFF;

            uSendLn("");

            size_t i = 0;
            while (i < WIELKOSC_BUFORA_SERIAL) {
                if (bufor[i] != 13) {
                   // uSendLn(bufor[i]);
                    polecenie[i] = bufor[i];
                    i++;
                } else {
                    i = 6;
                }
            }

            /* wpisana komenda jest prawidlowa */
            if (sprawdzPolecenie(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, stanyPinowCyfrowych, ILOSC_PINOW)) {

                if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
                    aktualizujTabeleStanow(zmienStanPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);

            /* zasygnalizuj zmiane pinow laser disable i laser emittion gate enable */

                    if (sprawdzonePolecenie.nrPinu == LASER_EMITION_GATE_PIN ) {
                        uSend("Zmieniono stan emition gate enable na: ");
                        uSendLn(sprawdzonePolecenie.nowyStan);
                    } else if (sprawdzonePolecenie.nrPinu == LAS_DISABLE_PIN) {
                        uSend("Zmieniono stan laser disable na :");
                        uSendLn(sprawdzonePolecenie.nowyStan);
                    }
 
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

            /* Wpisana komenda jest nieprawidlowa */
            } else {
                uSendLn("Nieprawidlowa instrukcja");
            }

            /* wyczysc tablice z komenda - wpisz NULL na wszystkich polach */
            for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
                polecenie[i] = 0;
            }

            buforPelny = 0;

            PRZERWANIE_PRZYCISK_ON;
            PRZERWANIE_TIMER1_ON;
            PRZERWANIE_TIMER2_ON;
        }

    }
 
}

void loop()
{
}
