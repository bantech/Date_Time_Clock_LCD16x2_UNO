/*
 * Time_NTP.pde
 * Example showing time sync to NTP time source
 *
 * This sketch uses the Ethernet library
 */
 
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <LiquidCrystal.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 

LiquidCrystal lcd(8,9,4,5,6,7);

// NTP Servers:

//const char timeServer[] = "time.nist.gov"; // time.nist.gov NTP server

IPAddress timeServer(132, 163, 96, 6); // AU ntp.cs.mu.

const int timeZone = 10;     // AEDT
int s=0;
int m=61;
int r=0;
EthernetUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

void setup() 
{

   //aff lcd
    lcd.begin(16,2); // set up the LCD's number of columns and rows:
    delay(200);

    //premiere requete ntp 
      lcd.setCursor(0,0);
    lcd.print("Koala Technology");
    lcd.setCursor(0,1);
    lcd.print(" NTP Date Time");
    delay(2500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" Init NTP time");
    
 // Serial.begin(115200);
 // while (!Serial) ; // Needed for Leonardo only
 // delay(250);
 // Serial.println(" TimeNTP Clock");
//  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
 //   while (1) {
  //    Serial.println("Failed to configure Ethernet using DHCP");
      delay(10000);
          lcd.setCursor(0,1);
          lcd.print("NTP Failed");
      }
  delay(500);
            lcd.setCursor(0,1);
          lcd.print("IP:");
          lcd.print(Ethernet.localIP());
 // Serial.print("IP number assigned by DHCP is ");
 // Serial.println(Ethernet.localIP());
  Udp.begin(localPort);
 // Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  delay(900);
  lcd.clear();
}

time_t prevDisplay = 0; // when the digital clock was displayed

void loop()
{  
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) { //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();  
      lcd16();
    }
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
/*  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); */
}

void printDigits(int digits){
  // utility for digital clock display: prints preceding colon and leading 0
 /* Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);*/
}

void lcd16()
{
  // affichage sur aff lcd
    lcd.setCursor(0, 0); // line 1 date 
    lcd.print("Date: ");
        if (day()<10)
      lcd.print("0");
    lcd.print(day());
    lcd.print("/");
    if (month()<10)
      lcd.print("0");
    lcd.print(month());
    lcd.print("/");
    lcd.print(year());
    lcd.setCursor(0,1); // line 2 time
    lcd.print("Time: ");
    lcd.print(hour());
    lcd.print(":");
    if (minute()<10)
      lcd.print("0");
    lcd.print(minute());
   
    r = second(2)-m;
    s = 1000 * r;
   // lcd.print(s);
    delay(s);
    
}

/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
