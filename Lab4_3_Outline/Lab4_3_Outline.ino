

#include <driverlib.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdio.h>
#include <string.h>

uint8_t count = 0;
volatile bool IO_button = 0;
volatile uint8_t flag = 0;

// your network name also called SSID
char ssid[] = "MOTO69F6";
// your network password
char password[] = "k6fzz48pmt";
// MQTTServer to use
char server[] = "io.adafruit.com";

void callback(char* topic, byte* payload, unsigned int length) {

  // Type Cast Input Bytes to Char
  char* str = (char*)payload;

  Serial.println(str);

  if (str[1] == 'N'){
    IO_button = 1;
    flag = 2;
    }
  if (str[1] == 'F'){
    IO_button = 0;
    }
  /* Check the Second Character of the char* pointer
   *  str[1] == 'N' ---> IO_button = ON 
   *  str[1] == 'F' ---> IO_button = OFF
   */
}

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);

void RGB_output(uint8_t count) {
  // Function to output to RGB LED based on count value between 0 and 7
   // Implement through simple switch statement
  GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
  
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0);
    }
   count = count/2;
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
    }
   count = count/2;
  if (count%2 == 1){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN2);
    }
}

void GPIO_Handler(void){

    uint_fast16_t interrupts;


    interrupts = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    
    GPIO_clearInterruptFlag(GPIO_PORT_P1, interrupts);
    
    if (interrupts & GPIO_PIN1)
    {
      if (flag==0){
        IO_button = 1;
        flag = 1;
      }
    }
    Serial.println(IO_button);

    
  }

void setup () {

    // Stop watchdog timer
    WDT_A_hold(WDT_A_BASE);

    // Set P1.0 to output direction
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    Serial.begin(115200);
  
    // Start Ethernet with the build in MAC Address
    // attempt to connect to Wifi network:
    Serial.print("Attempting to connect to Network named: ");
    // print the network name (SSID);
    Serial.println(ssid); 
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED) {
      // print dots while we wait to connect
      Serial.print(".");
      delay(300);
    }
  
    Serial.println("\nYou're connected to the network");
    Serial.println("Waiting for an ip address");
  
    while (WiFi.localIP() == INADDR_NONE) {
      // print dots while we wait for an ip addresss
      Serial.print(".");
      delay(300);
    }

    Serial.println("\nIP Address obtained");
    // We are connected and have an IP address.
    // Print the WiFi status.
    printWifiStatus();

    GPIO_registerInterrupt(GPIO_PORT_P1, GPIO_Handler);
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_enableInterrupt(GPIO_PORT_P1,GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT1);

    
}

void loop () {
  
    /* 
     *  Wait here until Sw1 on P1.1 or the IOT Button is pressed
     *  This should be a tight polling loop
     *  The processor will execute the loop until SW1 is pressed
     */

    while (!IO_button){

    if (!client.connected()) {
      Serial.println("Disconnected. Reconnecting....");


      if(!client.connect("seantang001", "seantang001", "aio_TrZV90DHx4yIUzEkpYfbK2oJuVYL")) {
        Serial.println("Connection failed");
      } else {
        Serial.println("Connection success");
        if(client.subscribe("seantang001/feeds/button1dashboard")) {
          Serial.println("Subscription successful");
        }
      }
    }

    client.poll();

    delay(1000);
      
      }

    count++;
    if (count > 7){
        count = 0;
    }
    RGB_output(count);

    if (flag == 1){
      IO_button = 0;
      }


    while (IO_button){

    if (!client.connected()) {
      Serial.println("Disconnected. Reconnecting....");


      if(!client.connect("seantang001", "seantang001", "aio_TrZV90DHx4yIUzEkpYfbK2oJuVYL")) {
        Serial.println("Connection failed");
      } else {
        Serial.println("Connection success");
        if(client.subscribe("seantang001/feeds/button1dashboard")) {
          Serial.println("Subscription successful");
        }
      }
    }
    client.poll();

    delay(1000);
      
      }
    flag = 0;
    /* 
     *  Wait here until Sw1 on P1.1 and the IOT Button is not pressed
     *  This should be a tight polling loop
     *  The processor will execute the loop until SW1 is not pressed
     */
    // Reconnect if the connection was lost

            // Delay

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
