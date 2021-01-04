
#include "functions.h"

byte przerwanie = 0;
byte stanPrzycisku = 0;
byte naliczoneCykleTimerDrugi = 0;


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

ISR(PCINT2_vect) {
    przerwanie = 1;
    // PORTH ^= (1 << PH4);
}

void aktualizujWyswietlaneWartosci(LCD5110 &wyswietlacz, int &counter, uint8_t* font) {
    wyswietlacz.setFont(font);
    wyswietlacz.printNumI(counter, 50, 7);
    wyswietlacz.update();
}



void setup()
{
    Serial.begin(9600);

    char polecenie[WIELKOSC_BUFORA_SERIAL];
    PolecenieInfo sprawdzonePolecenie;
    Pin stanyPinowCyfrowych[ILOSC_PINOW] =
    {
 
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

    /*obs³uga wyœwietlacza*/
    extern uint8_t SmallFont[];
    extern uint8_t TinyFont[];
    LCD5110 wyswietlacz(SCK_PIN, MOSI_PIN, DC_PIN, RST_PIN, CS_PIN);
    byte trybWyswietlacza = 0;
    byte poprzedniTrybWyswietlacza = 0;
    byte zmienonyEkranWyswietlacza = 0;
    bool pierwszeWykrycieNacisnieciaPrzycisku = true;

    /*przerwanie od przycisku - Pin Change Interrupt*/
    przerwaniePrzyciskUstawienie();

    /* timer u¿ywany do zniwelowania prze³¹czania styków */
    przyciskTimerUstawienie();
    
    /* timer - odœwie¿anie ekranu*/
    czestOdswEkranuTimerUstawienie();

    /* umo¿liwienie obs³ugi przerwañ*/
    sei(); 

    /* piny "laserowe" */
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
        if (przerwanie) {
            
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
                przerwanie = 0;
            }
        }
  
        if (naliczoneCykleTimerDrugi > 19) {
            PRZERWANIE_TIMER2_OFF;
            aktualizujWyswietlaneWartosci(wyswietlacz, ++counter, SmallFont);
            naliczoneCykleTimerDrugi = 0;
            PRZERWANIE_TIMER2_ON;
        }

        if (!zmienonyEkranWyswietlacza) {
           // counter++;
            wyswietl(trybWyswietlacza, wyswietlacz, TinyFont);
            zmienonyEkranWyswietlacza = 1;
            PRZERWANIE_TIMER2_ON;
        }

        uSend(bit_is_set(PINK, PK6));uSend(" ");uSend(poprzedniTrybWyswietlacza);uSend(" ");uSend(trybWyswietlacza);uSend(" ");uSendLn(counter);
        
        ///* czekaj na dane z bufora */
        //while (Serial.available() < 1) {
        //}

        //Serial.readBytesUntil('\r', polecenie, WIELKOSC_BUFORA_SERIAL);

        //for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
        //    uSend(polecenie[i]);
        //}
        //uSendLn();

        ///* wpisana instrukcja jest prawidlowa */
        //if (sprawdzPolecenie(&sprawdzonePolecenie, polecenie, WIELKOSC_BUFORA_SERIAL, stanyPinowCyfrowych, ILOSC_PINOW)) {
        //    /*
        //    uSendLn("=============================================");
        //    uSendLn("while(1): Informacje o otrzymanym poleceniu:");
        //    uSend("Rodzaj:");uSendLn(sprawdzonePolecenie.rodzaj);
        //    uSend("Pin:");uSendLn(sprawdzonePolecenie.nrPinu);
        //    uSend("Wartosc:");uSendLn(sprawdzonePolecenie.nowyStan);
        //    uSendLn("=============================================");
        //    */
        //    if (sprawdzonePolecenie.rodzajPolecenia == ZMIEN_STAN_CYFROWEG0) {
        //        //uSend("while(1): pin cyfrowy zapisz nowyStan");
        //        //uSendLn(sprawdzonePolecenie.nowyStan);
        //        aktualizujTabeleStanow(zmienStanPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu, sprawdzonePolecenie.nowyStan), sprawdzonePolecenie.nowyStan);
        //       // for (size_t i = 0; i < ILOSC_PINOW; i++) {
        //       //     uSend(i);uSend(" ");uSendLn(stanyPinowCyfrowych[i].stanPinu);
        //       // }
        //    } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_CYFROWY) {
        //        uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu);uSend(" ");
        //        wyswietlOpisPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
        //        uSend(" ");uSend(" Stan: ");
        //        uSendLn(podajStanPinuCyfrowego(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));
        //    } else if (sprawdzonePolecenie.rodzajPolecenia == ODCZYTAJ_ANALOGOWY){
        //        uSend("Pin nr: ");uSend(sprawdzonePolecenie.nrPinu); uSend(" ");
        //        wyswietlOpisPinu(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu);
        //        uSend(" "); uSend(" Wartosc: ");
        //        uSendLn(podajStanPinuAnalogowego(stanyPinowCyfrowych, ILOSC_PINOW, sprawdzonePolecenie.nrPinu));
        //    } else {
        //        wyswietlStanyPinow(stanyPinowCyfrowych, ILOSC_PINOW);
        //    }

        //} else {
        //    uSendLn("Nieprawidlowa instrukcja");
        //}

        ///* wyczysc tablice z poleceniem */
        //for (size_t i = 0; i < WIELKOSC_BUFORA_SERIAL; i++) {
        //    polecenie[i] = 0;
        //}
    }
    
}

void loop()
{
}
