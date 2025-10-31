# 🔄 Máquina Recicladora de PET ♻️➡️✨

## 📖 ¿Qué es este proyecto?

Esta máquina transforma botellas de plástico PET (las de gaseosa) en "hilo" para impresoras 3D.

**¿Cómo funciona?** Es como una máquina de hacer fideos, pero con plástico:
1. Cortás tiras de botellas de plástico
2. La máquina las derrite a 240°C (muy caliente!)
3. Un motor va tirando del plástico derretido
4. Sale un "hilo" perfecto para usar en impresoras 3D

**¡Convertimos basura en algo útil!** 🌍💚

---

## 🏗️ ¿Cómo está hecha?

La máquina tiene **DOS partes principales**:

### Parte 1: El Motor (Tira del plástico)
**"El Tractorcito Inteligente" 🚜**

Esta parte tira del plástico derretido, como cuando sacás masa de pizza estirada.

**¿Qué usa?**
- **Motor NEMA 17**: El "músculo" que tira 💪
- **Driver A4988**: Convierte las señales del Arduino en potencia para el motor
- **Arduino Uno**: El "cerebro" que controla todo 🧠
- **Potenciómetro**: Una perilla para controlar la velocidad (como el volumen de la radio)
- **Botón**: Para cambiar la dirección (adelante/atrás)

**¿Qué hace especial?**
- Se mueve SÚPER suave (microstepping 1/16 = pasos 16 veces más pequeños)
- Acelera y frena de forma gradual (no se sacude)
- Podés controlar la velocidad de 0% a 100%

---

### Parte 2: El Calentador (Derrite el plástico)
**"La Cocina de Plástico" 🔥**

Esta parte derrite el plástico PET a 240°C (la temperatura perfecta).

**¿Qué usa?**
- **Hotend V6**: El "horno" que calienta
- **Termistor NTC 100k**: Un "termómetro electrónico" que mide la temperatura
- **Módulo MOSFET**: Un interruptor electrónico que controla cuánta potencia darle al calentador
- **Display LCD**: Una pantallita que muestra la temperatura 📺
- **3 Botones**: Para prender/apagar y subir/bajar la temperatura

**¿Qué hace especial?**
- **Control PID profesional**: Como las impresoras 3D caras (Prusa)
- Llega exactamente a la temperatura que querés (±2-3°C de precisión)
- No se pasa ni se queda corta

---

## 📋 ¿Qué materiales necesito?

### Electrónica:
- [ ] 🧠 Arduino Uno (el cerebro)
- [ ] 💪 Motor NEMA 17 (modelo 17HS2408)
- [ ] 🎮 Driver A4988 (controla el motor)
- [ ] 📺 Display LCD 16x2 con I2C (la pantallita)
- [ ] ⚡ Módulo MOSFET (para controlar el calentador)
- [ ] 🌡️ Termistor NTC 100K (el termómetro)
- [ ] 🎚️ Potenciómetro 10K (la perilla de velocidad)
- [ ] 🔘 4 Botones pulsadores

### Componentes importantes:
- [ ] 🔥 Hotend V6 (el calentador)
- [ ] 🔌 Fuente de alimentación 24V
- [ ] 🔽 Módulo Step-down LM2596 (convierte 24V a 12V)
- [ ] 🔧 Resistencia 4.7kΩ (para el termómetro)
- [ ] ⭐ Resistencia 2.2kΩ (evita que el motor vibre al encender)
- [ ] 🔗 Cable jumper (para conectar 2 pines del A4988)
- [ ] 🛑 Botón de emergencia (para cortar todo en caso de problema)

---

## 🚀 ¿Cómo lo instalo?

