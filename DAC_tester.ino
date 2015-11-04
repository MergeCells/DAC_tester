
/*
 * 
 * 
 * 
 * 
 * 
*/


#include <LiquidCrystal.h>
// for LCD shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 12); // (rx, tx)

byte bow0[8] = {
  0b00100,
  0b00010,
  0b00001,
  0b10000,
  0b00001,
  0b00010,
  0b00100,
  0b00000
};

byte bow1[8] = {
  0b00010,
  0b00001,
  0b10000,
  0b01000,
  0b10000,
  0b00001,
  0b00010,
  0b00000
};

byte bow2[8] = {
  0b00001,
  0b10000,
  0b01000,
  0b00100,
  0b01000,
  0b10000,
  0b00001,
  0b00000
};

byte bow3[8] = {
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00100,
  0b01000,
  0b10000,
  0b00000
};


byte bow4[8] = {
  0b00100,
  0b01000,
  0b10000,
  0b00001,
  0b10000,
  0b01000,
  0b00100,
  0b00000
};

byte bow5[8] = {
  0b01000,
  0b10000,
  0b00001,
  0b00010,
  0b00001,
  0b10000,
  0b01000,
  0b00000
};

byte bow6[8] = {
  0b10000,
  0b00001,
  0b00010,
  0b00100,
  0b00010,
  0b00001,
  0b10000,
  0b00000
};

byte bow7[8] = {
  0b00001,
  0b00010,
  0b00100,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000
};



// false:noChange true:Changed
bool dataState = false;

// colum,row
int cursorState[2] = {0, 0};

int data11 = 0;
int data12 = 0;
int data13 = 0;
int data21 = 0;
int data22 = 0;
int data23 = 0;


void setup() {
  
  pinMode(2,OUTPUT);
  digitalWrite(2,LOW);
  
  pinMode(3,OUTPUT);
  digitalWrite(3,LOW);

  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.begin(16, 2);


  lcd.print("  HORIO  JAPAN");
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print("DAC Tester");
  lcd.createChar(0, bow0);
  lcd.createChar(1, bow1);
  lcd.createChar(2, bow2);
  lcd.createChar(3, bow3);
  lcd.createChar(4, bow4);
  lcd.createChar(5, bow5);
  lcd.createChar(6, bow6);
  lcd.createChar(7, bow7);
  delay(1500);
  lcd.setCursor(0, 0);
  lcd.print("Software serial");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("3GND 11:RX 12:TX");
  delay(3000);
  lcd.clear();

  // シリアル通信のグランドに使用
  //pinMode(2, OUTPUT);
  //digitalWrite(2, LOW);

  data11 = 1500;
  data12 = 0;
  data13 = 3000;
  data21 = 0;
  data22 = -1500;
  data23 = 3000;

  dataState = true;
}


void loop() {
  if (dataState) {
    lcd.noBlink();

    lcd.setCursor(0, 0);
    showData(data11);
    lcd.setCursor(5, 0);
    showData(data12);
    lcd.setCursor(10, 0);
    showData(data13);
    lcd.setCursor(15, 0);
    lcd.write(0x3E);

    lcd.setCursor(0, 1);
    showData(data21);
    lcd.setCursor(5, 1);
    showData(data22);
    lcd.setCursor(10, 1);
    showData(data23);
    lcd.setCursor(15, 1);
    lcd.write(0x3E);

    dataState = false;
    lcd.blink();
  }
  read_LCD_buttons();
  lcd.setCursor(cursorState[0], cursorState[1]);

  while (mySerial.available())
    Serial.write(mySerial.read());
  while (Serial.available())
    mySerial.write(Serial.read());
}

void showData(int data) {
  if (data >= 0)lcd.print(" ");
  else lcd.print("-");

  if (abs(data) < 10)  lcd.print("000");
  else if (abs(data) < 100)  lcd.print("00");
  else if (abs(data) < 1000)  lcd.print("0");

  if (data >= 0)lcd.print(data);
  else lcd.print(-data);
}

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int delayMSec = 200;
int adc_key_in = 0;

