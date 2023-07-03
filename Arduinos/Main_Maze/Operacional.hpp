#ifndef Operacional_hpp
#define Operacional_hpp

/*A classe Operacional tem como funcao juntar e abstrair, as leituras dos sensores,
e o funcionamento dos Atuadores.
As funcoes dessa classe ainda nao sao as ideias para utilizicacao no codigo main*/

/*!< Incluindo classes */
#include <Servo.h>
#include "Motor.hpp"
#include "Sensores_Novo.hpp"
#include "PID.hpp"
#include "Cor.hpp"

Cor cores;
Motor motores;
Sensores sensores;

#define CIRCUNFERENCIA_RODA 20
#define COMPRIMENTO_ROBO 8.5
#define NUM_PASSOS 10
#define ENCODER_CONSTANT 0.78530


/*! Constroi os PIDs*/
//KP, KI, KD, Setpoint, windup, limite
PID pidG(60.0, 0.1, 2.0, 0, 10, 50);  //Giroscopio


class Operacional {

private:

  int
    quadrado_ant[2];

  bool
    ultimo_passo,  //Encoder
    passo;

public:

  bool _black_flag = false;

  /********************** LEDS *********************/
  int led_resgate = 51;
  int led_sinal = 53;

  /*Inicia o LED*/
  void begin_LED() {
    pinMode(led_resgate, OUTPUT);
    pinMode(led_sinal, OUTPUT);
  }

  void ligaLED_resgate() {
    digitalWrite(led_resgate, HIGH);
  }

  void desligaLED_resgate() {
    digitalWrite(led_resgate, LOW);
  }
  void ligaLED_sinal() {
    digitalWrite(led_sinal, HIGH);
  }

  void desligaLED_sinal() {
    digitalWrite(led_sinal, LOW);
  }
  /********************** Servos *************************/

  Servo servo_camera;
  Servo servo_resgate;
  Servo servo_frontal;

  /*Inicializa os Servos*/
  void servos_begin() {
    // Inicia os servos
    servo_camera.attach(43);
    servo_resgate.attach(45);
    servo_frontal.attach(41);
    //Posicoes iniciais
    servo_frontal.write(36);
    servo_resgate.write(36);
    servo_camera.write(100);
  }

  /*Realiza as movimentacoes da camera*/
  void move_camera(char com) {
    if (com == 'E') servo_camera.write(100);      //Ajustar
    else if (com == 'F') servo_camera.write(60);  //Ajustar
    else if (com == 'D') servo_camera.write(30);  //Ajustar
  }

  /******************** ANGULO **********************/

  int _andar = 0;

  /*Identifica se existe uma rampa,*/
  int checar_andar() {

    static int count = 0;
    static int rampa = 0;

    //Verifica se existe inclinação constante
    float inclinacao = sensores.inclinacao_mpu();

    if (inclinacao >= 80.0) count++;                             //Subida
    else if (inclinacao >= 20.0 && inclinacao <= 40.0) count--;  //Descida

    //Rampas são marcadas pela metade!!!!
    if (count >= 100) rampa++;                      //Subida
    else if (count <= -100) rampa++;                //Descida
    else if (count > 100 && count > -100) rampa++;  //Nao existe Rampa


    if (rampa >= 2) {  //Subiu 1 andar
      rampa = 0;
      _andar++;
      return _andar;
    } else if (rampa <= -2) {  //Desceu 1 andar
      rampa = 0;
      _andar--;
      return _andar;
    } else {  //Mesmo andar
      return _andar;
    }
  }
  /******************** DISTANCIAS **********************/

  float correction_angle = 0.0;
  float trajetoria = 30.0;
  bool passagens[4];



  /********************************************************************* MOVIMENTACAO ********************************************************************/

  /**************************************** BÁSICAS ****************************************/

  /*! Girar o Robo */
  void girar(int vel, float ang) {
    int aux[] = { -vel, -vel, -vel, -vel };  // Inicia com valores de 'E'
    sensores.zerar_mpu();

    //Esquerda
    if (ang < 0) {
      while (sensores.angulo_mpu() > ang) {
        motores.potencia(aux);
      }
      //Direita
    } else if (ang >= 0) {
      aux[0] = vel;
      aux[1] = vel;
      aux[2] = vel;
      aux[3] = vel;
      while (sensores.angulo_mpu() < ang) {
        motores.potencia(aux);
        //sensores.calibrar_offset();
      }
    }
    motores.parar();
    sensores.zerar_mpu();
  }

  /*! Para todos motores*/

  /*! Volta de re quando entramos em um quadrado preto*/
  void sair_preto() {
    motores.mesma_potencia(-300);
  }

  /*! Espera 5 seg no azul*/
  void espera_azul() {
    motores.parar();
    delay(5000);
  }

  /*! Movimenta o robo para frente*/
  void movimento(int velocidade = 500) {
    int diferenca_lateral = pidG.calcular(sensores.angulo_mpu());

    motores.mesma_potencia(velocidade, diferenca_lateral);
  }

