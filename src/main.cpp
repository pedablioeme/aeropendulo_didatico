/*
===============================================
IFSC - Campus Araranguá
Curso Técnico Integrado em Eletromecânica
Projeto Integrador III

AEROPÊNDULO PARA ENSINO DE CONTROLE DE PROCESSOS

Autores:
 - Ana Laura de Souza Teixeira
 - Bryan da Conceição Teixeira
 - Ícaro da Silva Melo

Orientador: Prof. Werther Serralheiro
===============================================
*/

#include <Arduino.h>
#include <Servo.h>
#include <TimerOne.h>
#include <Wire.h>
#include <PID_v1.h>     // PID do Brett Beauregard (Library Manager: "PID")
#include <EEPROM.h>

#include "Globals.h"
#include "Aeropendulo.h"
#include "Menu.h"

// ---------- Definição do objeto do LCD (declarado extern em Globals.h) ----------
LiquidCrystal_I2C lcd(0x27, 20, 4);

// ---------- Objetos exclusivos deste arquivo ----------
Servo Esc;

// ---------- Definição das variáveis do menu e do encoder (declaradas extern em Globals.h) ----------
byte L_botao = 1;
byte LA_botao = 1;
byte tela = 1;                 // 1 a 4 = Menu Principal | 10, 20, 30, 40 = Submenus
int  referencia = 0;
int  rotacao = 0;
bool limpar_lcd = 0;
bool editando = false;         // controla se estamos alterando um valor ou navegando
volatile long counter = 0;
bool mFechada = false;         // ed Werther
volatile bool precisaSalvarEEPROM = false;

// ---------- Definição das variáveis do processo (declaradas extern em Globals.h) ----------
int   u = 0;       // % servo
float y = 0.0;      // graus
float yr = 0.0;
float Kp = 0.0;
float Ki = 0.0;
float Kd = 0.0;

// ---------- Variáveis exclusivas da biblioteca PID (a lib exige ponteiros para double) ----------
// o resto do código continua usando yr, y e u normalmente
double pidSetpoint = 0.0;   // <- yr
double pidInput    = 0.0;   // <- y
double pidOutput   = 0.0;   // -> u

PID pid(&pidInput, &pidOutput, &pidSetpoint, Kp, Ki, Kd, DIRECT);

// ---------- Funcionamento (exclusivas deste arquivo) ----------
int  leitura_inicial = 0;
long tempo_atual;
bool emergencia;
int  sinal_esc;

void (*resetFunc)() = 0;

void setup() {
  Serial.begin(115200);

  // caracteres customizados (usados só aqui, por isso ficam locais ao setup)
  static byte customChar1[] = { B01101, B10010, B01110, B00001, B01111, B10001, B01111, B00000 }; // ç
  static byte customChar2[] = { B00000, B00000, B01110, B10000, B10001, B01110, B00100, B01100 }; // a com til
  static byte customChar3[] = { B11000, B11000, B00000, B00000, B00000, B00000, B00000, B00000 }; // ° (graus)

  // inicializa LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  lcd.clear();

  // modo dos pinos
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(pin_pot, INPUT);

  // leitura inicial encoder
  referencia = digitalRead(CLK);

  // interrupção
  Timer1.initialize(1000);             // interrupção a cada 1 milissegundo
  Timer1.attachInterrupt(Timer);       // Timer() está definida em Menu.cpp

  // configuração do PID
  pid.SetSampleTime(Ts);               // mesmo Ts da malha (ms)
  pid.SetOutputLimits(0, 100);         // u em % (o padrão da lib seria 0-255)
  pid.SetMode(MANUAL);                 // só entra em AUTOMATIC quando mFechada = true

  // frase de abertura
  lcd.setCursor(0, 1); lcd.print("Aeropendulo Didatico");
  lcd.setCursor(0, 2); lcd.print("   ELETROMECANICA   ");
  delay(1000);
  lcd.clear();

  EEPROM.get(ADDR_KP, Kp);
  EEPROM.get(ADDR_KI, Ki);
  EEPROM.get(ADDR_KD, Kd);

  if (isnan(Kp) || isnan(Ki) || isnan(Kd)) {
    Kp = 0;
    Ki = 0;
    Kd = 0;
  }

  leitura_inicial = analogRead(pin_pot);
  Esc.attach(pin_esc);
  tempo_atual = millis();
}

void loop() {
  if (!emergencia) {
    menu();                                // Menu.cpp
    if ((millis() - tempo_atual) >= Ts) {
      leituraAngulo();
      if (mFechada) {
        fechaMalhaPID();
      }
      else if (pid.GetMode() == AUTOMATIC) {
        pid.SetMode(MANUAL);              // malha aberta: PID fica parado, u é manual
      }
      acionaSaida();
      imprime();
      tempo_atual = millis();
    }
    acionamentoSerial();
    verificaEmergencia();
    if (precisaSalvarEEPROM) {
      armazenaGanhos();
      precisaSalvarEEPROM = false;
    }
  }
  else {
    modoEmergencia();
  }
}

void leituraAngulo() {
  int leitura_pot = analogRead(pin_pot);
  int leitura_relativa = (leitura_pot - leitura_inicial);
  y = leitura_relativa * (angulo_max / 1023.00);
}

void acionaSaida() {
  sinal_esc = map(u, 0, 100, 0, 179);
  Esc.write(sinal_esc);
}

void imprime() {
  Serial.print("u:");
  Serial.print(u);
  Serial.print(" | yr:");
  Serial.print(yr);
  Serial.print(" | y:");
  Serial.println(y);
}

void fechaMalhaPID() {
  // transferência sem tranco: ao ligar a malha, a lib parte do u atual
  if (pid.GetMode() == MANUAL) {
    pidOutput = u;
    pidInput  = y;
    pid.SetMode(AUTOMATIC);               // Initialize() lê pidOutput e pidInput
  }

  // variáveis do código -> variáveis da biblioteca
  pidSetpoint = yr;
  pidInput    = y;
  pid.SetTunings(Kp, Ki, Kd);             // acompanha mudanças feitas no menu

  pid.Compute();

  // variáveis da biblioteca -> variáveis do código
  u = (int)round(pidOutput);
}

void armazenaGanhos() {
  EEPROM.put(ADDR_KP, Kp);
  EEPROM.put(ADDR_KI, Ki);
  EEPROM.put(ADDR_KD, Kd);
}

void acionamentoSerial() {
  if (Serial.available() > 0) {
    u = constrain(Serial.parseInt(), 0, 100);
  }
}

void verificaEmergencia() {
  if (y > ymax) {
    emergencia = true;
    Esc.write(0);
    u = 0;
    Timer1.detachInterrupt();
    lcd.clear();
  }
}

void modoEmergencia() {
  lcd.setCursor(0, 0); lcd.print("     EMERGENCIA     ");
  lcd.setCursor(0, 1); lcd.print("      ATIVADA       ");
  lcd.setCursor(0, 2); lcd.print("clique no bot"); lcd.write(1); lcd.print("o");
  lcd.setCursor(0, 3); lcd.print("para reiniciar");
  if (digitalRead(SW) == LOW) {
    resetFunc();
  }
}
