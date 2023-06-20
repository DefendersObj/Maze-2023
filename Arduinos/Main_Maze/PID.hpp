#ifndef PID_hpp
#define PID_hpp

/*! Esta classe implementa um algoritmo de controlador PID (Proportional-Integral-Derivative)
  para sistemas de controle. Ele calcula uma saída de controle com base no erro entre um
  ponto de ajuste desejado e o valor de entrada atual.*/

class PID {

private:
  float kp;                     // Proportional gain
  float ki;                     // Integral gain
  float kd;                     // Derivative gain
  float windup;
  float limite;
  float setpoint;               // Desired setpoint
  float output;                 // Control output
  float integral;               // Integral term
  float ErroAnterior;          // Previous erro
  float ultima_passagem = 0.0;  //Usada na medicao do tempo
  float tempo_atual = 0.0;
  float tempo_decorrido = 0.0;


public:

  /*! Construtor que define as constantes e o set setpoint*/
  PID(float kp, float ki, float kd, float setpoint, float windup, float limite) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
    this->windup = windup;
    this->setpoint = setpoint;
    output = 0.0;
    integral = 0.0;
    ErroAnterior = 0.0;
  }

  float tempo() {
    tempo_atual = (float)micros() / 1000000.0;
    tempo_decorrido = tempo_atual - ultima_passagem;
    ultima_passagem = tempo_atual;
    return tempo_decorrido;
  }

  /*!Calcula o PID*/
  float calcular(float input) {
    float erro = setpoint - input;

    float tempo_atual = tempo();


    // Proportional term
    float proportional = kp * erro;

    // Integral term
    if (erro <= windup && erro >= -windup) {
      integral += ki * erro * tempo_atual;
    }

    // Limitador
    if (integral > limite) {
      integral = limite;
    } else if (integral < -limite) {
      integral = -limite;
    }

    // Derivative term
    float derivative = (kd * (erro - ErroAnterior)) / tempo_atual;

    // Calculate the control output
    output = proportional + integral + derivative;

    // Update previous erro
    ErroAnterior = erro;

    return output;
  }
  /*!Zeramos todas referencias do PID chamado*/
  void zerar() {
    integral = 0;
    ErroAnterior = 0;
  }

  /*! Definir ou redefine setpoint*/
  void setSetpoint(float setpoint) {
    this->setpoint = setpoint;
  }

  /*! Redefine as constantes*/
  void setTunings(float kp, float ki, float kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
  }
};
#endif