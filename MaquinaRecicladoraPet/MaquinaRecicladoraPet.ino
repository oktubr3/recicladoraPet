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

// --- El calefactor con MOSFET 🔥 ---
const int mosfetPin = 9;         // Pin del MOSFET para el calefactor
float tempObjetivo = 50.0;       // Temperatura objetivo en °C (se puede cambiar con botones)
const float HISTERESIS = 2.0;    // Histéresis para evitar encendido/apagado constante
bool controlCalefactorActivo = false;  // ¿Está el control automático activado?

// --- Los botones del calefactor 🔘 ---
const int botonOnOff = 5;        // D5: Botón para prender/apagar el control del calefactor
const int botonBajar = 6;        // D6: Botón para bajar la temperatura objetivo
const int botonSubir = 7;        // D7: Botón para subir la temperatura objetivo

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

// 🔥 El MOSFET del calefactor
pinMode(mosfetPin, OUTPUT);  // Pin del calefactor
digitalWrite(mosfetPin, LOW);  // Apagado al inicio

// 🔘 Los botones - todos escuchan lo que hacemos
pinMode(botonInversion, INPUT_PULLUP);  // Botón de cambio de dirección
pinMode(botonOnOff, INPUT_PULLUP);      // Botón ON/OFF del calefactor
pinMode(botonBajar, INPUT_PULLUP);      // Botón para bajar temperatura
pinMode(botonSubir, INPUT_PULLUP);      // Botón para subir temperatura

// Empezamos yendo hacia adelante
digitalWrite(dirPin, direccionActual);
}

void loop() {
// 🔄 ¿ALGUIEN APRETÓ EL BOTÓN DE CAMBIO DE DIRECCIÓN?
static bool botonInversionAnterior = HIGH;  // ¿Cómo estaba el botón antes?
bool botonInversionActual = digitalRead(botonInversion);  // ¿Cómo está ahora?

// Si el botón estaba suelto y ahora está apretado...
if (botonInversionAnterior == HIGH && botonInversionActual == LOW) {
  // ¡Cambiamos de dirección! Si íbamos adelante, ahora atrás
  direccionActual = !direccionActual;
  digitalWrite(dirPin, direccionActual);
  delay(50);  // Esperamos un poquito para que el botón se calme
}
botonInversionAnterior = botonInversionActual;  // Recordamos para la próxima vez

// 🔥 ¿ALGUIEN APRETÓ EL BOTÓN ON/OFF DEL CALEFACTOR?
static bool botonOnOffAnterior = HIGH;
bool botonOnOffActual = digitalRead(botonOnOff);

if (botonOnOffAnterior == HIGH && botonOnOffActual == LOW) {
  // ¡Cambiamos el estado! Si estaba prendido, ahora apagado (y viceversa)
  controlCalefactorActivo = !controlCalefactorActivo;

  // Si apagamos el control, también apagamos el calefactor
  if (!controlCalefactorActivo) {
    digitalWrite(mosfetPin, LOW);
  }

  delay(50);  // Anti-rebote
}
botonOnOffAnterior = botonOnOffActual;

// ⬇️ ¿ALGUIEN APRETÓ EL BOTÓN PARA BAJAR TEMPERATURA?
static bool botonBajarAnterior = HIGH;
bool botonBajarActual = digitalRead(botonBajar);

if (botonBajarAnterior == HIGH && botonBajarActual == LOW) {
  // Bajamos la temperatura de a 5 grados
  tempObjetivo -= 5.0;

  // No dejamos que baje de 0°C (el agua se congela!)
  if (tempObjetivo < 0) {
    tempObjetivo = 0;
  }

  delay(50);  // Anti-rebote
}
botonBajarAnterior = botonBajarActual;

// ⬆️ ¿ALGUIEN APRETÓ EL BOTÓN PARA SUBIR TEMPERATURA?
static bool botonSubirAnterior = HIGH;
bool botonSubirActual = digitalRead(botonSubir);

if (botonSubirAnterior == HIGH && botonSubirActual == LOW) {
  // Subimos la temperatura de a 5 grados
  tempObjetivo += 5.0;

  // No dejamos que suba de 270°C (¡es muy peligroso!)
  if (tempObjetivo > 270) {
    tempObjetivo = 270;
  }

  delay(50);  // Anti-rebote
}
botonSubirAnterior = botonSubirActual;

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

  // 🔥 CONTROL DEL CALEFACTOR (solo si está activado)
  static bool calefactorEncendido = false;

  // Solo controlamos la temperatura si el botón ON/OFF está activado
  if (controlCalefactorActivo) {
    if (temperaturaActual < (tempObjetivo - HISTERESIS)) {
      // Si está muy por debajo, encender
      digitalWrite(mosfetPin, HIGH);
      calefactorEncendido = true;
    } else if (temperaturaActual > tempObjetivo) {
      // Si alcanzó o superó el objetivo, apagar
      digitalWrite(mosfetPin, LOW);
      calefactorEncendido = false;
    }
    // Entre (tempObjetivo - HISTERESIS) y tempObjetivo mantiene el estado anterior
  } else {
    // Si el control está desactivado, apagar el calefactor
    digitalWrite(mosfetPin, LOW);
    calefactorEncendido = false;
  }

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
    lcd.print("T:");
    lcd.print(temperaturaActual, 1);
    lcd.print("C ");

    // Mostramos el estado: ON si está activo y calentando, OFF si desactivado
    if (controlCalefactorActivo) {
      lcd.print(calefactorEncendido ? "HEAT" : "WAIT");  // HEAT=calentando, WAIT=esperando
    } else {
      lcd.print("OFF ");  // Control desactivado
    }

    lcd.setCursor(0, 1);
    lcd.print("Obj:");
    lcd.print(tempObjetivo, 0);
    lcd.print("C ");

    // Mostramos flechitas para recordar cómo cambiar
    lcd.print("D6- D7+");
  }
}
}