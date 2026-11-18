#pragma once

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// ---------- Pinos do encoder/botão ----------
// (usados tanto pelo menu quanto pela rotina de emergência, por isso ficam aqui)
#define SW  18
#define CLK 16
#define DT  14

// ---------- Objeto do LCD ----------
// Compartilhado entre main.cpp (onde é criado) e Menu.cpp (onde é usado)
extern LiquidCrystal_I2C lcd;

// ---------- Variáveis do menu e do encoder ----------
extern byte L_botao;
extern byte LA_botao;
extern byte tela;                 // 1 a 4 = Menu Principal | 10, 20, 30, 40 = Submenus
extern int  referencia;
extern int  rotacao;
extern bool limpar_lcd;
extern bool editando;             // controla se estamos alterando um valor ou navegando
extern bool malhaAtiva;
extern volatile long counter;
extern bool mFechada;             // ed Werther
extern bool modoDegrau;           // ed Werther
extern const unsigned long INTERVALO_DEGRAU; // 10 segundos por degrau
extern volatile bool precisaSalvarEEPROM;

// ---------- Variáveis do processo ----------
// Escritas pela malha de controle (main.cpp) e lidas/editadas pelo menu (Menu.cpp)
extern float u;      // % servo
extern float y;      // graus
extern float y_anterior;
extern float yr;
extern float Kp;
extern float Ki;
extern float Kd;
extern float termoI;
