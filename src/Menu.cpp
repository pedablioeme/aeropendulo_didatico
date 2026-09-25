#include <Arduino.h>

#include "Globals.h"
#include "Menu.h"

// Protótipos das funções auxiliares, usadas somente neste arquivo
static void atualizacao_do_display();
static void menu_inicial();
static void secao_malhaaberta();
static void secao_malhafechada();
static void secao_config();

void menu() {
  if (limpar_lcd == 1) {
    lcd.clear();
    limpar_lcd = 0;
  }
  else {
    atualizacao_do_display();
  }
}

static void atualizacao_do_display() {
  char cursorChar = editando ? ':' : '>'; // ">" indica navegação. ":" indica edição

  // MENU PRINCIPAL
  if (tela >= 1 && tela <= 4) {
    lcd.setCursor(0, 0); lcd.print(tela == 1 ? ">Malha Aberta      " : " Malha Aberta      ");
    lcd.setCursor(0, 1); lcd.print(tela == 2 ? ">Malha Fechada     " : " Malha Fechada     ");
    lcd.setCursor(0, 2); lcd.print(tela == 3 ? ">Modo: PID         " : " Modo: PID         ");
    lcd.setCursor(0, 3); lcd.print(tela == 4 ? ">Configura" : " Configura");
    lcd.write(2);
    lcd.write(1);
    lcd.print("o       ");
  }

  // SUBMENU 1: MALHA ABERTA
  else if (tela >= 11 && tela <= 14) {
    lcd.setCursor(0, 0); lcd.print(tela == 11 ? ">Voltar" : " Voltar");
    lcd.setCursor(0, 1); lcd.print(tela == 12 ? cursorChar : ' '); lcd.print(" u: "); lcd.print(u); lcd.print("%   ");
    lcd.setCursor(0, 2); lcd.print(tela == 13 ? " >y: " : "  y: "); lcd.print(y), lcd.write(3);
    lcd.setCursor(0, 3); lcd.print("                  ");
  }

  // SUBMENU 2: MALHA FECHADA
  else if (tela >= 21 && tela <= 24) {
    lcd.setCursor(0, 0); lcd.print(tela == 21 ? ">Voltar" : " Voltar");
    lcd.setCursor(0, 1); lcd.print(tela == 22 ? cursorChar : ' '); lcd.print(" yr: "); lcd.print(yr); lcd.write(3); lcd.print("   ");
    lcd.setCursor(0, 2); lcd.print(tela == 23 ? " >y: " : "  y: "); lcd.print(y), lcd.write(3);
    lcd.setCursor(0, 3); lcd.print("Modo: PID      ");
  }

  // SUBMENU 3: MODO
  else if (tela == 31) {
    lcd.setCursor(0, 0); lcd.print(">Voltar      ");
    lcd.setCursor(0, 1); lcd.print("  PID            ");
    lcd.setCursor(0, 2); lcd.print("  Fuzzy          ");
    lcd.setCursor(0, 3); lcd.print("  MPC            ");
  }

  // SUBMENU 4: CONFIGURAÇÃO
  else if (tela >= 41 && tela <= 44) {
    lcd.setCursor(0, 0); lcd.print(tela == 41 ? ">Voltar" : " Voltar");
    lcd.setCursor(0, 1); lcd.print(tela == 42 ? cursorChar : ' '); lcd.print(" Kp = "); lcd.print(Kp);
    lcd.setCursor(0, 2); lcd.print(tela == 43 ? cursorChar : ' '); lcd.print(" Ki = "); lcd.print(Ki);
    lcd.setCursor(0, 3); lcd.print(tela == 44 ? cursorChar : ' '); lcd.print(" Kd = "); lcd.print(Kd);
  }
}

// lógica de interrupção do timer (leitura do encoder)
void Timer() {
  rotacao = digitalRead(CLK);

  if (rotacao != referencia) {
    if (digitalRead(DT) != rotacao) counter++;    // gira para a direita = baixo
    else counter--;                               // gira para a esquerda = cima

    if (tela >= 1 && tela <= 4) menu_inicial();
    if (tela >= 11 && tela <= 14) secao_malhaaberta();
    if (tela >= 21 && tela <= 24) secao_malhafechada();
    if (tela >= 41 && tela <= 44) secao_config();
  }

  referencia = rotacao;

  // acionamento do botão
  L_botao = digitalRead(SW);
  if (L_botao == 0 && LA_botao == 1) {
    limpar_lcd = 1;

    // entra nos submenus
    if (tela == 1) tela = 11;
    else if (tela == 2) tela = 21;
    else if (tela == 3) tela = 31;
    else if (tela == 4) tela = 41;

    // sai dos submenus (voltar)
    else if (tela == 11) tela = 1;
    else if (tela == 21) tela = 2;
    else if (tela == 31) tela = 3;
    else if (tela == 41) tela = 4;

    // se clicar em uma variável, alterna o modo de edição
    else if (tela == 12 || tela == 22 || tela == 42 || tela == 43 || tela == 44) {
      editando = !editando;
    }
    if (!editando) {
      precisaSalvarEEPROM = true;
    }
  }
  LA_botao = L_botao;
}

static void menu_inicial() {
  // girando para direita (baixo)
  if (counter == 1) {
    if (tela < 4) tela++; else tela = 1;
    counter = 0;
  }

  // girando para esquerda (cima)
  else if (counter == -1) {
    if (tela > 1) tela--; else tela = 4;
    counter = 0;
  }
}

static void secao_malhaaberta() {
  mFechada = false;
  if (!editando) {
    if (counter == 1) {
      if (tela < 12) tela++; else tela = 11;
      counter = 0;
    }
    else if (counter == -1) {
      if (tela > 11) tela--; else tela = 12;
      counter = 0;
    }
  }
  // edição da entrada
  else {
    if (counter == 1) { u++; if (u > 100) u = 100; counter = 0; }
    else if (counter == -1) { u--; if (u < 0) u = 0; counter = 0; }
  }
}

static void secao_malhafechada() {
  mFechada = true;
  if (!editando) {
    if (counter == 1) {
      if (tela < 22) tela++; else tela = 21;
      counter = 0;
    }
    else if (counter == -1) {
      if (tela > 21) tela--; else tela = 22;
      counter = 0;
    }
  }
  // edição do setpoint
  else {
    if (counter == 1) { yr++; if (yr > 180) yr = 180; counter = 0; }
    else if (counter == -1) { yr--; if (yr < 0) yr = 0; counter = 0; }
  }
}

static void secao_config() {
  if (!editando) {
    if (counter >= 1) {
      if (tela < 44) tela++; else tela = 41;
      counter = 0;
    }
    else if (counter <= -1) {
      if (tela > 41) tela--; else tela = 44;
      counter = 0;
    }
  }
  // edição dos ganhos
  else {
    float step = 0.01; // ajuste do degrau
    if (tela == 42) { Kp += (counter * step); if (Kp < 0) Kp = 0; }
    else if (tela == 43) { Ki += (counter * step); if (Ki < 0) Ki = 0; }
    else if (tela == 44) { Kd += (counter * step); if (Kd < 0) Kd = 0; }
    counter = 0;
  }
}
