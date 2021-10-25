#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Servo.h>

WiFiClient espClient;
PubSubClient client(espClient);
long last_msg = 0; 
char msg[50];
int value = 0;

int act1 = 1;
int act2 = 3;
int sensor_b = 15;
int agua = 13;
int DHTPIN = 12;
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor;

//BBBBBBBBBBBBBBBBBBBBB
int luz1 = 14;
int luz2 = 2;
// 0 para servomotor
int luz_exterior = 4;
int digital_ldr = 5;
int pir = 16;
int valor;

char str_hum_data[10];
char str_temp_data[10];
unsigned long lastMsg = 0;

const char* ssid = "CLARO1_DC6C44";
const char* password = "LI23vORFxM";
const char* mqtt_server = "192.168.1.201";

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(act1, OUTPUT);  
  pinMode(act2, OUTPUT);
  pinMode(sensor_b, INPUT); 
  pinMode(agua, OUTPUT); 
  pinMode(luz1, OUTPUT);
  pinMode(luz2, OUTPUT);
  pinMode(luz_exterior, OUTPUT);
  pinMode(digital_ldr, INPUT);
  pinMode(pir, INPUT);
  dht.begin();
  servoMotor.attach(0);
}

void setup_wifi(){
  delay(10);
  Serial.println();
  Serial.print("Conectando a: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length){
  Serial.print("Mensaje recibido en topic: ");
  Serial.print(topic);
  Serial.print(", Message: ");
  String message_temp;
  for(int i = 0; i < length; i++){
    Serial.print((char)message[i]);
    message_temp += (char)message[i];
  }
  Serial.println();

  //Primer pin de salida
  if(String(topic) == "casa/output1"){
    Serial.print("Cambio de salida: ");
    if(message_temp == "on"){
      Serial.println("on");
      digitalWrite(act1, HIGH);
    }else{
      Serial.println("off");
      digitalWrite(act1, LOW); 
    }
  }
   

  if(String(topic) == "casa/output4"){
  Serial.print("Cambio de salida PWM: ");
  Serial.println("message_temp");
  analogWrite(luz1, message_temp.toInt());
  }
  if(String(topic) == "casa/output5"){
    Serial.print("Cambio de salida PWM: ");
    Serial.println("message_temp");
    analogWrite(luz2, message_temp.toInt());
  }
  if(String(topic) == "casa/servo"){
    Serial.print("Cambio de estado: ");
    if(message_temp == "on"){
      servoMotor.write(180);
    }else{
      servoMotor.write(0);
    }
   }
  if(String(topic) == "casa/ldr_pir"){
    Serial.print("Cambio de salida: ");
    if(message_temp == "ldr"){
        valor = 1;
    }else if(message_temp == "pir"){
        valor = 0;
    }
  }
}



void reconnect(){
  while(!client.connected()){           //Se repetira hasta que se conecte
    Serial.print("Intentando conexion MQTT...");
    if(client.connect("ESP12E")){
      Serial.println("conectado");
      client.subscribe("casa/output1");
      client.subscribe("casa/output4");
      client.subscribe("casa/output5");
      client.subscribe("casa/servo");
      client.subscribe("casa/ldr_pir");
    }else{
      Serial.print("Fallo, rc= ");
      Serial.print(client.state());
      Serial.println(" intentelo de nuevo en 5s");
      delay(5000);
    }
  }
}


void loop(){
  if(!client.connected()){
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    float hum_data = dht.readHumidity();
    Serial.println(hum_data);
    /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
    dtostrf(hum_data, 4, 2, str_hum_data);
    float temp_data = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
    dtostrf(temp_data, 4, 2, str_temp_data);
    lastMsg = now;
    Serial.print("Publish message: ");
    Serial.print("Temperature - "); Serial.println(str_temp_data);
    client.publish("casa/temperature", str_temp_data);
    Serial.print("Humidity - "); Serial.println(str_hum_data);
    client.publish("casa/humidity", str_hum_data);
    if(temp_data >= 20 or hum_data <= 80){
      digitalWrite(agua, HIGH);
    }else{
      digitalWrite(agua, LOW);
    }
  }

  if(valor == 1){
    int  valor1 = digitalRead(digital_ldr);
    digitalWrite(luz_exterior, valor1);
  }else if(valor == 0){
    int  valor2 = digitalRead(pir);
    if(valor2 == HIGH){
      digitalWrite(luz_exterior, HIGH);
      delay(5000);
    }else{
      digitalWrite(luz_exterior, LOW);
      delay(1);
    }
  }


  digitalWrite(act2,digitalRead(sensor_b));
}
