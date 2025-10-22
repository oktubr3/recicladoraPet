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
const int dirPin = 2;   // Pin que dice "adelante o atrás"
const int stepPin = 3;  // Pin que dice "da un pasito"

// --- La perilla de velocidad (como el acelerador) ---
const int potPin = A0;  // Aquí conectamos la perilla giratoria

// --- Variables para control del motor por interrupción ---
volatile unsigned int motorDelayMicros = 5000;  // Delay entre pasos (volatile porque se usa en interrupción)
volatile bool motorHabilitado = true;           // Si el motor está habilitado

// --- El termistor para medir temperatura 🌡️ ---
const int termistorPin = A1;             // Pin del termistor
const float R_FIJA = 4700.0;             // Resistencia fija de 4.7k ohmios
const float R_TERMISTOR_25C = 100000.0;  // Resistencia del termistor a 25°C (100k)
const float BETA = 3950.0;               // Coeficiente Beta del termistor (típico: 3950)

// --- El calefactor con MOSFET 🔥 ---
const int mosfetPin = 11;              // Pin del MOSFET para el calefactor (PWM - usa Timer2, no interfiere con motor)
float tempObjetivo = 240.0;            // Temperatura objetivo en °C (240 para PET)
bool controlCalefactorActivo = false;  // ¿Está el control automático activado?

// --- Control PID (como las Prusa) 🎯 ---
// El PID ajusta la potencia del calefactor de forma inteligente
// Ajustado para frenar MUCHO antes y evitar sobrepaso
float Kp = 8.0;    // Proporcional: reducido para reacción más suave
float Ki = 0.05;   // Integral: reducido para menos acumulación en subida
float Kd = 120.0;  // Derivativo: AUMENTADO para frenar mucho antes
float errorAnterior = 0.0;
float errorAcumulado = 0.0;

// --- Los botones del calefactor 🔘 ---
const int botonOnOff = 5;  // D5: Botón para prender/apagar el control del calefactor
const int botonBajar = 6;  // D6: Botón para bajar la temperatura objetivo
const int botonSubir = 7;  // D7: Botón para subir la temperatura objetivo

// --- El botón mágico de cambio ---
const int botonInversion = 4;  // 🔄 Botón para ir hacia atrás
bool direccionActual = HIGH;   // Guardamos si vamos adelante o atrás

// ⚡ INTERRUPCIÓN DEL TIMER1 - Controla el motor sin bloqueos
// Esta función se ejecuta automáticamente a intervalos regulares
// y genera pulsos para el motor paso a paso
volatile bool pulsoAlto = false;

ISR(TIMER1_COMPA_vect) {
  if (motorHabilitado) {
    // Alternamos el pin del motor (generamos pulsos automáticamente)
    if (pulsoAlto) {
      digitalWrite(stepPin, LOW);
      pulsoAlto = false;
    } else {
      digitalWrite(stepPin, HIGH);
      pulsoAlto = true;
    }
  }
}

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
  pinMode(dirPin, OUTPUT);   // Cable de dirección: ¡Tú das órdenes!
  pinMode(stepPin, OUTPUT);  // Cable de pasos: ¡Tú también!

  // 🔥 El MOSFET del calefactor
  pinMode(mosfetPin, OUTPUT);    // Pin del calefactor
  digitalWrite(mosfetPin, LOW);  // Apagado al inicio

  // 🔘 Los botones - todos escuchan lo que hacemos
  pinMode(botonInversion, INPUT_PULLUP);  // Botón de cambio de dirección
  pinMode(botonOnOff, INPUT_PULLUP);      // Botón ON/OFF del calefactor
  pinMode(botonBajar, INPUT_PULLUP);      // Botón para bajar temperatura
  pinMode(botonSubir, INPUT_PULLUP);      // Botón para subir temperatura

  // Empezamos yendo hacia adelante
  digitalWrite(dirPin, direccionActual);

  // --- CONFIGURACIÓN DEL TIMER1 PARA EL MOTOR ---
  // Timer1 generará interrupciones para mover el motor sin bloqueos
  noInterrupts();  // Desactivamos interrupciones mientras configuramos
  TCCR1A = 0;      // Modo normal del timer
  TCCR1B = 0;
  TCNT1 = 0;  // Contador en 0

  // Configuramos para modo CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << WGM12);
  // Prescaler de 8 (cada tick = 0.5 microsegundos a 16MHz)
  TCCR1B |= (1 << CS11);
  // Valor inicial del comparador (ajustable según velocidad)
  OCR1A = 10000;  // Genera interrupción cada 5ms aproximadamente
  // Habilitamos la interrupción por comparación
  TIMSK1 |= (1 << OCIE1A);
  interrupts();  // Reactivamos interrupciones
}


