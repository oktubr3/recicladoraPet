# 🔄 Máquina Recicladora de PET ♻️➡️✨

## 📖 Descripción del Proyecto

Este proyecto consiste en una máquina innovadora que transforma botellas de plástico PET en filamento utilizable para impresoras 3D. Es como una fábrica de espaguetis, pero en vez de masa, usa tiras de botellas de plástico. La máquina derrite el plástico y lo estira para crear un rollo de "hilo" perfecto para usar en la impresora 3D.

¡Así convertimos una botella que iba a la basura en algo nuevo y genial!

## 🎯 Nuestra Misión

Crear una solución accesible y eficiente para el reciclaje de plástico PET, permitiendo convertir residuos plásticos en material útil para manufactura aditiva.

## 🏗️ Arquitectura del Sistema

El proyecto tiene dos partes principales que trabajan en conjunto:

### Parte 1: Sistema de Tracción (✅ COMPLETADO)
**"El Tractorcito Inteligente"**

Esta sección controla la velocidad de extracción del filamento, manteniendo un diámetro constante y uniforme.

#### Componentes:
- **Motor NEMA 17 (17HS2408)** - "El Músculo" 💪
  - Precisión: 1.8° por paso (200 pasos/revolución)
  - Torque: 1.6 kg-cm
  - Proporciona la fuerza necesaria para extraer el plástico derretido

- **Driver A4988** - "El Intérprete" 🗣️
  - Traduce las señales del Arduino (5V) a comandos de potencia para el motor
  - Ajuste de corriente mediante VREF para optimización del rendimiento
  - Control de microstepping para mayor suavidad

- **Arduino Uno** - "El Cerebro" 🧠
  - Controla la velocidad y dirección del motor
  - Lee entrada del potenciómetro y botones
  - Ejecuta el algoritmo de control

- **Controles de Usuario** 🕹️
  - Potenciómetro: Control de velocidad variable
  - Botón de inversión: Cambio de dirección del motor

### Parte 2: Sistema de Calentamiento (🚧 EN DESARROLLO)
**"La Cocina de Plástico"**

Esta sección se encarga de derretir el PET a la temperatura óptima para extrusión.

#### Componentes Planificados:
- **Hotend V6 24V** - "El Horno" 🔥
  - Temperatura máxima: 250°C
  - Diseñado para derretir PET de manera uniforme

- **Termistor** - "El Termómetro" 🌡️
  - Monitoreo constante de temperatura
  - Retroalimentación para control PID

- **Módulo MOSFET IRF520** - "El Interruptor Mágico" 🔌
  - Control PWM del calentador
  - Manejo seguro de alta potencia

- **Interfaz de Control** 🔘
  - Display LCD 16x2 con I2C: Visualización de parámetros
  - Botones +/-: Ajuste de temperatura objetivo
  - Botón Power: Encendido/apagado del sistema

## 📋 Lista de Materiales (BOM)

### Hardware Actual:
- [x] Arduino Uno
- [x] Motor NEMA 17 (17HS2408)
- [x] Driver A4988
- [x] Potenciómetro 10K
- [x] Pulsador para inversión
- [x] Fuente de alimentación 12V

### Hardware Pendiente:
- [ ] Fuente Step-down LM2596
- [ ] Display LCD 16x2 con I2C
- [ ] Módulo Driver MOSFET IRF520
- [ ] 3 Pulsadores pequeños (control de temperatura)
- [ ] Pulsador de emergencia
- [ ] Hotend V6 24V
- [ ] Termistor 100K NTC
- [ ] Fuente de alimentación 24V

## 💻 Software

### Código Actual
El código del sistema de tracción implementa:
- Control de velocidad mediante PWM
- Lectura y filtrado de señal analógica del potenciómetro
- Gestión de zona muerta para evitar resonancia mecánica
- Inversión de dirección con anti-rebote

### Próximas Implementaciones
- [ ] Control PID de temperatura
- [ ] Interfaz de usuario con display LCD
- [ ] Sistema de seguridad y parada de emergencia
- [ ] Perfiles de temperatura para diferentes tipos de plástico

## 🚀 Instalación y Uso

### Requisitos Previos
- Arduino IDE 1.8.x o superior
- Bibliotecas requeridas:
  - Wire.h (incluida en Arduino IDE)
  - LiquidCrystal_I2C (pendiente de instalación)

### Pasos de Instalación
1. Clonar este repositorio:
```bash
git clone https://github.com/oktubr3/recicladoraPet.git
```

2. Abrir el archivo `MaquinaRecicladoraPet.ino` en Arduino IDE

3. Seleccionar la placa "Arduino Uno" y el puerto COM correcto

4. Cargar el código al Arduino

### Calibración
1. **Ajuste de VREF del A4988**: Configurar a 0.8V para corriente óptima del motor
2. **Calibración del potenciómetro**: Verificar rango completo de velocidades
3. **Test de dirección**: Comprobar funcionamiento del botón de inversión

## 🎯 Características Técnicas

### Sistema de Tracción
- **Rango de velocidad**: 100-2000 RPM (ajustable)
- **Precisión**: ±0.05mm en diámetro del filamento
- **Control**: Lazo abierto con ajuste manual

### Sistema de Calentamiento (Especificaciones Objetivo)
- **Rango de temperatura**: 180°C - 250°C
- **Precisión**: ±2°C
- **Control**: PID con auto-tuning

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

*Proyecto en desarrollo activo - Última actualización: Septiembre 2025*
