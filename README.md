# 🔄 Máquina Recicladora de PET ♻️➡️✨

## 📖 Descripción del Proyecto

Este proyecto consiste en una máquina innovadora que transforma botellas de plástico PET en filamento utilizable para impresoras 3D. Es como una fábrica de espaguetis, pero en vez de masa, usa tiras de botellas de plástico. La máquina derrite el plástico y lo estira para crear un rollo de "hilo" perfecto para usar en la impresora 3D.

La máquina combina:
- **Control de temperatura PID profesional** (como impresoras 3D Prusa)
- **Motor paso a paso con interrupciones** para precisión absoluta
- **Interfaz intuitiva** con display LCD y botones de control

¡Así convertimos una botella que iba a la basura en algo nuevo y genial!

## 🎯 Nuestra Misión

Crear una solución accesible y eficiente para el reciclaje de plástico PET, permitiendo convertir residuos plásticos en material útil para manufactura aditiva, con control de calidad profesional y operación confiable.

## 🏗️ Arquitectura del Sistema

El proyecto tiene dos partes principales que trabajan en conjunto:

### Parte 1: Sistema de Tracción (✅ COMPLETADO)
**"El Tractorcito Inteligente"**

Esta sección controla la velocidad de extracción del filamento con precisión absoluta mediante interrupciones por hardware.

#### Componentes:
- **Motor NEMA 17 (17HS2408)** - "El Músculo" 💪
  - Precisión: 1.8° por paso (200 pasos/revolución)
  - Torque: 1.6 kg-cm
  - Proporciona la fuerza necesaria para extraer el plástico derretido
  - Control por Timer1 con interrupciones (cero interferencia del display)

- **Driver A4988** - "El Intérprete" 🗣️
  - Conectado a pines D2 (DIR) y D3 (STEP)
  - Traduce las señales del Arduino (5V) a comandos de potencia para el motor
  - Ajuste de corriente mediante VREF para optimización del rendimiento
  - Control de microstepping para mayor suavidad

- **Arduino Uno** - "El Cerebro" 🧠
  - Controla la velocidad y dirección del motor mediante Timer1
  - Lee entrada del potenciómetro y botones cada 50ms
  - Ejecuta el algoritmo de control PID para temperatura
  - Maneja display LCD sin afectar el motor

- **Controles de Usuario** 🕹️
  - **Potenciómetro (A0)**: Control de velocidad variable (0-100%)
  - **Botón inversión (D4)**: Cambio de dirección del motor
  - Motor se apaga automáticamente por debajo del 2% de velocidad

### Parte 2: Sistema de Calentamiento (✅ COMPLETADO)
**"La Cocina de Plástico"**

Esta sección se encarga de derretir el PET a la temperatura óptima (240°C) con control PID profesional.

#### Componentes:
- **Hotend/Calefactor** - "El Horno" 🔥
  - Temperatura objetivo: 240°C (ajustable 0-270°C)
  - Control PWM con precisión de ±2-3°C en estado estable
  - Calentamiento inteligente que frena antes para evitar sobrepaso

- **Termistor NTC 100k** - "El Termómetro" 🌡️
  - Conectado a A1 con divisor de tensión (R fija: 4.7kΩ)
  - Monitoreo constante con ecuación Steinhart-Hart
  - Doble filtrado (promedio de 10 lecturas + filtro exponencial)
  - Lectura estable sin ruido

- **Módulo MOSFET** - "El Interruptor Mágico" 🔌
  - Conectado a pin D11 (PWM con Timer2)
  - Control PWM del calentador (0-255 niveles de potencia)
  - Separado del Timer1 del motor para evitar interferencias

- **Display LCD 16x2 con I2C** 📺
  - Dirección I2C: 0x27 (o 0x3F)
  - Conectado a pines A4 (SDA) y A5 (SCL)
  - Muestra temperatura actual y objetivo
  - Indica estado: [ON], [OFF], o [--] (en espera)
  - Muestra velocidad/dirección temporalmente (3 segundos) al ajustar

