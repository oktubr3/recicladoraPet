// 🎯 Código final para controlar la velocidad del motor.
// La estabilidad y suavidad se ajustan en el HARDWARE (VREF y Microstepping).

// --- Pines del Motor ---
const int dirPin = 2;
const int stepPin = 3;

// --- Pin del Potenciómetro ---
const int potPin = A0;

// --- Pin del Botón de Inversión ---
const int botonInversion = 4;  // 🔄 Botón para cambiar dirección
bool direccionActual = HIGH;   // Variable para guardar la dirección

void setup() {
// Configurar los pines del motor como SALIDA
pinMode(dirPin, OUTPUT);
pinMode(stepPin, OUTPUT);

// 🔘 Configurar el botón como ENTRADA con resistencia pull-up interna
pinMode(botonInversion, INPUT_PULLUP);

// Dirección inicial del motor
digitalWrite(dirPin, direccionActual);
}

void loop() {
// 🔄 VERIFICAR BOTÓN DE INVERSIÓN
static bool botonAnterior = HIGH;  // Estado anterior del botón
bool botonActual = digitalRead(botonInversion);

// Detectar cuando se PRESIONA el botón (flanco descendente)
if (botonAnterior == HIGH && botonActual == LOW) {
  // Invertir la dirección
  direccionActual = !direccionActual;
  digitalWrite(dirPin, direccionActual);
  
  // Pequeña pausa para evitar rebotes
  delay(50);
}
botonAnterior = botonActual;

// 1. Leer el valor del potenciómetro varias veces y promediar
//    Esto elimina el ruido y las lecturas erráticas
int suma = 0;
for(int i = 0; i < 5; i++) {
  suma += analogRead(potPin);
  delayMicroseconds(100);
}
int valorPot = suma / 5;

// 2. Convertir el valor a un rango de delay 
//    TRUCO: Evitamos la zona problemática del medio (4000-6000)
int delayMotor;
if (valorPot < 400) {
  // Zona lenta: 10000 a 6500
  delayMotor = map(valorPot, 0, 400, 10000, 6500);
} else if (valorPot > 600) {
  // Zona rápida: 4000 a 500
  delayMotor = map(valorPot, 600, 1023, 4000, 500);
} else {
  // 🎯 ZONA MEDIA: Saltamos la resonancia!
  // Usamos una velocidad fija que no cabecee
  delayMotor = 5000;
}


// 3. Mover el motor con pulso optimizado
digitalWrite(stepPin, HIGH);
delayMicroseconds(25);  // Pulso corto y constante
digitalWrite(stepPin, LOW);
delayMicroseconds(delayMotor - 25);  // Resto del delay
}