### Paso 1: Instalar el programa Arduino IDE 💻
1. 📥 Descargá Arduino IDE desde [arduino.cc](https://www.arduino.cc)
2. 🖱️ Instalalo en tu computadora

### Paso 2: Descargar este proyecto 📦
1. 📥 Descargá este proyecto (botón verde "Code" → "Download ZIP")
2. 📂 Descomprimí el archivo ZIP

### Paso 3: Instalar la biblioteca del display 📚
1. 🖥️ Abrí Arduino IDE
2. 🔧 Andá a: **Herramientas** → **Administrar Bibliotecas**
3. 🔍 Buscá: **"LiquidCrystal I2C"**
4. ✅ Instalá la versión de **Frank de Brabander**

### Paso 4: Subir el código al Arduino ⬆️
1. 📄 Abrí el archivo: `MaquinaRecicladoraPet/MaquinaRecicladoraPet.ino`
2. 🔌 Conectá tu Arduino a la computadora con el cable USB
3. ⚙️ Seleccioná: **Herramientas** → **Placa** → **Arduino Uno**
4. 🔌 Seleccioná: **Herramientas** → **Puerto** → (el puerto donde está tu Arduino)
5. ➡️ Hacé click en el botón **"➜"** (Subir) y esperá ⏳

---

## ⚠️ IMPORTANTE: Configuración de Hardware

### 🔊 Problema común: Motor vibra al encender

Cuando encendés el Arduino, durante los primeros 3 segundos el motor puede vibrar o girar solo. Esto pasa porque los cables "flotan" (no tienen un valor definido) hasta que el programa arranca.

**✅ Solución (OBLIGATORIA):**

#### 1. 🔗 Conectar dos pines del A4988 juntos
En el módulo A4988, buscá los pines **RST** y **SLP** (están uno al lado del otro). Conectalos con un cable o jumper.

#### 2. ⭐ Agregar una resistencia al pin STEP

Esta es LA SOLUCIÓN más importante:

```
                    Cable del Arduino
Arduino D3 ────────────────────┐
                                │
                                ├──── Pin STEP del A4988
                                │
                           [Resistencia
                            2.2kΩ]
                                │
                               GND (tierra)
```

**¿Cómo se conecta?**
- **NO desconectes** el cable que va del Arduino al A4988
- **Agregá** una resistencia de 2.2kΩ entre el pin STEP del A4988 y GND
- La resistencia va "en paralelo" (al costado), no corta el cable
- Podés usar resistencias de: 2.2kΩ, 4.7kΩ o 10kΩ

**¿Por qué funciona?**
La resistencia "tira" el pin hacia GND (0 voltios) cuando el Arduino no está mandando señales. Así el motor no se mueve durante el arranque.

#### 3. 🎯 Conectar los pines de microstepping (IMPORTANTE para movimiento suave)

El microstepping hace que el motor se mueva SÚPER suave, sin vibraciones. Tenés que conectar:

- 📍 **Arduino D8** → **MS1** del A4988
- 📍 **Arduino D12** → **MS2** del A4988
- 📍 **Arduino D13** → **MS3** del A4988

Estos 3 pines le dicen al motor que se mueva en pasos 16 veces más pequeños (3200 pasos por vuelta en vez de 200). ¡Por eso el movimiento es tan suave!

---

## 🎮 ¿Cómo se usa?

### 🎚️ Controlar la velocidad del motor:
- 🔄 Girá el **potenciómetro** (la perilla)
- ➡️ Hacia la derecha = más rápido 🏃
- ⬅️ Hacia la izquierda = más lento 🐢
- 🛑 Si lo ponés en 0%, el motor se apaga solo

### 🔀 Cambiar la dirección:
- 🔘 Presioná el **botón conectado al pin D4**
- ↔️ El motor cambia entre adelante y atrás

### 🌡️ Controlar la temperatura:
- 🔘 **Botón conectado al pin D5**: Prender/Apagar el calentador 🔥
- 🔘 **Botón conectado al pin D6**: Bajar temperatura (de a 5°C) ❄️
- 🔘 **Botón conectado al pin D7**: Subir temperatura (de a 5°C) 🔥
- 🎯 Temperatura inicial: **240°C** (perfecta para PET)

### 📺 Leer el display:
**Pantalla normal** (muestra la temperatura):
```
T:240.0C  [ON]
Obj:240C
```
- 🌡️ **T:** = Temperatura actual
- 🔥 **[ON]** = Está calentando
- ✅ **[--]** = Está en la temperatura correcta (esperando)
- ⭕ **[OFF]** = Apagado
- 🎯 **Obj:** = Temperatura objetivo

**Pantalla temporal** (cuando ajustás la velocidad):
```
Velocidad:  50%
Dir: Adelante
```
o
```
Velocidad:  50%
Dir: Atras
```
⏱️ Esta pantalla se muestra por 3 segundos cuando tocás el potenciómetro o el botón de dirección.

---

## 🔧 Solución de problemas

### 🔊 El motor vibra cuando enciendo el Arduino

**❓ Causa:** Los cables "flotan" durante los primeros 3 segundos del arranque.

**✅ Solución:**
1. ❓ ¿Conectaste los pines **RST y SLP** del A4988?
2. ❓ ¿Agregaste la **resistencia 2.2kΩ** entre STEP y GND?
3. ❓ ¿La resistencia está en **paralelo** (no corta el cable del Arduino)?

✨ Si hiciste todo eso, el motor NO debería moverse al encender.

---

### 🔊 El motor vibra TODO el tiempo

**❓ Posibles causas:**
1. **⚙️ Microstepping mal configurado:**
   - Verificá que los pines **D8, D12, D13** del Arduino estén conectados a **MS1, MS2, MS3** del A4988
   - Estos pines controlan el modo de microstepping (1/16 step = movimiento súper suave)

2. **⚡ Poca corriente:**
   - El potenciómetro del A4988 (VREF) debe estar en 0.8V
   - Si no tenés multímetro, giralo POQUITO en sentido horario

3. **🔗 RST y SLP no conectados:**
   - Verificá que esos dos pines estén conectados con un jumper

---

### 📺 El display no muestra nada

**✅ Soluciones:**
1. **🎚️ Ajustar el contraste:** En la parte de atrás del display hay un potenciómetro chiquito. Giralo hasta que veas las letras.
2. **📍 Dirección I2C incorrecta:**
   - 🖥️ Abrí el código en Arduino IDE
   - 🔍 Buscá la línea 34: `LiquidCrystal_I2C lcd(0x27, 16, 2);`
   - 🔄 Cambiá `0x27` por `0x3F`
   - ⬆️ Volvé a subir el código

---

### 🌡️ La temperatura no es correcta

1. 🔍 **Verificá las conexiones del termistor** (el sensor de temperatura)
2. 🔧 **Verificá la resistencia de 4.7kΩ** (debe estar bien conectada)
3. 🏠 La temperatura ambiente debería mostrar entre 18°C y 25°C

---

## 📊 Datos técnicos (para los curiosos) 🤓

### 🎮 Motor:
- ⚙️ **Pasos por vuelta:** 3200 (con microstepping 1/16)
- 🏃 **Velocidad:** Ajustable de 100 a 2000 RPM
- ✨ **Suavidad:** Movimiento ultra suave, sin vibraciones
- 🚀 **Aceleración:** Gradual (no se sacude al arrancar/frenar)

### 🌡️ Temperatura:
- 📏 **Rango:** 0°C a 270°C
- 🎯 **Precisión:** ±2-3°C
- 🧠 **Control:** PID profesional (como impresoras 3D Prusa)
- 🔥 **Temperatura para PET:** 240°C

### 📺 Display:
- 📐 **Tamaño:** 16 caracteres × 2 líneas
- ⚡ **Actualización:** Cada 0.2 segundos (sin parpadeo)

---

## 🛡️ Seguridad

**⚠️ IMPORTANTE - LEÉ ESTO:**

1. 🔥 **El calentador llega a 240°C** (¡puede quemarte!):
   - 🚫 NO lo toques cuando está encendido
   - ⏰ Esperá 10 minutos después de apagar para que se enfríe

2. 🛑 **Botón de emergencia**:
   - ✅ Siempre tené el botón de emergencia conectado
   - ⚡ Si algo sale mal, presionalo y corta toda la energía

3. 👨‍👩‍👧 **Supervisión de un adulto**:
   - ⚠️ Este proyecto usa temperaturas altas y electricidad
   - 🤝 Pedile a un adulto que te supervise

4. 💨 **Ventilación**:
   - 🪟 Usá la máquina en un lugar ventilado
   - 👃 El plástico derretido puede tener olor

---

## 🎓 ¿Querés aprender más?

### 🤖 Sobre Arduino:
- 📚 [Documentación oficial de Arduino](https://www.arduino.cc/reference/es/)
- 🎥 [Tutoriales en español](https://www.arduino.cc/en/Tutorial/HomePage)

### ♻️ Sobre el PET:
- 🔬 [¿Qué es el PET?](https://es.wikipedia.org/wiki/Tereftalato_de_polietileno)
- 🥤 El PET es el plástico de las botellas de gaseosa
- 🌡️ Se derrite a 240-260°C

### 🌍 Sobre reciclaje:
- ♻️ [Precious Plastic](https://preciousplastic.com/) - Proyecto de reciclaje de plástico global
- 🤖 [Recyclebot](https://www.appropedia.org/Recyclebot) - Máquinas similares

---

## 💚 Misión del proyecto

**🌍 Queremos ayudar al planeta** convirtiendo botellas de plástico (que iban a la basura) en material útil para crear cosas nuevas con impresoras 3D.

Si cada persona recicla sus botellas, podemos:
- 🗑️ Reducir la basura plástica
- 🎨 Crear material útil
- 💰 Ahorrar dinero
- 🌳 Proteger el medio ambiente

**¡Juntos podemos convertir la basura en tesoros!** 🌍♻️✨

---

## 📄 Licencia

Este proyecto es **libre y gratuito** (Licencia MIT). Podés:
- ✅ Usarlo para lo que quieras
- 🔧 Modificarlo
- 🤝 Compartirlo
- 🏗️ Construir tu propia máquina

---

## 👥 Créditos

Creado con ❤️ para ayudar al planeta 🌍 y enseñar programación 💻.

---

**❓ ¿Preguntas? 🐛 ¿Problemas? 💡 ¿Mejoras?**
Abrí un "Issue" en GitHub y te ayudamos! 🚀
