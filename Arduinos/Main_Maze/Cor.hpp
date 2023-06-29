#ifndef Cor_hpp
#define Cor_hpp

class Cor {

private:

  // Pins
  const int photocellPin = 11;  // Analog pin para o TIL78 phototransistor
  const int redPin = 25;        // Digital pin para o canal vermelho do RGB LED
  const int greenPin = 27;      // Digital pin para o canal verde do RGB LED
  const int bluePin = 29;       // Digital pin para o canal azul do RGB LED

  //Thresholds  das cores (CALIBRAR!!!!)
  const int blueThreshold = 500;
  const int whiteThreshold = 1000;
  const int silverThreshold = 1500;
  const int blackThreshold = 2000;

  /*Setup*/
  void begin() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
  }

  /*Altera os valores RGB do LED*/


public:

  /*Calibra os parametros de cor para a pista*/
  void calibrar_todos() {
    //Preto
    Serial.println("Bora Calibrar o PRETO");
    calibrar_cor();
    //Prata
    Serial.println("Bora Calibrar o PRATA");
    calibrar_cor();
    //Azul
    Serial.println("Bora Calibrar o AZUL");
    calibrar_cor();
    //Branco
    Serial.println("Bora Calibrar o Branco");
    calibrar_cor();
  }

  /*Calibra uma cor individualmente*/
  void calibrar_cor() {
    //Liga o LED
    

    int leitura;

    /*Integra o valor de 200 leituras*/
    for (int i = 0; i < 200; i++) {
      leitura += analogRead(photocellPin) / i;
    }

    //Resultados
    Serial.print("Valor Estimado: ");
    Serial.println(leitura);
    delay(8000);
  }

  /*Faz a leitura da cor atual*/
  char ler() {

    char cor;
    unsigned int offset = analogRead(photocellPin);

    digitalWrite(redPin, HIGH);
    delayMicroseconds(100);
    unsigned int red = analogRead(photocellPin);
    digitalWrite(redPin, LOW);
    delayMicroseconds(100);

    digitalWrite(bluePin, HIGH);
    delayMicroseconds(100);
    unsigned int blue = analogRead(photocellPin);
    digitalWrite(bluePin, LOW);
    delayMicroseconds(100);

    digitalWrite(greenPin, HIGH);
    delayMicroseconds(100);
    unsigned int green = analogRead(photocellPin);
    digitalWrite(greenPin, LOW);
    delayMicroseconds(100);



    return cor;
  }
};

#endif