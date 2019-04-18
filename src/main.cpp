#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPIFlash.h>
#include <SPI.h>

//******************Green Antenna*************************************************
#define NODEID        2    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
#define SECONDNODEID     1

//******************White Antenna*************************************************
// #define NODEID        1    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
// #define SECONDNODEID     2 //todo change variable name to node 2

#define FREQUENCY   RF69_433MHZ
#define NETWORKID     100  //the same on all nodes that talk to each other (range up to 255)

#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!

#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI      -80
//*********************************************************************************************

#define SERIAL_BAUD   115200

const int ledPin15Minutes =  3;
const int ledPin30Minutes =  4;
const int ledPin45Minutes =  5;
const int ledPin60Minutes =  6;
const int ledPinStatus =  7;

const int buttonPinIncrementTime = 14;
const int buttonPinSendMessage = 15;
const int buttonPinReset = 16;

char buff[20];
int timeTillTea = 0;

RFM69_ATC radio;
// RFM69 radio;

void setup() {

  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
  radio.encrypt(ENCRYPTKEY);
  radio.enableAutoPower(ATC_RSSI);
  Serial.println("RFM69_ATC Enabled (Auto Transmission Control)\n");

  pinMode(buttonPinIncrementTime, INPUT);
  pinMode(buttonPinSendMessage, INPUT);
  pinMode(buttonPinReset, INPUT);

  pinMode(ledPin15Minutes, OUTPUT);
  pinMode(ledPin30Minutes, OUTPUT);
  pinMode(ledPin45Minutes, OUTPUT);
  pinMode(ledPin60Minutes, OUTPUT);
  pinMode(ledPinStatus, OUTPUT);

  digitalWrite(ledPin15Minutes, LOW);
  digitalWrite(ledPin30Minutes, LOW);
  digitalWrite(ledPin45Minutes, LOW);
  digitalWrite(ledPin60Minutes, LOW);
  digitalWrite(ledPinStatus, LOW);
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

long lastPeriod = 0;
void loop() {
  //todo implement millis so that code does not read false positives
  if(digitalRead(buttonPinIncrementTime) == HIGH){
    timeTillTea = timeTillTea + 15;
    if(timeTillTea == 15){
      Serial.print("Times Button Pressed: ");
      Serial.println(timeTillTea);
      digitalWrite(ledPin15Minutes, HIGH);
      digitalWrite(ledPin30Minutes, LOW);
      digitalWrite(ledPin45Minutes, LOW);
      digitalWrite(ledPin60Minutes, LOW);
      digitalWrite(ledPinStatus, LOW);
    }else if(timeTillTea == 30){
      Serial.print("Times Button Pressed: ");
      Serial.println(timeTillTea);
      digitalWrite(ledPin15Minutes, HIGH);
      digitalWrite(ledPin30Minutes, HIGH);
      digitalWrite(ledPin45Minutes, LOW);
      digitalWrite(ledPin60Minutes, LOW);
      digitalWrite(ledPinStatus, LOW);
    }else if(timeTillTea == 45){
      Serial.print("Times Button Pressed: ");
      Serial.println(timeTillTea);
      digitalWrite(ledPin15Minutes, HIGH);
      digitalWrite(ledPin30Minutes, HIGH);
      digitalWrite(ledPin45Minutes, HIGH);
      digitalWrite(ledPin60Minutes, LOW);
      digitalWrite(ledPinStatus, LOW);
    }else if(timeTillTea == 60){
      Serial.print("Times Button Pressed: ");
      Serial.println(timeTillTea);
      digitalWrite(ledPin15Minutes, HIGH);
      digitalWrite(ledPin30Minutes, HIGH);
      digitalWrite(ledPin45Minutes, HIGH);
      digitalWrite(ledPin60Minutes, HIGH);
      digitalWrite(ledPinStatus, LOW);
    }else if(timeTillTea > 60){
      timeTillTea = 0;
      Serial.print("Times Button Pressed: ");
      Serial.println(timeTillTea);
      digitalWrite(ledPin15Minutes, LOW);
      digitalWrite(ledPin30Minutes, LOW);
      digitalWrite(ledPin45Minutes, LOW);
      digitalWrite(ledPin60Minutes, LOW);
      digitalWrite(ledPinStatus, LOW);
    }
    delay(200);
  }

  if(digitalRead(buttonPinSendMessage) == HIGH){
    Serial.print("Sending[");
    Serial.print("msg");
    Serial.print("]: ");
    sprintf(buff, "moty: %d", timeTillTea);
    byte buffLen=strlen(buff);
    Serial.print(buff);
    if (radio.sendWithRetry(SECONDNODEID, buff, buffLen)){
      Blink(ledPinStatus,500);
      Serial.print(" ok!");
    }
    else{
      Serial.print(" nothing...");
      Blink(ledPin15Minutes,500);
      delay(300);
      Blink(ledPin15Minutes,500);
      delay(300);
      Blink(ledPin15Minutes,500);
      delay(300);
    }
    Serial.println();
    Blink(LED_BUILTIN,3);
    delay(200);
  }


    //process any serial input
  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 's'){
      Serial.print("Sending[");
      Serial.print("msg");
      Serial.print("]: ");
        Serial.print("moaty");
      if (radio.sendWithRetry(SECONDNODEID, "moaty", 5)){
        Serial.print(" ok!");
      }
      else{
         Serial.print(" nothing...");
      }
      Serial.println();
      Blink(LED_BUILTIN,3);
    }

    if (input == 't')
    {
      byte temperature =  radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
      byte fTemp = 1.8 * temperature + 32; // 9/5=1.8
      Serial.print( "Radio Temp is ");
      Serial.print(temperature);
      Serial.print("C, ");
      Serial.print(fTemp); //converting to F loses some resolution, obvious when C is on edge between 2 values (ie 26C=78F, 27C=80F)
      Serial.println('F');
    }
  }

  //check for any received packets
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
    }
    Blink(LED_BUILTIN,3 );
    Blink(ledPinStatus, 500);
    Serial.println();
  }
}