  /****************************************** TROCA DE QUADRADO *********************************************/
  /*! São definidos os parametros de distancia e do Encoder, para a troca*/

  /*Um dos parametros da troca*/
  bool troca_encoder() {

    //Checa se foram passos suficientes
    if (sensores.passos_cm >= trajetoria) {

      sensores.zerar_encoder();
      return true;
    } else {
      return false;
    }
  }


  /*Busca uma alteração brusca entre as medidas de distancia, fornecendo um parametro mais preciso para o Encoder*/

  //Direita frontal e Esquerda frontal
  float last_dist[2] = { sensores.dist[2], sensores.dist[4] };
  bool limitador = false;  //Limita o numero de vezes em que zeramos o Encoder

  void trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 2, 4 };

    sensores.ler_dist_rapido(aux);


    //Filtro lado direito
    if ((sensores.dist[2] <= 15.0 && last_dist[0] >= 20.0) || (sensores.dist[2] >= 20.0 && last_dist[0] <= 15.0)) {
      if (abs(sensores.dist[2] - last_dist[0]) >= 20.0) {  //Procura diferença
        trajetoria = 12.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.println("parede direita/encoder");
          limitador = true;
        }
      }
    }
    //Filtro lado Esquerdo
    if ((sensores.dist[4] <= 15.0 && last_dist[1] >= 20.0) || (sensores.dist[4] >= 20.0 && last_dist[1] <= 15.0)) {
      if (abs(sensores.dist[4] - last_dist[1]) >= 20.0) {  //Procura diferença
        trajetoria = 12.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.println("parede esquerda/encoder");
          limitador = true;
        }
      }
    }

    //Salvamos para proximas iteracoes
    last_dist[0] = sensores.dist[2];
    last_dist[1] = sensores.dist[4];
  }

  /*Utilizada apos finalizar uma movimentacao, para previnir erros*/
  void zerar_trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 2, 4 };

    sensores.ler_dist_rapido(aux);

    last_dist[0] = sensores.dist[2];
    last_dist[1] = sensores.dist[4];
    limitador = false;
  }


  /* Verifica a troca de quadrado */
  bool troca_quadrado() {

    //Sensores que devem ser atualizados
    int aux[1] = { 0 };

    sensores.ler_dist_rapido(aux);
    delay(5);

    // Troca pelas distancias
    if (sensores.dist[0] <= 7.5 && sensores.dist[0] != 0.0) {

      Serial.print("Troquei pela distancia: ");
      Serial.println(sensores.dist[0]);
      return true;

      // Troca pelo Encoder
    }
    if (troca_encoder() == true) {
      Serial.println("Troquei pelo Encoder");
      return true;

      // Nao houve troca
    } else {
      return false;
    }
  }

  /************************************ CORREÇÃO ******************************************/


  /* Correcao de trajetoria dividida em 4 etapas
    Etapa 1: Andamos 10 pulsos do encoder, simultaneamente fazemos a leitura de 2 sensoresde distancia (1 de cada lado)
    Etapa 2: Escolhemos o lado com o menor desvio padrao  
    Etapa 3: Calculamos o quao fora do quadrado + o angulo de erro
    Etapa 4: Calculamos e atualizamos o a trajetoria restante + o angulo para correcao */
  void correcao_trajetoria() {

    int sen[] = { 2, 4 };
    int controle = 0;
    bool lado = false;
    sensores.ler_dist_rapido(sen);


    float leituras[10][2];
    sensores.zerar_encoder();
    unsigned int last_passo = sensores.passos;
    while (sensores.passos < 10) {
      movimento();
      if (last_passo != sensores.passos) {
        sensores.ler_dist_rapido(sen);
        leituras[controle][0] = sensores.dist[4];
        leituras[controle][1] = sensores.dist[2];
        controle++;
        last_passo = sensores.passos;
      }
    }
    motores.parar();

    float derivada_esquerda = 0.0, derivada_direita = 0.0;

    for (uint8_t i = 1; i < 10; i++) {
      derivada_esquerda += leituras[i][0] - leituras[i - 1][0];
      derivada_direita += leituras[i][1] - leituras[i - 1][1];
    }
    derivada_esquerda /= 10.0;
    derivada_direita /= 10.0;

    float desvio_esquerdo = 0.0;
    float desvio_direito = 0.0;

    for (uint8_t i = 1; i < 10; i++) {
      desvio_esquerdo += pow((leituras[i][0] - leituras[i - 1][0]) - derivada_esquerda, 2);
      desvio_direito += pow((leituras[i][1] - leituras[i - 1][1]) - derivada_direita, 2);
    }

    desvio_esquerdo = sqrt(desvio_esquerdo / 9.0);
    desvio_direito = sqrt(desvio_direito / 9.0);


    Serial.print("desvio esquerdo: ");
    Serial.print(desvio_esquerdo);
    Serial.print(" desvio direito: ");
    Serial.println(desvio_direito);

    if (desvio_esquerdo > 0.5 && desvio_direito > 0.5) {
      trajetoria = 22;
      return;
    }


    if (desvio_esquerdo >= desvio_direito) lado = 1;

    float b1 = 0, b2 = 0, b3 = 0, b4 = 0;

    for (float x = 1; x <= 10; x++) {
      b1 += (x * ENCODER_CONSTANT) * leituras[(int)x - 1][lado];
      b2 += (x * ENCODER_CONSTANT);
      b3 += leituras[(int)x - 1][lado];
      b4 += pow((x * ENCODER_CONSTANT), 2);
    }

    b1 *= 10;
    float b23 = b2 * b3;
    b4 *= 10;
    float bp = pow(b2, 2);

    float B = (b1 - b23) / (b4 - bp);
    float A = (b3 - B * b2) / 10;

    float angulo = (90 - (acos(B) * (180.0 / M_PI)));

    float dist = A + (B * (10 * ENCODER_CONSTANT));
    float real_dist = cos(angulo / (180.0 / M_PI)) * dist;
    float lateral_dist = 7.0 - fmod(real_dist, 30.0);
    float h = (10 * ENCODER_CONSTANT) * cos(angulo * (M_PI / 180.0));
    float t = sqrt(pow(30 - h, 2) + pow(lateral_dist, 2));
    float angulo_trajetoria = asin(lateral_dist / t) * (180.0 / M_PI);

    angulo *= -1;

    if (lado) {
      angulo_trajetoria *= -1;
      angulo *= -1;
    }

    float correcao = angulo + angulo_trajetoria;

    trajetoria = t;
    if (isnan(t)) {
      trajetoria = 22;
      correcao = 0;
    }

    correction_angle = angulo_trajetoria;

    Serial.print("angulo: ");
    Serial.print(angulo);
    Serial.print(" dist: ");
    Serial.print(dist);
    Serial.print(" dist real: ");
    Serial.print(real_dist);
    Serial.print(" dist quadrado: ");
    Serial.print(lateral_dist);
    Serial.print(" trajetoria: ");
    Serial.print(t);
    Serial.print(" angulo trajetoria: ");
    Serial.println(angulo_trajetoria);

    girar(300, correcao);
  }


  /**************************************************************** COMANDOS ******************************************************************************/
  /*! Realiza todas inicializações*/
  void iniciar() {
    sensores.begin_mpu();
    servos_begin();
    //
    begin_LED();
    // Coomeçam ligados
    ligaLED_sinal();
    ligaLED_resgate();
  }


  int _kits = 9;
  char _lado;

  /*! Movimentamos 1 quadrado para Frente e buscamos por vitimas */
  void frente() {

    //Checa se estámos em quadrado azul antes de sair
    if (cores.buscar() == 'p') delay(5000);

    sensores.zerar_encoder();
    sensores.zerar_mpu();
    zerar_trajetoria_por_parede();

    //Calcula e orienta uma nova trajetoria
    correcao_trajetoria();
    sensores.zerar_encoder();


    /*Loop ate a troca de quadrado*/
    while (troca_quadrado() == false) {

      //Saida do preto
      if (cores.buscar() == 'b') {
        trajetoria = 10.0;
        _black_flag = true;
        while (troca_quadrado() == false) movimento(-500);
        return;
      }

      trajetoria_por_parede();  //Busca mudancas nas paredes laterais
      movimento(500);
      //Serial.println(sensores.passos_cm);
    }

    motores.parar();

    //Desvira
    if (correction_angle <= 30.0 && correction_angle >= -30.0) {
      girar(300, -correction_angle);
    }
  }


  /*! Busca vitimas com a camera*/
  void buscar_vit() {

    //Lado Esquerdo
    move_camera('E');
    _kits = com.camera('L');
    if (_kits != 9) {
      _lado = 'E';
      return;
    }

    //Frente
    move_camera('E');
    _kits = com.camera('L');
    if (_kits != 9) {
      _lado = 'F';
      return;
    }

    //Lado Direito
    move_camera('E');
    _kits = com.camera('L');
    if (_kits != 9) {
      _lado = 'D';
      return;
    }
  }

  /*Distribui os kits nescessarios, recebe o numero de kits e o lado do resgate*/
  void resgate(int kits, char lado) {
    int aux = 0;
    servo_camera.write(36);  //Posicao de repouso

    //Resgate do lado Esquerdo
    if (lado == 'E')
      while (aux < kits) {
        servo_resgate.write(36);  //Abre para o lado direito
        delay(100);
        servo_resgate.write(36);  //Posicao de repouso
        aux++;
      }

    //Resgate do lado Direito
    else if (lado == 'E')
      while (aux < kits) {
        servo_resgate.write(36);  //Abre para o lado esquerdo
        delay(100);
        servo_resgate.write(36);  //Posicao de repouso
        aux++;
      }

    //Resgate na Frente
    else if (lado == 'F') {
      //Gira o robo para realizar o resgate
      girar(500, 90.0);

      while (aux < kits) {
        servo_resgate.write(36);  //Abre para o lado direito
        delay(100);
        servo_resgate.write(36);  //Posicao de repouso
        aux++;
      }

      //Gira para a posicao inicial
      girar(500, -90.0);
    }
  }
};
#endif