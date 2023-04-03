/* 
Nazwa programu: Teleporta Blindow
Autor: Telepator - Marcin Szczepański
Data utworzenia: 03.2023
Ostatnia modyfikacja: 03.04.2023
Wersja: 1.00
Opis programu: Program służy do sterowania roletami za pomocą dwóch przekaźników i mikrokontrolera.
Pierwszy przekaźnik odpowiada za włączanie i wyłączanie napięcia, drugi za kierunek ruchu rolety (w górę lub w dół).
Dzięki temu rozwiązaniu nie może dojść do sytuacji, w której sygnał trafi jednocześnie do obu kierunków. 
Program rozróżnia krótkie i długie przyciśnięcia klawiszy oraz posiada funkcję włączania grup poprzez jednoczesne naciśnięcie klawiszy góra i dół.
Włączenie grupy umożliwia sterowanie wieloma roletami za pomocą jednego włącznika dwu klawiszowego.
Włączniki są podłączone do wejść analogowych poprzez dzielniki napięcia
Wymagania systemowe: Program wymaga co najmniej dwóch wyjść cyfrowych oraz jednego wejścia analogowego na każdą roletę,którą chcemy sterować.
Jeśli chcemy sterować większą ilością rolet, należy dobrać odpowiedni mikrokontroler oraz dostosować program do liczby sterowanych urządzeń.
Do uruchomienia programu potrzebny jest również kompatybilny programator mikrokontrolera oraz środowisko programistyczne do jego programowania, takie jak np. Arduino IDE.
*/


int flag = 1;  
unsigned long debounceDelay = 10;  // dla wszystkich włączników
unsigned long timeToFullMotion = 30000;  // 30000 czas do automatycznego zamknięcia rolety (jednakowy dla wszystkich rolet), zamiast trzymania przycisku. Należy go dostosować względem swoich rolet


class Roleta {
  private:
    int _number;
    volatile unsigned long previousMicros;
    volatile unsigned long currentMillis;  // = millis();
    volatile unsigned long buttonTime;
    volatile unsigned long timeToClose;
    int _state = 0; // 0 nie pracuje, 1 pracuje
    int _direction = 0; // 0 otwieranie , 1 zamykanie 
    int _pinOnOff;  // pin w arduino odpowiedzialny za włączenie przekaźnika 1 i zasilanie przekaźnika 2
    int _pinUpDown;  // pin w arduino odpowiedzialny za 2 przekaźnik , czyli kierunek góra lub dół. Dzięki takiemu rozwiązaniu, roleta nigdy nie dostanie napięcia jednocześnie w obu kierunkach.
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 20;
    bool flaga = 1;
    bool autoClose = 0;  // zamykanie/otwieranie "do końca" po krótkim wciśnięciu
    int _group = 0; // 0 brak, 1 , 2 3, - kolejne grupy. W tej wersji 0 jeszcze może być grupą.


  public:
    Roleta(int _pin, int _pin2, const int number) { // konstruktor z argumentami
      _pinOnOff = _pin;
      _pinUpDown = _pin2;
      _number = number;
    }
    unsigned long getTime() {
      currentMillis = millis();
      return currentMillis;
    }
    void setPinOnOff(int pin) {
      _pinOnOff = pin;
    }
    void setPinUpDown(int pin) {
      _pinUpDown = pin;
    }
    void addToGroup(int numberOfGroup) {
      _group = numberOfGroup;
    }
    bool isInGroup(int g) {
      return _group == g;
    }
    void turnToTheEnd () {
      if (autoClose == 1) {
        timeToClose = getTime() - previousMicros;
//        Serial.print(" previousMicros w turnToEnd :");
//        Serial.println(previousMicros);
        if (timeToClose > timeToFullMotion) {
          digitalWrite(_pinOnOff, LOW); // stop
//          Serial.println("wyłączenie po 30 s");
          _state = 0;
          autoClose = 0;
        }
      }
    }

