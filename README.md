# Teleporta-Blindow
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
