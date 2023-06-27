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

Motor motores;
Sensores sensores;

#define CIRCUNFERENCIA_RODA 20
#define COMPRIMENTO_ROBO 8.5
#define NUM_PASSOS 10


/*! Constroi os PIDs*/
//KP, KI, KD, Setpoint, windup, limite
PID pidG(60.0, 0.1, 2.0, 0, 10, 50);  //Giroscopio
PID pidF(1, 0.0, 0.1, 7, 10, 50);     //Frontal


class Operacional {

private:

  int
    quadrado_ant[2];

  bool
    ultimo_passo,  //Encoder
    passo;

public:

  /**************** CORES *****************/
  char cor() {
    //Le o sensor
    char cor = 'w';
    return cor;
  }

  /**************** PIDS ******************/

  /*! PID para diagonal */
  int PID_diagonal() {
    int aux = pidG.calcular(sensores.angulo_mpu());
    return aux;
  }

  /*! Setar PID para andar em diagonal*/
  void setar_PID_diagonal(int setpoint = 0) {
    pidG.setSetpoint(setpoint);
  }

  /******************** ANGULO **********************/
  void zerar_mpu() {
    sensores.zerar_mpu();
  }

  /******************** DISTANCIAS **********************/

  float correction_angle = 0.0;
  float trajetoria = 30.0;
  float lateral_dist = 0.0;
  float first_correction = 0.0;
  bool passagens[4];

  /*! Verifica a existencia de passagens nas quatro direcoes*/
  void medir_passagens() {
    sensores.ler_dist();
    if (sensores.dist[0] >= 25) passagens[0] = true;
    if (sensores.dist[1] + sensores.dist[2] / 2 >= 25) passagens[1] = true;
    if (sensores.dist[3] >= 25) passagens[2] = true;
    if (sensores.dist[4] + sensores.dist[5] / 2 >= 25) passagens[3] = true;
  }

