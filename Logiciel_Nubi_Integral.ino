/*
 * Bienvenue dans le code de Nubi ce programme est amené à évoluer avec le temps et les aides de la communauté
 * N'hésitez pas vérifier si vous avez la derniere version sur le site : 
 * https://github.com/Faisable/Nubi
 * 
 * Nous vous souhaitons de passer des heures à reprogrammer cette petite station et nous faire partager vos 
 * créations
 *  
 *  Olivier 
 *  
 *  Nos kits sont basés sur la philosophie open source par conséquent nous essayons le plus possible de citer nos sources
 *  si par mégarde l'une d'entre elles est absente. Merci de nous en informer. 
 *  
 * 
 */
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <Wire.h>

#include <ESP8266WiFi.h>
#include <time.h>
/* 
 *  On demande si Nubi doit se connecter au wifi 
 *  si oui on devra spécifier le nom de la box et son mot de passe Wifi un peu plus loin 
 *  true = Vrai 
 *  false= Faux
*/
bool Wifi_actif=false;
//bool Wifi_actif=true;

// Voici les deux lirairies graphiques de chez Adafruit, elles permettent d'écrire sur l'afficheur
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//Les Neopixels sont des leds RGB pilotable, elles sont connectées sur la Broche D4 
// 5 leds composent Nubi 

#include <Adafruit_NeoPixel.h>

// Le capteur de température se branche en D7 c'est un capteur DHT11 mesurant humidité et température:

#define DHTPIN D7             // Capteur DTH11 en D7  
#define DHTTYPE    DHT11      // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);

//On définit la taille de l'écran, son type etc  :

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//   https://learn.adafruit.com/dht/overview

uint32_t delayMS;

int timezone = 2 * 3600;
int dst = 1;

float humidite;
float temperature;

// c'est la broche sur laquelle on branche capteur MQ-2 soit A0  
const int mqxPin = A0;
// On définit le nom de la box et son mot de passe wifi pour permettre à Nubi d'avoir l'heure

const char* ssid = "MAboxXXX";
const char* password = "Motdepasse";

//On peut rajouter un Buzzer pour que Nubi chante au démarrage saurez vous le connecter ?

#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

//Variable utile pour le gardien du temps

unsigned long previousMillis_Mise_a_jour_bandeau_led = 0;
unsigned long previousMillis_Mise_a_jour_Heure = 0;
unsigned long previousMillis_Mise_a_jour_Afficheur = 0;
unsigned long previousMillis_Mesure_Qualite_Air = 0;
unsigned long previousMillis_Mesure_Temp_Humidite = 0;


const long interval_Mise_a_jour_Afficheur   =    1000;
const long interval_Mise_a_jour_Heure       =    4000;
const long interval_Mise_a_jour_bandeau_led =    1500;
const long interval_Mesure_Temp_Humidite    =    3000;
const long interval_Mesure_Qualite_Air      =    3100;

// Branchement des leds RGB 
// Déclaration pour le fonctionement des LED RGB

#define PIN        D4 // On Trinket or Gemma, suggest changing this to 1
#define NUMPIXELS 5 // Popular NeoPixel ring size

#define Icone_Maison 4
#define Icone_Wifi 3
#define Icone_Humidite 2
#define Icone_Temperature 1
#define Icone_Air 0

#define V_Orange  128,80,0
#define V_Rouge   150,0,0
#define V_Vert    0,150,0
#define V_Bleu    0,0,150
#define V_Noir    0,0,0

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#define LOGO_HEIGHT   21
#define LOGO_WIDTH    84

//Les mesures de Seuil qui permettront de faire changer les couleurs ou d'alerter si on dépasse les seuils

//Température

#define Valeur_Froid_temp 16
#define Valeur_Chaud_temp 30

//Humidité

#define Valeur_Sec_hum      30
#define Valeur_Humide_hum   70

//Qualité de l'air

int Valeur_qualite=0;
#define Seuil_qualite_air      50

bool Wifi_OK =false;


