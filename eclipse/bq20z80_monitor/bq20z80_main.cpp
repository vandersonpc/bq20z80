/*
 *  Texas Instrument DQ20Z80 Reader Software
 *
 *  Read data from MacBook Battery Model A1185
 *
 *  2018 By Vanderson Pimenta Carvalho <vandersonpc@gmail.com>
 *
 *  Arduino I2C Interface
 *
 */
#include "Arduino.h"
#include "Wire.h"

/* Software Version */

char const *softare_version = "1.01";

/* BQ20Z80 Address Definition */

#define BQ20Z80  0x0B // i2c Address

/* BQ20Z80 SBS Commadns  definition */

#define BQ_MANUFACTURER_ACCESS      0x00
#define BQ_DEVICE_PART_NUMBER       0x01        // ------
#define BQ_DEVICE_FIRMWARE_VERSION  0x02        // -- Chip specific commands
#define BQ_DEVICE_HARDWARE_VERSION  0x03        // ------
#define BQ_DEVICE_UNSEAL_KEY        0x36720414

#define BQ_DEVICE_SEAL              0x20        // --
#define BQ_LEDS_ON                  0x32        // -- Unsealed commands
#define BQ_LEDS_OFF                 0x33        // --
#define BQ_DISPLAY_ON               0x34        // ------

#define BATTERY_TEMPERATURE         0x08
#define BATTERY_VOLTAGE             0x09
#define BATTERY_CURRENT             0x0A
#define BATTERY_AVERAGE_CURRENT     0x0B
#define BATTERY_ABSOLUTE_SOC        0x0E        // S.O.C = State of Charge
#define BATTERY_REMAIN_CAPACITY     0x0F
#define BATTERY_STATUS              0x16
#define BATTERY_CYCLE_COUNT         0x17
#define BATTERY_DESIGN_CAPACITY     0x18
#define BATTERY_MANUFACTORY_DATE    0x1B
#define BATTERY_SERIAL_NUMBER       0x1C
#define BATTERY_MANUFACTURER_NAME   0x20
#define BATTERY_MODEL               0x21
#define BATTERY_MANUFACTURER_INFO   0x23
#define BATTERY_STATE_OF_HEALTH     0x4F

/* Pin Alert Input Pin */

#define BS_ALERT_PIN                11         // Battery Connected Input
#define BAT_CONNECTED_LED_PIN       13         // LED Pin

/**/

/*
 * Setup ()
 *
 */

void setup() {

  Serial.begin(115200); // Start Serial comms
  while (!Serial);
  pinMode(BS_ALERT_PIN, INPUT_PULLUP);
  pinMode(BAT_CONNECTED_LED_PIN, OUTPUT);
  Wire.begin();         // Start I2C Communication
  delay(200);

  /* Init Message */
  Serial.println("===========================");
  Serial.println("   MacBook Battery Reader  ");
  Serial.print("=====================");
  Serial.print(softare_version);
  Serial.println("==");
  Serial.println("2018 - By Vanderson Pimenta");
  Serial.println();
  Serial.print("DQ20Z80 Address: ");
  Serial.println(BQ20Z80);
  Serial.println("---------------------------");
  Serial.println();
  delay(2000);

}

/***************************** INIT
BASE FUNCIONS *********************/

/*
 * Read String
 *
 */

// pass a pointer to a char[] that can take up to 33 chars
// will return the length of the string received
int readString(uint8_t address, char* result)
{
  int pos = 0;
  int len;

  // Read the length of the string
  Wire.beginTransmission(BQ20Z80);
  Wire.write(address);
  Wire.endTransmission(false);
  Wire.requestFrom(BQ20Z80, 1, true);
  len = Wire.read();    // length of the string
        len++;          // plus one to allow for the length byte on the reread
                        // if len > 32 then the it will be truncated to 32 by requestFrom
                        // Now that we know the length, repeat the read to get all the string data.
                        // we need to write the address again and do a restart so its a valid SMBus transaction
  Wire.beginTransmission(BQ20Z80);
  Wire.write(address);
  Wire.endTransmission(false);
  len = Wire.requestFrom(BQ20Z80, len, true);    // readRequest returns # bytes actually read

        len--;                                   // we won't move the first byte as its not part of the string
  if (len > 0)
  {
                Wire.read();
    for (pos = 0; pos < len; pos++)
      result[pos] = Wire.read();
  }
  result[pos] = '\0';  // append the zero terminator

  return len;
}


/*
 * read 8 bits dara from DQ20Z80
 *
 */
uint8_t dq_read(uint8_t address)
{
  uint8_t registerValue;
  Wire.beginTransmission(BQ20Z80);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(BQ20Z80,1,true);
  registerValue = Wire.read();
  Wire.endTransmission();
  return registerValue;
}

/*
 * Read 16 bits Data fom DQ20Z80
 *
 */

int16_t dq_read16u(uint8_t address)
{
  uint16_t registerValue;
  Wire.beginTransmission(BQ20Z80);
  Wire.write(address);
  Wire.endTransmission(false);
  Wire.requestFrom(BQ20Z80,2,true);
  registerValue = Wire.read();
  registerValue |= (Wire.read()*256);
  return registerValue;
}

/*
 * Read 32 bits data
 *
 */

 uint32_t bq_read32u(uint8_t address)
{
  uint32_t registerValue;
  Wire.beginTransmission(BQ20Z80);
  Wire.write(address);
  Wire.endTransmission(false);
  Wire.requestFrom(BQ20Z80,5,true);
  Wire.read();
  registerValue = Wire.read();
  registerValue |= (Wire.read()<<8);
  registerValue |= (Wire.read() << 16);
  registerValue |= (Wire.read() << 24);
  return registerValue;
}

