#include <ArduinoJson.h>
#include <MFRC522.h>


#define SS_PIN 2
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);
#define ledverde 5
#define buzzer 4
#define ledvermelho 16


#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

String id;
char st[20];
String conteudo2;
String conteudo;
const char* ssid = "GeoMK_2.4GHz";
const char* password = "geomk@666";


void setup() {

  Serial.begin(9600);
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("ip ");
  Serial.println(WiFi.localIP());
  pinMode (ledverde, OUTPUT);
  pinMode (ledvermelho, OUTPUT);
  pinMode (buzzer, OUTPUT);


  SPI.begin();      // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();

}

void json() {

  StaticJsonDocument<1024> doc;
  JsonObject root = doc.to<JsonObject>();
  root ["ID"] = conteudo;
  serializeJsonPretty(root, Serial);

}


void loop() {
  conteudo = "";
  digitalWrite(buzzer, HIGH);



  if ( ! mfrc522.PICC_IsNewCardPresent())

  {
    return;

  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {

    return;
  }

  tone(buzzer, 2500, 100);//buzzer inicia
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    conteudo.trim();

  }



  if (conteudo != conteudo2 && conteudo != "") {

    json();



  }
  conteudo2 = conteudo;
  HTTPClient http;
  http.begin("http://172.16.8.51:8121");              //Specify request destination
  http.addHeader("Content-Type", "application/json");    //Specify content-type header

  int httpCode = http.POST(conteudo);   //Send the request
  String payload = http.getString();    //Get the response payload

  Serial.println(httpCode);   //Print HTTP return code

  http.end();  //Close connection



  digitalWrite(ledvermelho, LOW);
  digitalWrite(ledverde, HIGH);
  delay(500);
  digitalWrite(ledverde, LOW);
  digitalWrite(ledvermelho, HIGH);
  delay(500);


  noTone(buzzer);

}
