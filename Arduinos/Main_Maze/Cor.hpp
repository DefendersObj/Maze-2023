#ifndef Cor_hpp
#define Cor_hpp

class Cor {

private:

  // Pins
  const int photocellPin = 11;  // Analog pin para o TIL78 phototransistor
  const int redPin = 25;        // Digital pin para o canal vermelho do RGB LED
  const int greenPin = 27;      // Digital pin para o canal verde do RGB LED
  const int bluePin = 29;       // Digital pin para o canal azul do RGB LED

  /*Setup*/


  /*Altera os valores RGB do LED*/


public:
  unsigned int _red;
  unsigned int _green;
  unsigned int _blue;


  void begin() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    digitalWrite(redPin, HIGH);
    digitalWrite(bluePin, HIGH);
    digitalWrite(greenPin, HIGH);
  }

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
    while (!Serial.available())
      ;
    Serial.read();

    float media_r = 0;
    float media_g = 0;
    float media_b = 0;

    for (float i = 1; i <= 100; i++) {
      ler();
      media_r += ((float)_red - media_r) / i;
      media_g += ((float)_green - media_g) / i;
      media_b += ((float)_blue - media_b) / i;
    }

    Serial.print(" vermelho: ");
    Serial.print(media_r);
    Serial.print(" green: ");
    Serial.print(media_g);
    Serial.print(" blue: ");
    Serial.println(media_b);
  }

  /*Faz a leitura da cor atual*/
  void ler() {

    _red = 0;
    _blue = 0;
    _green = 0;
    unsigned int off = 0;

    for (uint8_t i = 0; i < 100; i++) off += analogRead(photocellPin);

    /*LOW = liga
      HIGH = desliga */

    digitalWrite(redPin, LOW);
    delay(10);
    for (uint8_t i = 0; i < 100; i++) _red += analogRead(photocellPin);
    digitalWrite(redPin, HIGH);
    delay(10);
    _red = off - _red;

    off = 0;
    for (uint8_t i = 0; i < 100; i++) off += analogRead(photocellPin);

    digitalWrite(bluePin, LOW);
    delay(10);
    for (uint8_t i = 0; i < 100; i++) _blue += analogRead(photocellPin);
    digitalWrite(bluePin, HIGH);
    delay(10);
    _blue = off - _blue;

    off = 0;
    for (uint8_t i = 0; i < 100; i++) off += analogRead(photocellPin);

    digitalWrite(greenPin, LOW);
    delay(10);
    for (uint8_t i = 0; i < 100; i++) _green += analogRead(photocellPin);
    digitalWrite(greenPin, HIGH);
    delay(10);


    _green = off - _green;

    if (_green >= 1000000) _green = 0;
    if (_red >= 1000000) _red = 0;
    if (_blue >= 1000000) _blue = 0;
  }

  /*Verifica a cor atual*/
  char
  buscar() {
    ler();
    Serial.println("Terminei de de ler");
    /*Serial.print("Vermelho: ");
    Serial.println(_red);
    Serial.print("Verde: ");
    Serial.println(_green);
    Serial.print("Azul: ");
    Serial.println(_blue);*/

    if (_red >= 0.0 && _red <= 90.0 && _green >= 0.0 && _green <= 90.0 && _blue >= 0.0 && _blue <= 90.0) return 'b';
    //else if (_red >= 70.0 && _red <= 350.0 && _green >= 105.0 && _green <= 200.0 && _blue >= 111.0 && _blue <= 220.0) return 'p';
    //else if (_red >= 1000.0 && _red <= 2500.0 && _green >= 1000.0 && _green <= 2500.0 && _blue >= 1000.0 && _blue <= 2500.0) return 's';
    //else if (_red >= 350 && _red <= 1000.0 && _green >= 200.0 && _green <= 1000.0 && _blue >= 220.0 && _blue <= 1000.0) return 'w';
    else return 'w';
  }
};

#endif