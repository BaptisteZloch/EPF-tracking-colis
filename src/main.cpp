#include <Arduino.h>
//#include "FS.h"     //ancien système de gestion de fichier
#include "LittleFS.h"          //nouveau système de gestion de fichier
#include <ESP8266WiFi.h>       //wifi
#include <ESPAsyncWebServer.h> //webserver asynchrone
#include <SD.h>                //Gestion de la garde SD
#include <SPI.h>               //SPI bus
#include <RTClib.h>            //RTC
#include <Wire.h>              //I2C bus
#include <math.h>
#include <Adafruit_MPU6050.h> //MPU6050 accerometer
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>      //OLED screen
#include <Adafruit_GFX.h>          //OLED screen
#include <Digital_Light_TSL2561.h> //light sensor

#define epglogo_width 128 //taille de l'écran
#define epflogo_height 32 //taille de l'écran

AsyncWebServer server(80); //déclaration du serveur

RTC_DS3231 rtc; //real time clock declaration

Adafruit_MPU6050 mpu;                                        //accelerometer declaration
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire); //OLED screen declaration
float angley = 0;                                            //x angle called roll
float anglex = 0;                                            //y angle called pitch
DateTime now;                                                //date value in unixtime
float temperature = 0;                                       // temperature value
int visibleLux = 0;                                          //lux value
float a_i = 0;                                               //acceleration en z à l'instant i
float a_i_1 = 0;                                             //acceleration en z à l'instant i-1
float Te = 0.01;                                             //Période d'échantillonnage pour l'intégral
float vitessez;                                              //vitesse en Z

const char *ssid = "Tracking_colis"; //wifi name
const int chipSelect = D10;          // SD CS pin used for ESP8266

File myFile; //fichier

boolean disco = true; //variable disco = disconnected ou pas
static const uint8_t image_data_logoEPF[512] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x80, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0x03, 0x80, 0x00, 0x01, 0xff, 0x80, 0x00, 0x1f, 0xff,
    0xfe, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x01, 0x80, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x0f, 0xff,
    0xfe, 0x00, 0x80, 0xe0, 0x0f, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x38, 0x00, 0x00, 0x1f, 0xff,
    0xfe, 0x00, 0x83, 0xf8, 0x0e, 0x07, 0xff, 0xff, 0x87, 0xff, 0xf8, 0x18, 0x1f, 0xff, 0xff, 0xff,
    0xfe, 0x01, 0x03, 0x84, 0x0e, 0x0f, 0xff, 0xff, 0x87, 0xff, 0xfc, 0x18, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x02, 0x07, 0x02, 0x0e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xfc, 0x18, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x02, 0x06, 0x01, 0x0e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xfc, 0x18, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x02, 0x06, 0x01, 0x0e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xf8, 0x18, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x04, 0x0c, 0x00, 0x8e, 0x10, 0x00, 0x78, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x1f, 0xff,
    0xfe, 0x04, 0x0c, 0x00, 0x8e, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x0f, 0xff,
    0xfe, 0x04, 0x0c, 0x00, 0x8e, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x0f, 0xff,
    0xfe, 0x0c, 0x08, 0x01, 0x8e, 0x00, 0x00, 0x78, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x1f, 0xff,
    0xfe, 0x0c, 0x18, 0x01, 0x8e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x0c, 0x18, 0x03, 0x8e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x0c, 0x18, 0x07, 0x0e, 0x1f, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x04, 0x18, 0x0e, 0x0e, 0x0f, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x06, 0x18, 0x1c, 0x0e, 0x07, 0xff, 0xff, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x03, 0xb0, 0xe0, 0x0f, 0x00, 0x00, 0x01, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0xff, 0x00, 0x0f, 0x80, 0x00, 0x01, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x30, 0x00, 0x0f, 0xe0, 0x00, 0x01, 0x87, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x30, 0x00, 0x0f, 0xfe, 0x00, 0x03, 0xcf, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x20, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x20, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xfe, 0x00, 0x20, 0x00, 0x0c, 0x67, 0x37, 0x1e, 0x6a, 0xd9, 0x8b, 0x51, 0x68, 0xf1, 0xe3, 0xff,
    0xfe, 0x00, 0x60, 0x00, 0x0d, 0xda, 0xd7, 0x7e, 0xaa, 0x56, 0xb9, 0x57, 0x6a, 0xf7, 0xed, 0xff,
    0xfe, 0x00, 0x40, 0x00, 0x0c, 0x5e, 0xd7, 0x1e, 0xba, 0x57, 0x89, 0x51, 0x69, 0xf1, 0xe7, 0xff,
    0xfe, 0x00, 0x40, 0x00, 0x0d, 0xde, 0xd7, 0x7e, 0xba, 0x94, 0xba, 0x57, 0x6a, 0x97, 0x3b, 0xff,
    0xff, 0x00, 0x40, 0x00, 0x1d, 0xda, 0xd7, 0x7e, 0xba, 0x96, 0xba, 0x57, 0x6a, 0xf7, 0xed, 0xff,
    0xff, 0xc0, 0x40, 0x00, 0x3c, 0x67, 0x31, 0x1e, 0x7a, 0xd9, 0x8b, 0x51, 0x9a, 0xf1, 0xf1, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void testdrawbitmapEPF(void) //print the EPF icon
{
  display.clearDisplay(); //vide l'écran
  display.drawBitmap(     //dessin du logo
      (display.width() - epglogo_width) / 2,
      (display.height() - epflogo_height) / 2,
      image_data_logoEPF, epglogo_width, epflogo_height, 1);
  display.display(); //affichage du logo
  delay(2000);
}

