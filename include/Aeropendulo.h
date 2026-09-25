#pragma once

// ---------- Pinos de acionamento/leitura ----------
#define pin_esc 6   // saída (ESC do motor)
#define pin_pot 4   // entrada (potenciômetro / sensor de ângulo)

// ---------- Constantes do processo ----------
#define ymax       80.0
#define angulo_max 360.00
#define Ts         50      // período de amostragem da malha, em ms

// ---------- Endereços na EEPROM (espaçados de 4 em 4 bytes para os floats) ----------
#define ADDR_KP 0
#define ADDR_KI 4
#define ADDR_KD 8

// ---------- Funções da malha de controle, definidas em main.cpp ----------
void leituraAngulo();
void acionaSaida();
void imprime();
void fechaMalhaPID();
void armazenaGanhos();
void acionamentoSerial();
void verificaEmergencia();
void modoEmergencia();
