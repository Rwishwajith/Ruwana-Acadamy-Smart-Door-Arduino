#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const char* a = "Authorized access"; 
const char* b = "Access denied";

void setup() {
  pinMode(8, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.write("Team-Skyblue");
  delay(4000);
  lcd.clear();
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
}

void loop() { 
  lcd.setCursor(0, 0);
  lcd.scrollDisplayLeft();
  lcd.write("RUWANA ACADEMY - KULIYAPITIYA");
  delay(500); 
  
  if (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s == a) {
          lcd.clear();
          digitalWrite(8, HIGH);   
          lcd.setCursor(4, 0);
          lcd.write("Welcome");
          lcd.setCursor(4, 1);
          lcd.write("5 Sec...");
          delay(5000); 
          lcd.clear();             
          digitalWrite(8, LOW);   
          delay(1000);  
          
       //   lcd.scrollDisplayLeft();
       //   delay(1000);
          }
          
       else if (s == b){
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.write("Access Denied");
          delay(5000);
          lcd.clear();
          digitalWrite(8, LOW);   
          delay(1000);  
          }
      
      Serial.print(s);
  }
  
}
