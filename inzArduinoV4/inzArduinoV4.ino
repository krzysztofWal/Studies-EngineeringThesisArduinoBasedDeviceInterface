
#include "functions.h"

volatile byte przerwaniePrzycisk = 0;
volatile byte stanPrzycisku = 0;
volatile byte naliczoneCykleTimerDrugi = 0;
volatile byte przerwanieBledy = 0;


ISR(TIMER1_COMPA_vect) {
   // uSendLn(bit_is_clear(PRZYCISK_PORT, PRZYCISK_REJ_POZYCJA));
    if (bit_is_set(PRZYCISK_PORT, PRZYCISK_REJ_POZYCJA)) { // bit jest 1 - przycisk przycisniety
        stanPrzycisku = PRZYCISK_WCIAZ_WCISNIETY;
    }
    else if (bit_is_clear(PRZYCISK_PORT, PRZYCISK_REJ_POZYCJA)) { //bit jest 0 - przycisk nieprzycisniety
        stanPrzycisku = PRZYCISK_ZOSTAL_PUSZCZONY;
    }
}

ISR(TIMER2_COMPA_vect) {
    naliczoneCykleTimerDrugi++;
}

ISR(PCINT0_vect) {
    przerwaniePrzycisk = 1;
    uSendLn("Przerwanie siê generuje");
}

ISR(PCINT2_vect) {
    przerwanieBledy = 1;
}