void read_LCD_buttons() {
  int buttonState = 0;
  adc_key_in = analogRead(A0);
  if ( abs(adc_key_in - analogRead(A0)) > 20 ) return ;
  if (adc_key_in > 1000) buttonState = btnNONE;
  else if (adc_key_in < 50)   buttonState = btnRIGHT;
  else if (adc_key_in < 250)  buttonState = btnUP;
  else if (adc_key_in < 450)  buttonState = btnDOWN;
  else if (adc_key_in < 650)  buttonState = btnLEFT;
  else if (adc_key_in < 850)  buttonState = btnSELECT;

  dataState = true;

  switch (buttonState) {
    case btnNONE:
      dataState = false;
      break;

    case btnRIGHT:
      cursorState[0] += 1;
      if (cursorState[0] > 15) cursorState[0] -= 16;
      delay(delayMSec);
      break;

    case btnUP:
      if (cursorState[1] == 0) {
        if (cursorState[0] == 15) {
          transmit();
        } else if (cursorState[0] == 0) {
          data11 *= (-1);
        } else if (cursorState[0] == 5) {
          data12 *= (-1);
        } else if (cursorState[0] == 10) {
          data13 = 8000;
        } else if (cursorState[0] < 5) {
          data11 += ((data11<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 10) {
          data12 += ((data12<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 15) {
          data13 += ((data13<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        }
      } else if (cursorState[1] == 1) {
        if (cursorState[0] == 15) {
          transmit();
        } else if (cursorState[0] == 0) {
          data21 *= (-1);
        } else if (cursorState[0] == 5) {
          data22 *= (-1);
        } else if (cursorState[0] == 10) {
          data23 = 8000;
        } else if (cursorState[0] < 5) {
          data21 += ((data21<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 10) {
          data22 += ((data22<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 15) {
          data23 += ((data23<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        }
      }
      delay(delayMSec);
      break;

    case btnDOWN:
      if (cursorState[1] == 0) {
        if (cursorState[0] == 15) {
          stopNow();
        } else if (cursorState[0] == 0) {
          data11 *= (-1);
        } else if (cursorState[0] == 5) {
          data12 *= (-1);
        } else if (cursorState[0] == 10) {
          data13 = 0;
        } else if (cursorState[0] < 5) {
          data11 -= ((data11<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 10) {
          data12 -= ((data12<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 15) {
          data13 -= ((data13<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        }
      } else if (cursorState[1] == 1) {
        if (cursorState[0] == 15) {
          stopNow();
        } else if (cursorState[0] == 0) {
          data21 *= (-1);
        } else if (cursorState[0] == 5) {
          data22 *= (-1);
        } else if (cursorState[0] == 10) {
          data23 = 0;
        } else if (cursorState[0] < 5) {
          data21 -= ((data21<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 10) {
          data22 -= ((data22<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        } else if (cursorState[0] < 15) {
          data23 -= ((data23<0)?-1:1) * ((4 - cursorState[0] % 5)>2?1000:((4 - cursorState[0] % 5)>1?100:((4 - cursorState[0] % 5)>0?10:1)));
        }
      }
      delay(delayMSec);
      break;

    case btnLEFT:
      cursorState[0] -= 1;
      if (cursorState[0] < 0) cursorState[0] += 16;
      delay(delayMSec);
      break;

    case btnSELECT:
      if (cursorState[1] == 0)cursorState[1] = 1;
      else if (cursorState[1] == 1)cursorState[1] = 0;
      delay(delayMSec);
      break;

    default:
      break;
  }

  data11 = (data11 < -2000) ? -2000 : (data11 > 2000) ? 2000 : data11;
  data12 = (data12 < -2000) ? -2000 : (data12 > 2000) ? 2000 : data12;
  data13 = (data13 < 0) ? 0 : (data13 > 8000) ? 8000 : data13;
  data21 = (data21 < -2000) ? -2000 : (data21 > 2000) ? 2000 : data21;
  data22 = (data22 < -2000) ? -2000 : (data22 > 2000) ? 2000 : data22;
  data23 = (data23 < 0) ? 0 : (data23 > 8000) ? 8000 : data23;

}


int transmitDelay = 50;
String transmitData = "";

void makeString(int data) {
  if (data >= 0) transmitData += "0";
  else transmitData += "1";

  if (abs(data) < 10)  transmitData += "000";
  else if (abs(data) < 100)  transmitData += "00";
  else if (abs(data) < 1000)  transmitData += "0";

  if (data >= 0) transmitData += data;
  else transmitData += (- data);
}

void transmit() {
  if (cursorState[1] == 0) {
    transmitData = "j";
    transmitData += "1";
    makeString(data11);
    makeString(data12);
    makeString(data13);
    transmitData += "x";
    mySerial.println(transmitData);
  } else if (cursorState[1] == 1) {
    transmitData = "j";
    transmitData += "1";
    makeString(data21);
    makeString(data22);
    makeString(data23);
    transmitData += "x";
    mySerial.println(transmitData);
  }

  lcd.noBlink();
  delay(transmitDelay);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write(0x3E);
  delay(transmitDelay);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)0);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)1);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)2);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)3);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write(0x3E);
  delay(transmitDelay);
  lcd.blink();
}

void stopNow(){
  mySerial.println("j0000000000000000x");

  lcd.noBlink();
  delay(transmitDelay);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write(0x3C);
  delay(transmitDelay);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)4);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)5);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)6);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write((byte)7);
  delay(transmitDelay);
  lcd.setCursor(cursorState[0], cursorState[1]);
  lcd.write(0x3C);
  delay(transmitDelay);
  lcd.blink();
}