static const unsigned char PROGMEM logo_bmp[] =
{
  // 'logo_Nubi, 84x21px

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0xc0, 0x00, 0x03, 0x80, 0x00, 0x70, 
  0x70, 0x00, 0x03, 0x80, 0x00, 0xe0, 0x00, 0x0f, 0xe0, 0x00, 0x70, 0x70, 0x00, 0x03, 0x00, 0x00, 
  0xe0, 0x00, 0x1e, 0xf8, 0x00, 0x78, 0x70, 0x00, 0x03, 0x00, 0x00, 0xc0, 0x00, 0x30, 0x18, 0x00, 
  0x78, 0x60, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x78, 0xe0, 0x00, 0x07, 0x00, 
  0x00, 0x00, 0x00, 0x60, 0x0c, 0x00, 0x78, 0xe1, 0xc3, 0x87, 0x78, 0x0f, 0xc0, 0x01, 0xe0, 0x0e, 
  0x00, 0xf8, 0xe1, 0x83, 0x87, 0xfc, 0x0f, 0xc0, 0x03, 0xc0, 0x07, 0x80, 0xfc, 0xe1, 0x83, 0x87, 
  0xfc, 0x0f, 0xc0, 0x06, 0x00, 0x07, 0xe0, 0xec, 0xe3, 0x83, 0x87, 0x1e, 0x01, 0xc0, 0x0e, 0x00, 
  0x00, 0x60, 0xec, 0xe3, 0x83, 0x86, 0x0e, 0x01, 0xc0, 0x3e, 0x00, 0x00, 0x30, 0xec, 0xc3, 0x83, 
  0x0e, 0x0e, 0x01, 0x80, 0x70, 0x00, 0x00, 0x30, 0xcf, 0xc3, 0x83, 0x0e, 0x0e, 0x03, 0x80, 0xe0, 
  0x00, 0x00, 0x31, 0xc7, 0xc3, 0x87, 0x0e, 0x0e, 0x03, 0x80, 0xc0, 0x00, 0x00, 0x39, 0xc7, 0xc3, 
  0x07, 0x0e, 0x0e, 0x03, 0x80, 0xc0, 0x00, 0x00, 0x31, 0xc7, 0xc3, 0x07, 0x0e, 0x0c, 0x03, 0x80, 
  0xc0, 0x00, 0x00, 0x31, 0xc7, 0xc3, 0x07, 0x0e, 0x1c, 0x03, 0x80, 0xe0, 0x00, 0x00, 0x71, 0xc7, 
  0xc3, 0x8f, 0x1e, 0x3c, 0x03, 0x80, 0x70, 0x00, 0x00, 0xe1, 0xc3, 0x83, 0xfe, 0x1f, 0xf8, 0x3f, 
  0xf0, 0x3f, 0xff, 0xff, 0xc1, 0x83, 0x83, 0xfe, 0x1f, 0xf0, 0x3f, 0xf0, 0x1f, 0xff, 0xff, 0x03, 
  0x83, 0x81, 0xe6, 0x1c, 0xe0, 0x3f, 0xe0
};
void nubi_chante()
{
 for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(D5, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(D5);
  } 
  
}
void Initialisation_Nubi()
{

  //initialisation des IO

    //*********Initialisation Ecran
    
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C  );  // initialize with the I2C addr 0x3D (for the 128x64)
  delay(2000);
  display.clearDisplay();   // clears the screen and buffer
  Affiche_logo_Nubi();

 //*********Initialisation Bande_led
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear(); // Set all pixel colors to 'off'  
  
  pixels.setPixelColor(Icone_Air,           pixels.Color(V_Bleu));
  pixels.setPixelColor(Icone_Temperature,   pixels.Color(V_Rouge));
  pixels.setPixelColor(Icone_Humidite,      pixels.Color(V_Vert));
  pixels.setPixelColor(Icone_Wifi,          pixels.Color(V_Rouge));
  pixels.setPixelColor(Icone_Maison,        pixels.Color(V_Bleu));
 
  pixels.show();
  delay(1500);
  pixels.setPixelColor(Icone_Air,           pixels.Color(V_Noir));
  pixels.setPixelColor(Icone_Temperature,   pixels.Color(V_Noir));
  pixels.setPixelColor(Icone_Humidite,      pixels.Color(V_Noir));
  pixels.setPixelColor(Icone_Wifi,          pixels.Color(V_Noir));
  pixels.setPixelColor(Icone_Maison,        pixels.Color(V_Bleu));
 
  pixels.show();
  //*********Initialisation Capteur DHT11
  dht.begin();
  sensor_t sensor;
  
  //*********Initialisation Capteur MQ-2


//*********Initialisation WIFI
  if(Wifi_actif==true)
  {
    Nubi_Connection_Wifi_heure();
  }

 
}
void Affiche_logo_Nubi()
{  display.clearDisplay();
   display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  nubi_chante();
}