void aktualizujWyswietlaneWartosci(byte* odczytaneWartosci, Pin* stanyPinowCyfrowych, LCD5110& wyswietlacz, byte trybWyswietlacza, uint8_t* font, uint8_t* fontNapisy) {
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
        {POW_SUP_FAULT_PIN, PIN_CYFROWY, 0, "Power supply fault", INPUT},
        {RESERVED_FAULT_PIN, PIN_CYFROWY, 0, "Reserved fault", INPUT},
        {BEAM_COLL_FAULT_PIN, PIN_CYFROWY, 0, "Beam collimator fault", INPUT},
        {POW_AMP_CUR_FAULT_PIN, PIN_CYFROWY, 0, "Power-amp current fault", INPUT},
        {PRE_AMP_CUR_FAULT_PIN, PIN_CYFROWY, 0, "Pre-amp currernt fault", INPUT},
        {BASE_PLATE_TEMP_FAULT_PIN, PIN_CYFROWY, 0, "Base plate temp fault", INPUT},
        {SEED_LAS_TEMP_FAULT_PIN, PIN_CYFROWY, 0, "Seed laser temp fault", INPUT},
    };

    byte poprzedniStanPinowBledow[ILOSC_PINOW_BLEDOW];

    Pin laserReady = { LASER_READY_PIN, PIN_CYFROWY, 0, "Laser ready", INPUT };

    byte poprzedniStanLaserReady;

    Pin laserDisable = { LAS_DISABLE_PIN, PIN_CYFROWY, 0, "Laser disable", OUTPUT };

    Pin wszystkiePiny[ILOSC_PINOW] =
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

        
        {BASE_PLATE_TEMP_MON_PIN, PIN_ANALOGOWY, 0, "Base plate temp", INPUT},
        {PRE_AMP_CUR_MON_PIN, PIN_ANALOGOWY, 0, "Pre amp diode current", INPUT},
        {POW_AMP_CUR_MON_PIN, PIN_ANALOGOWY, 0, "Power amp diode current", INPUT},

        {A_S_CUR_S_P_PIN, PIN_ANALOGOWY, 0, "Active-State current set point", INPUT},
       {S_C_S_P_PIN, PIN_ANALOGOWY, 0, "Simmer current set point", INPUT}
    
    };

    /*obs³uga wyœwietlacza*/
    extern uint8_t SmallFont[];
    extern uint8_t TinyFont[];
    extern uint8_t MediumNumbers[];
    LCD5110 wyswietlacz(SCK_PIN, MOSI_PIN, DC_PIN, RST_PIN, CS_PIN);
    byte trybWyswietlacza = 0;
    byte poprzedniTrybWyswietlacza = 0;
    byte zmienonyEkranWyswietlacza = 0;
    bool pierwszeWykrycieNacisnieciaPrzycisku = true;
    byte odczytaneWartosci[ILOSC_CYFR_ADC] = {0, 0 ,0, 0, 0, 0 };

    /*ekspander pinow*/
    Adafruit_MCP23017 mcp;

    /*pierwsze siedem LEDow odpowiada kolejnym pinom w tablicy pinyBledow*/
    byte ledNrPin[ILOSC_LEDOW] = { LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN, LED6_PIN, LED7_PIN, LED_LASER_DISABLE_PIN, LED_LASER_READY_PIN, LED_LAS_EMIT_GATE_ENABLE_PIN };

    
    /* piny  */
    for (size_t i = 0; i < ILOSC_PINOW; i++) {
        if (wszystkiePiny[i].rodzajPinu == PIN_CYFROWY) {
            pinMode(wszystkiePiny[i].rzeczywistyNrPinu, wszystkiePiny[i].inOut);
        }
        /*ustaw wszystkie wejscia na 0*/
        if (wszystkiePiny[i].inOut == OUTPUT) {
            digitalWrite(wszystkiePiny[i].rzeczywistyNrPinu, LOW);
        }
       //uSend(podajStanPinuCyfrowego(wszystkiePiny, ILOSC_PINOW, wszystkie[i].rzeczywistyNrPinu))
    }

    mcp.begin();
    for (size_t i = 0; i < ILOSC_LEDOW; i++) {
        mcp.pinMode(ledNrPin[i], OUTPUT);
        if (ledNrPin[i] == LED_LASER_DISABLE_PIN || ledNrPin[i] == LED_LAS_EMIT_GATE_ENABLE_PIN) {
            mcp.digitalWrite(ledNrPin[i], LOW);
        }
    }

    /* pin laser ready */ 
    poprzedniStanLaserReady = podajStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu);

    /*piny bledow */
    for (size_t i = 0; i < ILOSC_PINOW_BLEDOW; i++) {
        poprzedniStanPinowBledow[i] = podajStanPinuCyfrowego(pinyBledow, ILOSC_PINOW_BLEDOW, pinyBledow[i].rzeczywistyNrPinu);
        uSendLn(podajStanPinuCyfrowego(pinyBledow, ILOSC_PINOW_BLEDOW, pinyBledow[i].rzeczywistyNrPinu));
        /* jesli stan jest niski, to znaczy ¿e jest fault*/
        if (poprzedniStanPinowBledow[i] == LOW) {
            mcp.digitalWrite(ledNrPin[i], HIGH);
          //  uSend("Zmieniono na dodatnie: "); uSendLn(i);
        }
        else if (poprzedniStanPinowBledow[i] == HIGH) {
            mcp.digitalWrite(ledNrPin[i], LOW);
        //    uSend("Zmieniono na wylaczone"); uSendLn(i);
        }
    }

    /* przycisk prze³¹czaj¹cy wartosci wyswietlane na ekranie */
    pinMode(PRZYCISK_PIN, INPUT_PULLUP);

    /*przerwanie od przycisku - Pin Change Interrupt*/
    przerwaniePrzyciskUstawienie();


    /* timer u¿ywany do zniwelowania prze³¹czania styków */
    przyciskTimerUstawienie();
    
    /* timer - odœwie¿anie ekranu*/
    czestOdswEkranuTimerUstawienie();

    /* umo¿liwienie obs³ugi przerwan*/
    sei(); 
  
    /* wyswietl status poczatkowy */
    uSendLn("===\r\nStatus poczatkowy:");
    wyswietlStanyPinow(wszystkiePiny, ILOSC_PINOW);
    uSendLn("===");

    wyswietlacz.InitLCD(60);
    wyswietlacz.setFont(SmallFont);
    wyswietlacz.clrScr();
    wyswietlacz.print("HALKO", CENTER, 20);
    wyswietlacz.update();
    delay(500);
    wyswietlacz.clrScr();
    wyswietlacz.update();

    wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);

    int counter = 0;

    while (1) {

        /* ====== obsluga laser ready =======*/
        if (podajStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu) != poprzedniStanLaserReady) {
            poprzedniStanLaserReady = podajStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu);
            mcp.digitalWrite(LED_LASER_READY_PIN, poprzedniStanLaserReady);
            //uSend("Zmiana stanu laser ready na: "); uSendLn(poprzedniStanLaserReady);
        }

        /* ======= obsluga bledow (LEDOW) =======- */
       obslugaLedowBledow(wszystkiePiny, ILOSC_PINOW, pinyBledow, poprzedniStanPinowBledow, ILOSC_PINOW_BLEDOW, ledNrPin, mcp);

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
                if (stanPrzycisku == PRZYCISK_WCIAZ_WCISNIETY) {
                    setNext(trybWyswietlacza);
                    zmienonyEkranWyswietlacza = 0; 
                }
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
        //    PRZERWANIE_TIMER2_OFF;
            odczytajWartosciADC(trybWyswietlacza, odczytaneWartosci);

            /*
            uSend(odczytaneWartosci[0]);
            uSend(odczytaneWartosci[1]);
            uSend(odczytaneWartosci[2]);

            uSend(" ");

            uSend(odczytaneWartosci[3]);
            uSend(odczytaneWartosci[4]);
            uSendLn(odczytaneWartosci[5]);
            */

            aktualizujWyswietlaneWartosci(odczytaneWartosci, wszystkiePiny, wyswietlacz, trybWyswietlacza, MediumNumbers, SmallFont);
            naliczoneCykleTimerDrugi = 0;
        //    PRZERWANIE_TIMER2_ON;
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
            if (sprawdzPolecenie(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, wszystkiePiny, ILOSC_PINOW)) {

                if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
                    aktualizujTabeleStanow(zmienStanPinu(wszystkiePiny, ILOSC_PINOW, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);

            /* zasygnalizuj zmiane pinow laser disable i laser emittion gate enable */

                    if (sprawdzonePolecenie.nrPinu == LASER_EMITION_GATE_PIN ) {
                        mcp.digitalWrite(LED_LAS_EMIT_GATE_ENABLE_PIN, sprawdzonePolecenie.nowyStan);
                    } else if (sprawdzonePolecenie.nrPinu == LAS_DISABLE_PIN) {
                            mcp.digitalWrite(LED_LASER_DISABLE_PIN, sprawdzonePolecenie.nowyStan);
                    }
 
                } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_CYFROWY) {
                    uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu);uSend(" ");
                    wyswietlOpisPinu(wszystkiePiny, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
                    uSend(" ");uSend(" Stan: ");
                    uSendLn(podajStanPinuCyfrowego(wszystkiePiny, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));

                } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_ANALOGOWY){
                    uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu); uSend(" ");
                    wyswietlOpisPinu(wszystkiePiny, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
                    uSend(" "); uSend(" Wartosc: ");
                    uSendLn(podajStanPinuAnalogowego(wszystkiePiny, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));

                } else {
                    wyswietlStanyPinow(wszystkiePiny, ILOSC_PINOW);
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
