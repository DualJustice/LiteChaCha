#include "Arduino.h"
#include "HardwareSerial.h"

#include "BigNumber.h"


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	BigNumber::begin();

	BigNumber a;
	BigNumber b;
	BigNumber c;
	BigNumber d;

	BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949";
	BigNumber A24 = 121665;

	Serial.print("p: ");
	Serial.println(p);

	a = 1000;
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	c = 1000;

	b = a + c;
	Serial.print("b (should be 2000): ");
	Serial.println(b);
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	c = 3000;
	Serial.print("c (should be 3000): ");
	Serial.println(c);

	d = c/a;
	Serial.print("d (should be 3): ");
	Serial.println(d);
	Serial.print("c (should be 3000): ");
	Serial.println(c);
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	b = b/a;
	Serial.print("b (should be 2): ");
	Serial.println(b);

	a = "123456789123456789123456789123456789123456789123456789123456789123456789123456789";
	Serial.print("a: ");
	Serial.println(a);

	b = a % p;
	Serial.println("b (should be 22421996477724801930119104195479351870985469883947857395004571021392927325521): ");
	Serial.print("             ");
	Serial.println(b);

	c = "123456789123456789123456789123456789123456789";
	d = c.pow(2);
	Serial.println("d (should be 15241578780673678546105778311537878076969977811918920046486820281054720515622620750190521)");
	Serial.print("             ");
	Serial.println(d);

	d = 2;
	a = ((c.pow(2))*d) % p;
	Serial.println("a (should be 34448047961358672903238494434736815569263625689538145824675882968395199193724): ");
	Serial.print("             ");
	Serial.println(a);

	a = (A24*a) % p;
	Serial.println("a (should be 27085274043245416359623012795841485351924544798296351024048183386182588322350): ");
	Serial.print("             ");
	Serial.println(a);

	p = 99;
	a = 30;
	b = 75;
	c = (a + b) % p;
	Serial.print("c (should be 6): ");
	Serial.println(c);

	b = 35;
	c = (a - b) % p;
	if(c < 0) {
		c = c + p;
	}
	Serial.print("c (should be 94): ");
	Serial.println(c);
}


void loop() {}