void Mise_a_jour_bandeau_led()
{
  //*************capteur Temperature***************
  
Serial.println("Mise_a_jour_bandeau_led()");
Serial.println(floorf (temperature));

 if(floorf (temperature)<Valeur_Froid_temp)  {  pixels.setPixelColor(Icone_Temperature,   pixels.Color(V_Bleu  )); Serial.println("il fait Froid");}  
 if(floorf (temperature)>=Valeur_Chaud_temp){   pixels.setPixelColor(Icone_Temperature,   pixels.Color(V_Rouge ));  Serial.println("il fait Chaud");  }    else    {   pixels.setPixelColor(Icone_Temperature,   pixels.Color(V_Vert ));Serial.println("Temperature ok ");  }
 
   //*************capteur Humidite***************

Serial.println(floorf (humidite));

 if(floorf (humidite)<Valeur_Sec_hum)  {  pixels.setPixelColor(Icone_Humidite,   pixels.Color(V_Rouge  )); Serial.println("il fait trop sec");}  else    {   pixels.setPixelColor(Icone_Humidite,   pixels.Color(V_Vert ));Serial.println("Humidité ok ");  }
 if(floorf (humidite)>=Valeur_Humide_hum){   pixels.setPixelColor(Icone_Humidite,   pixels.Color(V_Rouge ));Serial.println("il fait trop humide");  }    

 //*************capteur Qualité AIR***************

Serial.println(Valeur_qualite);

 if(Valeur_qualite>Seuil_qualite_air)  {  pixels.setPixelColor(Icone_Air,   pixels.Color(V_Rouge  )); Serial.println("Air vicié");}  else    {   pixels.setPixelColor(Icone_Air,   pixels.Color(V_Vert ));Serial.println("Air ok ");  }
     
  //if(Wifi_OK==true){ pixels.setPixelColor(Icone_Wifi,   pixels.Color(V_Orange  ));}
  if(Wifi_OK==false){ pixels.setPixelColor(Icone_Wifi,   pixels.Color(V_Bleu  ));}
//*************WIFI***************

 pixels.show();
}
void Mise_a_jour_Afficheur()
{
   Serial.println("Mise_a_jour_Afficheur()");
    char temp_buff[5]; char hum_buff[5];
    
    char temp_disp_buff[11] = "Tmp:"; //remplit un tableau avec le mot TMP
    char hum_disp_buff[11] = "Hum:";  //remplit un tableau avec le mot Hum
    
    // appending temp/hum to buffers
    dtostrf(temperature,2,1,temp_buff); // transforme un chiffe à virgule en texte
    strcat(temp_disp_buff,temp_buff);   // rassemble les deux parties du texte à afficher
    
    dtostrf(humidite,2,1,hum_buff);     // transforme un chiffe à virgule en texte
    strcat(hum_disp_buff,hum_buff);     // rassemble les deux parties du texte à afficher
    
    // routine for displaying text for temp/hum readout
    
    display.clearDisplay();    display.setTextSize(2);    display.setTextColor(WHITE);
    
    display.setCursor(0,0);   display.println(temp_disp_buff);  display.setCursor(100,0); display.println("C");//20
    
    display.setCursor(0,20);  display.println("Air:");          display.setCursor(45,20); if(Valeur_qualite>Seuil_qualite_air)  {display.println("Aerez");} else {display.println("Sain");}//display.println("Aerez");
    
    display.setCursor(0,45);  display.println(hum_disp_buff);   display.setCursor(100,45); display.println("%");
    
    
    display.display();
    
    delay(2000);  
  
  }
void Mesure_temp_Humidite(void)
  {
    Serial.println("Mesure_temp_Humidite()");
    
     // Recuperer la valeur de la temperature et l'imprime sur la liaison serie.
  sensors_event_t event; //on déclare un objet event de type Sensors event 
  
  dht.temperature().getEvent(&event);
  
  if (isnan(event.temperature))         {Serial.println(F("Erreur de lecture temperature!"));  }  else {    /*Serial.print(F("Temperature: ")); */  temperature=event.temperature;   /*  Serial.print(temperature);          Serial.println(F("°C"));  */  }
  
  // Recuperer la valeur de l humidite et l'imprime sur la liaison serie.
  
  dht.humidity().getEvent(&event);
  
  if (isnan(event.relative_humidity))   {Serial.println(F("Erreur de lecture humidite!"));  }     else {   /* Serial.print(F("Humidite: "));   */   humidite=event.relative_humidity; /*  Serial.print(humidite);             Serial.println(F("%")); */  }
   
    
  }