  /*! Estima o angulo atual com base em dois valores de distancia*/
  void angulo() {

    sensores.ler_dist();
    float df = sensores.dist[1],
          dt = sensores.dist[2],
          et = sensores.dist[4],
          ef = sensores.dist[5];
    float angulo,
      cat_op,
      frente,
      tras;

    bool lado = false;

    //Decidimos qual lado tem menores diferenças para medir o angulo
    if (abs(ef - et) <= abs(df - dt)) {
      frente = ef;
      tras = et;
      lado = true;
      Serial.print("lado esquerdo escolhido: ");
      Serial.print("frontal: ");
      Serial.print(frente);
      Serial.print(" traseiro: ");
      Serial.println(tras);


    } else {
      frente = df;
      tras = dt;
      Serial.print("lado direito escolhido: ");
      Serial.print("frontal: ");
      Serial.print(frente);
      Serial.print(" traseiro: ");
      Serial.println(tras);
    }


    if (frente > tras) {
      cat_op = frente - tras;
    } else {
      cat_op = tras - frente;
    }

    //Filtro
    angulo = atan(cat_op / COMPRIMENTO_ROBO) * (180.0 / M_PI);
    lateral_dist = cos(angulo / (180.0 / M_PI)) * ((frente + tras) / 2.0);
    lateral_dist = 7.0 - fmod(lateral_dist, 30.0);
    if (!lado) {
      lateral_dist *= -1.0;
    }
    /*if (frente <= 45.0 && tras <= 45.0) {
    
    } else {
      angulo = 0.0;
      Serial.println("Valores Invalidos para correcao");
    }
*/
    //Correcao dependendo da angulacao
    if (frente > tras && lado == true) {
      angulo = angulo * -1.0;
    } else if (frente < tras && lado == false) {
      angulo = angulo * -1.0;
    }

    first_correction = angulo;

    Serial.print("Angulo correcao: ");
    Serial.println(angulo);
    Serial.print("lateral dist/teste: ");
    Serial.println(lateral_dist);
  }


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
    parar();
    sensores.zerar_mpu();
  }

  /*! Para todos motores*/
  void parar() {
    for (int i = 0; i < 5; i++) motores.mesma_potencia(0);
  }

  /*! Volta de re quando entramos em um quadrado preto*/
  void sair_preto() {
    motores.mesma_potencia(-300);
  }

  /*! Espera 5 seg no azul*/
  void espera_azul() {
    parar();
    delay(5000);
  }

  /*! Movimenta o robo para frente*/
  void movimento(int velocidade = 500) {
    int diferenca_lateral = pidG.calcular(sensores.angulo_mpu());

    motores.mesma_potencia(velocidade, diferenca_lateral);
  }

  /****************************************** TROCA DE QUADRADO *********************************************/
  /*! São definidos os parametros de distancia e do Encoder, para a troca*/
  void setar_quadrado() {
    sensores.zerar_encoder();
    zerar_trajetoria_por_parede();
  }

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
  float last_dist[2] = { sensores.dist[1], sensores.dist[5] };
  bool limitador = false;  //Limita o numero de vezes em que zeramos o Encoder

  void trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 1, 5 };

    sensores.ler_dist_rapido(aux);


    //Filtro lado direito
    if (sensores.dist[1] <= 50.0 && last_dist[0] <= 50.0) {
      if (abs(sensores.dist[1] - last_dist[0]) >= 20.0) {  //Procura diferença
        trajetoria = 20.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.println("parede direita/encoder");
          limitador = true;
        }
      }
    }
    //Filtro lado Esquerdo
    if (sensores.dist[5] <= 50.0 && last_dist[1] <= 50.0) {
      if (abs(sensores.dist[5] - last_dist[1]) >= 20.0) {  //Procura diferença
        trajetoria = 20.0;
        if (limitador == false) {
          sensores.zerar_encoder();
          Serial.println("parede esquerda/encoder");
          limitador = true;
        }
      }
    }

    //Salvamos para proximas iteracoes
    last_dist[0] = sensores.dist[1];
    last_dist[1] = sensores.dist[5];
  }

  /*Utilizada apos finalizar uma movimentacao, para previnir erros*/
  void zerar_trajetoria_por_parede() {

    //Sensores que devem ser atualizados
    int aux[4] = { 1, 5 };

    sensores.ler_dist_rapido(aux);

    last_dist[0] = sensores.dist[1];
    last_dist[1] = sensores.dist[5];
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


  /*Tendo a distancia nescessaria para atingir o centro do proximo quadrado,
   e o angulo que devemos manter para alcancala, esta funcao ajusta o robo no dado angulo, e atualiza a distancia
   nescessaria para a troca pelo encoder*/
  void correcao_trajetoria() {
    float correction = correction_angle + first_correction;
    if (correction < -2.0 && correction > -30.0) girar(200, correction);
    else if (correction > 2.0 && correction < 30.0) girar(200, correction);
  }

  /*Cálculo do comprimento da nova trajetória */
  void calcular_trajetoria() {
    trajetoria = 30.0;

    /*
    sensores.ler_dist();
    float cateto;
    


    //Escolhemos a parede com menor diferença para a nova trajetoria, e aplicamos um filtro
    if (abs(sensores.dist[1] - sensores.dist[2]) < abs(sensores.dist[4] - sensores.dist[5]) && sensores.dist[1] <= 50.0 && sensores.dist[2] <= 50.0) {  //Próximo da Direita
      cateto = -(7 - fmod(((sensores.dist[1] + sensores.dist[2]) / 2), 30.0));
      Serial.print("correcao de trajetoria, lado direito: frontal: ");
      Serial.print(sensores.dist[1]);
      Serial.print(" traseiro: ");
      Serial.println(sensores.dist[2]);
    }
    //Só aplicamos o filtro
    else if (sensores.dist[5] <= 50.0 && sensores.dist[4] <= 50.0) {  //Próximo da Esquerda
      cateto = (7 - fmod(((sensores.dist[5] + sensores.dist[4]) / 2), 30.0));
      Serial.print("correcao de trajetoria, lado esquerdo: frontal: ");
      Serial.print(sensores.dist[5]);
      Serial.print(" trazeiro: ");
      Serial.println(sensores.dist[4]);
    }
    //Caso nao nescessite de correcao
*/
    //Nova distancia a ser percorrida (Deve ser passada para a troca)
    trajetoria = sqrt(pow(lateral_dist, 2) + pow(30.0, 2));
    correction_angle = atan(lateral_dist / 30.0) * (180.0 / M_PI);
    Serial.print("desalinhamento lateral: ");
    Serial.println(lateral_dist);
    Serial.print("Trajetoria: ");
    Serial.println(trajetoria);
    Serial.print("Angulo trajetoria: ");
    Serial.println(correction_angle);
  }
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

  /**************************************************************** COMANDOS ******************************************************************************/
  /*! Realiza todas inicializações*/
  void iniciar() {
    sensores.begin_mpu();
    servos_begin();
    //
    begin_LED();
    // Coomeçam desligados
    ligaLED_sinal();
    ligaLED_resgate();
  }


  int _kits = 9;
  char _lado;

  /*! Movimentamos 1 quadrado para Frente e buscamos por vitimas */
  void frente(bool busca = false) {

    setar_quadrado();
    zerar_mpu();
    setar_quadrado();

    //Calcula e orienta uma nova trajetoria
    angulo();
    calcular_trajetoria();
    correcao_trajetoria();
    sensores.zerar_encoder();

    //Procura por vítimas no quadrado da frente
    if(busca = true) buscar_vit();

    /*Loop ate a troca de quadrado*/
    while (troca_quadrado() == false) {
      Serial.println(sensores.passos_cm);
      //Busca mudancas nas paredes laterais para uma troca mais precisa
      trajetoria_por_parede();
      movimento(500);
      //Serial.println(sensores.passos_cm);
    }

    parar();

    //Desvira
    if (correction_angle <= 30.0 && correction_angle >= -30.0) {
      girar(200, -correction_angle);
    }

    //Caso tenhamos encontrado uma vítima realizamos seu resgate
    if (_kits != 9) {  // 9 = nenhuma vitima
      ligaLED_resgate();
      resgate(_kits, _lado);
      desligaLED_resgate();
    }
  }


  /*! Busca vitimas com a camera*/
  void buscar_vit() {

    move_camera('E');
    //com.envia_lado('L');  //Envia lado obeservado para OpenMV / Left
    //_kits = com.kits();
    if (_kits != 9) {
      _lado = 'E';
      return;
    }
    delay(2000);

    move_camera('F');
    //com.envia_lado('F');  //Front
    //_kits = com.kits();
    if (_kits != 9) {
      _lado = 'F';
      return;
    }
    delay(2000);

    move_camera('D');
    //com.envia_lado('R');  //Right
    //_kits = com.kits();
    if (_kits != 9) {
      _lado = 'D';
      return;
    }
    delay(2000);
  }
};
#endif