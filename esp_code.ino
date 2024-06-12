//Al tener conectado el serial del atmega no se va a subir el código
//Verificar FINGERPRINT siempre al configurar
//ACTUAL:  "4F:24:1F:D7:71:96:66:3A:E8:C3:9D:66:FA:E6:2E:FF:8E:48:DB:F8"
//Carpeta documentos>Arduino>Librerias>firebase-arduino>src>firebase client HTTP
//Código proyecto integrador 2024-2 (Arduino-ESP8266)

//Librerías-
#include <ESP8266WiFi.h>  // v. 3.1.2
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//Variables estación meteorológica

/***********/
//Variables
#define WIFI_SSID "PI2024" //red del celular
#define WIFI_PASSWORD "proyecto123"

//DB
#define FIREBASE_HOST "api-weather-2024-default-rtdb.firebaseio.com"  //nota:mirar siempre los fingerprint actualizado del proyecto para que todo funcione correctamente
#define FIREBASE_AUTH "I8qgfWmVEnj2ldkeniAgERuYy4pgTpOME0NkNtSY"



//variable tiempo de envio a la base de datos
int tiempoMedicion = 12; //segundos
unsigned long tiempoAntMedicion = 0;

//variables tiempo de lectura de sensores
int tiempoLect = 5;
unsigned long tiempoAntLect = 0;

//Vairables para partir cadena desde Serial
#define separador '=' //Delimitador de cada variable
String cadena = ""; // Variable a guardar la cadena del serial poner "" cuando sea por serial***
String campo; 
int inicio, fin; //Delimitar la cadena a cada valor 

float winddir = 0; //Variable dirección del viento Grados°
float windSpeedkm = 0; //Variable velocidad del viento KM
float humidity = 0; //Variable de humedad sensor HTU21D
float temp_c = 0;  //Variable temperatura sensor HTU21D
float rainin = 0; //Variable nivel de lluvia del meter kit
float pressure = 0; //Variable de presion del sensor MPL3115
float light_level = 0; //Variable nivel de luz
float batt_level = 0; //nivel de bateria prueba

void setup() {
  //Iniciar serial y verificar si existe conexion con el serial del arduino
  Serial.begin(9600); 
  //while(!Serial){;} //activar cuando este por serial ****
  

  //Conectarse a la red
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("conectando...");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Conectado a la red: " + String(WIFI_SSID));
  Serial.println(WiFi.localIP()); //Mostrar IP, ya conectado

  //conectarse a la base de datos
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  //conectarse a la base de datos
  }
  
void loop() {
  //Verificar conexión con FIREBASE
  /*int Version = Firebase.getInt("version");

  if(Firebase.failed()){
    Serial.print("Conexión con la base de datos falló");
    Serial.println(Firebase.error());
  } else{
    Serial.println("Conexión exitosa a FIREBASE");
    Serial.println(Version);  
  }*/


  //Recepción de los datos del ESP para guardarlo en variables
  //Crear funcion pasandole como argumento la variable cadena
  //Habilitar codigo cuando se hagan pruebas con el serial *******
  
  if (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') { // Procesar cuando se recibe una nueva línea
      procesarCadena(cadena);
      cadena = ""; // Limpiar la cadena para la próxima lectura
    } else {
      cadena += inChar; // Agregar carácter a la cadena
    }
    delay(1);
  }
  delay(1);
  /*************/
  
  //Se ejecutaran o tendran valor cada 5 segundos
  //para todas las variables hay que ponerle un millis de 5 segundos
  //cuando se pongan las demas, crear una función que obtenga valores
  //del serial cada 5 segundos con millis y devolverlos para subirlos
  //a la base de datos cada 30 segundos.

  //El cargado de información debe ser en un tiempo largo, para no
  //Gastar recursos de la DB y que nos cobren por usarla
  //el valor se subirá cada 12 segundos segun la variable global

  ///Mandarle los datos de las variables a la función para cargarlos en la base de datos
  uploadingVariables(winddir, windSpeedkm, humidity, temp_c, rainin, pressure, light_level, batt_level);
  /**********/
  
}

//funcion obtener fecha

String getTime(){
  WiFiClient client;
  HTTPClient http;

  String timeS = "";
  http.begin(client, "http://worldtimeapi.org/api/timezone/America/Bogota");
  int httpCode = http.GET();

  if(httpCode == HTTP_CODE_OK ||httpCode == HTTP_CODE_MOVED_PERMANENTLY){
    String payload = http.getString();
    int beginS = payload.indexOf("datetime");
    int endS = payload.indexOf("day_of_week");

    timeS = payload.substring(beginS + 11, endS - 3);
    //timeS tiene la fecha completa de la api
  }
  return timeS;
}

//Enviar variables a la base de datos
//Actualizar los parámetros para subir la información a la base de datos de forma correcta

void uploadingVariables(float value, float value2, float value3, float value4, float value5, float value6, float value7, float value8){
  if(millis() - tiempoAntMedicion >= tiempoMedicion * 1000){
    //Subir temperatura al server
    tiempoAntMedicion = millis();
    Serial.print("uploadingTemperatura ->");
    Serial.println(value);

    String Time = getTime();
    String path = "mediciones/";
    StaticJsonBuffer < 200 > jsonBuffer;
    JsonObject& data = jsonBuffer.createObject();
    
    data["time"] = Time;
    data["WindDir"] = value;
    data["WindSpeed"] = value2;
    data["Humidity"] = value3; 
    data["Temperature"] = value4;
    data["Rain"] = value5;
    data["Pressure"] = value6;
    data["Light"] = value7;
    data["Batt"] = value8;

    Firebase.push(path, data);
  }
}

//IMPORTANTE -- PONER EL MILLIS DE 5 SEGUNDOS A LA FUNCION DE LA CADENA 
void procesarCadena(String cadena) {
  inicio = 0;
  fin = cadena.indexOf(separador, inicio);
  int contador = 0;

  if(millis() - tiempoAntLect >= tiempoLect * 1000){
      while (fin != -1) {
        campo = cadena.substring(inicio, fin);
        asignarValor(contador, campo);
    
        inicio = fin + 1;
        fin = cadena.indexOf(separador, inicio);
        contador++;
       }

  // Último valor después del último separador
  campo = cadena.substring(inicio);
  asignarValor(contador, campo);

  /* Imprimir los valores para verificar
  Serial.print("winddir: "); Serial.println(winddir);
  Serial.print("windSpeedkm: "); Serial.println(windSpeedkm);
  Serial.print("humidity: "); Serial.println(humidity);
  Serial.print("temp_c: "); Serial.println(temp_c);
  Serial.print("rainin: "); Serial.println(rainin);
  Serial.print("pressure: "); Serial.println(pressure);
  Serial.print("light_level: "); Serial.println(light_level);
  */
   }
  
  
}

void asignarValor(int index, String valor) {
  switch (index) {
    case 0:
      winddir = valor.toFloat();
      break;
    case 1:
      windSpeedkm = valor.toFloat();
      break;
    case 2:
      humidity = valor.toFloat();
      break;
    case 3:
      temp_c = valor.toFloat();
      break;
    case 4:
      rainin = valor.toFloat();
      break;
    case 5:
      pressure = valor.toFloat();
      break;
    case 6:
      light_level = valor.toFloat();
      break;
    case 7:
      batt_level = valor.toFloat();
      break;
  }
}