boolean bon = true;

String dataCapture() //méthode de capture des données
{
  ////Read acceleromter data : Temperature et accéleration, vitesse
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  temperature = temp.temperature;
  anglex = atan(a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * 180 / PI;
  angley = atan(-1 * a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * 180 / PI;
  a_i = a.acceleration.z;
  //intégration numérique de l'accélération
  vitessez = (a_i + a_i_1) / 2;
  vitessez = vitessez * Te;
  a_i_1 = a_i;
  vitessez = (vitessez * 100) - 8.6; //converison en cm/s
  ////lecture  l'heure du RTC
  now = rtc.now();
  ////lecture de la luminosité
  visibleLux = TSL2561.readVisibleLux();

  if (anglex > 45.0 || angley > 45.0 || anglex < -45.0 || angley < -45.0 || temperature < 5 || temperature > 25 || visibleLux > 100 || vitessez > 10)
  {
    bon = false;
  }

  ////formattage des données type csv ";"
  String data_str = (String)now.unixtime() + ";" + (String)anglex + ";" + (String)angley + ";" + (String)temperature + ";" + (String)visibleLux + ";" + (String)vitessez + "\n"; //csv format string
  Serial.print(data_str);                                                                                                                                                        //affichage dans le temrinal série (DEBUG)
  return data_str;                                                                                                                                                               //renvoie la string des données
}

void dataSaving(String data_str) //sauvegarde des données dans la SD
{
  myFile = SD.open("/data.csv", FILE_WRITE); //ouvre data.csv en mode écriture
  myFile.println(data_str);                  //écriture de la ligne dans le fichier
  myFile.close();                            //Ferme data.csv
}

void initSensor() //méthode d'initialisation des capteurs
{
  //-------------------RTC--------------------------
  if (!rtc.begin())
  { //starting RTC module
    Serial.println("RTC pas trouvé");
    while (1)
      ; //point d'arret
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //ajustement de l'heure
  Serial.println("RTC initialisé !");

  //-------------------Accelerometer_Temperature--------------------------
  if (!mpu.begin(0x69))
  {
    Serial.println("Erreur de connexion au MPU6050");
    while (1)
    {
      delay(10);
    }
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.println("Capteur de Acceleromètre/temperature initialisé !");

  //-------------------Light--------------------------
  TSL2561.init();
  Serial.println("Capteur de Lumière initialisé !");
}

void initWifi() //méthode d'initialisation du wifi
{
  //-------------------WIFI_AP--------------------------
  // Serial.println("Création du point d'accès ... ");
  if (WiFi.softAP(ssid)) //création du point d'accès wifi
  {
    Serial.print("Adresse IP du point d'accès : ");
    Serial.println(WiFi.softAPIP()); //affichage de l'IP
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("   Wifi : ");
    display.print("   \n    ");
    display.println((String)ssid); //affichage du ssid sur l'écran
    /*display.println("");
    display.print("Site : ");
    display.print(WiFi.softAPIP()); //affichage de l'IP*/
    display.display(); //affichage
  }
  else
  {
    Serial.println("Erreur wifi...");
  }
}

void endWifi() //Arret du wifi (économie d'énergie)
{
  WiFi.softAPdisconnect(true); //deconnexion
  Serial.println("Point d'accès déconnecté !");
}

void initSD() //initialisation de la carte SD
{
  //-------------------SD--------------------------
  if (!SD.begin(chipSelect)) //sélection de la broche CS
  {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("SD initialisée !");
  Serial.println("TIMESTAMP;X_ANGLE;Y_ANGLE;TEMPERATURE;LUMINOSITE;Z_VITESSE"); //formattage de la donnée
}

void initScreen() //initialisation de l'écran
{
  //-------------------OLED Screen--------------------------

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setRotation(0);
  testdrawbitmapEPF(); // Draw a small bitmap image
  Serial.println("Ecran OLED initialisé !");
}

void initServerSPIFFS()
{
  Serial.println(F("Initializing FS..."));
  if (LittleFS.begin()) //demarrage de littleFS (remplacant de SPIFFS)
  {
    Serial.println(F("LittleFS system mounted with success"));
  }
  else
  {
    Serial.println(F("An Error has occurred while mounting LittleFS"));
  }

  //-------------------SERVER--------------------------
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });
  server.on("/temp.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/temp.html", "text/html");
  });
  server.on("/lum.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/lum.html", "text/html");
  });
  server.on("/chute.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/chute.html", "text/html");
  });
  server.on("/angle.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/angle.html", "text/html");
  });
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/w3.css", "text/css");
  });
  server.on("/map.png", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/map.png", "image/png");
  });
  server.on("/val_colis", HTTP_GET, [](AsyncWebServerRequest *request) {
    String rep = "";
    if (bon)
    {
      rep = "oui";
    }
    else
    {
      rep = "non";
    }
    request->send(200, "text/plain", rep);
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  server.on("/canvasjs.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/canvasjs.min.js", "text/javascript");
  });

  server.on("/liredata", HTTP_GET, [](AsyncWebServerRequest *request) {
    File datafile = SD.open("/data.csv", FILE_READ); //ouverture du fichier data.csv en mode écriture
    String dataString = "";
    String temp = "";            //string temporaire
    while (datafile.available()) //Lecture du fichier jusqu'à la fin
    {
      temp = datafile.readString(); //sstockage dans le string temporaire
      //Serial.print("Contenu :");    //DEBUG in temrinal
      //Serial.println(temp);         //DEBUG in terminal
      dataString += temp; //Ajout la string temporaire a notre string de données
    }
    datafile.close();                             //fermeture du fichier
    request->send(200, "text/plain", dataString); //Envoie de la string dans le javascript pour son traitement et affichage
  });
}

