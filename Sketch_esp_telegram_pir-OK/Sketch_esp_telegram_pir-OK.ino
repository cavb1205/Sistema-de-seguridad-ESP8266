/*Sistema seguridad con deteccion de movimiento y envio de notificaciones por medio de chatbot de telegram
**
*/

#include <ESP8266WiFi.h>
#include <SPI.h>  
#include <TelegramBot.h> 

int calibrationTime = 30;        
long unsigned int lowIn;         
boolean lockLow = true;
boolean takeLowTime;
long unsigned int pause = 5000;  
byte sensorPin = 2; // Pin para el sensor de movimiento
byte ledRojo = 0; // pin para el led rojo
int estado = false;
int contador;



/////// Wifi Settings ///////

const char* ssid = "Chuquicamata";
const char* password = "27101961";
//const char* ssid = "AndroidAP";
//const char* password = "1234567890";

///token telegram////

const char* BotToken = "528549984:AAHY_edBdNQa7vvpgEsJYWmbYzuEhvIRDco";    // Token de Telegram se obtiene configurando el Bot

WiFiClientSecure client;
TelegramBot bot(BotToken,client);


void setup()
{
  
  Serial.begin(115200); 
  delay(10);
  pinMode(sensorPin,INPUT);
  pinMode(ledRojo, OUTPUT);
   
   // Conectando a la WiFi
  Serial.println();
  Serial.print("Contectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado de forma correca a ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  digitalWrite(sensorPin, LOW);
  Serial.print("calibrando sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVO");

  bot.begin();
  //message m = bot.getUpdates();
  //bot.sendMessage(m.chat_id,"El dispositivo de seguridad esta listo para ser activado");
  
}//end setup


void loop()
{  
  Serial.println("INICIA EL LOOP");

  message m = bot.getUpdates(); // A la espera de nuevos mensajes de Telegram
   
  // Si el mensaje es On se activa la alarma, es sensible a mayúsculas
  if (m.text.equals("On")) 
  {      
    Serial.println("Mensaje recibido se activa la alarma");
    bot.sendMessage(m.chat_id, "La Alarma está preparada y lista.");
    estado = true;
    contador=0;
    while(estado){
    Serial.println("Contador es:");
    Serial.println(contador);
    delay(1000);
    Serial.println("Antes del if HIGH");
    if(digitalRead(sensorPin) == HIGH) {
      Serial.println("ingresa al if HIGH");
      digitalWrite(ledRojo,HIGH);  //se enciende el led rojo
      delay(50);
      if(lockLow){  
        lockLow = false;            
        Serial.println("---");
        Serial.println("Se ha detectado movimiento!");
        Serial.print(millis()/1000);
        Serial.println(" sec");
        bot.sendMessage(m.chat_id, "Alarma Casa activada por movimiento!!"); //Mensaje de alerta Telegram
        }
        takeLowTime = true;
        
       }
    
    Serial.println("final del if HIGH");    
    delay(1000);
    Serial.println("Antes del if LOW");
    
    if(digitalRead(sensorPin) == LOW) {
      Serial.println("ingresa al LOW");
      digitalWrite(ledRojo,LOW);  //se apaga el led rojo
      delay(50);
      if(takeLowTime){
        lowIn = millis();          
        takeLowTime = false;       
        }
        
    
      if(!lockLow && millis() - lowIn > pause){
        lockLow = true;                        
        Serial.print("movimiento finalizado a: ");     
        Serial.print((millis() - pause)/1000);
        Serial.println(" sec");
        
        }
        
      }
     Serial.println("Despues del if LOW");  
     delay(1000);
     
    if(contador == 20){
      Serial.println("Ingresa al if del contador==10");
      contador = 0;
      message m = bot.getUpdates();
      if( m.chat_id != 0){
      Serial.println(m.text);
  
      if (m.text.equals("Off"))  //condicion para desactivar la alarma
        {  
         Serial.println("Mensaje recibido, se apaga la alarma.");  
         bot.sendMessage(m.chat_id, "La Alarma está desactivada.");
         Serial.println("Apaga alarma en el if del while");
         estado = false;
         digitalWrite(ledRojo,LOW);
         digitalWrite(sensorPin, LOW);
         Serial.println("Estado de la alarma cambia a ");
         Serial.println(estado);
         delay(1000);
         }
         
       else if (m.text.equals("Estado"))
            {            
              Serial.println("Mensaje recibido, se envia el estado actual de la alarma.");
              Serial.println(estado);
              if (estado == true)
              {
                bot.sendMessage(m.chat_id, "La Alarma está activada");  
              }
              else {
                bot.sendMessage(m.chat_id, "La Alarma está desactivada");
              }
            }
            delay(1000);
      }     
      }
      contador = contador + 1;
    }
  
} 
 
  else  if (m.text.equals("Off"))  //condicion para desactivar la alarma
  {  
   Serial.println("Mensaje recibido, se apaga la alarma.");  
   bot.sendMessage(m.chat_id, "La Alarma está desactivada.");
   Serial.println("Apaga alarma en el if inicial");
   estado = false;
   digitalWrite(ledRojo,LOW);
   digitalWrite(sensorPin, LOW);
   Serial.println("Estado de la alarma cambia a ");
   Serial.println(estado);
   delay(1000);
   }
   
    else if (m.text.equals("Estado"))
      {
        Serial.println("Mensaje recibido, se envia el estado actual de la alarma.");
        Serial.println(estado);
        if (estado == true)
        {
          bot.sendMessage(m.chat_id, "La Alarma está activada");  
        }
        else {
          bot.sendMessage(m.chat_id, "La Alarma está desactivada");
        }
        delay(1000);
      }
Serial.println("FIN DEL LOOP");

} // fin loop
