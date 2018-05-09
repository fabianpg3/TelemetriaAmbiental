//----------------Include necesarios----------------
#include <Wire.h>
//#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <RFTransmitter.h>
#include <stdlib.h>
#include <SD.h>

//---------define necesarios----------------------

#define emisorID 22
#define pinSalida 11
#define MPU 0x68

//------------Declaraciones varias-------------------

RFTransmitter emisor(pinSalida, emisorID); //configuracion del emisor
Adafruit_BMP280 sensorPresion; //configuracion del sensor de presion

//-------variables y constantes----------------
float Temp = 0;
float Alt = 0;
float Presion = 0;
const int chipSelect = 4;
const int MQ7 = A3;
int sensorCO = 0;
char buffer1[20] = "";
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ;

String strAltitud, strMensaje;
//--------------funciones------------
void enviarMensaje(char*msg) {
  emisor.send((byte *)msg, strlen(msg) + 1);
  delay(100);
  emisor.resend((byte *)msg, strlen(msg) + 1);
}

void visualizarEnPantalla(){
  Serial.println("----------------INICIO-----------------------");
  Serial.print("Temperatura: "); Serial.print(Temp); Serial.println(" C");
  Serial.print("Presion: "); Serial.print(Presion); Serial.println(" Pa");
  Serial.print("Altura: "); Serial.print(Alt); Serial.println(" m");
  Serial.print("Valor de CO (analogRead): "); Serial.println(sensorCO);
  Serial.print ("Aceleracion en X: "); Serial.println(AcX);
  Serial.print ("Aceleracion en Y: "); Serial.println(AcY);
  Serial.print ("Aceleracion en Z: "); Serial.println(AcZ);
  Serial.print ("Giroscopio en X: ");  Serial.println(GyX);
  Serial.print ("Giroscopio en Y: "); Serial.println(GyY);
  Serial.println(buffer1);//prueba del mensaje a enviar por la antena
  Serial.println("-----------------FIN---------------------");
}
//-----------------------Setup--------------------------

void setup() {
  Serial.begin(9600);
  Serial.println(F("Sistema de telemetria ambiental"));
  enviarMensaje("Sistema listo, despegue en 1 minuto");
  
  Serial.print("Iniciando tarjeta SD.."); // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Error en la tarjeta, no se ha encontrado ninguna memoria extraible");
    return;
  }
  Serial.println("Tarjeta iniciada correctamente");
  SD.remove("datalog.txt");

  strAltitud = String("Altitud: ");
  strMensaje=String();
  File archivoDatos = SD.open("datalog.txt", FILE_WRITE);
  delay(1000);
  
  if (!sensorPresion.begin()) {
    Serial.println(F("Error, no se ha encontrado ningun sensor de presion valido"));
    //while (1);
  }
  Wire.begin(); //Inicia la comunicacion i2c
  Wire.beginTransmission(MPU); ////se conecta con el acelerometro
  Wire.write(0x6B);//de acuerdo a la hoja de datos, este es el registro necesario como primer paso para comunicarse con el sensor
  Wire.write(0);
  Wire.endTransmission(true);
  archivoDatos.println("Sistema iniciado correctamente");
  archivoDatos.println("Inicia la recoleccion de datos");
  archivoDatos.println("Altitud   Presion    SensorCO    Acell X   Acell Y   Acell Z   Gyro X   Gyro Y   tiempo (ms)  ");
  archivoDatos.close(); //--------------------
  





}

//-----------------programa principal------------------
void loop() {
  File archivoDatos = SD.open("datalog.txt", FILE_WRITE);
  sensorCO = analogRead(MQ7); //obtiene el valor del sensor de gas CO. Como se trata de una entrada analogica, el valor leido oscila entre 0 y 1023 por lo cual para convertirlo a PPM, lo procesamos luego en computadora.
  Temp = sensorPresion.readTemperature(); //Obtiene la temperatura ambiental del sensor de presion
  Presion = sensorPresion.readPressure(); //Obtiene la medicion de presion atmosferica del sensor de presion
  Alt = sensorPresion.readAltitude(1013.25); //Obtiene la altura aproxima. Es ajustado a las condiciones locales, el valor entre parentesis corresponde a la presion a nivel del mar en hPa
  
  Wire.beginTransmission(MPU); //Inicia la comunicacion con el Acelerometro
  Wire.write(0x3B); //De acuerdo a la hoja de datos, este es el registro para obtener los valores de aceleracion
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); //Una vez enviado el registro, se leen los valores devueltos por el acelerometro

  AcX = Wire.read() << 8 | Wire.read(); //Cada valor ocupa 2 registros, esta y las siguientes dos lineas, se realiza un corrimiento de bits.
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(MPU);
  Wire.write(0x43);  //De acuerdo a la hoja de datos, este es el registro para obtener los valores del giroscopio
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 4, true); //Una vez enviado el registro, se leen los valores devueltos por el giroscopio
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  
  
  
  archivoDatos.print(Alt); archivoDatos.print("  "); archivoDatos.print(Presion); archivoDatos.print("  "); archivoDatos.print(sensorCO); archivoDatos.print("  "); archivoDatos.print(AcX); archivoDatos.print("  "); archivoDatos.print(AcY); archivoDatos.print("  "); //guarda las mediciones en la memoria SD
  archivoDatos.print(AcZ); archivoDatos.print("  "); archivoDatos.print(GyX); archivoDatos.print("  "); archivoDatos.print(GyY); archivoDatos.print("    "); archivoDatos.println(millis()); 
  archivoDatos.close();
  
  strMensaje=sensorCO;
  strMensaje.toCharArray(buffer1, 50);
  enviarMensaje(buffer1);
  visualizarEnPantalla(); //funcion para visualizar los datos en el monitor serie del Arduino IDE
 
  delay(300);
  
}




