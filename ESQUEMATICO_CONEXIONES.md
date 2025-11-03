# 🔌 Esquemático de Conexiones - Máquina Recicladora PET

## 📋 Tabla de Conexiones Completa

### 🎮 Motor NEMA 17 + Driver A4988

| Arduino Pin | →  | A4988 Pin | Descripción |
|-------------|-------|-----------|-------------|
| **D2** | → | **DIR** | Dirección del motor (adelante/atrás) |
| **D3** | → | **STEP** | Pasos del motor (cada pulso = 1 paso) |
| **D8** | → | **MS1** | Microstepping bit 1 |
| **D12** | → | **MS2** | Microstepping bit 2 |
| **D13** | → | **MS3** | Microstepping bit 3 |
| **5V** | → | **VDD** | Alimentación lógica del driver |
| **GND** | → | **GND** | Tierra común |

**⚠️ IMPORTANTE - Resistencia Pull-Down:**
```
Pin STEP (A4988) ──┬──→ Arduino D3
                   │
              [Resistencia
               2.2kΩ]
                   │
                  GND
```

**🔗 Jumper RST-SLP:**
- Conectar con un cable jumper: **RST ↔ SLP** (pines del A4988)

**⚡ Alimentación del motor:**
- **12V+** → **VMOT** del A4988
- **GND** → **GND** del A4988

**🔌 Salidas del motor:**
- A4988 **1A, 1B, 2A, 2B** → Motor NEMA 17 (4 cables)

---

### 🌡️ Sistema de Calefacción (Hotend V6)

| Componente | Pin | →  | Destino |
|------------|-----|-------|---------|
| **Termistor NTC 100K** | Cable 1 | → | **A1** (Arduino) |
| **Termistor NTC 100K** | Cable 2 | → | **5V** |
| **Resistencia 4.7kΩ** | Terminal 1 | → | **A1** (Arduino) |
| **Resistencia 4.7kΩ** | Terminal 2 | → | **GND** |

**Circuito del Termistor:**
```
5V ─────┬──── Termistor ────┬──── A1 (Arduino)
        │                    │
        │              [Resistencia
        │                4.7kΩ]
        │                    │
        └────────────────────┴──── GND
```

| Módulo MOSFET | Pin | →  | Destino |
|---------------|-----|-------|---------|
| **MOSFET** | Signal (S) | → | **D11** (Arduino - PWM) |
| **MOSFET** | VCC | → | **5V** (Arduino) |
| **MOSFET** | GND | → | **GND** (Arduino) |
| **MOSFET** | V+ | → | **24V+** (Fuente) |
| **MOSFET** | V- | → | **GND** (Fuente) |
| **MOSFET** | OUT+ | → | **Hotend V6 +** |
| **MOSFET** | OUT- | → | **Hotend V6 -** |

---

### 📺 Display LCD 16x2 con I2C

| LCD I2C | Pin | →  | Arduino Pin |
|---------|-----|-------|-------------|
| **VCC** | | → | **5V** |
| **GND** | | → | **GND** |
| **SDA** | | → | **A4** (o pin SDA) |
| **SCL** | | → | **A5** (o pin SCL) |

---

### 🎚️ Potenciómetro (Control de Velocidad)

| Potenciómetro 10K | Pin | →  | Destino |
|-------------------|-----|-------|---------|
| Terminal 1 | | → | **GND** |
| Terminal central (wiper) | | → | **A0** (Arduino) |
| Terminal 3 | | → | **5V** |

---

### 🔘 Botones de Control

| Botón | Pin | →  | Arduino Pin | Función |
|-------|-----|-------|-------------|---------|
| **Botón 1** | Terminal 1 | → | **D4** | Cambiar dirección motor |
| **Botón 1** | Terminal 2 | → | **GND** | |
| **Botón 2** | Terminal 1 | → | **D5** | ON/OFF calefactor |
| **Botón 2** | Terminal 2 | → | **GND** | |
| **Botón 3** | Terminal 1 | → | **D6** | Bajar temperatura (-5°C) |
| **Botón 3** | Terminal 2 | → | **GND** | |
| **Botón 4** | Terminal 1 | → | **D7** | Subir temperatura (+5°C) |
| **Botón 4** | Terminal 2 | → | **GND** | |

**📝 Nota:** Los botones usan la resistencia pull-up interna del Arduino (INPUT_PULLUP), por eso van directo a GND.

---

### ⚡ Sistema de Alimentación

| Fuente | Salida | →  | Destino |
|--------|--------|-------|---------|
| **Fuente 24V** | 24V+ | → | VMOT (A4988) + Entrada Step-Down |
| **Fuente 24V** | GND | → | GND común |
| **Step-Down LM2596** | Entrada | ← | 24V (Fuente) |
| **Step-Down LM2596** | Salida 12V | → | Arduino Vin |
| **Step-Down LM2596** | GND | → | GND común |

---

