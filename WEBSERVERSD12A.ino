/*--------------------------------------------------------------
  Program:      eth_websrv_SD

  Description:  Arduino web server that serves up a basic web
                page. The web page is stored on the SD card.
  
  Hardware:     Arduino Uno and official Arduino Ethernet
                shield. Should work with other Arduinos and
                compatible Ethernet shields.
                2Gb micro SD card formatted FAT16
                
  Software:     Developed using Arduino 1.0.3 software
                Should be compatible with Arduino 1.0 +
                SD card contains web page called index.htm
   References:   - WebServer example by David A. Mellis and 
                  modified by Tom Igoe
                - SD card examples by David A. Mellis and
                  Tom Igoe
                - Ethernet library documentation:
                  http://arduino.cc/en/Reference/Ethernet
                - SD Card library documentation:
                  http://arduino.cc/en/Reference/SD

  Date:         10 January 2013
 
  Author:       W.A. Smith, http://startingelectronics.org
  //RL09022019  Added snd IP address 
--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IPAddress ip(192, 168, 1, 139); // IP address, may need to change depending on network
IPAddress ip(192, 168, 178, 64); // IP address, may need to change depending on network
//EthernetServer server(80);  // create a server at port 80
EthernetServer server = EthernetServer(80);
File webFile;
boolean receiving = false ; // to keep track weather we are  
                             // getting data
void setup()
{
    Serial.begin(9600);       // for debugging
    // Connect with DHCP
    if (!Ethernet.begin(mac)){
      Serial.println("could not configure Ethernet with DHCP.");
      return;
      }
    else{
      Serial.println("Ethernet configured");
    }
    //Ethernet.begin(mac, ip);  // initialize Ethernet device
    server.begin();           // start to listen for clients
    
    Serial.print("server is at ");
     Serial.println(Ethernet.localIP());
    
     pinMode(13,OUTPUT);
  
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for STAGE.HTM file
    if (!SD.exists("STAGE.HTM")) {
        Serial.println("ERROR - Can't find STAGE.HTM file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found STAGE.HTM file.");
}
void loop()
{
    EthernetClient client = server.available();  // try to get client
    // Gets a client that is connected to the server and has data available for reading.
    // The connection persists when the returned client object goes out of scope; 
    // you can close it by calling client.stop(). 
    
    // server.available() Returns a EthernetClient object with data waiting; 
    // if no Client has data available for reading, this object will 
    // evaluate to false in an if-statement
    if (client) {  // got client with data waiting
      // An HTTP request ends with a blank line
     //   Serial.println("got client waiting");
        boolean currentLineIsBlank = true;// a blank current always indicates the 
        boolean sentHeader = false;   
            // data to be sent or received
        while (client.connected()) {
          // Whether or not the client is connected. 
          // Note that a client is considered connected if the connection 
          // has been closed but there is still unread data
          // Returns true if the client is connected, false if not.  
            if (client.available()) {   // client data available to read
              // Returns the number of bytes available for reading
              // (that is, the amount of data that has been written to the 
              // client by the server it is connected to).
              // Returns the number of bytes available. 
              // Note: on the internet it says:
              // Client.available() returns the number of bytes that
              // a client (remote client) may have written. 
              // If you're writing an HTTP server, the first 
              // client data would be: GET /URL HTTP/1.0.
              // You then write back to that client with Client.write(). For example:
              // c.write("HTTP/1.0 200 OK")
             // Serial.println("Client is available");
                char c = client.read(); // read 1 byte (character) from client
              //  Serial.println("Character c = " + 'c');
                 // Serial.println("Character c is " + c);
                   //Read the next byte received from the server the client is connected to 
                   // (after the last call to read()).
                   // Returns the next byte (or character), or -1 if none is available. 
          //      if (c=='4'){
          //          digitalWrite(13,HIGH);
          //          delay(1000);
          //          digitalWrite(13,LOW);
           //       }
        //        RL removed feb 2 
        //        if (c == '\n' && currentLineIsBlank) {
                    // last line of client request is blank and ends with \n
                    // respond to client only after last line received  
                    // send a standard http response header
                    if (receiving && c == ' ') receiving = false; // Done receiving
                    if (c == '?') receiving = true; // found arguments
                      // this looks at the GET request
                     if (receiving){
                      //if (c== 1)
                      Serial.println ("Receeicved " + c);
                      break;                     
                     }
                     if (!sentHeader){
                      
                     
                    
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-Type: text/html");
                    client.println("Connection: close");
                    client.println();
                     }
                    // send web page
                    webFile = SD.open("STAGE.HTM");        // open web page file
                    if (webFile) {
                        while(webFile.available()) {
                            client.write(webFile.read()); // send web page to client
                        }
                        webFile.close();
                    }
                    break;
                    sentHeader - true;
               if (c == '\n' && currentLineIsBlank) break;
                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(5);      // give the web browser time to receive the data
        client.stop(); // close the connection
    } // end if (client)
} // end of loop
