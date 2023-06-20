#ifndef Ultrassonico_hpp
#define Ultrassonico_hpp


#define MAX_DISTANCE 200 /*!<Distancia Maxima (em cm).*/
#define TIMEOUT 50000    /*!<Tempo maximo de espera para o retorno do pulso do ultrassonico.*/



class Ultrassonico {

private:

  byte _trigger, _echo;
  float _correcao = 0.0;

  /*!<Retorna intervalo de tempo entre as chamadas*/
  

public:

  Ultrassonico(byte trigger, byte echo, float correcao = 0.0){
    _trigger = trigger;
    _echo = echo;
    _correcao = correcao;
    pinMode(_trigger, OUTPUT);
    digitalWrite(_trigger, LOW);
    pinMode(_echo, INPUT);
  }

  float read(){
    unsigned long tempo;

    digitalWrite(_trigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigger, LOW);

    tempo = micros();
    while(digitalRead(_echo) == 0 && (micros() - tempo) < 10000);
    if((micros() - tempo) > 10000) return 0.0;

    tempo = micros();
    while(digitalRead(_echo) == true && (micros() - tempo) < 11764);
    if( (micros() - tempo) > 11764) return 0.0;

    float distancia = 0.017 * (float)(micros() - tempo);

    distancia += _correcao;

    return distancia;

  }

  float media(unsigned int num_mean){
    float average = 0;
    for(float x = 1 ; x <= num_mean ; x++){
      average += (read() - average)/x;
      delay(20);
    }
    return average;
  }


  
};
#endif