## 🎨 Diagrama Visual Simplificado

```
                    ARDUINO UNO
        ┌──────────────────────────────────┐
        │                                  │
        │  D2  ────────────→ DIR (A4988)  │
        │  D3  ─┬──────────→ STEP (A4988) │
        │       │                          │
        │       └─[2.2kΩ]─→ GND            │
        │                                  │
        │  D4  ←───[Botón]──→ GND          │
        │  D5  ←───[Botón]──→ GND          │
        │  D6  ←───[Botón]──→ GND          │
        │  D7  ←───[Botón]──→ GND          │
        │                                  │
        │  D8  ────────────→ MS1 (A4988)   │
        │  D11 ────────────→ MOSFET Signal │
        │  D12 ────────────→ MS2 (A4988)   │
        │  D13 ────────────→ MS3 (A4988)   │
        │                                  │
        │  A0  ←───[Pot]────┬─ 5V          │
        │                   └─ GND         │
        │                                  │
        │  A1  ←───[Termistor + 4.7kΩ]─┬─ 5V
        │                               └─ GND
        │                                  │
        │  A4  ←─────────── SDA (LCD)      │
        │  A5  ←─────────── SCL (LCD)      │
        │                                  │
        │  5V  ────┬────────────────────   │
        │          ├→ LCD VCC              │
        │          ├→ A4988 VDD            │
        │          ├→ MOSFET VCC           │
        │          └→ Pot + Termistor      │
        │                                  │
        │  GND ────┴────────────────────   │
        │          └→ Común a todos        │
        │                                  │
        │  Vin ←─── 12V (Step-Down)        │
        └──────────────────────────────────┘

        A4988 Driver
        ┌──────────────┐
        │ VMOT ← 24V   │
        │ GND  ← GND   │
        │ 1A,1B,2A,2B  │──→ NEMA 17
        │ RST↔SLP      │ (con jumper)
        └──────────────┘

        MOSFET Module
        ┌──────────────┐
        │ V+ ← 24V     │
        │ V- ← GND     │
        │ OUT+,OUT-    │──→ Hotend V6
        └──────────────┘
```

---

## ✅ Lista de Verificación de Conexiones

Antes de encender, verificá:

- [ ] 🔌 Todos los GND están conectados (tierra común)
- [ ] ⚡ Fuente de 24V conectada correctamente
- [ ] 🔽 Step-down configurado en 12V de salida
- [ ] 🔗 Resistencia 2.2kΩ entre STEP y GND (pull-down)
- [ ] 🔗 Jumper RST-SLP en el A4988
- [ ] 📍 Pines MS1, MS2, MS3 conectados (D8, D12, D13)
- [ ] 🌡️ Termistor con resistencia 4.7kΩ a GND
- [ ] 📺 Display I2C en pines correctos (A4=SDA, A5=SCL)
- [ ] 🔘 Los 4 botones conectados a GND
- [ ] 🎚️ Potenciómetro entre GND y 5V, centro a A0
- [ ] 🔥 Hotend conectado al MOSFET (respetando polaridad)
- [ ] 💪 Motor NEMA 17 conectado al A4988

---

## 🎯 Configuración del Driver A4988

### Ajuste de Corriente (VREF)

El potenciómetro pequeño en el A4988 controla la corriente del motor:

**Para NEMA 17 (17HS2408):**
- **VREF recomendado:** 0.8V
- **Fórmula:** VREF = Corriente_motor × 8 × R_sense
- Para 1A: VREF = 1.0 × 8 × 0.1 = 0.8V

**🔧 Cómo medir:**
1. Conectá el multímetro en modo voltaje DC
2. Cable negro (COM) → GND del A4988
3. Cable rojo → Tornillo del potenciómetro
4. Girá el potenciómetro hasta ver 0.8V
5. Girá en sentido horario para aumentar
6. Girá en sentido antihorario para disminuir

---

## 🛡️ Seguridad

⚠️ **IMPORTANTE:**

1. **SIEMPRE desconectá la alimentación** antes de hacer cambios
2. **Verificá polaridad** del hotend y motor antes de conectar
3. **NO toques** el A4988 ni el hotend cuando estén encendidos (se calientan mucho)
4. **Botón de emergencia:** Mantené un botón para cortar la alimentación rápidamente
5. **Ventilación:** Trabajá en un lugar ventilado
6. **Supervisión adulta:** Este proyecto maneja altas temperaturas y voltajes

---

## 📸 Fotos Recomendadas (para agregar después)

Para hacer este manual más completo, sería útil agregar fotos de:
- [ ] Conexiones del A4988 con todos los cables
- [ ] Circuito del termistor con la resistencia
- [ ] Módulo MOSFET conectado
- [ ] Display LCD con cables I2C
- [ ] Potenciómetro conectado
- [ ] Vista general del circuito armado

---

**✨ Con estas conexiones, tu máquina recicladora estará lista para funcionar!**
