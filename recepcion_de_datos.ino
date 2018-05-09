//--------------------lIBRERIAS NECESARIAS---------------------
#include <PinChangeInterruptHandler.h>
#include <RFReceiver.h>
#include <LiquidCrystal.h>
#include <stdlib.h>

//------------------Inicializaciones necesarias----------------------
RFReceiver receiver(8);
LiquidCrystal pantalla(12, 11, 5, 4, 3, 2);

//-------Variables necesarias--------------------
float CO=0;
float ppm=0;
//------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("-----------Desarrollado por Fabian Picado------------------");
  Serial.println("Inicializando sistema de telemetria ambiental..");
  pantalla.begin(16, 2);
  pantalla.print("Inicializando..");
  pantalla.blink();
  receiver.begin();
  delay(3000);
  Serial.println("Sistema iniciado. Esperando conexión con el cohete");
  pantalla.clear();
  pantalla.print("Esperando ");
  pantalla.setCursor(0, 1);
  pantalla.print("conexion..");
  pantalla.blink();

}
void loop() {
  char mensaje[MAX_PACKAGE_SIZE];
  byte emisorId = 0;
  byte paqueteId = 0;
  byte longitud = receiver.recvPackage((byte *)mensaje, &emisorId, &paqueteId);
  if (paqueteId==1){
    pantalla.clear();
    Serial.println("Conexion establecida con sistema abordo");
    pantalla.print("Conex. lista"); 
    pantalla.setCursor(0,1);
    pantalla.print("60 segundos..");
    }
  else{
      
    pantalla.noBlink();
    pantalla.clear();
    CO = atoi(mensaje);
    CO = CO *5;
    CO= CO/1023;
    CO=CO*1.07;
    ppm = 3.03 * pow(2.07,CO);
    pantalla.print("CO(ppm): "); pantalla.print(ppm);
    pantalla.setCursor(0, 1);
    pantalla.print("Paquete: "); pantalla.print(paqueteId);
    Serial.println("");
    Serial.print("Paquete: "); Serial.println(paqueteId);
    Serial.print("Emisor: "); Serial.println(emisorId);
    Serial.print("Mensaje: "); Serial.println(mensaje);
  
  }
}
