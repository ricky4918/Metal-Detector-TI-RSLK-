#ifndef __CC3200R1M1RGC__
#include <SPI.h>
#endif
#include <WiFi.h>


const int left_slp_pin = 31;  // P3.7
const int left_dir_pin = 14;  // P1.7
const int left_pwm_pin = 40;  // P2.7
const int right_slp_pin = 11;  // P3.6
const int right_dir_pin = 15;  // P1.6
const int right_pwm_pin = 39;  // P2.6


const int Alarm = A8;

// network name also called SSID
char ssid[] = "cmpe195f"; //it needs to be changed whenever we use different wifi
// network password
char password[] = "12341234";//it needs to be changed whenever we use different wifi

int keyIndex = 0;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);      // initialize serial communication
  

  pinMode(left_slp_pin, OUTPUT);      // sets the digital pin as output
  pinMode(left_dir_pin, OUTPUT);      // sets the digital pin as output
  pinMode(left_pwm_pin, OUTPUT);      // sets the digital pin as output

  pinMode(right_slp_pin, OUTPUT);      // sets the digital pin as output
  pinMode(right_dir_pin, OUTPUT);      // sets the digital pin as output
  pinMode(right_pwm_pin, OUTPUT);

  
  pinMode(Alarm, INPUT_PULLUP);


  Serial.print("Attempting to connect to Network named: ");

  Serial.println(ssid); 

  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  
 
  printWifiStatus(); //Printing WiFi status

  Serial.println("Starting webserver on port 80");
  server.begin();                          
  Serial.println("Webserver started!");
 
  
  
}



void loop() {
  int i = 0;
  
  WiFiClient client = server.available();   
  int AlarmState = digitalRead(Alarm);
        Serial.println(AlarmState); //getting signal when metal detector alarms
        delay(1000);
         


  if (client) {                            
    Serial.println("new client");          
    char buffer[150] = {0};                 // make a buffer to hold incoming data
    while (client.connected()) {           
      if (client.available()) {         
        char c = client.read();          
        Serial.write(c);                    // print it out the serial monitor if the byte is a newline character
        if (c == '\n') {                   

          
          if (strlen(buffer) == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<html><head><title>Energia CC3200 WiFi Web Server</title></head><body align=center>");
            client.println("<h1 align=center><font color=\"red\">CMPE195F GROUP4 Project Website</font></h1>");
            client.println("Move <button onclick=\"location.href='/F'\">FOWARD</button><br>");
            client.println("Move <button onclick=\"location.href='/L'\">LEFT</button>");
            client.println("<button onclick=\"location.href='/S'\">STOP</button>");
            client.println("<button onclick=\"location.href='/R'\">RIGHT</button><br>");
            client.println("Move <button onclick=\"location.href='/B'\">BACKWARD</button><br>");
            client.println("<button onclick=\"location.href='/M'\">Metal Detecting</button><br>");
            client.println("Alarm Signal: "); client.println(AlarmState);

            
             

            
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear the buffer:
            memset(buffer, 0, 150);
            i = 0;
          }
        }
        else if (c != '\r') {   
          buffer[i++] = c;     
        }

        //move foward
        if (endsWith(buffer, "GET /F")) {
         turnOnMotor();
         setLeftMotorSpeed(25);
         setRightMotorSpeed(25);
         runMotorForward();
        }
        //move left
        if (endsWith(buffer, "GET /L")) {
         setLeftMotorSpeed(0);
         setRightMotorSpeed(25);
        }
        //stop moving
        if (endsWith(buffer, "GET /S")) {
         setLeftMotorSpeed(0);
         setRightMotorSpeed(0);
         turnOffMotor();
        }

        //move right
        if (endsWith(buffer, "GET /R")) {
         setLeftMotorSpeed(25);
         setRightMotorSpeed(0);
        }
        //move backward
        if (endsWith(buffer, "GET /B")) {
         setLeftMotorSpeed(10);
         setRightMotorSpeed(10);
         runMotorBackward();
         turnOnMotor();
        }

        
       
          
      
        
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
    
  }
}


//a way to check if one array ends with another array
boolean endsWith(char* inString, const char* compString) {
  int compLength = strlen(compString);
  int strLength = strlen(inString);
  
  //compare the last "compLength" values of the inString
  int i;
  for (i = 0; i < compLength; i++) {
    char a = inString[(strLength - 1) - i];
    char b = compString[(compLength - 1) - i];
    if (a != b) {
      return false;
    }
  }
  return true;
}

void printWifiStatus() {
  
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

 
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void setLeftMotorSpeed(int speed) {
  int val = map(speed, 0, 100, 0, 255);
  analogWrite(left_pwm_pin, val);
}

void setRightMotorSpeed(int speed) {
  int val = map(speed, 0, 100, 0, 255);
  analogWrite(right_pwm_pin, val);
}

void turnOnMotor() {
  digitalWrite(left_slp_pin, HIGH);
  digitalWrite(right_slp_pin, HIGH);
}

void turnOffMotor() {
  digitalWrite(left_slp_pin, LOW);
  digitalWrite(right_slp_pin, LOW);
}

void runMotorForward() {
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);
}

void runMotorBackward() {
  digitalWrite(left_dir_pin, HIGH);
  digitalWrite(right_dir_pin, HIGH);
}
