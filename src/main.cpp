#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPIFlash.h>
#include <SPI.h>

//******************Green Antenna*************************************************
// #define NODEID        2    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
// #define NETWORKID     100  //the same on all nodes that talk to each other (range up to 255)
// #define GATEWAYID     1

//******************White Antenna*************************************************
#define NODEID        1    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
#define NETWORKID     100  //the same on all nodes that talk to each other (range up to 255)
#define GATEWAYID     2 //todo change variable name to node 2

#define FREQUENCY   RF69_433MHZ

#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!

#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI      -80
//*********************************************************************************************
#define SERIAL_BAUD   115200

const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  6;      // the number of the LED pin
int buttonState = 0;
char buff[20];
byte sendSize=0;
int timesButtonPressed = 0;
SPIFlash flash(SS_FLASHMEM, 0xEF30); //EF30 for 4mbit  Windbond chip (W25X40CL)

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

void setup() {
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.encrypt(ENCRYPTKEY);
  //radio.setFrequency(919000000); //set frequency to some custom frequency

//Auto Transmission Control - dials down transmit power to save battery (-100 is the noise floor, -90 is still pretty good)
//For indoor nodes that are pretty static and at pretty stable temperatures (like a MotionMote) -90dBm is quite safe
//For more variable nodes that can expect to move or experience larger temp drifts a lower margin like -70 to -80 would probably be better
//Always test your ATC mote in the edge cases in your own environment to ensure ATC will perform as you expect
#ifdef ENABLE_ATC
  radio.enableAutoPower(ATC_RSSI);
#endif

  char buff[50];
  sprintf(buff, "\nTransmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
  Serial.println(buff);

  if (flash.initialize())
  {
    Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
    flash.readUniqueId();
    for (byte i=0;i<8;i++)
    {
      Serial.print(flash.UNIQUEID[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
  else
    Serial.println("SPI Flash MEM not found (is chip soldered?)...");

#ifdef ENABLE_ATC
  Serial.println("RFM69_ATC Enabled (Auto Transmission Control)\n");
#endif
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
  //process any serial input
  buttonState = digitalRead(buttonPin);
  //todo implement millis so that code does not read false positives
  if(digitalRead(buttonPin) == HIGH){
    timesButtonPressed = timesButtonPressed + 15;
    if(timesButtonPressed == 15){
      Serial.print("Times Button Pressed: ");
      Serial.println(timesButtonPressed);
      //todo light 1 LED
    }else if(timesButtonPressed == 30){
      Serial.print("Times Button Pressed: ");
      Serial.println(timesButtonPressed);
      //todo light 2 LEDs
    }else if(timesButtonPressed == 45){
      Serial.print("Times Button Pressed: ");
      Serial.println(timesButtonPressed);
      //todo light 3 LEDs
    }else if(timesButtonPressed == 60){
      Serial.print("Times Button Pressed: ");
      Serial.println(timesButtonPressed);
      //todo light 4 LEDs
    }else if(timesButtonPressed > 60){
      timesButtonPressed = 0;
      Serial.print("Times Button Pressed: ");
      Serial.println(timesButtonPressed);
      //todo turn off all LEDs
    }
    delay(500);
  }
  buttonState = 0;

  // if(buttonState == HIGH){
  //   Serial.print("Sending[");
  //   Serial.print("msg");
  //   Serial.print("]: ");
  //     Serial.print("moty");
  //   if (radio.sendWithRetry(GATEWAYID, "moty", 4)){
  //     Serial.print(" ok!");
  //   }
  //   else{
  //      Serial.print(" nothing...");
  //   }
  //   Serial.println();
  //   Blink(LED_BUILTIN,3);
  //     delay(500);
  // }

  if (Serial.available() > 0)
  {
    char input = Serial.read();
    if (input == 's'){
      Serial.print("Sending[");
      Serial.print("msg");
      Serial.print("]: ");
        Serial.print("moaty");
      if (radio.sendWithRetry(GATEWAYID, "moaty", 5)){
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
    Blink(ledPin, 500);
    Serial.println();
  }
}