- **Panel de Control** 🎮
  - **D5**: ON/OFF del control de temperatura
  - **D6**: Bajar temperatura (-5°C por pulsación)
  - **D7**: Subir temperatura (+5°C por pulsación)

#### Control PID (Como Prusa i3):
El sistema implementa control PID profesional con parámetros optimizados:
- **Kp = 8.0**: Respuesta proporcional al error
- **Ki = 0.05**: Corrección de error acumulado
- **Kd = 120.0**: Anticipación y frenado (evita sobrepaso)

Este control garantiza:
- ✅ Llegada suave a temperatura objetivo sin sobrepaso significativo
- ✅ Estabilidad excelente en estado estable (±2°C)
- ✅ Respuesta rápida a cambios de temperatura
- ✅ Eficiencia energética (potencia variable, no ON/OFF)

## 📋 Lista de Materiales (BOM)

### Hardware Implementado:
- [x] Arduino Uno
- [x] Motor NEMA 17 (17HS2408)
- [x] Driver A4988
- [x] Potenciómetro 10K (control de velocidad)
- [x] Pulsador para inversión de dirección
- [x] Display LCD 16x2 con I2C (dirección 0x27)
- [x] Módulo MOSFET para calefactor
- [x] 3 Pulsadores para control de temperatura (ON/OFF, +, -)
- [x] Termistor NTC 100K
- [x] Resistencia 4.7kΩ (divisor de tensión del termistor)
- [x] Fuente de alimentación 24V
- [x] Módulo Step-down LM2596 (24V → 12V para Arduino y motor)
- [x] Pulsador de emergencia
- [x] Hotend V6

## 💻 Software

### Código Implementado (MaquinaRecicladoraPet.ino)
El sistema completo incluye:

#### Control de Motor:
- ✅ Control mediante interrupciones por hardware (Timer1)
- ✅ Generación de pulsos automática en ISR (Interrupt Service Routine)
- ✅ Control de velocidad variable mediante potenciómetro
- ✅ Lectura y filtrado de señal analógica (promedio de 5 muestras)
- ✅ Gestión de zona muerta (apagado automático < 2%)
- ✅ Inversión de dirección con anti-rebote
- ✅ Arquitectura no bloqueante (cero interferencia con display/sensores)

#### Control de Temperatura:
- ✅ Algoritmo PID profesional (Kp=8.0, Ki=0.05, Kd=120.0)
- ✅ Lectura de termistor NTC 100k con ecuación Steinhart-Hart
- ✅ Doble filtrado: promedio de 10 lecturas + suavizado exponencial
- ✅ Control PWM del calefactor (0-255 niveles de potencia)
- ✅ Ajuste de temperatura objetivo mediante botones (±5°C)
- ✅ Activación/desactivación por botón
- ✅ Límites de seguridad por software (0-270°C)

#### Interfaz de Usuario:
- ✅ Display LCD I2C 16x2 (actualización cada 200ms)
- ✅ Vista principal: temperatura actual/objetivo y estado del calefactor
- ✅ Vista temporal (3s): velocidad y dirección al ajustar controles
- ✅ Panel de 4 botones (inversión + control de temperatura)
- ✅ Lectura no bloqueante de entradas cada 50ms

#### Arquitectura:
- ✅ Timer1 dedicado al motor (no bloqueante)
- ✅ Timer2 para PWM del calefactor (pin D11)
- ✅ Separación de timers para evitar conflictos
- ✅ Comunicación I2C para display (pines A4/A5)

## 🚀 Instalación y Uso

### Requisitos Previos
- Arduino IDE 1.8.x o superior
- Bibliotecas requeridas:
  - Wire.h (incluida en Arduino IDE)
  - LiquidCrystal_I2C (instalar desde Administrador de Bibliotecas)