    void turn(int _stateButton, int _directionMove) { // 0 - off, 1 - onn,   0 - otwiera , 1-zamyka, 2 - opcja
      _state = _stateButton;
      _direction = _directionMove;
//      Serial.print("STATE na początku turn :");
//      Serial.print(_state);
//      Serial.print(" previousMicros :");
//      Serial.print(previousMicros);
//      Serial.print(" oraz number : ");
//      Serial.println(_number);
      if (autoClose == 1) {
        digitalWrite(_pinOnOff, LOW); // stop
//        Serial.print("ZATRZYMANIE WCIŚNIĘCIEM - roleta :");
//        Serial.println(_pinOnOff);
        autoClose = 0;
      } else {
        if (_state == 1 ) {
          previousMicros = getTime();  // czas start
//          Serial.print("Zaczynamy liczyć previousMicros  =");
//          Serial.println(previousMicros);
          digitalWrite(_pinOnOff, HIGH);
//          Serial.print("Zamykamy roletę ");
//          Serial.print(_pinOnOff);

          if (_direction == 0) {
            digitalWrite(_pinUpDown, LOW);
//            Serial.println("w górę");
          } else if (_direction == 1) {
            digitalWrite(_pinUpDown, HIGH);
//            Serial.println("w dół");
          } else { // opcjonalnie . Wymyśliłem , że będzie to grupowanie rolet, ale w obrembie jednego pomieszczenie. Wciśnięcie obu zepnie rolety razem
            //digitalWrite(_pinUpDown, LOW);
//            Serial.print("oba wcisniete dla rolety:");
//            Serial.println(_pinOnOff);
          }

        } else if (_state == 0 ) {
          buttonTime = (getTime() - previousMicros);
//          Serial.print("Wyniki:  getTime() = ");
//          Serial.print(getTime());
//          Serial.print("previousMicros = ");
//          Serial.print(previousMicros);
//          Serial.print(" wynik buttontime = ");
//          Serial.println(buttonTime);
          if (buttonTime < 80UL) {
            if (autoClose) {
              autoClose = 0;
              digitalWrite(_pinOnOff, LOW);
//              Serial.print("Stop autoclose");
            } else {
              autoClose = 1; // ustawienie flagi na domykanie do "końca"
//              Serial.print("Wciśnięcie poniżej 80 ms  dokładnie = ");
//              Serial.println(buttonTime);  
            }

          } else {
//            Serial.print("stop czas był większy od 80 a dokładnmie =");
//            Serial.println(buttonTime);
            digitalWrite(_pinOnOff, LOW);
            autoClose = 0;
          }
        }
      }
    }
};

const int roletaLength = 5;
Roleta roleta[roletaLength] = {
  Roleta(0, 1, 0),  // instancja rolety z pinami dla 2 przekaźników, 2 pierwsze argumenty to piny cyfrowe strujące przekaźnikami danej rolety, 3 to numer/nazwa rolety
  Roleta(2, 3, 1),  // instancja rolety z pinami dla 2 przekaźników
  Roleta(4, 5, 2),   // instancja rolety z pinami dla 2 przekaźników
  Roleta(6, 7, 3),   // instancja rolety z pinami dla 2 przekaźników
  Roleta(8, 9, 4)   // instancja rolety z pinami dla 2 przekaźników
};

class AnalogButton {
  private:
    int _analogPin;
    int value;
    int state2 = 0;
    int buttonState = 0;
    int lastButtonState = 0;
    unsigned long lastDebounceTime = 0;
    int blindNumber;
    bool combined = 0; // Grupowanie rolet do jednego włącznika 0 off 1 - on
    int _group = 0; // 0 brak, 1 , 2 3, - kolejne grupy
  public:
    AnalogButton(const int analogPin) {
      _analogPin = analogPin;
    }
    void readValue() {
      value = analogRead(_analogPin);
      //      Serial.print(" pin ");
      //      Serial.print(_analogPin);
      //      Serial.print(" val ");
      //      Serial.println(value);
    }
    void toggleCombined() {
      combined = !combined;
    }
    bool isCombined() {
      return combined;
    }
    void setGroup(int numberOfGroup) {
      _group = numberOfGroup;
    }
    int getGroup() {
      return _group;
    }
    int state() {

      readValue();
      if (value != lastButtonState) {
        lastDebounceTime = millis();
      }
      if ((millis() - lastDebounceTime) > debounceDelay) {
        if (value != buttonState) {
          buttonState = value;
          if (value > 968 ) {  // niewciśniete

            state2 = 0;
          } else if (abs(value - 682) <= 55) {    // ~ w przybliżeniu 3,33v lewy 2 x 10kohm. Zamykanie. warość 682 i pozostałe wynikaja z użycia odpowiednich dzielników napięcia.

            state2 = 1;
          } else if (abs(value - 512) <= 55) { // 2,5v  prawy 1 x 10kohm. Otwieranie

            state2 = 2;
          } else if (abs(value - 408) <= 55) {  // +-2v oba wciśnięte. Włączenie grupy

            state2 = 3;
          }
        }
      }
      lastButtonState = value;
      return state2;
    }
    void setBlindNumber(int _blindNumber) {
      blindNumber = _blindNumber;
    }
    int getBlindNumber() {
      return blindNumber;
    }
};

