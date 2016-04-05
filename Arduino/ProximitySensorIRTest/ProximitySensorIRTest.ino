void setup() {
  // put your setup code here, to run once:
  pinMode(A1, INPUT); 
  pinMode(A0, OUTPUT); 
  pinMode(A2, OUTPUT); 
  digitalWrite(A0, HIGH); 
  digitalWrite(A2, LOW); 

  Serial.begin(115200); 
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A1)); 
  
  delay(10); 
  
}