/***************************** END BASE FUNCIONS *********************/

/*
 * Read Battery Model and Information
 *
 */
 void readBattModel()
 {
  char strModel[33];
  readString(BATTERY_MODEL,strModel);
  Serial.print("Bat Model: ");
  Serial.print(strModel);         // print the character
  Serial.println(""); // new line
 }

 /*
 * Read Battery Information
 *
 */
 void readBattInfo()
 {
   Serial.print("Battery Health: ");
   Serial.print(dq_read16u(BATTERY_MANUFACTURER_INFO), HEX);       // Print Absolute SOC
   Serial.println("");
  //char strInfo[33];
 // readString(BQ_INFO,strInfo);
  //Serial.print("Bat Info: ");
  //Serial.print(strInfo);         // print the character
  //Serial.println(""); // new line
 }


 /*
 * Read Battery Manufacturer Name
 *
 */
 void readBattMName()
 {
  char strInfo[33];
  readString(BATTERY_MANUFACTURER_NAME,strInfo);
  Serial.print("Manufacturer Name: ");
  Serial.print(strInfo);         // print the character
  Serial.println(""); // new line
 }

 /*
  * Read Absolute State of Capacity in %
  *
  */
  void readBattSOC()
  {
   Serial.print("Absolute SOC: ");
   Serial.print(dq_read(BATTERY_ABSOLUTE_SOC));       // Print Absolute SOC
   Serial.println(" %");
  }

 /*
  * Read State of Health in %
  *
  */
  void readBattSOH()
  {
   Serial.print("Battery Health: ");
   Serial.print(dq_read16u(BATTERY_STATE_OF_HEALTH));       // Print Absolute SOC
   Serial.println(" %");
  }


/*
 *   Read Battery Temperature
 *
 */

void readBattTemp()
{
  Serial.print("Temperature: ");
  Serial.print((dq_read16u(BATTERY_TEMPERATURE) *0.1) - 273.15);
}

/*
 * Read Battery Cycles
 *
 */

 void readBattCycles()
{
  Serial.print("Cycle Count: ");
  Serial.print(dq_read16u(BATTERY_CYCLE_COUNT));
  Serial.println("");
}

/*
 * Read Battery Voltage
 *
 */

 void readBattVolts()
{
  Serial.print("Voltage: ");
  Serial.print(dq_read16u(BATTERY_VOLTAGE) *0.001);
  Serial.println(" V");
}

/*
 * Read Battery Current
 *
 */

 void readBattAmps()
{
  Serial.print("Current: ");
  Serial.print(dq_read16u(BATTERY_CURRENT) *0.001);
  Serial.println(" A");
}

/*
 * Read Battery Design Capacity
 *
 */

 void readBattDC()
{

  Serial.print("Design Capacitty: ");
  Serial.print(dq_read16u(BATTERY_DESIGN_CAPACITY));
  Serial.println(" mAH");
}


/*
 * Read Battery Remain Capacity
 *
 */

 void readBattRC()
{

  Serial.print("Remain Capacitty: ");
  Serial.print(dq_read16u(BATTERY_REMAIN_CAPACITY));
  Serial.println(" mAH");
}

/*
 * Read Battery Serial Number
 *
 */

 void readBattSN()
{

  Serial.print("Serial Number: ");
  Serial.print(dq_read16u(BATTERY_SERIAL_NUMBER));
  Serial.println();
}

/*
 * Read Battery Status
 *
 */

 void readBattStatus()
{

  Serial.print("Bat Status Flags: ");
  Serial.print(dq_read16u(BATTERY_STATUS), BIN);
  Serial.println();
}


/*
 * Read Battery Manufacure Date
 *
 */

 void readBattDate()
{
  int16_t fulldate;
  int8_t year;
  int8_t month;
  int8_t day;

  fulldate = dq_read16u(BATTERY_MANUFACTORY_DATE);

  /*
   *  (Year - 1980) x 512 + month x 32 + day
   *
   *  0xbyyyyyyymmmmddddd
   *
   */

  year  = (fulldate >> 9);                        // Year  - First 7 bits
  month = (fulldate & 0xb0000000111111111) >> 4;  // Month - Middle 4 bits
  day   = (fulldate & 0xb0000000000011111);       // Day   - Last 5 bits


  Serial.print("Date [YYYY.MM.DD]: ");
  Serial.print(1980 + year);       // Print Status
   Serial.print(".");
  Serial.print(month);       // Print Status
   Serial.print(".");
  Serial.print(day);       // Print Status
  Serial.println();
}

/*
 * Read Battery Pin
 */

 void readBattPinAlert()
 {
   int pinStatus = digitalRead(BS_ALERT_PIN);
  if (!pinStatus)
  {
     Serial.println("Battery Connected [OK]");
     digitalWrite(BAT_CONNECTED_LED_PIN,HIGH);
  }
  else
   {
     Serial.println("Battery Connected [ERROR]");
     digitalWrite(BAT_CONNECTED_LED_PIN,LOW);
   }
 }

/*
 * Main Loop
 */

void loop() {
  Serial.println();
  readBattPinAlert();
  //readBattInfo();
  readBattModel();
  readBattMName();
  readBattDate();
  readBattSN();
  readBattStatus();
  readBattSOH();
  readBattSOC();
  readBattTemp();
  readBattCycles();
  readBattVolts();
  readBattAmps();
  readBattDC();
  readBattRC();
  delay(2000);      //delay 2 seconds
}