const int numAnalogPins = 6;
const int analogPins[numAnalogPins] = {A0, A1, A2, A3, A4, A5};

AnalogButton analogButtons[numAnalogPins] = {
  AnalogButton(analogPins[0]),  // włącznik w rozdzelnicy
  AnalogButton(analogPins[1]),  // kuchnia wschód
  AnalogButton(analogPins[2]),  // kuchnia południe
  AnalogButton(analogPins[3]),   // łazienka
  AnalogButton(analogPins[4]),   // sypialnia lewa strona
  AnalogButton(analogPins[5])    // sypialnia prawa strona
};
void setup() {

  for (int i = 0; i < 6; i++) {
    pinMode(analogPins[i], INPUT);
  }
  for (int i = 0; i < 10; i++) {
    pinMode(i, OUTPUT);
  }
  Serial.begin(9600);
  // przypisanie włącznika do rolety i konfiguracja grup
  analogButtons[0].setBlindNumber(99); // rozdzielnica
  analogButtons[0].setGroup(0);
  
  analogButtons[1].setBlindNumber(0); // kuchnia wschód
  analogButtons[1].setGroup(1);
  roleta[0].addToGroup(1);

  analogButtons[2].setBlindNumber(1); // kuchnia południe
  analogButtons[2].setGroup(1);
  roleta[1].addToGroup(1);

  analogButtons[3].setBlindNumber(2); // Łazienka

  analogButtons[4].setBlindNumber(3); // sypialnia lewe okno
  analogButtons[4].setGroup(2);
  roleta[3].addToGroup(2);
  
  analogButtons[5].setBlindNumber(4); // sypialnie prawe okno
  analogButtons[5].setGroup(2);
  roleta[4].addToGroup(2);






}
void loop() {



  for (int i = 1; i < 6; i++) { //
    int state = analogButtons[i].state();  // zmienić na przerwania lub częstsze skanowanie, ponieważ na wolnych mikrokontrolerach wykonanie poniższego kodu może zająć więcej czasu niż planowane krótkie przyciąśnięcie przycisku
    int blindNumber = analogButtons[i].getBlindNumber();
    //    Serial.print("numer rolety");
    //       Serial.println(blindNumber);
    if (state == 0 && flag == 0) {  
      if (analogButtons[i].isCombined()) {  // jeżeli są grupy to znajdz rolety z tej grupy, w przeciwnym razie wykonaj turn() na rolecie przypisanej do przełącznika
        for (int j = 0 ; j < roletaLength; j++ ) {  // iterowanie po roletach
          if (roleta[j].isInGroup(analogButtons[i].getGroup())) {   // jęzeli numer grupy przypisanej do przycisku jest numerem grupy w rolecie, to na tej instancji rolety wykonaj turn()
            roleta[j].turn(0, 0);  // klawisz zwolniony
          }
        }
      } else {
        roleta[blindNumber].turn(0, 0);
      }
      flag = 1;
    } else if (state == 1) {
      if (analogButtons[i].isCombined()) {
        for (int j = 0 ; j < roletaLength; j++ ) {
          if (roleta[j].isInGroup(analogButtons[i].getGroup())) {
            roleta[j].turn(1, 1);
          }
        }
      } else {
        roleta[blindNumber].turn(1, 1);
      }
      flag = 0;
    } else if (state == 2) {
      if (analogButtons[i].isCombined()) {
        for (int j = 0 ; j < roletaLength; j++ ) {
          if (roleta[j].isInGroup(analogButtons[i].getGroup())) {
            roleta[j].turn(1, 0);
          }
        }
      } else {
        roleta[blindNumber].turn(1, 0);
      }
      flag = 0;
    } else if (state == 3) {
      analogButtons[i].toggleCombined();
      flag = 0;
    }
  }
  for (int i = 0 ; i < roletaLength; i++ ) {
    roleta[i].turnToTheEnd();
  }



} // loop
