
#include "typedefs.h"
#include "functions.h"
#include "libraries/LCD5110_Graph/LCD5110_Graph.h"
#include "avr/pgmspace.h"

byte przerwanie = 0;
byte timerFlag = 0;
byte option = 0;
bool first = true;


void setNext() {
    if (option < 3) {
        option++;
    }
    else {
        option = 0;
    }
}

ISR(TIMER1_COMPA_vect) {
    if (bit_is_clear(PINK, PK6)) { // bit jest 0 - przycisk przycisniety
        timerFlag = BUTTON_STILL_PRESSED;
    } else if (bit_is_set(PINK, PK6)) { //bit jest 1 - przycisk nieprzycisniety
        timerFlag = BUTTON_NOT_PRESSED_ANYMORE;
    }
        //TCNT1 = LOAD; //wyzeruj timer
  //  PORTH ^= (1 << PH4);
}

ISR(PCINT2_vect) {
    przerwanie = 1;
   // PORTH ^= (1 << PH4);
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

    
    extern uint8_t SmallFont[];
    LCD5110 wyswietlacz(SCK_PIN, MOSI_PIN, DC_PIN, RST_PIN, CS_PIN);
    byte stanWyswietlacza = 0;

    //byte trybWyswietlacza = 0;
    //unsigned long czasNacisniecia = 0;
    //byte czasDebounce = 20;
    //byte stanPrzycisku = 0;

    pinMode(BUTTON_PIN, INPUT);

    /*przerwanie od przycisku*/
    DDRK &= ~(1 << PK6); //PK6 jako wejœcie
    PCMSK2 = (1 << PCINT22); //interrupt for pin 22 - pk6 - A14
    PCICR |= (1 << PCIE2); //pin change interrupt enable dla PCIE2
   


    DDRH |= (1 << PH4); //dioda jako wyjœcie
    /*timer- przerwania*/
    TCCR1A = 0; // timer 1 control register A - wyzerowanie , Arduino podobno lubi ustawiaæ
    TCCR1B |= (1u << 1); // ustawienie 010 na bitach 2:0 - wybór preskalera
    TCCR1B &= ~(5u);

    OCR1A = COMP; //wartoœc porównawcza
    
    sei(); // enable global interrupts

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

    //wyswietlacz.InitLCD(60);
    //wyswietlacz.setFont(SmallFont);
    //wyswietlacz.clrScr();
    //wyswietlacz.print("HALKO", CENTER, 20);
    //wyswietlacz.update();
    //delay(5000);
    //wyswietlacz.clrScr();
    //wyswietlacz.update();

    int counter = 0;

    while (1) {
       // if (przerwanie == 1) {
            //uSendLn("siup");
           // 
            //czasNacisniecia = millis();
           
        //    if (stanPrzycisku == 1 & millis() - czasNacisniecia) {
         //       setNext();
                
         //   }
           // state ^= state;
            if (przerwanie) {
               
                //delay(100);
                ////stanPrzycisku ^= stanPrzycisku;
                //czasNacisniecia = millis();
                //if (millis() - czasNacisniecia > czasDebounce) {
                //    setNext();
                //    counter++;
                //}
                if (first) {
                    PCICR &= ~(1 << PCIE2); //disable interrupts on button pin
                    TCNT1 = LOAD; //wartoœæ wczytywana przez timer na pocz¹tku odliczania
                    TIMSK1 = (1 << OCIE0A); // timer1 compare interrupt enable
                    //wlacz timer
                    first = false;
                }

                if (timerFlag != 0) {

                    if (timerFlag == BUTTON_STILL_PRESSED) {
                        counter++;
                    }
                    TIMSK1 &= ~(1 << OCIE0A); // timer 1 compare interrupt disable
                    timerFlag = DO_NOT_CHECK_BUTTON_YET;

                    first = true;
                    PCICR |= (1 << PCIE2); //enable interrupts on button pin
                    przerwanie = 0;
                }
                //przerwanie = 0;
                //PCICR |= (1 << PCIE2);
            }
       // }
        uSend(counter);uSend(" ");//uSend(millis());uSend(" ");uSendLn(option);
            uSendLn(bit_is_clear(PINK, PK6));


        //uSendLn("Wpisz komende: ");

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