### Pasos de Instalación
1. Clonar este repositorio:
```bash
git clone https://github.com/oktubr3/recicladoraPet.git
```

2. Instalar la biblioteca LiquidCrystal_I2C:
   - En Arduino IDE: Herramientas → Administrar Bibliotecas
   - Buscar "LiquidCrystal I2C"
   - Instalar la versión de Frank de Brabander

3. Abrir el archivo `MaquinaRecicladoraPet/MaquinaRecicladoraPet.ino` en Arduino IDE

4. Seleccionar la placa "Arduino Uno" y el puerto COM correcto

5. Cargar el código al Arduino

### Calibración Inicial
1. **Ajuste de VREF del A4988**: Configurar a 0.8V para corriente óptima del motor
2. **Verificación de display LCD**: Si no muestra nada, ajustar el potenciómetro en la parte trasera del módulo I2C
3. **Dirección I2C**: Si el display no funciona con 0x27, cambiar a 0x3F en el código (línea 11)
4. **Calibración del termistor**: Verificar que la temperatura ambiente sea coherente (18-25°C)
5. **Test de motor**: Verificar rango completo de velocidades y dirección

### Operación
1. **Control de velocidad**: Girar potenciómetro (apagado automático < 2%)
2. **Cambio de dirección**: Presionar botón D4
3. **Control de temperatura**:
   - D5: Activar/desactivar calefactor
   - D6: Bajar temperatura objetivo (-5°C)
   - D7: Subir temperatura objetivo (+5°C)
4. **Temperatura por defecto**: 240°C (óptimo para PET)

## 🎯 Características Técnicas

### Sistema de Tracción
- **Rango de velocidad**: 100-2000 RPM (ajustable mediante potenciómetro)
- **Resolución de control**: 200 pasos/revolución (NEMA 17)
- **Zona muerta**: Apagado automático bajo 2% de velocidad
- **Control**: Lazo abierto con interrupciones por hardware (Timer1)
- **Tiempo de respuesta**: <1ms (control por ISR)
- **Interferencia**: Cero (arquitectura no bloqueante)

### Sistema de Calentamiento
- **Rango de temperatura**: 0°C - 270°C (ajustable)
- **Temperatura por defecto**: 240°C (óptimo para PET)
- **Precisión**: ±2-3°C en estado estable
- **Control**: PID profesional (Kp=8.0, Ki=0.05, Kd=120.0)
- **Resolución PWM**: 256 niveles (0-255)
- **Frecuencia de actualización**: 200ms (5 Hz)
- **Sensor**: Termistor NTC 100k con ecuación Steinhart-Hart
- **Filtrado**: Doble etapa (promedio + exponencial)

### Interfaz de Usuario
- **Display**: LCD 16x2 con backlight, protocolo I2C
- **Actualización de pantalla**: 200ms (sin parpadeo)
- **Lectura de botones**: 50ms con anti-rebote por software
- **Vista temporal**: 3 segundos tras ajuste de velocidad/dirección

## 🛡️ Seguridad

- **Botón de parada de emergencia**: Corte inmediato de energía
- **Protección térmica**: Límites de temperatura en software
- **Aislamiento eléctrico**: Separación de circuitos de potencia y control
- **Indicadores visuales**: LED de estado y alarmas en display

## 📚 Recursos Adicionales

- [Documentación Arduino](https://www.arduino.cc/reference/en/)
- [Guía de calibración A4988](https://www.pololu.com/product/1182)
- [Propiedades térmicas del PET](https://en.wikipedia.org/wiki/Polyethylene_terephthalate)

## 📄 Licencia

Este proyecto está bajo Licencia MIT - ver el archivo LICENSE para más detalles.

---

**¡Juntos podemos convertir la basura en tesoros!** 🌍💚

*Proyecto en desarrollo activo - Última actualización: Octubre 2025*
