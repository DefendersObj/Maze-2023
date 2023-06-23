#ifndef Sensores_hpp
#define Sensores_hpp

#include <MPU6050.h>           /*!< Inclusão da biblioteca do MPU */
#include <Adafruit_MLX90614.h> /*!< Inclusão da biblioteca do MLX */
#include "Ultrassonico.hpp"    /*!< Inclusão da classe dos Ultrassonicos */

#define OFFSET 0.91        //0.88 /*!< Valor de correcao para MPU */
#define DIMENSIONAL 1.014  // 6.8  /*!< Constante de correcao para MPU*/

MPU6050 gyroscope;

/*Ordem dos Ultrassonicos em sentido Horário! */
Ultrassonico frente(36, 42, -0.5);
Ultrassonico direita_f(48, 50, 0.0);
Ultrassonico direita_t(38, 40, 0.5);
Ultrassonico tras(12, 13);
Ultrassonico esquerda_f(26, 32, -0.1);
Ultrassonico esquerda_t(28, 30, 1.5);

class Sensores {

private:

  float ultima_passagem = 0.0;  //Usada na medicao do tempo
  float angulo_z = 0.0;         // Angulo atual
  float tempo_atual = 0.0;
  float tempo_decorrido = 0.0;

  /*!<Retorna intervalo de tempo entre as chamadas*/
  float tempo() {
    tempo_atual = (float)micros() / 1000000.0;
    tempo_decorrido = tempo_atual - ultima_passagem;
    ultima_passagem = tempo_atual;
    return tempo_decorrido;
  }

public:

  /******************** ULTRASSONICO ********************/
  float dist[6];  //Armazena os valores das leituras

  /*Percorre cada sensor e exibe os resultados. EM SENTIDO HORARIO!!!*/
  void ler_dist() {
    dist[0] = frente.media(10);
    dist[1] = direita_f.media(10);
    dist[2] = direita_t.media(10);
    dist[3] = tras.media(10);
    dist[4] = esquerda_t.media(10);
    dist[5] = esquerda_f.media(10);
  }

  /*Recebe em um array com o endereço dos sensores que devem ser lidos*/
  void ler_dist_rapido(int numbers[]) {

    for (int i = 0; i < 6; i++) {
      switch (numbers[i]) {
        case 0:  //Frente
          dist[0] = frente.read();
          break;
        case 1:  //Direita Frente
          dist[1] = direita_f.read();
          break;
        case 2:  //Direita Tras
          dist[2] = direita_t.read();
          break;
        case 3:  //Tras
          dist[3] = tras.read();
          break;
        case 4:  //Esquerda tras
          dist[4] = esquerda_t.read();
          break;
        case 5:  //Esquerda Frente
          dist[5] = esquerda_f.read();
          break;
        default:  //Condicao de saída
          break;
      }
    }
  }


  /******************** MPU ***********************/

  /*!< Inicializa MPU */
  void begin_mpu() {
    gyroscope.begin();              //Iniciando giroscopio.
    gyroscope.config_filter(6);     //Setando a 5Hz filtro passa baixa.
    gyroscope.config_gyro(1);       //Setando completos 200°/s ecala.
    gyroscope.config_accel(3);      //Setando 16g completos de escala.
    gyroscope.convert_value(true);  //Ajuste na conversão de valor
  }

  /*!<Retorna o angulo atual da MPU*/
  float angulo_mpu() {
    angulo_z += ((gyroscope.z_gyro() - OFFSET) * DIMENSIONAL) * tempo();
    //Serial.print("Z ang: ");
    //Serial.print(gyroscope.z_gyro());
    //Serial.print(" Angulo: ");
    //Serial.println(angulo_z);
    return -angulo_z;
  }

  /*!< Funcao que zera as referencia da MPU*/
  void zerar_mpu() {

    ultima_passagem = (float)micros() / 1000000.0;
    angulo_z = 0.0;
  }


  /*!< Caso tenha troca da MPU e ESSENCIAL recalibrar.
      Guia para a calibracao: 
      Manter o robo em uma mesa ESTAVEL,
      sem NENHUM tipo de interferencia ate o fim da calibracao.
      Ao fim da medicao subistituir a constante OFFSET pelo novo valor*/
  void calibrar_offset() {
    float offset;
    for (float i = 1; i < 10000; i++) {
      offset += (gyroscope.z_gyro() - offset) / i;
      Serial.println(i);
    }
    Serial.print("Offset estimado: ");
    Serial.println(offset);
  }

  /*!< Funcao que le a inclincao em relacao ao solo com MPU*/
  void inclinacao_mpu() {
    static int count = 0;

    float x_value = gyroscope.x_accel();
    float z_value = gyroscope.z_accel();

    float teta = atan(x_value / z_value) * 180 / PI;  //Formula para o angulo de inclinacao

    if (count >= 100) {
      count = 0;
    } else if (teta <= 0) count++;
    else count = 0;
  }

  /****************** ENCODER *******************/

  volatile int passos;
  float passos_cm;

  /*!< Zera todos parametros para o Encoder >*/
  bool zerar_encoder() {
    passos = 0;
    passos_cm = 0.0;
  }
  /***************** MLX TEMPERATURA *****************/
};
#endif