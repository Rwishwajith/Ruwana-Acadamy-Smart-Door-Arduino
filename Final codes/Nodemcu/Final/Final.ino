
/*
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  #                                                                     #
  #                                                                     #
  #                 Installation :                                      #
  # NodeMCU ESP8266/ESP12E    RFID MFRC522 / RC522                      #
  #         D2       <---------->   SDA/SS                              #
  #         D5       <---------->   SCK                                 #
  #         D7       <---------->   MOSI                                #
  #         D6       <---------->   MISO                                #
  #         GND      <---------->   GND                                 #
  #         D1       <---------->   RST                                 #
  #         3V/3V3   <---------->   3.3V                                #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

//----------------------------------------Include the Libraries-----------------------------------------------------------------------------------------------------------------------------//
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>

#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>---
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
#define FIREBASE_HOST "ruwana-academy.firebaseio.com" //--> URL address of your Firebase Realtime Database. https://classroom-bae15.firebaseio.com/
#define FIREBASE_AUTH "5gbUzUWlTuJeZfzo34PU9a6LZokAjblBGvwPRttg" //--> Your firebase database secret code.   Project Settings > servise acc > secret

#define SS_PIN D2  //--> SDA / SS is connected to pinout D2
#define RST_PIN D1  //--> RST is connected to pinout D1
MFRC522 mfrc522(SS_PIN, RST_PIN);  //--> Create MFRC522 instance.

#define ON_Board_LED 2  //--> Defining an On Board LED, used for indicators when the process of connecting to a wifi router
#define LED_D8 15 //--> Defines an LED Pin. D8 = GPIO15

Servo servo;   //Reaname servo

//----------------------------------------SSID and Password of your WiFi router-------------------------------------------------------------------------------------------------------------//
const char* ssid = "SLT-4G_B3908";            //Router ssid
const char* password = "prolink12345";      //Router password
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//

ESP8266WebServer server(80);  //--> Server on port 80

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;

//-----------------------------------------------------------------------------------------------SETUP--------------------------------------------------------------------------------------//
void setup() {
  Serial.begin(115200); //--> Initialize serial communications with the PC
  
  servo.attach(2); //D4
  servo.write(0);
  
  SPI.begin();      //--> Init SPI bus
  mfrc522.PCD_Init(); //--> Init MFRC522 card
  delay(500);

  WiFi.begin(ssid, password); //--> Connect to your WiFi router
  Serial.println("");

  pinMode(ON_Board_LED, OUTPUT);
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off Led On Board

  pinMode(LED_D8,OUTPUT); //--> LED port Direction output
  digitalWrite(LED_D8, LOW); //--> Turn off Led
  
  //----------------------------------------Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
  Serial.print(".");
  //----------------------------------------Make the On Board Flashing LED on the process of connecting to the wifi router.
  digitalWrite(ON_Board_LED, LOW);
  delay(250);
  digitalWrite(ON_Board_LED, HIGH);
  delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH); //--> Turn off the On Board LED when it is connected to the wifi router.
  //----------------------------------------If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor---------------------------//
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Please tag a card or keychain to see the UID !");
  Serial.println("");

   //----------------------------------------Firebase Realtime Database Configuration-----------------------------------------------------------------------------------------------------//
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}
//-----------------------------------------------------------------------------------------------LOOP---------------------------------------------------------------------------------------//
void loop() {
 
 // readsuccess = getid();

 if (!mfrc522.PICC_IsNewCardPresent()) {  //new card
    return ;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {   //one card
    return ;
  }


  //Serial.print("THE UID OF THE SCANNED CARD IS : ");

  for (int i = 0; i < 4; i++) {
    readcard[i] = mfrc522.uid.uidByte[i]; //storing the UID of the tag in readcard
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
 //  return 1;

  //if (readsuccess) {
    digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;    //Declare object of class HTTPClient

    String UIDresultSend, postData;
    UIDresultSend = StrUID;

    //Post Data
    postData = "UIDresult=" + UIDresultSend;

    http.begin("http://192.168.1.13/pro_/getUID.php");  //Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); //Specify content-type header

    int httpCode = http.POST(postData);   //Send the request
    String payload = http.getString();    //Get the response payload

  // Serial.println(UIDresultSend);  
  //Serial.println(httpCode);   //Print HTTP return code
  //Serial.println(payload);    //Print request response payload

    http.end();  //Close connection
    delay(1000);
    digitalWrite(ON_Board_LED, HIGH);
  
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
 //Show UID on serial monitor
 // Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
  //   Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  //   Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
 // Serial.println();
  //Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "47 24 34 33") //change here the UID of the card/cards that you want to give access
  {
            //----------------------------------------Get data from the Firebase Realtime Database-----------------------------------------//
               String getData1 = Firebase.getString("Information/47243433/Fee");  //change as your firebase path
               String getData2 = Firebase.getString("Information/47243433/Subject"); //change as your firebase path
  
              // Serial.print("Email: ");
              // Serial.println(getData1);
              // Serial.print("Mobile: ");
              // Serial.println(getData2);
  
               if (Firebase.failed()) {                 // Conditions for handling errors.
               Serial.print("Getting /Stat failed :");
               Serial.println(Firebase.error()); 
               delay(500); 
               return;
               }
        //----------------------------------------Conditions for open the door--------------------------------------------------------//
               if ((getData1 == "YES") || (getData2 == "YES") || (getData1 == "yes")) {
                    digitalWrite(LED_D8, HIGH);
                    servo.write(180);
                    Serial.println("Authorized access");
                  //  Serial.println("Door Open 5Sec..");
                  //  Serial.println();
                    delay(5000);
                    digitalWrite(LED_D8, LOW);
                    servo.write(0);
                 //   Serial.println("Door Close");
                    delay(1000);
                    }
               else{ Serial.println(" Access denied");
                }      
              }
 
 
  else if (content.substring(1) == "B7 60 64 33") //change here the UID of the card/cards that you want to give access
  {

        //----------------------------------------Get data from the Firebase Realtime Database-----------------------------------------//
               String getData3 = Firebase.getString("Information/B9D09D84/Fee");  //change as your firebase path https://ruwana-academy.firebaseio.com/Information/B9D09D84   Information/B9D09D84/Tute
               String getData4 = Firebase.getString("Information/B9D09D84/Subject"); //change as your firebase path Information/B9D09D84/Fee
  
              // Serial.print("Email: ");
              // Serial.println(getData3);
              // Serial.print("Mobile: ");
              // Serial.println(getData4);
  
               if (Firebase.failed()) {                 // Conditions for handling errors.
               Serial.print("Getting /Stat failed :");
               Serial.println(Firebase.error()); 
               delay(500); 
               return;
               }
        //----------------------------------------Conditions for open the door--------------------------------------------------------//
               if ((getData3 == "YES") || (getData4 == "YES") || (getData3 == "yes")) {
                    digitalWrite(LED_D8, HIGH);
                    servo.write(180);
                    Serial.println("Authorized access");
                  //  Serial.println("Door Open 5Sec..");
                  //  Serial.println();
                    delay(5000);
                    digitalWrite(LED_D8, LOW);
                    servo.write(0);
                  //  Serial.println("Door Close");
                    delay(1000);
                    }
               else{ Serial.println(" Access denied");
                }      
              }

 else if (content.substring(1) == "B9 D0 9D 84") //change here the UID of the card/cards that you want to give access
  {

        //----------------------------------------Get data from the Firebase Realtime Database-----------------------------------------//
               String getData5 = Firebase.getString("Information/B9D09D84/Fee");  //change as your firebase path
               String getData6 = Firebase.getString("Information/B9D09D84/Subject");  //change as your firebase path
  
              // Serial.print("Email: ");
              // Serial.println(getData5);
              // Serial.print("Mobile: ");
              // Serial.println(getData6);
  
               if (Firebase.failed()) {                 // Conditions for handling errors.
               Serial.print("Getting /Stat failed :");
               Serial.println(Firebase.error()); 
               delay(500); 
               return;
               }
        //----------------------------------------Conditions for open the door--------------------------------------------------------//
               if ((getData5 == "YES") || (getData6 == "YES") || (getData5 == "yes") ) {
                    digitalWrite(LED_D8, HIGH);
                    servo.write(180);
                    Serial.println("Authorized access");
                    //Serial.println("Door Open 5Sec..");
                    //Serial.println();
                    delay(5000);
                    digitalWrite(LED_D8, LOW);
                    servo.write(0);
                  //  Serial.println("Door Close");
                    delay(1000);
                    }
                else{ Serial.println(" Access denied");
                }    
              }             
 

  else if (content.substring(1) == "DA 40 B8 12") //change here the UID of the card/cards that you want to give access
  {

        //----------------------------------------Get data from the Firebase Realtime Database-----------------------------------------//
               String getData7 = Firebase.getString("Information/DA40B812/Fee");  //change as your firebase path
               String getData8 = Firebase.getString("Information/DA40B812/Subject");  //change as your firebase path
  
             //  Serial.print("Email: ");
             //  Serial.println(getData7);
             //  Serial.print("Mobile: ");
             //  Serial.println(getData8);
  
               if (Firebase.failed()) {                 // Conditions for handling errors.
               Serial.print("Getting /Stat failed :");
               Serial.println(Firebase.error()); 
               delay(500); 
               return;
               }
        //----------------------------------------Conditions for open the door--------------------------------------------------------//
               if ((getData7 == "YES") || (getData8 == "YES") || (getData7 == "yes")) {
                    digitalWrite(LED_D8, HIGH);
                    servo.write(180);
                    Serial.println("Authorized access");
                  //  Serial.println("Door Open 5Sec..");
                  //  Serial.println();
                    delay(5000);
                    digitalWrite(LED_D8, LOW);
                    servo.write(0);
                  //  Serial.println("Door Close");
                    delay(1000);
                    }
               else{ Serial.println(" Access denied");
                }      
              } 

 else   {
    Serial.println(" Access denied");
    delay(500);
  }
}
//----------------------------------------Procedure to change the result of reading an array UID into a string------------------------------------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
