/*
    Name:       RoboticArmController.ino
    Created:	1/11/2019 10:00:43 AM
    Author:     Tim-PC\Tim
	==> Designed for use with Arduino Bluetooth Controller App <==
	Commands:
		1: forward
		2: reverse
		3: left
		4: right
		0: stop motion
		5: base down
		6: base up
		b: stop base
		7: mid up
		N: stop mid up
		8: mid down
		M: stop mid down
		9: rot left
		:: rot right
		;: claw open
		<: claw close
		o: stop claw and rot
		x: stop all

		note: need to get batteries with much higher current capability and low vdroop(in other words, low internal resistance). Also,
			  the left rotation isn't working properly, most likely a problem with the signal wire to the h-bridge from either D6 or A4.
			    **01/13/19**
*/
#include <SoftwareSerial.h>
SoftwareSerial BTSerial(11, 10); //RX, TX

int ForwardPins[] = { 7, 6, 5, 4, 3, 2 }; //for both of these, one pin to each respective halfbridge enable pin
int ReversePins[] = { A5, A4, A3, A2, A1, A0 };
int LedPin = 13;
byte forwardcommands = 0b00000000;
byte reversecommands = 0b00000000;

void setup()
{
	for (int i = 5; i >= 0; i--)
	{
		pinMode(ForwardPins[i], OUTPUT);
		pinMode(ReversePins[i], OUTPUT);
		digitalWrite(ForwardPins[i], HIGH);
		digitalWrite(ReversePins[i], HIGH);
	}
	delay(100);
	for (int i = 5; i >= 0; i--)
	{
		digitalWrite(ForwardPins[i], LOW);
		digitalWrite(ReversePins[i], LOW);
	}
	digitalWrite(LedPin, HIGH);
	Serial.begin(115200);
	BTSerial.begin(115200);
	while(!BTSerial) { ; }
	BTSerial.println("Ready");
}

void loop()
{
	digitalWrite(LedPin, HIGH);
	if (BTSerial.available())
	{
		digitalWrite(LedPin, LOW);
		int comnd = BTSerial.read();
		Serial.println(comnd);
		if (comnd == 49) //forward, ascii '1'
		{
			forwardcommands |= (1 << 4);
			forwardcommands |= (1 << 5);
			reversecommands &= ~(1 << 4);
			reversecommands &= ~(1 << 5);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 50) //reverse, ascii '2'
		{
			forwardcommands &= ~(1 << 4);
			forwardcommands &= ~(1 << 5);
			reversecommands |= (1 << 4);
			reversecommands |= (1 << 5);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 51) //left, ascii '3'
		{
			forwardcommands |= (1 << 4);
			forwardcommands &= ~(1 << 5);
			reversecommands &= ~(1 << 4);
			reversecommands |= (1 << 5);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 52) //right, ascii '4'
		{
			forwardcommands &= ~(1 << 4);
			forwardcommands |= (1 << 5);
			reversecommands |= (1 << 4);
			reversecommands &= ~(1 << 5);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 48) //stop motion, ascii '0'
		{
			forwardcommands &= ~(1 << 4);
			forwardcommands &= ~(1 << 5);
			reversecommands &= ~(1 << 4);
			reversecommands &= ~(1 << 5);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 53) //basedown, ascii 5
		{
			forwardcommands |= (1 << 3);
			reversecommands &= ~(1 << 3);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 54) //baseup, ascii 6
		{
			forwardcommands &= ~(1 << 3);
			reversecommands |= (1 << 3);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 98) //stop base, ascii b
		{
			forwardcommands &= ~(1 << 3);
			reversecommands &= ~(1 << 3);
		}
		else if (comnd == 55) //midup, ascii 7
		{
			forwardcommands |= (1 << 2);
			reversecommands &= ~(1 << 2);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 78) //stop midup, ascii 'N'
		{
			forwardcommands &= ~(1 << 2);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 56) //middown, ascii 8
		{
			forwardcommands &= ~(1 << 2);
			reversecommands |= (1 << 2);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 77) //stop middown, ascii 'M'
		{
			reversecommands &= ~(1 << 2);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 57) //rotleft, ascii 9
		{
			forwardcommands |= (1 << 1);
			reversecommands &= ~(1 << 1);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 58) //rotright, ascii :
		{
			forwardcommands &= ~(1 << 1);
			reversecommands |= (1 << 1);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 59) //clawopen, ascii ;
		{
			forwardcommands |= (1 << 0);
			reversecommands &= ~(1 << 0);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 60) //clawclose, ascii <
		{
			forwardcommands &= ~(1 << 0);
			reversecommands |= (1 << 0);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 111) //stop claw + rot, ascii 'o'
		{
			forwardcommands &= ~(1 << 0);
			reversecommands &= ~(1 << 0);
			forwardcommands &= ~(1 << 1);
			reversecommands &= ~(1 << 1);
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		else if (comnd == 120) //stop everything, ascii 'x'
		{
			forwardcommands = 0b00000000;
			reversecommands = 0b00000000;
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		/*else if (comnd == 33) //ONLY FOR TESTING - NOT WITH MOTORS - all on, ascii '!'
		{
			forwardcommands = 0b00111111;
			reversecommands = 0b00111111;
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}*/
		else
		{
			BTSerial.print("Invalid Command: '");
			BTSerial.print(comnd);
			BTSerial.println("'");
			forwardcommands = 0b00000000;
			reversecommands = 0b00000000;
			Execute(forwardcommands, reversecommands, ForwardPins, ReversePins);
		}
		digitalWrite(LedPin, HIGH);
		BTSerial.println("Finished");
		Serial.println("Finished");
	}

}
void Execute(byte forwardcmd, byte reversecmd, int forwardpin[], int reversepin[])
{
	byte forwards = forwardcmd;
	byte reverses = reversecmd;
	for (int i = 5; i >= 0; i--)
	{
		digitalWrite(forwardpin[i], (forwards & (1 << i)));
	}
	for (int i = 5; i >= 0; i--)
	{
		digitalWrite(reversepin[i], (reverses & (1 << i)));
	}
}