void setup()
{
  //-------------------Serial & I2C--------------------------
  Serial.begin(115200);
  Wire.begin();
  //-------------------initialisations--------------------------
  initScreen();
  initSensor();
  initSD();
  //initWifi();
  initServerSPIFFS();
}

void loop()
{
  switch (digitalRead(D2))
  {
  case 1: //interrupteur à 1 => capture de données
    //Serial.println("1");
    if (!disco) //si pas encore déconnecté On arrête le serveur et le wifi => économie d'énergie, on entre dans cette boucle à la première iteration du loop
    {
      server.end();           //stop serveur
      endWifi();              //stop wifi
      disco = true;           //le tout est bien déconnecté
      testdrawbitmapEPF();    //on affichage le logo EPF le temps de la capture => pub
      SD.remove("/data.csv"); //on supprime le fichier précédent
    }
    dataSaving(dataCapture()); //capture et enregistrement des données
    delay(1000);               //pause entre les mesures
    break;
    //rajouter LED rouge
  case 0:
    //Serial.println("0");
    if (disco) //si déconnecté on reconnecte le wifi et le serveur web
    {
      initWifi();
      server.begin();
      Serial.println("Serveur actif !");
      disco = false; //On n'est plus déconnecté
    }
    delay(1000);
    break;
  }
}