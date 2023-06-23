#ifndef Cor_hpp
#define Cor_hpp

class Cor {

private:

  // Pins
  const int photocellPin = A0;  // Analog pin para o TIL78 phototransistor
  const int redPin = 2;         // Digital pin para o canal vermelho do RGB LED
  const int greenPin = 3;       // Digital pin para o canal verde do RGB LED
  const int bluePin = 4;        // Digital pin para o canal azul do RGB LED

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
  void LED(int red, int green, int blue) {
    analogWrite(redPin, red);
    analogWrite(greenPin, green);
    analogWrite(bluePin, blue);
  }

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
    LED(255, 255, 255);

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
    int leitura = analogRead(photocellPin);

    //Liga o LED
    LED(255, 255, 255);

    Serial.print("Valor da leitura: ");
    Serial.println(leitura);

    if (leitura < blueThreshold) {  // Azul/Poça
      cor = 'P';
      Serial.println("Azul!!");
    } else if (leitura < whiteThreshold) {  // Branco
      cor = 'W';
      Serial.println("Branco!!");
    } else if (leitura < silverThreshold) {  // Prata
      cor = 'S';
      Serial.println("Prata!!");
    } else if (leitura < blackThreshold) {  // Preto
      cor = 'B';
      Serial.println("Preto!!");
    }

    return cor;
  }

#endif