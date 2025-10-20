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
const float ANTICIPACION = 10.0; // Apagar 10°C antes para evitar sobrepaso
const float HISTERESIS = 5.0;    // Diferencia para volver a encender
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
// LECTURA DE BOTONES (solo cada cierto tiempo para no afectar el motor)
static unsigned long tiempoUltimaLecturaBotones = 0;
static bool botonInversionAnterior = HIGH;
static bool botonOnOffAnterior = HIGH;
static bool botonBajarAnterior = HIGH;
static bool botonSubirAnterior = HIGH;

// Leer botones solo cada 50ms para no interrumpir el motor constantemente
if (millis() - tiempoUltimaLecturaBotones > 50) {
  tiempoUltimaLecturaBotones = millis();

  // 🔄 Botón de cambio de dirección
  bool botonInversionActual = digitalRead(botonInversion);
  if (botonInversionAnterior == HIGH && botonInversionActual == LOW) {
    direccionActual = !direccionActual;
    digitalWrite(dirPin, direccionActual);
  }
  botonInversionAnterior = botonInversionActual;

  // 🔥 Botón ON/OFF del calefactor
  bool botonOnOffActual = digitalRead(botonOnOff);
  if (botonOnOffAnterior == HIGH && botonOnOffActual == LOW) {
    controlCalefactorActivo = !controlCalefactorActivo;
    if (!controlCalefactorActivo) {
      digitalWrite(mosfetPin, LOW);
    }
  }
  botonOnOffAnterior = botonOnOffActual;

  // ⬇️ Botón bajar temperatura
  bool botonBajarActual = digitalRead(botonBajar);
  if (botonBajarAnterior == HIGH && botonBajarActual == LOW) {
    tempObjetivo -= 5.0;
    if (tempObjetivo < 0) {
      tempObjetivo = 0;
    }
  }
  botonBajarAnterior = botonBajarActual;

  // ⬆️ Botón subir temperatura
  bool botonSubirActual = digitalRead(botonSubir);
  if (botonSubirAnterior == HIGH && botonSubirActual == LOW) {
    tempObjetivo += 5.0;
    if (tempObjetivo > 270) {
      tempObjetivo = 270;
    }
  }
  botonSubirAnterior = botonSubirActual;
}

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
bool motorApagado = false;  // Para saber si apagamos el motor

// Si la perilla está muy baja (menos de 2%), apagar el motor
if (valorPot < 20) {  // 20 de 1023 es aproximadamente 2%
  motorApagado = true;
} else if (valorPot < 400) {
  // MODO TORTUGA 🐢: Súper lento para empezar
  delayMotor = map(valorPot, 20, 400, 10000, 6500);
} else if (valorPot > 600) {
  // MODO LIEBRE 🐰: Rápido cuando giramos mucho la perilla
  delayMotor = map(valorPot, 600, 1023, 4000, 500);
} else {
  // ZONA PROHIBIDA: Aquí el motor tiembla, ¡la saltamos!
  delayMotor = 5000;  // Velocidad fija sin temblores
}


// 3. ¡HACEMOS QUE EL MOTOR DÉ UN PASITO! (solo si no está apagado)
if (!motorApagado) {
  // Es como darle un toquecito al motor para que avance
  digitalWrite(stepPin, HIGH);         // ¡Despierta motor!
  delayMicroseconds(25);                // Toque cortito
  digitalWrite(stepPin, LOW);          // Ya puedes descansar
  delayMicroseconds(delayMotor - 25);  // Esperamos antes del próximo paso
}

// 4. CALCULAR TEMPERATURA Y ACTUALIZAR DISPLAY
static float temperaturaActual = 0.0;
static int valorPotAnterior = 0;
static unsigned long tiempoUltimoCambio = 0;
static int contadorPasos = 0;
contadorPasos++;

// Actualizar cada 500 pasos (más rápido que antes)
if (contadorPasos >= 500) {
  contadorPasos = 0;

  // Leemos el termistor VARIAS VECES y promediamos para suavizar
  float sumaTemperaturas = 0;
  for (int i = 0; i < 10; i++) {
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
    float tempLectura = steinhart - 273.15;  // Convertir a Celsius

    sumaTemperaturas += tempLectura;
    delayMicroseconds(100);  // Pequeña pausa entre lecturas
  }

  // Promediamos las 10 lecturas para suavizar
  float tempInstantanea = sumaTemperaturas / 10.0;

  // Filtro adicional: promedio con la lectura anterior (suavizado exponencial simple)
  if (temperaturaActual == 0.0) {
    // Primera lectura
    temperaturaActual = tempInstantanea;
  } else {
    // 80% de la anterior + 20% de la nueva = cambio gradual
    temperaturaActual = (temperaturaActual * 0.8) + (tempInstantanea * 0.2);
  }

  // 🔥 CONTROL DEL CALEFACTOR (solo si está activado)
  static bool calefactorEncendido = false;

  // Solo controlamos la temperatura si el botón ON/OFF está activado
  if (controlCalefactorActivo) {
    // Apagar ANTICIPACION grados antes del objetivo para evitar sobrepaso
    if (temperaturaActual >= (tempObjetivo - ANTICIPACION) && calefactorEncendido) {
      // Estamos cerca del objetivo, apagar
      digitalWrite(mosfetPin, LOW);
      calefactorEncendido = false;
    }
    // Encender si está muy por debajo (objetivo - anticipación - histéresis)
    else if (temperaturaActual < (tempObjetivo - ANTICIPACION - HISTERESIS)) {
      // Está frío, encender
      digitalWrite(mosfetPin, HIGH);
      calefactorEncendido = true;
    }
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
    int velocidadPercent;
    if (motorApagado) {
      velocidadPercent = 0;  // Motor apagado = 0%
    } else {
      velocidadPercent = map(delayMotor, 10000, 500, 0, 100);
    }
    if (velocidadPercent < 100) lcd.print(" ");  // Alinear números
    if (velocidadPercent < 10) lcd.print(" ");
    lcd.print(velocidadPercent);
    lcd.print("%    ");

    lcd.setCursor(0, 1);
    lcd.print("Dir: ");
    if (direccionActual == HIGH) {
      lcd.print("Adelante   ");
    } else {
      lcd.print("Atras      ");
    }
  } else {
    // MODO NORMAL: Mostrar temperaturas
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temperaturaActual, 1);
    lcd.print("C   ");  // 2 espacios extra antes del estado

    // Mostrar estado [ON] / [OFF] (movido 2 espacios a la derecha)
    if (controlCalefactorActivo) {
      if (calefactorEncendido) {
        lcd.print("[ON] ");
      } else {
        lcd.print("[--]");  // En espera
      }
    } else {
      lcd.print("[OFF]");
    }

    lcd.setCursor(0, 1);
    lcd.print("Obj:");
    lcd.print(tempObjetivo, 0);
    lcd.print("C          ");  // Espacios para limpiar
  }
}
}