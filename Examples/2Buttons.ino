#include <AnalogButton.h>


AnalogButton button1(A0, 13); // GND
AnalogButton button2(A0, 512); // Analogvalue 512

void setup() {
	//Serial connection.
	Serial.begin(9600);	
	
	button1.attachClick(button1Klicked);
	button1.attachDoubleClick(button1DoppelKlicked);
	button2.attachClick(button2Klicked);
	button2.attachDoubleClick(button2DoppelKlicked);
}

// the loop function runs over and over again until power down or reset
void loop() {
	button.tick();
	delay(10);
}

void button1Klicked(){
	Serial.println("button1 Klicked");
}

void button1DoppelKlicked(){
	Serial.println("button1 DoppelKlicked");
}
void button2Klicked(){
	Serial.println("button2 Klicked");
}

void button2DoppelKlicked(){
	Serial.println("button2 DoppelKlicked");
}