void loop() {
  // Variable para controlar cuándo mostrar pantalla de velocidad/dirección
  static unsigned long tiempoUltimoCambioMotor = 0;

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
      tiempoUltimoCambioMotor = millis();  // Mostrar pantalla de dirección
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

  // 1. LEEMOS LA PERILLA DE VELOCIDAD Y ACTUALIZAMOS EL TIMER
  static unsigned long tiempoUltimaLecturaPot = 0;
  static int valorPot = 512;  // Valor medio por defecto
  static int delayMotor = 5000;
  static bool motorApagado = false;

  // Leer potenciómetro solo cada 50ms (no necesitamos leerlo más seguido)
  if (millis() - tiempoUltimaLecturaPot > 50) {
    tiempoUltimaLecturaPot = millis();

    // Leemos la perilla 5 veces y promediamos
    int suma = 0;
    for (int i = 0; i < 5; i++) {
      suma += analogRead(potPin);
      delayMicroseconds(100);
    }
    valorPot = suma / 5;

    // Convertimos la perilla en velocidad
    if (valorPot < 20) {
      // Menos de 2%: motor apagado
      motorApagado = true;
      motorHabilitado = false;
    } else {
      motorApagado = false;
      motorHabilitado = true;

      // Calculamos el delay según la perilla
      if (valorPot < 400) {
        delayMotor = map(valorPot, 20, 400, 10000, 6500);
      } else if (valorPot > 600) {
        delayMotor = map(valorPot, 600, 1023, 4000, 500);
      } else {
        delayMotor = 5000;
      }

      // Actualizamos la frecuencia del Timer1
      // OCR1A = (delay_microsegundos * 2) porque prescaler es 8 y cada tick es 0.5us
      noInterrupts();
      OCR1A = delayMotor * 2;
      interrupts();
    }
  }

  // 2. CALCULAR TEMPERATURA Y ACTUALIZAR DISPLAY
  static float temperaturaActual = 0.0;
  static int valorPotAnterior = 0;
  static unsigned long tiempoUltimaActualizacionDisplay = 0;

  // Actualizar display cada 200ms (suficientemente rápido pero sin bloquear)
  if (millis() - tiempoUltimaActualizacionDisplay > 200) {
    tiempoUltimaActualizacionDisplay = millis();

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

    // 🔥 CONTROL PID DEL CALEFACTOR (como las impresoras Prusa)
    static int potenciaPWM = 0;
    static bool calefactorEncendido = false;

    if (controlCalefactorActivo) {
      // Calculamos el ERROR: cuánto falta para llegar al objetivo
      float error = tempObjetivo - temperaturaActual;

      // P: Proporcional - mientras más lejos, más potencia
      float P = Kp * error;

      // I: Integral - acumulamos el error en el tiempo
      errorAcumulado += error;
      // Limitamos para evitar que se vaya al infinito
      if (errorAcumulado > 1000) errorAcumulado = 1000;
      if (errorAcumulado < -1000) errorAcumulado = -1000;
      float I = Ki * errorAcumulado;

      // D: Derivativo - qué tan rápido cambia el error (anticipa)
      float D = Kd * (error - errorAnterior);
      errorAnterior = error;

      // Sumamos los tres componentes del PID
      float salidaPID = P + I + D;

      // Convertimos a PWM (0-255) y limitamos
      potenciaPWM = (int)salidaPID;
      if (potenciaPWM > 255) potenciaPWM = 255;
      if (potenciaPWM < 0) potenciaPWM = 0;

      // Aplicamos la potencia calculada al MOSFET (PWM)
      analogWrite(mosfetPin, potenciaPWM);

      // Consideramos "encendido" si está dando más del 10% de potencia
      calefactorEncendido = (potenciaPWM > 25);

    } else {
      // Si el control está desactivado, apagar completamente
      analogWrite(mosfetPin, 0);
      potenciaPWM = 0;
      calefactorEncendido = false;
      errorAcumulado = 0;  // Reseteamos el acumulador
      errorAnterior = 0;
    }

    // Detectar si cambió la velocidad (diferencia mayor a 10)
    if (abs(valorPot - valorPotAnterior) > 10) {
      tiempoUltimoCambioMotor = millis();  // Marcar el tiempo del cambio
      valorPotAnterior = valorPot;
    }

    // Si pasaron menos de 3 segundos desde el último cambio, mostrar velocidad
    if (millis() - tiempoUltimoCambioMotor < 3000) {
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