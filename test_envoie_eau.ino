#include <SPI.h>  //bibliothèque de communication SPI
#include <RH_RF95.h> // bibliothèque de com LoRa

#define SS 18 //broche ESP 32 DV KIT HELTEC
#define RST 14 //broche ESP 32 DV KIT HELTEC
#define DI0 26 //broche ESP 32 DV KIT HELTEC

#define RF95_FREQ 868.0 //fréquence de travail ESP32, peut être changer en 433 ou 916 en fonction de l'usage

RH_RF95 rf95(SS, DI0); // [heltec|ttgo] ESP32 Lora OLED avec puce sx1278

void setup()
{
  pinMode(RST, OUTPUT);
  digitalWrite(RST, HIGH);
 
  while (!Serial);
  Serial.begin(115200); //Vitesse de communication HELTEC ESP32
  delay(100);
 
  Serial.println("Arduino LoRa Transmetteur!"); // Pour définire que nous sommes le transmetteur
 
  //Séquence de reset
  digitalWrite(RST, LOW);
  delay(10);
  digitalWrite(RST, HIGH);
  delay(10);
 
  while (!rf95.init()) {
    Serial.println("LoRa signal en erreur"); //Erreur de programmetion vérifier la fréquence, la tension, le paramétrage broche
    while (1);
  }
  Serial.println("LoRa Signal intialise!"); // tout va bien
 
  // Si la fréquence est incorrect
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Mauvaise frequence ?");
    while (1);
  }
  Serial.print(" Freq utilisé: "); Serial.println(RF95_FREQ); // Juste une information de la fréquence
 
   rf95.setTxPower(23, false);
}
 
int16_t packetnum = 0;  // Paquet du compteur
 
void loop()
{
  Serial.println("Envoye au Serveur Shield Lora"); //Début loop
   
  char radiopacket[20] = "Salut #      ";
  itoa(packetnum++, radiopacket+13, 10);
  Serial.print("Envoyer ! "); Serial.println(radiopacket);
  radiopacket[19] = 0;
 
  Serial.println("Envoie en cours..."); delay(10);
  rf95.send((uint8_t *)radiopacket, 20);
 
  Serial.println("Transmission"); delay(10);
 
  // Attente retour Ping Pong
 
  rf95.waitPacketSent();
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
 
  Serial.println("Attente de réponse"); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  {
    // Recéption du message retour ? 
    if (rf95.recv(buf, &len))
   {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);   
    }
    else
    {
      Serial.println("Pas de réponse");
    }
  }
  else
  {
    Serial.println("Module Shield Lora en marche ? ");
  }
  delay(1000);
}
