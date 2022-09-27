#define ADC_BT PA0
#define ADC_AC PA1
#define ADC_ZONE_1 200
#define ADC_ZONE_2 400
#define ADC_ZONE_3 600
#define ADC_ZONE_4 800
#define ADC_ZONE_5 1000
#define ADC_AVG 5

#define GPIO_ON 0
#define GPIO_OFF 1

#define BT_CHARGE PA10

#define LED1 PA4
#define LED2 PA5
#define LED3 PA6
#define LED4 PA7
#define LED5 PA9

#define GPIO_BUZZ PA13
#define GPIO_RELAY PA14

int adcValue1 = 0;
int adcValue2 = 0;


#include <FlashStorage_STM32.h>

void flashOpenCount() {
  uint16_t address = 0;
  int number;

  Serial.print(F("\nStart FlashStoreAndRetrieve on ")); Serial.println(BOARD_NAME);
  Serial.println(FLASH_STORAGE_STM32_VERSION);

  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  // Read the content of emulated-EEPROM
  EEPROM.get(address, number);

  // Print the current number on the serial monitor
  Serial.print("Number = 0x"); Serial.println(number, HEX);

  // Save into emulated-EEPROM the number increased by 1 for the next run of the sketch
  EEPROM.put(address, (int) (number + 1));
  EEPROM.commit();

}
void gpioInit() {

  pinMode(BT_CHARGE, OUTPUT);
  digitalWrite(BT_CHARGE, LOW);

  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW);
  pinMode(LED4, OUTPUT);
  digitalWrite(LED4, LOW);
  pinMode(LED5, OUTPUT);
  digitalWrite(LED5, LOW);

  pinMode(GPIO_BUZZ, OUTPUT);
  digitalWrite(GPIO_BUZZ, LOW);
  pinMode(GPIO_RELAY, OUTPUT);
  digitalWrite(GPIO_RELAY, LOW);
}
void chargeOn() {
  digitalWrite(BT_CHARGE, HIGH);
}
void chargeOff() {
  digitalWrite(BT_CHARGE, LOW);
}

void relayOn() {
  digitalWrite(GPIO_RELAY, HIGH);
}
void relayOff() {
  digitalWrite(GPIO_RELAY, LOW);
}


int adcCheckBTValue() {
  adcValue1 = 0;
  for (int x = 0; x <= ADC_AVG; x++) {
    adcValue1 = adcValue1 + analogRead(ADC_BT);
    delay(5);
  }
  adcValue1 = adcValue1 / ADC_AVG;
  Serial.print("BT = \t");
  Serial.println(adcValue1);
  if (adcValue1 > ADC_ZONE_5) {
    chargeOff();
    return 5;
  }
  if (adcValue1 > ADC_ZONE_4) {
    chargeOff();
    return 4;
  }
  if (adcValue1 > ADC_ZONE_3) {
    chargeOn();
    return 3;
  }
  if (adcValue1 > ADC_ZONE_2) {
    chargeOn();
    return 2;
  }
  if (adcValue1 > ADC_ZONE_1) {
    chargeOn();
    relayOff();
    return 1;
  }
  chargeOn();
  Serial.print("very low\r\n");
  return 0;

}


int adcCheckACValue() {
  adcValue2 = 0;
  for (int x = 0; x <= ADC_AVG; x++) {
    adcValue2 = adcValue2 + analogRead(ADC_AC);
    delay(5);
  }
  adcValue2 = adcValue2 / ADC_AVG;
  Serial.print("AC = \t");
  Serial.println(adcValue2);
  if (adcValue2 > 512) {
    relayOff();
    return 1;
  } else {
    relayOn();
    return 1;
  }
  return 0;
}

void ledBTlevel(int level) {
  Serial.println(level);
  switch (level) {
    case 5:
      digitalWrite(LED1, GPIO_ON);
      digitalWrite(LED2, GPIO_ON);
      digitalWrite(LED3, GPIO_ON);
      digitalWrite(LED4, GPIO_ON);
      digitalWrite(LED5, GPIO_ON);
      break;
    case 4:
      digitalWrite(LED1, GPIO_ON);
      digitalWrite(LED2, GPIO_ON);
      digitalWrite(LED3, GPIO_ON);
      digitalWrite(LED4, GPIO_ON);
      digitalWrite(LED5, GPIO_OFF);
      break;
    case 3:
      digitalWrite(LED1, GPIO_ON);
      digitalWrite(LED2, GPIO_ON);
      digitalWrite(LED3, GPIO_ON);
      digitalWrite(LED4, GPIO_OFF);
      digitalWrite(LED5, GPIO_OFF);
      break;
    case 2:
      digitalWrite(LED1, GPIO_ON);
      digitalWrite(LED2, GPIO_ON);
      digitalWrite(LED3, GPIO_OFF);
      digitalWrite(LED4, GPIO_OFF);
      digitalWrite(LED5, GPIO_OFF);
      break;
    case 1:
      digitalWrite(LED1, GPIO_ON);
      digitalWrite(LED2, GPIO_OFF);
      digitalWrite(LED3, GPIO_OFF);
      digitalWrite(LED4, GPIO_OFF);
      digitalWrite(LED5, GPIO_OFF);
      break;
    case 0:
      digitalWrite(LED1, GPIO_OFF);
      digitalWrite(LED2, GPIO_OFF);
      digitalWrite(LED3, GPIO_OFF);
      digitalWrite(LED4, GPIO_OFF);
      digitalWrite(LED5, GPIO_OFF);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  gpioInit();
  flashOpenCount();
}

void loop() {
  // put your main code here, to run repeatedly:
  ledBTlevel(adcCheckBTValue());
  adcCheckACValue();
}
