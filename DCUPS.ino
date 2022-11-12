
#define GPIO_ON 0
#define GPIO_OFF 1


#define ADC_BT PA0
#define ADC_AC PA1
/*
   电阻为512k对62K
  23.5  510 62  2.547202797   790.4047468
  24    510 62  2.601398601   807.221869
  24.5  510 62  2.655594406   824.0389913
  25    510 62  2.70979021    840.8561136
  26    510 62  2.818181818   874.4903581
  輸入電壓
*/
#define ADC_OFFSET 100   //非工程人員改為0即可 測試值100

#define ADC_ZONE_1 790-ADC_OFFSET //23.5
#define ADC_ZONE_2 807-ADC_OFFSET //24
#define ADC_ZONE_3 824-ADC_OFFSET //24.5
#define ADC_ZONE_4 840-ADC_OFFSET //25
#define ADC_ZONE_5 874-ADC_OFFSET //26

#define ADC_AVG 5

//侦测有无AC之阀值
#define AC_ADC_VAL 20 //AC檢知位準
#define checkTheBatteryCycle 10  // 充電檢查週期
int checkTheBatteryCycleCount = 0;



int noHavePower = 0;
int ledChargelevelCount = 0;
#define BT_CHARGE PA10

#define LED1 PA4
#define LED2 PA9
#define LED3 PA7
#define LED4 PA6
#define LED5 PA5
#define ledCycle 1
int ledCycleCount = 0;




#define GPIO_BUZZ PA13
#define BUZZ_ZONE_1 30
#define BUZZ_ZONE_2 50
#define BUZZ_ZONE_3 70
#define BUZZ_ZONE_4 100
#define BUZZ_ZONE_5 200



#define GPIO_RELAY PA14

