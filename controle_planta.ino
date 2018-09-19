#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//***Portas Digitais***//
const int led_alto_brilho = 6;
const int RELAY1 = 7;
const int sensor_agua = 8;
//********************//

//***Portas Analogicas***//
const int sensor_LDR = A0;
const int potenc_led = A3;
const int sensor_umidade = A2;
//***********************//

//***Variáveis de armazenamento***//
//Sensor Umidade
int leitura_umidade = 0;
int pwm_umidade = 0;
//Reservatorio de água
int sensor_reservatorio = 0;
//Sensor LDR
int valor_LDR = 0;
int valor_potenc_led = 0;
int pwm_potenc_led = 0;
//*******************************//

//***Variáveis de controle***//
//Referente a Irrigacao
bool irrigacao_ativa = false;
//Referente ao Reservatorio
bool reservatorio_vazio = false;
//Referente ao LCD
bool lcd_print_vazio = false;
//Referente a Iluminacao
bool led_ativo = false;
//***************************//

byte byte_caractereSeta[8] = {
 0b00000,
 0b00000,
 0b00010,
 0b00001,
 0b00001,
 0b00010,
 0b00000,
 0b00000
};

void setup() {
 Serial.begin(9600);
 pinMode(led_alto_brilho, OUTPUT);
 pinMode(sensor_umidade, INPUT);
 pinMode(RELAY1, OUTPUT);
 pinMode(sensor_agua, INPUT);
 lcd.createChar(0, byte_caractereSeta);
 //Display LCD 16x2
 lcd.begin(16, 2);
}
void loop() {
 sensor_reservatorio = digitalRead(sensor_agua);
 (sensor_reservatorio == 1) ? reservatorio_vazio = false : reservatorio_vazio = true;
 
 Serial.print("Reservatorio: ");
 Serial.print(sensor_reservatorio);
 Serial.println();
 
 
 //******** Controle do de Umidade e Irrigação *************//
 leitura_humidade = analogRead(sensor_umidade);
 pwm_umidade = map(leitura_umidade, 0, 1023, 100, 0); //Porcentagem de umidade da planta
 
 //Mostra a porcentagem de umidade do sola no serial Monitor
 Serial.print("Humidade: ");
 Serial.print(pwm_umidade);
 Serial.println(" %");
 
 if (!reservatorio_vazio) {
    if (pwm_umidade < 25 && !irrigacao_ativa) {
      irrigacao_ativa = true;
    }
    else if (pwm_umidade > 45 && irrigacao_ativa) {
      irrigacao_ativa = false;
    }
    
    if (irrigacao_ativa && digitalRead(RELAY1) == HIGH) {
     //Acionar o rele da bomba
     Serial.println("IRRIGACAO ATIVADA --- ");
     digitalWrite(RELAY1, LOW);
     //Escrever no LCD
     delay(10);
     lcd.clear();
     lcd.setCursor(0,1);
     delay(10);
     lcd.print("Irrigando Planta");
    }
    else if (!irrigacao_ativa && digitalRead(RELAY1) == LOW) {
     //Desacionar rele da bomba
     Serial.println("IRRIGACAO DESATIVADA --- ");
     digitalWrite(RELAY1, HIGH);
    }
    lcd_print_vazio = false;
    
    if (!irrigacao_ativa) {
      informacoes(valor_LDR, pwm_umidade);
    }
 } else {
   Serial.println("Reservatorio Vazio");
   
   if (digitalRead(RELAY1) == LOW) {
     //Desligar o relé caso esteja ligado com o reservatório vazio
     digitalWrite(RELAY1, HIGH);
   }
   if (!lcd_print_vazio) {
     lcd.clear();
     delay(50);
     lcd.setCursor(0, 0);
     lcd.print("Reservatorio");
     lcd.setCursor(0, 1);
     lcd.print("VAZIO");
     lcd_print_vazio = true;
   }
 }
 //*********************************************************//

 
 /************** Controle do LED da Planta **********************/
 valor_LDR = analogRead(sensor_LDR);
 (valor_LDR < 110) ? led_ativo = true : led_ativo = false;
 if (led_ativo) {
   valor_potenc_led = analogRead(potenc_led);
   pwm_potenc_led = map(valor_potenc_led, 0, 1023, 0, 255);
   analogWrite(led_alto_brilho, pwm_potenc_led);
 } else {
    digitalWrite(led_alto_brilho, LOW);
 }
}
 //*********************************************************//

 
//************* FUNÇÔES ******************//
void informacoes(int ldr, int umidade) {
   //Informações do sensor de Umidade
   lcd.setCursor(0,0);
   lcd.write((uint8_t)0);
   lcd.setCursor(1,0);
   lcd.print(" Umidade: ");
   lcd.setCursor(12,0);
   lcd.print(umidade);
   lcd.setCursor(14,0);
   lcd.print("%");
   //Informacoes do sensor LDR
   lcd.setCursor(0,2);
   lcd.write((uint8_t)0);
   lcd.setCursor(1,2);
   lcd.print(" Inten.luz: ");
   lcd.setCursor(13,1);
   lcd.print(ldr);
   lcd.setCursor(15,0);
   lcd.print(" ");
   delay(400);
}
 //*********************************************************//
