<div align="center">

# Aeropêndulo Didático

</div>

## Sobre

Este projeto contém o código desenvolvido para um **Arduino Mega 2560** responsável pelo acionamento e gerenciamento de uma bancada de aeropêndulo, também conhecida como pêndulo subamortecido.

O programa permite operar a planta em:

- **Malha aberta**, para observação da resposta do sistema ao acionamento aplicado;
- **Malha fechada**, utilizando um controlador **PID** para acompanhamento de uma referência;
- **Menu interativo**, que possibilita a configuração e o acompanhamento da planta pelo usuário.

## Funcionalidades

- Leitura da variável de processo por meio do sensor de posição;
- Acionamento do atuador responsável pela movimentação do pêndulo;
- Operação em malha aberta;
- Controle PID em malha fechada;
- Exibição de informações e parâmetros por meio de um menu interativo;
- Monitoramento da resposta da planta durante os ensaios;
- Sistema de segurança quando o ângulo atinge um valor predeterminado.

## Modos de operação

### Malha aberta

O sinal de controle é aplicado ao atuador sem realimentação. Esse modo é indicado para observar o comportamento dinâmico da planta e realizar ensaios de identificação.

### Malha fechada (PID)

O sinal de controle é calculado a partir do erro entre a referência e a variável medida. Os parâmetros proporcional, integral e derivativo podem ser utilizados para avaliar o desempenho do sistema de controle.

## Como utilizar

......

## O Aeropendulo

Resumo: Devido à alta complexidade e carga teórica restrita ao nível final das graduações de engenharia que exige o entendimento da área de controle de processos e o alto custo de bancadas didáticas no mercado, este projeto desenvolveu uma bancada didática para ensino de controle, usando a variável ângulo gerada de um pêndulo subamortecido. Tendo em vista a inviabilidade do ensino puramente teórico desta área, dada seu conteúdo já citado denso, busca-se transmitir de maneira empírica os conceitos requisitados em controle por meio desta bancada. A metodologia consistiu na construção de uma planta física em uma estrutura de perfil de alumínio, acionada por um conjunto de motor brushless e ESC, utilizando um Arduino Mega para aquisição de dados e controle. Os resultados experimentais em malha aberta evidenciaram a visualização do comportamento físico do sistema, que retratou com fidelidade a oscilação característica de sistemas subamortecidos.


<p align="center">
  <img src="/imagens/aeropendulo.png" alt="Aeropendulo" width="800px">
</p>

O pêndulo foi construído em uma estrutura de perfil de alumínio 30x30 com um formato de base retangular composto de hastes verticais, sustentadas por placas, que suportarão um eixo linear [1] de 8mm de diâmetro e de 300mm de comprimento. Neste eixo, será unida uma barra de alumínio que irá ser o pêndulo propriamente dito. Além disso, estará fixado um mancal [2] em cada uma de suas duas extremidades, o qual serve de apoio e o sujeita à ação de rotação. Para aferir o ângulo do aeropêndulo, empregamos um potenciometro multivoltas [3]. O dispositivo integrado ao eixo linear exerce sua função pela alteração de sua resistência à medida que o eixo é rotacionado. A tensão de saída, portanto, é interpretada pelo Arudino e é tranformada em uma variável correspondente à variável de ângulo controlada θ(t).  

Essa variação de ângulo se dá em decorrência da movimentação do pêndulo através de seu sistema de propulsão (sistema pêndulo-motor-esc-hélice). Essa propulsão está relacionada ao torque de um motor brushless modelo A2212 1400KV acoplado a uma hélice [4], que produzirá uma força de empuxo resultando em um eventual movimento de rotação no eixo. O motor é energizado eletricamente e acionado via sinal PWM (do inglês Pulse Width Modulation) enviado a um ESC [5] modelo Skywalker V2  20A, da marca Hobbywing, o qual executa sua função por meio de sinais elétricos temporizados que são traduzidos em mudanças de velocidade do motor. Todo o sistema será energizado eletricamente por meio de uma fonte chaveada de 12V.

Como unidade de controle do sistema, será empregado um microcontrolador Arduino Mega, cujo propósito é poder comportar um programa baseado em modelagens matemáticas explicadas pela teoria do controle de processos, que consiga exitosamente implementar uma porcentagem dada no ESC capaz de manter o pêndulo no ângulo desejado pelo controlador. Portanto, a variável manipulada é a porcentagem de sinal enviado ao controle eletrônico de velocidade, de modo que quanto maior for a velocidade aplicada, maior será a sua propulsão. Esta ação resultará em um aumento na altura do pêndulo e, por consequência, no ângulo. Logo, a variável controlada será o ângulo entre a posição inicial (atribuída ao ângulo 0° marcado pelo repouso do pêndulo em sentido da força da gravidade) e final do pêndulo (ângulo máximo limitado pela barreira física na estrutura do aeropêndulo).