int adcValue1 = 0;
int adcValue2 = 0;
int level = 0;
int lowPowerBuzzCount = 50;
//计数周期使用
int noAcPowerCountTime = 0;
//GPIO初始化
void gpioInit() {

  pinMode(BT_CHARGE, OUTPUT);
  digitalWrite(BT_CHARGE, HIGH);

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
/*
  执行Buzz次数
  使用方式输入 int times做为次数
*/
void runBuzz(int times) {
  for (int x = 0; x < times; x++) {
    digitalWrite(GPIO_BUZZ, HIGH);
    delay(100);
    digitalWrite(GPIO_BUZZ, LOW);
    delay(100);
  }
}

/*
  读取电池电压
  return相对应数值
  提供给显示副程式使用
*/
int adcCheckBTValue() {
  adcValue1 = 0;
  chargeOff();
  delay(50);//等待电容放电
  for (int x = 0; x <= ADC_AVG; x++) {
    adcValue1 = adcValue1 + analogRead(ADC_BT);
    delay(5);
  }
  adcValue1 = adcValue1 / ADC_AVG;
  chargeOn();
  Serial.print("BT:");
  Serial.println(adcValue1);
  if (adcValue1 > ADC_ZONE_5) {
    lowPowerBuzzCount = BUZZ_ZONE_5;
    return 5;
  }
  if (adcValue1 > ADC_ZONE_4) {
    lowPowerBuzzCount = BUZZ_ZONE_4;
    return 4;
  }
  if (adcValue1 > ADC_ZONE_3) {
    lowPowerBuzzCount = BUZZ_ZONE_3;
    return 3;
  }
  if (adcValue1 > ADC_ZONE_2) {
    lowPowerBuzzCount = BUZZ_ZONE_2;
    return 2;
  }
  if (adcValue1 > ADC_ZONE_1) {
    lowPowerBuzzCount = BUZZ_ZONE_1;

    return 1;
  }
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
  Serial.print("AC:");
  Serial.println(adcValue2);
  if (adcValue2 > AC_ADC_VAL) {
    relayOff();
    return 0;
  } else {
    //停电状态
    relayOn();
    //计数次数
    return 1;
  }
  return 0;
}
/*
  todo 评估是否加入呼吸效果
  输入 int level 对应状态
  目前不会闪烁或是呼吸效果
*/
void ledBTlevel(int level) {
  Serial.println(level);
  if (noHavePower == 1) {
    //沒有電的狀況下燈號閃爍
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
  } else {
    //有電的狀況下跑燈狀態
    //level = 5 => 5,4
    //level = 4 => 5,4,3
    //level = 3 => 5,4,3,2
    //level = 2 => 5,4,3,2,1
    //level = 1 => 5,4,3,2,1,0
    Serial.print("ledChargelevelCount\t");
    Serial.println(ledChargelevelCount);
    Serial.print("level\t");
    Serial.println(level);

    switch (ledChargelevelCount) {
      case 5:
        digitalWrite(LED1, GPIO_ON);
        digitalWrite(LED2, GPIO_ON);
        digitalWrite(LED3, GPIO_ON);
        digitalWrite(LED4, GPIO_ON);
        digitalWrite(LED5, !digitalRead(LED5));
        ledChargelevelCount = level;
        break;
      case 4:
        digitalWrite(LED1, GPIO_ON);
        digitalWrite(LED2, GPIO_ON);
        digitalWrite(LED3, GPIO_ON);
        digitalWrite(LED4, GPIO_ON);
        digitalWrite(LED5, GPIO_OFF);
        ledChargelevelCount++;
        break;
      case 3:
        digitalWrite(LED1, GPIO_ON);
        digitalWrite(LED2, GPIO_ON);
        digitalWrite(LED3, GPIO_ON);
        digitalWrite(LED4, GPIO_OFF);
        digitalWrite(LED5, GPIO_OFF);
        ledChargelevelCount++;
        break;
      case 2:
        digitalWrite(LED1, GPIO_ON);
        digitalWrite(LED2, GPIO_ON);
        digitalWrite(LED3, GPIO_OFF);
        digitalWrite(LED4, GPIO_OFF);
        digitalWrite(LED5, GPIO_OFF);
        ledChargelevelCount++;
        break;
      case 1:
        digitalWrite(LED1, GPIO_ON);
        digitalWrite(LED2, GPIO_OFF);
        digitalWrite(LED3, GPIO_OFF);
        digitalWrite(LED4, GPIO_OFF);
        digitalWrite(LED5, GPIO_OFF);
        ledChargelevelCount++;
        break;
      case 0:
        digitalWrite(LED1, GPIO_OFF);
        digitalWrite(LED2, GPIO_OFF);
        digitalWrite(LED3, GPIO_OFF);
        digitalWrite(LED4, GPIO_OFF);
        digitalWrite(LED5, GPIO_OFF);
        ledChargelevelCount++;
        break;
      default:
        ledChargelevelCount = level;
        break;



    }
  }
}

void setup() {
  Serial.begin(9600);
  gpioInit();
  ledBTlevel(1);
  delay(100);
  ledBTlevel(2);
  delay(100);
  ledBTlevel(3);
  delay(100);
  ledBTlevel(4);
  delay(100);
  ledBTlevel(5);
  delay(100);

  digitalWrite(GPIO_BUZZ, HIGH);
  delay(100);
  digitalWrite(GPIO_BUZZ, LOW);
  delay(100);
  digitalWrite(GPIO_BUZZ, HIGH);
  delay(100);
  digitalWrite(GPIO_BUZZ, LOW);
  delay(100);
  while (0) {
    digitalWrite(GPIO_BUZZ, HIGH);
    delay(100);
    digitalWrite(GPIO_BUZZ, LOW);
    delay(100);
    digitalWrite(BT_CHARGE, !digitalRead(BT_CHARGE));
    delay(1000);
  }
}

void loop() {
  if (checkTheBatteryCycleCount < checkTheBatteryCycle) {
    checkTheBatteryCycleCount++;
  } else {
    checkTheBatteryCycleCount = 0;
    level = adcCheckBTValue();
    ledBTlevel(level);//检查电池并直接输出LED
  }
  noHavePower = adcCheckACValue(); //读取AC是否有电力输入,如果有则不开启继电器
  if (noHavePower == 1) {
    if (noAcPowerCountTime > lowPowerBuzzCount) {
      noAcPowerCountTime = 0;
      //Serial.print(noAcPowerCountTime);
      runBuzz(1);
    } else {
      noAcPowerCountTime++;
      //Serial.print(noAcPowerCountTime);
    }
  } else {
    noAcPowerCountTime = 0;
  }
}
