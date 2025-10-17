// 🎯 ¡MÁQUINA RECICLADORA DE BOTELLAS!
// Este código controla el motorcito que tira del plástico derretido
// ¡Como una máquina de hacer fideos pero con plástico reciclado!

// --- LIBRERÍAS PARA EL DISPLAY ---
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- CONFIGURACIÓN DEL DISPLAY LCD I2C ---
// (Si no funciona con 0x27, prueba 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- Los cables del Motor (como los pedales de una bici) ---
const int dirPin = 2;     // Pin que dice "adelante o atrás"
const int stepPin = 3;     // Pin que dice "da un pasito"

// --- La perilla de velocidad (como el acelerador) ---
const int potPin = A0;     // Aquí conectamos la perilla giratoria

// --- El termistor para medir temperatura 🌡️ ---
const int termistorPin = A1;     // Pin del termistor
const float R_FIJA = 4700.0;     // Resistencia fija de 4.7k ohmios
const float R_TERMISTOR_25C = 100000.0;  // Resistencia del termistor a 25°C (100k)
const float BETA = 3950.0;       // Coeficiente Beta del termistor (típico: 3950)

// --- El botón mágico de cambio ---
const int botonInversion = 4;  // 🔄 Botón para ir hacia atrás
bool direccionActual = HIGH;   // Guardamos si vamos adelante o atrás

void setup() {
// --- INICIAMOS EL DISPLAY ---
lcd.init();
lcd.backlight();
lcd.setCursor(0, 0);
lcd.print("Recicladora PET");
lcd.setCursor(0, 1);
lcd.print("Iniciando...");
delay(1500);
lcd.clear();

// Le decimos al Arduino qué hace cada cable:
// Estos dos cables le dan órdenes al motor
pinMode(dirPin, OUTPUT);    // Cable de dirección: ¡Tú das órdenes!
pinMode(stepPin, OUTPUT);    // Cable de pasos: ¡Tú también!

// 🔘 El botón es diferente, él nos da información a nosotros
pinMode(botonInversion, INPUT_PULLUP);  // ¡Escuchamos al botón!

// Empezamos yendo hacia adelante
digitalWrite(dirPin, direccionActual);
}

void loop() {
// 🔄 ¿ALGUIEN APRETÓ EL BOTÓN DE CAMBIO?
static bool botonAnterior = HIGH;  // ¿Cómo estaba el botón antes?
bool botonActual = digitalRead(botonInversion);  // ¿Cómo está ahora?

// Si el botón estaba suelto y ahora está apretado...
if (botonAnterior == HIGH && botonActual == LOW) {
  // ¡Cambiamos de dirección! Si íbamos adelante, ahora atrás
  direccionActual = !direccionActual;
  digitalWrite(dirPin, direccionActual);

  // Esperamos un poquito para que el botón se calme
  delay(50);
}
botonAnterior = botonActual;  // Recordamos para la próxima vez

// 1. LEEMOS LA PERILLA DE VELOCIDAD
// La leemos 5 veces y sacamos el promedio (como cuando
// preguntas 5 veces qué pizza quieren y eliges la más votada)
int suma = 0;
for(int i = 0; i < 5; i++) {
  suma += analogRead(potPin);  // Leemos la perilla
  delayMicroseconds(100);       // Mini-pausa entre lecturas
}
int valorPot = suma / 5;  // Dividimos por 5 para el promedio

// 2. CONVERTIMOS LA PERILLA EN VELOCIDAD
// ¡TRUCO GENIAL! El motor tiembla con velocidades medias,
// así que las saltamos (como evitar un bache en la calle)
int delayMotor;  // Tiempo entre pasos (más tiempo = más lento)
if (valorPot < 400) {
  // MODO TORTUGA 🐢: Súper lento para empezar
  delayMotor = map(valorPot, 0, 400, 10000, 6500);
} else if (valorPot > 600) {
  // MODO LIEBRE 🐰: Rápido cuando giramos mucho la perilla
  delayMotor = map(valorPot, 600, 1023, 4000, 500);
} else {
  // ZONA PROHIBIDA: Aquí el motor tiembla, ¡la saltamos!
  delayMotor = 5000;  // Velocidad fija sin temblores
}


// 3. ¡HACEMOS QUE EL MOTOR DÉ UN PASITO!
// Es como darle un toquecito al motor para que avance
digitalWrite(stepPin, HIGH);         // ¡Despierta motor!
delayMicroseconds(25);                // Toque cortito
digitalWrite(stepPin, LOW);          // Ya puedes descansar
delayMicroseconds(delayMotor - 25);  // Esperamos antes del próximo paso

// 4. CALCULAR TEMPERATURA Y ACTUALIZAR DISPLAY
static float temperaturaActual = 0.0;
static int valorPotAnterior = 0;
static unsigned long tiempoUltimoCambio = 0;
static int contadorPasos = 0;
contadorPasos++;

if (contadorPasos >= 2000) {
  contadorPasos = 0;

  // Leemos el termistor y calculamos temperatura
  int lecturaADC = analogRead(termistorPin);

  // Calculamos la resistencia del termistor
  // Conexión: 5V -> R_fija -> A1 -> Termistor -> GND
  // Fórmula: R_termistor = R_fija * ADC / (1023 - ADC)
  float resistenciaTermistor = R_FIJA * lecturaADC / (1023.0 - lecturaADC);

  // Calculamos temperatura usando ecuación de Steinhart-Hart simplificada
  float steinhart;
  steinhart = resistenciaTermistor / R_TERMISTOR_25C;
  steinhart = log(steinhart);
  steinhart /= BETA;
  steinhart += 1.0 / 298.15;
  steinhart = 1.0 / steinhart;
  temperaturaActual = steinhart - 273.15;  // Convertir a Celsius

  // Detectar si cambió la velocidad (diferencia mayor a 10)
  if (abs(valorPot - valorPotAnterior) > 10) {
    tiempoUltimoCambio = millis();  // Marcar el tiempo del cambio
    valorPotAnterior = valorPot;
  }

  // Si pasaron menos de 3 segundos desde el último cambio, mostrar velocidad
  if (millis() - tiempoUltimoCambio < 3000) {
    // MODO TEMPORAL: Mostrar velocidad y dirección
    lcd.setCursor(0, 0);
    lcd.print("Velocidad: ");
    int velocidadPercent = map(delayMotor, 10000, 500, 0, 100);
    lcd.print(velocidadPercent);
    lcd.print("%   ");

    lcd.setCursor(0, 1);
    lcd.print("Direccion: ");
    if (direccionActual == HIGH) {
      lcd.print(">   ");  // Adelante
    } else {
      lcd.print("<   ");  // Atrás
    }
  } else {
    // MODO NORMAL: Mostrar temperaturas
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperaturaActual, 1);
    lcd.print("C   ");

    lcd.setCursor(0, 1);
    lcd.print("Deseada: --C    ");  // Por ahora sin temperatura deseada
  }
}
}