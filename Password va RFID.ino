
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
String tagUID = "53 A4 37 3E";

MFRC522 rfid(SS_PIN, RST_PIN); 

MFRC522::MIFARE_Key key;

SoftwareSerial mySerial(3, 4);

LiquidCrystal_I2C lcd(0x27, 20, 4);
char password[3] = {'1', '2', '3'};// Ban co the thay doi mat khau
int count = 0;
int count_pass = 0;
int count_RFID = 0;
int position = 0;
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {  A0, A1, A2, A3 };
byte colPins[COLS] = {  2, 1, 0 };
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


// Tao 2 den
int RedpinLock = 7;
int GreenpinUnlock = 6;

void setup()
{


  pinMode(RedpinLock, OUTPUT);
  pinMode(GreenpinUnlock, OUTPUT);
  lcd.init();                      // khoi chay lcd
  lcd.init();


  SPI.begin();

  rfid.PCD_Init();

}


void loop()
{
  // keypad
  char key = keypad.getKey();
  if (count == 0 && count_RFID == 0) {
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Password or RFID");
    lcd.setCursor(0, 1);
  }
  if (key != NO_KEY) {

    lcd.print("*");
    count++;
    if (key == password[position])
    {
      position ++;
    }
    //Khi nhap dung mat khau
    if (position == 3)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Lock Open");
      digitalWrite(GreenpinUnlock, HIGH);
      count = 0;
      position = 0;
      delay(5000);
      digitalWrite(GreenpinUnlock, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Locked");
      delay(3000);

    }
    
    delay(100);
    //Khi sai maat khau
    if (count % 3 == 0 && count < 9 && position != 3 && count != 0)
    {

      position = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Password!");
      digitalWrite(RedpinLock, HIGH);
      delay(5000);
      digitalWrite(RedpinLock, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Try Again:");
      lcd.setCursor(0, 1);
    }
  }
  //Khi sai qua 3 lan
  if (count == 9) {

    Alarm();
    for ( int i = 0 ; i < 10; i++)
    {
      LedAlarm();
    }
    delay(5000);

    count = 0;
    position = 0;
  }

  //rfid
  // Kiem tra the tu
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  if ( ! rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);


  rfid.PICC_HaltA(); // Halt PICC

  String tag = "";
  for (byte j = 0; j < rfid.uid.size; j++)
  {
    tag.concat(String(rfid.uid.uidByte[j] < 0x10 ? " 0" : " "));
    tag.concat(String(rfid.uid.uidByte[j], HEX));
  }
  tag.toUpperCase();
  //Khi dung the
  if (tag.substring(1) == tagUID)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Valid tag");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Lock Open");
    digitalWrite(GreenpinUnlock, HIGH);
    count_RFID = 0;
    position = 0;
    delay(5000);
    digitalWrite(GreenpinUnlock, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locked");
    delay(3000);
  }
  
  //Khi sai the
  else {
    lcd.clear();

    digitalWrite(RedpinLock, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("Invalid tag!");

    delay(3000);
    digitalWrite(RedpinLock, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Try Again!");
    count_RFID ++;
  }
  //Khi sai qua 3 lan
  if ( count_RFID == 3)
  {

    Alarm();
    for ( int i = 0 ; i < 10; i++)
    {
      LedAlarm();
    }
    delay(5000);

    count_RFID = 0;
  }
}

//Ham canh bao bang den
void LedAlarm()
{
  digitalWrite(RedpinLock, HIGH);
  delay(800);
  digitalWrite(RedpinLock, LOW);
  delay(600);
}

//Ham gui tin nhan ve
void Alarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm!");
  mySerial.begin(9600);
  mySerial.println("AT+CMGF=1");  
  delay(5000);
  mySerial.println("AT+CMGS=\"+0582884589\"");
  delay(5000);
  mySerial.println(" co ai dang co gang dot nhap vao nha ban");
  mySerial.println((char)26);
}

void LockedPosition(int locked)
{
  if (locked)
  {
    digitalWrite(GreenpinUnlock, LOW);

  }
  else
  {
    digitalWrite(GreenpinUnlock, HIGH);

  }
}