void Mesure_Qualite_Air()
{
  Serial.println("Mesure_Qualite_Air()");
  Valeur_qualite =map(analogRead(mqxPin), 0,1024,0,100);
  Serial.println(Valeur_qualite);
}
void Recuperer_meteo()
{}
void Nubi_Connection_Wifi_heure()
{
  if(Wifi_actif==true)
  {
  display.setTextSize(1);
  display.setTextColor(BLACK);
  
  display.setCursor(0,0);
  display.println("Wifi connecting to ");
  display.println( ssid );

  WiFi.begin(ssid,password);
 
  display.println("\nConnecting");

  display.display();

  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      display.print("."); 
      display.display();    
      pixels.setPixelColor(Icone_Wifi,   pixels.Color(V_Orange));
pixels.show();   
  }
pixels.setPixelColor(Icone_Wifi,   pixels.Color(V_Vert));
pixels.show();
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  display.setCursor(0,0);
  
  display.println("Wifi Connected!");
  display.print("IP:");
  display.println(WiFi.localIP() );
  Serial.println(WiFi.localIP() );
  display.display();

  configTime(timezone, dst, "server 0.fr.pool.ntp.org","ntp.midway.ovh");
  display.println("\nWaiting for NTP...");

  while(!time(nullptr)){
     Serial.print("*");
     
     delay(1000);
  }
  display.println("\nTime response....OK"); 
  display.display();  
  delay(1000);

  display.clearDisplay();
  display.display();
  
  
  }
  }
void Mise_a_jour_de_l_heure()
{
  if(Wifi_actif==true)
  {
    time_t now = time(nullptr);
  
  struct tm* p_tm = localtime(&now);
  
  Serial.print(p_tm->tm_mday);
  Serial.print("/");
  Serial.print(p_tm->tm_mon + 1);
  Serial.print("/");
  Serial.print(p_tm->tm_year + 1900);
  
  Serial.print(" ");
  
  Serial.print(p_tm->tm_hour);
  Serial.print(":");
  Serial.print(p_tm->tm_min);
  Serial.print(":");
  Serial.println(p_tm->tm_sec);
  
  // Clear the buffer.
  display.clearDisplay();
 
  display.setTextSize(3);
  display.setTextColor(WHITE);
  
  display.setCursor(0,0);
  display.print(p_tm->tm_hour);
  display.print(":");
  if( p_tm->tm_min <10)
    display.print("0"); 
  display.print(p_tm->tm_min);
  
  display.setTextSize(2);
  display.setCursor(90,5);
  display.print(".");
  if( p_tm->tm_sec <10)
    display.print("0"); 
  display.print(p_tm->tm_sec);

  display.setTextSize(1);
  display.setCursor(0,40);
  display.print(p_tm->tm_mday);
  display.print("/");
  display.print(p_tm->tm_mon + 1);
  display.print("/");
  display.print(p_tm->tm_year + 1900);

  display.display();

  delay(1000); // update every 1 sec
  
  }
  }
void Le_gardien_du_temps()
{
    unsigned long currentMillis = millis();
        
  if (currentMillis - previousMillis_Mesure_Qualite_Air       >= interval_Mesure_Qualite_Air    )             {  previousMillis_Mesure_Qualite_Air      = currentMillis;  Mesure_Qualite_Air();       }  
  if (currentMillis - previousMillis_Mesure_Temp_Humidite     >= interval_Mesure_Temp_Humidite  )             {  previousMillis_Mesure_Temp_Humidite    = currentMillis;  Mesure_temp_Humidite();     }
  if (currentMillis - previousMillis_Mise_a_jour_bandeau_led  >= interval_Mise_a_jour_bandeau_led)            {  previousMillis_Mise_a_jour_bandeau_led = currentMillis;  Mise_a_jour_bandeau_led();  }
  if (currentMillis - previousMillis_Mise_a_jour_Afficheur    >= interval_Mise_a_jour_Afficheur)              {  previousMillis_Mise_a_jour_Afficheur   = currentMillis;  Mise_a_jour_Afficheur();    }  
  if (currentMillis - previousMillis_Mise_a_jour_Heure    >= interval_Mise_a_jour_Heure)              {  previousMillis_Mise_a_jour_Heure   = currentMillis;     Mise_a_jour_de_l_heure();    }
  }

void setup() {
  Serial.begin(9600);  // on ouvre une liaison serie avec la vitesse de 9600 Bauds
  Initialisation_Nubi();
}

void loop() {
  Le_gardien_du_temps();
  
}
