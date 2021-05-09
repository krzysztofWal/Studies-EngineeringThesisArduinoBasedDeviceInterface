
#include "functions.h"

/*zmienne globalne wykorzystywnae przez przerwania*/
volatile byte przerwaniePrzycisk = 0;
volatile byte stanPrzycisku = 0;
volatile byte naliczoneCykleTimerDrugi = 0;

/*funkcje przerwań*/
ISR(TIMER1_COMPA_vect) {
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
}


void setup()
{	
	/* ======== deklaracje i definicje zmiennych ======== */ 
    /*obsluga komunikacji z komputerem*/
    char bufor[WIELKOSC_BUFORA_SERIAL];
    bool buforPelny = false;
    byte index = 0;
    
	/* obsluga pinow (wejsc i wyjsc z lasera)*/
    Pin pinyBledow[ILOSC_PINOW_BLEDOW] = {
        {POW_SUP_FAULT_PIN, 30, PIN_CYFROWY, 0, "Power supply fault [when LOW]", INPUT},
        {BEAM_COLL_FAULT_PIN, 31, PIN_CYFROWY, 0, "Beam collimator fault", INPUT},
        {POW_AMP_CUR_FAULT_PIN, 32, PIN_CYFROWY, 0, "Power-amp current fault", INPUT},
        {PRE_AMP_CUR_FAULT_PIN, 33, PIN_CYFROWY, 0, "Pre-amp currernt fault", INPUT},
        {BASE_PLATE_TEMP_FAULT_PIN, 34, PIN_CYFROWY, 0, "Base plate temp fault", INPUT},
        {SEED_LAS_TEMP_FAULT_PIN, 35, PIN_CYFROWY, 0, "Seed laser temp fault", INPUT},
        {RESERVED_FAULT_PIN, 36, PIN_CYFROWY, 0, "Reserved fault", INPUT},
    };

    byte poprzedniStanPinowBledow[ILOSC_PINOW_BLEDOW];

 /* jesli STATE_SEL_N_PIN nie beda po kolei, wystapi problem z funkcja ustawCharakt()
	!!!!-- stany pinow w tej tablicy nie są aktualizowane --!!!!
 */
    Pin pinyCharakterystyk[8] = {
        {STATE_SEL_0_PIN,  20, PIN_CYFROWY,0, "State select: 0", OUTPUT},
        {STATE_SEL_1_PIN,  21, PIN_CYFROWY,0, "State select: 1", OUTPUT},
        {STATE_SEL_2_PIN,  22, PIN_CYFROWY,0, "State select: 2", OUTPUT},
        {STATE_SEL_3_PIN,  23, PIN_CYFROWY,0, "State select: 3", OUTPUT},
        {STATE_SEL_4_PIN,  24, PIN_CYFROWY,0, "State select: 4", OUTPUT},
        {STATE_SEL_5_PIN,  25,PIN_CYFROWY, 0, "State select: 5", OUTPUT},
        {STATE_SEL_6_PIN,  26, PIN_CYFROWY,0, "State select: 6", OUTPUT},
        {STATE_SEL_7_PIN,  27,PIN_CYFROWY, 0, "State select: 7", OUTPUT},
    }
    

    Pin laserReady = { LASER_READY_PIN, 15, PIN_CYFROWY, 0, "Laser ready [when HIGH]", INPUT };

    byte poprzedniStanLaserReady;

    Pin laserDisable = { LAS_DISABLE_PIN, 14, PIN_CYFROWY, 0, "Laser disable", OUTPUT };

    /* wartosci w tej tablicy sa uaktualniane */
    Pin pinyLasera[ILOSC_PINOW] = {
        
        {GLOBAL_ENABLE_PIN, 10, PIN_CYFROWY, 0, "Global Enable", OUTPUT},
        {LASER_EMITION_GATE_PIN, 11, PIN_CYFROWY, 0, "Laser emission gate", OUTPUT},
        {MODE_SELECT_BIT, 12, PIN_CYFROWY, 0, "Pulsed/CW mode select", OUTPUT},
        {ALIGNMENT_LAS_ENABLE_PIN, 13, PIN_CYFROWY, 0, "Alignment laser enable", OUTPUT},

        laserDisable, 
        laserReady,

        pinyCharakterystyk[0],
        pinyCharakterystyk[1],
        pinyCharakterystyk[2],
        pinyCharakterystyk[3],
        pinyCharakterystyk[4],
        pinyCharakterystyk[5],
        pinyCharakterystyk[6],
        pinyCharakterystyk[7],

        pinyBledow[0],
        pinyBledow[1],
        pinyBledow[2],
        pinyBledow[3],
        pinyBledow[4],
        pinyBledow[5],
        pinyBledow[6],

        {BASE_PLATE_TEMP_MON_PIN, 40, PIN_ANALOGOWY, 0, "Base plate temp", INPUT},
        {PRE_AMP_CUR_MON_PIN, 41, PIN_ANALOGOWY, 0, "Pre amp diode current", INPUT},
        {POW_AMP_CUR_MON_PIN, 42, PIN_ANALOGOWY, 0, "Power amp diode current", INPUT},

        {A_S_CUR_S_P_PIN, 43, PIN_ANALOGOWY, 0, "Active-State current set point", INPUT},
        {S_C_S_P_PIN, 44, PIN_ANALOGOWY, 0, "Simmer current set point", INPUT}
    
    };

    /*obsługa wyświetlacza*/
    extern uint8_t TinyFont[];
    extern uint8_t MediumNumbers[];
    LCD5110 wyswietlacz(SCK_PIN, MOSI_PIN, DC_PIN, RST_PIN, CS_PIN);
    /* tryb 0 - monitorowane prady, tryb 1 - temperatura podstawy, tryb 2 - ustawiane potencjometrami wartosci*/
    byte trybWyswietlacza = 0;
    byte zmienionyEkranWyswietlacza = 0;
    byte pierwszeWykrycieNacisnieciaPrzycisku = 1;
    byte odczytaneWartosci[ILOSC_CYFR_ADC] = {0, 0 ,0, 0, 0, 0 };
	
	/* ==== obsługa LEDów ==== */
    /*ekspander pinow*/
    Adafruit_MCP23017 mcp;
    mcp.begin();
	
    /*pierwsze siedem LEDow odpowiada kolejnym pinom w tablicy pinyBledow*/
    byte ledNrPin[ILOSC_LEDOW] = { LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN, LED6_PIN, LED7_PIN, LED_LASER_READY_PIN, LED_LASER_DISABLE_PIN,  LED_LAS_EMIT_GATE_ENABLE_PIN };
	

	/* ======= ustawianie typów pinów (cyfrowy/analogowy; wejcie/ wyjscie) i poczatkowych stanow ========*/
	
	/* rozpoczęcie komunikacji szeregowej */
	Serial.begin(9600);
	
    /* piny - ustawienie input/output i ustawienie startowych wartości
    wyjść sterujących na stan niski,
    diod informujących o stanach wejść na stan odpowiadający stanom odczytanym z wejść
    */
    for (size_t i = 0; i < ILOSC_PINOW; i++) {
        if (pinyLasera[i].rodzajPinu == PIN_CYFROWY) {
            pinMode(pinyLasera[i].rzeczywistyNrPinu, pinyLasera[i].inOut);
        }
        /*ustaw wszystkie wyjscia na 0*/
        if (pinyLasera[i].inOut == OUTPUT) {
            digitalWrite(pinyLasera[i].rzeczywistyNrPinu, LOW);
        }
    }

	/* piny ekspandera (LEDy)*/
    for (size_t i = 0; i < ILOSC_LEDOW; i++) {
        mcp.pinMode(ledNrPin[i], OUTPUT);
        /*ustaw stan niski na LEDach sygnalizujace LED_LASER_DISABLE i LED_LAS_EMIT_GATE */
        if (ledNrPin[i] == LED_LASER_DISABLE_PIN || ledNrPin[i] == LED_LAS_EMIT_GATE_ENABLE_PIN) {
            mcp.digitalWrite(ledNrPin[i], LOW);
        }
    }

    /* pin laser ready i LED sygnalizujący*/ 
    poprzedniStanLaserReady = zwrocStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu);
    /*odczytaj stan LASER_READY_PIN i odpowiednio ustaw diode sygnalizujaca LED_LASER_READY*/
    if (poprzedniStanLaserReady == HIGH) {
        mcp.digitalWrite(LED_LASER_READY_PIN, HIGH);
    } else {
        mcp.digitalWrite(LED_LASER_READY_PIN, LOW);
    }

    /*piny błędów i LEDy sygnalizujące błędy*/
    for (size_t i = 0; i < ILOSC_PINOW_BLEDOW; i++) {
        poprzedniStanPinowBledow[i] = zwrocStanPinuCyfrowego(pinyBledow, ILOSC_PINOW_BLEDOW, pinyBledow[i].rzeczywistyNrPinu);
        /* odczytaj wejscia pinow bledow i odpowiednio zasygnalizuj diodami*/
        if (poprzedniStanPinowBledow[i] == LOW) {
            mcp.digitalWrite(ledNrPin[i], HIGH);
        }
        else if (poprzedniStanPinowBledow[i] == HIGH) {
            mcp.digitalWrite(ledNrPin[i], LOW);
        }
    }
	
	/* przycisk przełączajacy wartości wyświetlane na ekranie */
    pinMode(PRZYCISK_PIN, INPUT);

    /*pin informujący o wybranym napięcia dla sterowania poziomami prądów w głównym wzmacniaczu
    stan wysoki - jedno, przechodzące przez układ elektroniczny
    stan niski - dwa, bezpośrednio z dwóch źródeł zewnętrznych
    */
    pinMode(WYBOR_ZRODLA_NAPIECIA_PIN, INPUT);

    /*przerwanie od przycisku - Pin Change Interrupt*/
    przerwaniePrzyciskUstawienie();

    /* timer używany do zniwelowania przełączania styków */
    przyciskTimerUstawienie();
    
    /* timer - odświeżanie ekranu*/
    czestOdswEkranuTimerUstawienie();

    /* umożliwienie obsługi przerwań*/
    sei(); 
  
    /* wyswietl status poczatkowy */
    sSendLn("Status poczatkowy:");
    wyswietlStanyWszystkichPinow(pinyLasera, ILOSC_PINOW);
    
    wyswietlacz.InitLCD(60);
    wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);


    while (1) {

        /* ====== obsługa laser ready =======*/
        if (zwrocStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu) != poprzedniStanLaserReady) {
            poprzedniStanLaserReady = zwrocStanPinuCyfrowego(&laserReady, 1, laserReady.rzeczywistyNrPinu);
            mcp.digitalWrite(LED_LASER_READY_PIN, poprzedniStanLaserReady);
            //uSend("Zmiana stanu laser ready na: "); uSendLn(poprzedniStanLaserReady);
        }

        /* ======= obsluga bledow (LEDOW) =======- */
       obslugaLedowBledow(pinyBledow, poprzedniStanPinowBledow, ILOSC_PINOW_BLEDOW, ledNrPin, mcp);

        /* ===== obsluga przycisku ======= */
       
       if (przerwaniePrzycisk) {
            
            if (pierwszeWykrycieNacisnieciaPrzycisku) {
                /* wykrycie nacisniecia przycisku - wlaczanie pierwszego timera*/
                PRZERWANIE_PRZYCISK_OFF;
                TCNT1 = WARTOSC_WCZYTYWANA; //warto�� wczytywana przez timer na pocz�tku odliczania
                PRZERWANIE_TIMER1_ON;
                pierwszeWykrycieNacisnieciaPrzycisku = 0;
            }

            if (stanPrzycisku != NIE_SPRAWDZAJ_STANU_PRZYCISKU) {
                PRZERWANIE_TIMER2_OFF;
                PRZERWANIE_TIMER1_OFF;
                if (stanPrzycisku == PRZYCISK_WCIAZ_WCISNIETY) {
                    przelaczTrybWyswietlacza(trybWyswietlacza, digitalRead(WYBOR_ZRODLA_NAPIECIA_PIN));
                    zmienionyEkranWyswietlacza = 0; 
               }
                stanPrzycisku = NIE_SPRAWDZAJ_STANU_PRZYCISKU;
                pierwszeWykrycieNacisnieciaPrzycisku = 1;
                PRZERWANIE_PRZYCISK_ON;
                przerwaniePrzycisk = 0;
            }
        }

        /* ==== obsluga zmiany wyswietlanych danych ==== */
        if (!zmienionyEkranWyswietlacza) {
            wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);
            zmienionyEkranWyswietlacza = 1;
            PRZERWANIE_TIMER2_ON;
        }

        /* ==== obsluga odswiezania wartosci na wyswietlaczu ==== */
        if (naliczoneCykleTimerDrugi > 19) {
            odczytajWartosciMonitorow(trybWyswietlacza, odczytaneWartosci);
            aktualizujWyswietlaneWartosci(odczytaneWartosci, wyswietlacz, trybWyswietlacza, MediumNumbers);
            naliczoneCykleTimerDrugi = 0;
        }

        /* ==== odbieranie komend od uzytkownika ==== */

        /*    
         *     Jesli jest znak do odbioru w serialu dopisz go do tablicy bufor na pozycji indeks i uaktualnij indeks
         *     Nastepie sprawdz czy bufor pelny lub czy wpisany znak to CR lub LF, jesli ktorykolwiek z tych spelniony
         *     ustaw flage buforPelny na 1
         */
         
        if (Serial.available() > 0) {
            bufor[index] = Serial.read();
            sSend(bufor[index]);
            index++;
            if (index == WIELKOSC_BUFORA_SERIAL || bufor[index - 1] == 13 || bufor[index - 1] == 10) {
                index = 0;
                buforPelny = 1;
            }
        }
        
        /* ==== obsluga wpisanej komendy ==== */

        /*
         * 
         *    Jesli bufor jest pelny to obsluz komende wpisana w buforze
         *    Wyzeruj flagę bufor pelny
         */
        if (buforPelny) {
            PRZERWANIE_PRZYCISK_OFF;
            PRZERWANIE_TIMER1_OFF;
            PRZERWANIE_TIMER2_OFF;

            sSendLn();

            obsluzKomende(pinyLasera, ILOSC_PINOW, bufor, mcp);

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
