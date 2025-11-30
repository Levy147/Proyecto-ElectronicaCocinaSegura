# 🏠 Sistema de Cocina Segura con ESP32

Sistema inteligente de seguridad para cocina que monitorea gas, humo, temperatura y presencia de niños usando ESP32 y MicroPython.

## 🚀 Características Principales

- 🔥 **Detección de Gas/Humo** - Sensor MQ-2 para fugas peligrosas
- 🌡️ **Control de Temperatura** - Monitoreo constante de la estufa
- 👶 **Detección de Niños** - Sensor PIR para presencia cercana
- 🚨 **Sistema de Alertas** - LEDs y buzzer para notificaciones
- 💨 **Ventilación Inteligente** - Control automático de ventilador

## 🛠️ Hardware Requerido

| Componente | Pin ESP32 |
|------------|-----------|
| Sensor Gas/Humo | GPIO 23 |
| Sensor Temperatura | GPIO 19 (ADC) |
| Sensor Movimiento | GPIO 18 |
| LED Rojo | GPIO 22 |
| LED Naranja | GPIO 21 |
| Buzzer | GPIO 5 |

## ⚡ Instalación Rápida

1. Clona el repositorio:
\`\`\`bash
git clone https://github.com/Levy147/Proyecto-ElectronicaCocinaSegura.git
\`\`\`

2. Sube el código a tu ESP32:
\`\`\`bash
python -m esptool --chip esp32 write_flash 0x1000 firmware.bin
\`\`\`

3. Ejecuta el sistema:
\`\`\`bash
ampy --port COM3 put src/main.py
\`\`\`

## 🔗 Diagrama

[![Wokwi Diagram](https://img.shields.io/badge/Wokwi-Live_Diagram-blue?style=for-the-badge&logo=arduino)](https://wokwi.com/projects/449016603296951297)

**👉 [Ver Diagrama Interactivo en Wokwi](https://wokwi.com/projects/449016603296951297)**

## 🎯 Lógica del Sistema

- **Gas detectado** → LED rojo + Buzzer continuo
- **Temperatura >30°C + Niño** → LED naranja intermitente + Buzzer pulsante
- **Solo humo** → Ventilador automático


---
*Desarrollado con ❤️ usando MicroPython*
