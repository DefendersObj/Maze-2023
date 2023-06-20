#ifndef Comunicacao_hpp
#define Comunicacao_hpp


/*!<*/

class Comunicacao {

private:


public:

  void envio_motores(int pot_int[4]) {
    while(Serial3.available())Serial3.read();
    while(1){
      Serial3.print('i');

      for (byte i = 0; i < 4; i++) {
        Serial3.println(pot_int[i]);
        //Serial.println(pot_int[i]);
      }

      Serial3.print('p');

      unsigned long to_timer = millis();
      while(!Serial3.available() && (millis() - to_timer) < 2);
      if( (millis() - to_timer) < 5 ){
        if(Serial3.read() == 'k')break;
      }
    }
    //delay(500);
  }
};
#endif