
#define boton1 0xEFAA55
#define boton2 0xEF9A65
#define boton3 0xEFCA35
#define boton4 0xEFBA45
#define botonP 0xEF8A75


//#ifndef UNIT_TEST
//#include <Arduino.h>
//#endif

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ESP8266WiFi.h>          
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>          
#include <EEPROM.h>


IPAddress server(192, 168, 0, 56); // IP de la raspberry Pi
const char* host = "MandoIR"; // nombre del entorno

int pulsador = 0;
boolean estado_pulsador = LOW;
boolean estado_button1=0;
boolean estado_button2=0;
boolean estado_button3=0;
boolean estado_button4=0;
boolean estado_buttonp=0;
boolean leyendo_codigos=0;

uint16_t RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);

decode_results results;

#define BUFFER_SIZE 100

WiFiClient wclient;
PubSubClient client(wclient, server);

void callback(const MQTT::Publish& pub) {
  Serial.println (pub.payload_string());
    if(pub.payload_string() == "boton1on")
    {
      estado_button1=1;
    }
    if(pub.payload_string() == "boton1off")
    {
      estado_button1=0;
    }  
    if(pub.payload_string() == "boton2on")
    {
      estado_button2=1;
    }
    if(pub.payload_string() == "boton2off")
    {
      estado_button2=0;
    }
    if(pub.payload_string() == "boton3on")
    {
      estado_button3=1;
    }
    if(pub.payload_string() == "boton3off")
    {
      estado_button3=0;
    }
    if(pub.payload_string() == "boton4on")
    {
      estado_button4=1;
    }
    if(pub.payload_string() == "boton4off")
    {
      estado_button4=0;
    }

     //**************** ESTE BOTON ESTA PROGRAMADO PARA APAGAR LOS OTROS 4 *********************
        
    if(pub.payload_string() == "botonPon")
    {
      Serial.println("Apagando Todo");
      estado_buttonp=1;
      estado_button1=0;
      estado_button2=0;
      estado_button3=0;
      estado_button4=0;
      EEPROM.write(0,estado_button1);
      EEPROM.write(1,estado_button2);
      EEPROM.write(2,estado_button3);
      EEPROM.write(3,estado_button4);
      EEPROM.commit();
      client.publish("Button1",String(estado_button1));
      client.publish("Button2",String(estado_button2));
      client.publish("Button3",String(estado_button3));
      client.publish("Button4",String(estado_button4));
      delay(500);
      estado_buttonp=0;
      client.publish("Buttonp",String(estado_buttonp));    
    }
    
   //*****************************************************************************************
   
    if(pub.payload_string() == "botonPoff")
    {
      //estado_buttonp=0;
    }               
}

void setup() {
   pinMode(pulsador,INPUT);
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  client.set_callback(callback);
    if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(WiFi.SSID());
    Serial.println("...");
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    if (WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("WiFi not connected");
      WiFiManager wifiManager;  
      if (!wifiManager.startConfigPortal("MandoIRSetup")) { // SSID 
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        ESP.reset();
        delay(5000);
        }
      Serial.print("connected to ");
      Serial.println(WiFi.SSID());
      EEPROM.commit();
      delay(2000);
    }
    else
   {
     Serial.println("WiFi connected");                       
   }
  }   
  estado_button1=EEPROM.read(0);
  estado_button2=EEPROM.read(1);
  estado_button3=EEPROM.read(2);
  estado_button4=EEPROM.read(3); 

  
    
  delay(200);
  irrecv.enableIRIn(); 
}

void loop() {
estado_pulsador=digitalRead(pulsador);
if (estado_pulsador==LOW)
{
leyendo_codigos=1;
delay(200);
leer_codigos();
}
if (WiFi.status() == WL_CONNECTED)
{

{
  if (!client.connected()) 
  {
    if (client.connect("ESP8266: MandoIR"))  
    {   
      client.subscribe(host+(String)"/#");
    }
  }
    client.loop();
  }
  delay(1);
} 
 delay(50); 

   if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    //serialPrintUint64(results.value, HEX);
    //Serial.println("");
    irrecv.resume();  // Receive the next value
    if (results.value == boton1)
    {
      Serial.println("Has pulsado el boton 1");
      if (estado_button1==LOW)
      {
       estado_button1=HIGH;
       }
       else
       {
         estado_button1=LOW;
       }
       client.publish("Button1",String(estado_button1)); 
       Serial.print("Button1 = ");
       Serial.println(estado_button1);
       EEPROM.write(0,estado_button1);
       EEPROM.commit();      
      }
    if (results.value == boton2)
    {
      Serial.println("Has pulsado el boton 2");
      if (estado_button2==LOW)
      {
       estado_button2=HIGH;
       }
       else
       {
         estado_button2=LOW;
       }
       client.publish("Button2",String(estado_button2));
       Serial.print("Button2 = ");
       Serial.println(estado_button2);        
        EEPROM.write(1,estado_button2);
        EEPROM.commit();
    }
    if (results.value == boton3)
    {
      Serial.println("Has pulsado el boton 3");
      if (estado_button3==LOW)
      {
       estado_button3=HIGH;
       }
       else
       {
         estado_button3=LOW;
       }
       client.publish("Button3",String(estado_button3));
       Serial.print("Button3 = ");
       Serial.println(estado_button3);       
       EEPROM.write(2,estado_button3);
       EEPROM.commit();       
    }  
     if (results.value == boton4)
    {
      Serial.println("Has pulsado el boton 4");
      if (estado_button4==LOW)
      {
       estado_button4=HIGH;
       }
       else
       {
         estado_button4=LOW;
       }
       client.publish("Button4",String(estado_button4));
       Serial.print("Button4 = ");
       Serial.println(estado_button4);       
       EEPROM.write(3,estado_button4);
       EEPROM.commit();       
    }

    //**************** ESTE BOTON ESTA PROGRAMADO PARA APAGAR LOS OTROS 4 *********************
    
     if (results.value == botonP)
    {
      Serial.println("Has pulsado el boton P");
      Serial.println("Apagando Todo");
      estado_buttonp=1;
      estado_button1=0;
      estado_button2=0;
      estado_button3=0;
      estado_button4=0;
      EEPROM.write(0,estado_button1);
      EEPROM.write(1,estado_button2);
      EEPROM.write(2,estado_button3);
      EEPROM.write(3,estado_button4);
      EEPROM.commit();
      client.publish("Button1",String(estado_button1));
      client.publish("Button2",String(estado_button2));
      client.publish("Button3",String(estado_button3));
      client.publish("Button4",String(estado_button4));
      delay(500);
      estado_buttonp=0;
      client.publish("Buttonp",String(estado_buttonp));   
    }
  //*****************************************************************************************
                
  }
  delay(100);
}

void leer_codigos()
{
Serial.println("Entrando en la lectura de códigos");
  while (leyendo_codigos==1)
  {
    estado_pulsador=digitalRead(pulsador);
    if (estado_pulsador==LOW)
    {
    leyendo_codigos=0;
    }
     if (irrecv.decode(&results)) {
    // print() & println() can't handle printing long longs. (uint64_t)
    Serial.print("El código leido es : ");
    serialPrintUint64(results.value, HEX);
    Serial.println("");
    irrecv.resume();  // Receive the next value
     }
    delay(50); 
  }
Serial.println("Saliendo de la lectura de códigos");
delay(200);
}

