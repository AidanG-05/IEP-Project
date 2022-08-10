#include <Wire.h>
#include "RichShieldDHT.h"
#include "RichShieldTM1637.h"
#include "RichShieldIRremote.h"

#define CLK 10
#define DIO 11
#define LED_RED 4
#define LED_GREEN 5
#define LED_BLUE 6
#define LED_YELLOW 7
#define RECV_PIN 2

#define KEY_POWER 0x45
#define KEY_MENU 0x47
#define KEY_TEST 0x44
#define KEY_PLUS 0x40
#define KEY_BACK 0x43
#define KEY_PREV 0x07
#define KEY_PLAY 0x15
#define KEY_NEXT 0x09
#define KEY_ZERO 0x16
#define KEY_MINUS 0x19
#define KEY_C 0x0D
#define KEY_ONE 0x0C
#define KEY_TWO 0x18
#define KEY_THREE 0x5E
#define KEY_FOUR 0x08
#define KEY_FIVE 0x1C
#define KEY_SIX 0x5A
#define KEY_SEVEN 0x42
#define KEY_EIGHT 0x52
#define KEY_NINE 0x4A

int8_t tempList[10];
int8_t humiList[10];
int8_t aveTemp;
int8_t aveHumi;
int8_t AVGTemp;

IRrecv IR(RECV_PIN);
TM1637 disp(CLK, DIO);
DHT dht;

int IRKeyEntry(void);
void POWER_ON();
void collectTemperature();
void displayTemperature(int8_t temperature);
void displayaveTemperature(int8_t templist[]);
void collectHumidity(int8_t humiList[]);
void displayHumidity(int8_t humidity);
void displayaveHumidity(int8_t humiList[]);
void Exit();





void setup()
{
	disp.init();
	dht.begin();
	IR.enableIRIn();
	Serial.begin(9600);
}
void loop()
{
	int8_t h = dht.readHumidity();
	int8_t t = dht.readTemperature();
	if (isnan(t) || isnan(h))
	{
		displayError();
	}
	else
	{

		collectTemperature();
		collectHumidity(humiList);
		//delay(3000);

	}
	int KeyNum = 6;
	POWER_ON();
	do
	{
		KeyNum = IRKeyEntry();
	} while (KeyNum > 5);
	switch (KeyNum)
	{
	case 1:
	{
		displayTemperature(t);
		Serial.println(" ");
		Serial.println(t);
	}break;
	case 2:
	{
		Serial.println(" ");
		Serial.println(AVGTemp);
		disp.display(AVGTemp);
	}break;
	case 3:
	{
		displayHumidity(h);
		Serial.println(" ");
		Serial.println(h);
	}break;
	case 4:
	{
		displayaveHumidity(humiList);
		displayHumidity(h);

	} break;
	}

}
/*Function: Read 10 values of the temperature and store it in tempList
Parameter: void
Return void */
void collectTemperature()
{
	int i;
	for (i = 0; i < 10; i++)
	{
		//float t;
		tempList[i] = dht.readTemperature();
		delay(1000);
	}
}
/* Function: Display temperature on 4-digit digital tube */
/* Parameter: -int8_t temperature, temperature range is -40 ~ 125 degrees celsius */
/* Return Value: void */
void displayTemperature(int8_t temperature)
{
	int8_t Temperature[4];
	if (temperature < 0)
	{
		Temperature[0] = INDEX_NEGATIVE_SIGN;
		temperature = abs(temperature);
	}
	else if (temperature < 100) Temperature[0] = INDEX_BLANK;
	else Temperature[0] = temperature / 100;
	temperature %= 100;
	Temperature[1] = temperature / 10;
	Temperature[2] = temperature % 10;
	Temperature[3] = 12;	//index of 'C' for celsius degree symbol.
	disp.display(Temperature);
}

void displayaveTemperature(int8_t templist[])
{
	collectTemperature();
	int i;
	int8_t sum = 0;
	int8_t aveTemp;
	for (i = 0; i < 10; i++)
	{
		sum = sum + templist[i];
	}
	aveTemp = sum / 10;
	AVGTemp = aveTemp; //displayTemperature(aveTemp);
	Serial.println(" Avg Temp: ");
	Serial.println(aveTemp);
}
//Function: displaty humidity on 4-digit digital tube
//Parameters: int8_t humidity
void displayHumidity(int8_t humidity)
{
	int8_t Humidity[4];
	if (humidity < 100)Humidity[0] = INDEX_BLANK;
	else Humidity[0] = humidity / 100;
	humidity %= 100;
	Humidity[1] = humidity / 10;
	Humidity[2] = humidity % 10;
	Humidity[3] = 18;	          //index of 'H' for celsius degree symbol.
	disp.display(Humidity);
	Serial.println("Humidity");
	Serial.println(humidity);
}
void collectHumidity(int8_t humiList[])
{
	// int8_t humiList[10];
	int8_t i;
	for (i = 0; i < 10; i++)
	{
		int8_t h;
		humiList[i] = h;
		delay(250);
	}
}
void displayaveHumidity(int8_t humiList[])
{
	void collectHumidity(int8_t HumiList[]);
	int8_t i;
	int8_t sum = 0;
	int8_t aveHumi;
	for (i = 0; i < 10; i++)
	{
		sum = sum + humiList[i];
	}
	aveHumi = sum / 10;
	disp.display(aveHumi);

}
void displayError()
{
	disp.display(3, 14);//display "E"
}
void Exit()
{
	Serial.println("Exiting");
	digitalWrite(LED_RED, HIGH);
	delay(500);
	digitalWrite(LED_RED, LOW);
}
void POWER_ON()
{
	if (IR.keycode == 0x45)
	{
		Serial.println("Press the following number to control:\n1:Current temperature \n2: Average temperature \n3: Current Humidity \n4: Average Humidity ");
		digitalWrite(LED_RED, HIGH);
		digitalWrite(LED_GREEN, LOW);
		digitalWrite(LED_YELLOW, LOW);
	}
}
int IRKeyEntry(void)
{
	int num = 0;
	if (IR.decode())
	{
		if (IR.isReleased())
			num = IR.keycode;
		if (num == KEY_ONE) num = 1;
		else if (num == KEY_TWO) num = 2;
		else if (num == KEY_THREE) num = 3;
		else  num == KEY_C;
		IR.resume(); //Receive the next value
	}
	return(num);
}