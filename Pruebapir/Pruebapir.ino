int led = 4;
int pir = 5;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
  Serial.begin(115200);
}

void loop() {
  int estado = digitalRead(pir);
  if(estado == HIGH){
    digitalWrite(led,HIGH);
    Serial.println("Pir Activo");
    delay(4000);
 
  }else{
    digitalWrite(led,LOW);
    Serial.println("Pir NO Activo");
    delay(1);

  }

}
