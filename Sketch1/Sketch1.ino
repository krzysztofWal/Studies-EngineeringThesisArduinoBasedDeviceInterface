/*
 Name:		Sketch1.ino
 Created:	04.01.2021 13:56:26
 Author:	Randomowa Nazwa
*/


char receivedChar[5];
boolean newData = false;
byte index = 0;
bool bufferFull = false;
int counter = 0;

ISR(TIMER3_COMPA_vect) {
    counter++;


}


void setup(){
    Serial.begin(9600);
    

    TCCR3A = 0; // timer 1 control register A - wyzerowanie , Arduino podobno lubi ustawiaæ
    TCCR3B |= 3u; TCCR3B &= ~(4u); // ustawienie 011 na bitach 2:0 - wybór preskalera /64
    OCR3A = 16383;
    TCNT3 = 0;
    TIMSK3 = (1 << OCIE3A);

    sei();

    while (1) {

        if (bufferFull) {
            for (size_t i = 0; i < 5; i++) {
                Serial.println(receivedChar[i]);
            }
            bufferFull = false;
        }

        Serial.println(counter);
        if (Serial.available() > 0) {
            receivedChar[index++] = Serial.read();
            if (index == 5) {
                bufferFull = true;
                index = 0;
            }
        }
    }

}

void loop (){

}