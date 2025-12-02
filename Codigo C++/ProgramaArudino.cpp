/*
 * SISTEMA DE COCINA SEGURA CON ESP32
 * Version: 2.0
 * Autor: [Herbert Steven Galeano Flores]
 * 
 * Sensores:
 * - Gas/Humo: Pin 23 (digital)
 * - Temperatura: Pin 19 (analógico)
 * - Movimiento: Pin 18 (digital)
 * 
 * Actuadores:
 * - LED Rojo: Pin 22
 * - LED Naranja: Pin 21
 * - Buzzer: Pin 5
 * - Ventilador: Pin 17
 */

// Definición de pines
const int PIN_GAS_HUMO = 23;
const int PIN_TEMPERATURA = 19;  // Analógico
const int PIN_MOVIMIENTO = 18;
const int PIN_LED_ROJO = 22;
const int PIN_LED_NARANJA = 21;
const int PIN_BUZZER = 5;
const int PIN_VENTILADOR = 17;    // Control de ventilador

// Constantes del sistema
const int TEMPERATURA_UMBRAL = 30;      // °C
const unsigned long TIEMPO_ALERTA = 5000;     // 5 segundos
const unsigned long TIEMPO_VENTILACION = 10000; // 10 segundos
const int INTERVALO_LECTURA = 100;      // ms
const int UMBRAL_HUMO = 3;              // Lecturas para detectar humo

// Variables del sistema
bool estadoAlertaGas = false;
bool estadoAlertaNino = false;
bool ventiladorActivo = false;
bool humoDetectado = false;
unsigned long tiempoVentiladorOn = 0;
int contadorHumo = 0;
int ultimaLecturaGas = 0;

// Variables para el buzzer
bool buzzerActivo = false;
unsigned long tiempoBuzzer = 0;
int frecuenciaBuzzer = 0;

void setup() {
  // Configurar pines
  pinMode(PIN_GAS_HUMO, INPUT);
  pinMode(PIN_MOVIMIENTO, INPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_NARANJA, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_VENTILADOR, OUTPUT);
  
  // El pin de temperatura es analógico, no necesita pinMode
  
  // Inicializar estados
  apagarTodo();
  
  // Iniciar comunicación serial
  Serial.begin(115200);
  delay(100);
  
  Serial.println("==========================================");
  Serial.println("SISTEMA DE COCINA SEGURA INICIALIZADO");
  Serial.println("==========================================");
  Serial.print("Ventilador configurado en pin: ");
  Serial.println(PIN_VENTILADOR);
  Serial.print("Temperatura umbral: ");
  Serial.print(TEMPERATURA_UMBRAL);
  Serial.println("°C");
  Serial.println("Monitoreo iniciado...");
  Serial.println("==========================================");
}

void loop() {
  // Leer sensores
  int gasDetectado = digitalRead(PIN_GAS_HUMO);
  float temperaturaActual = leerTemperatura();
  int movimientoDetectado = digitalRead(PIN_MOVIMIENTO);
  
  // Mostrar estado actual en serial
  Serial.print("Estado - Gas: ");
  Serial.print(gasDetectado);
  Serial.print(" | Temp: ");
  Serial.print(temperaturaActual);
  Serial.print("°C | Movimiento: ");
  Serial.print(movimientoDetectado);
  Serial.print(" | Ventilador: ");
  Serial.println(ventiladorActivo ? "ON" : "OFF");
  
  // Lógica principal del sistema
  
  // 1. Procesar detección de GAS/HUMO
  if (gasDetectado == HIGH) {
    if (!estadoAlertaGas) {
      estadoAlertaGas = true;
      Serial.println("ALERTA: GAS/HUMO DETECTADO!");
      Serial.println("Ventilador APAGADO por seguridad");
    }
    alertaGasHumo(true);
    humoDetectado = false;
    contadorHumo = 0;
  } else {
    if (estadoAlertaGas) {
      estadoAlertaGas = false;
      Serial.println("Gas/humo normalizado");
    }
    alertaGasHumo(false);
    
    // 2. Controlar ventilador basado en humo (sin gas)
    controlarVentiladorHumo(gasDetectado);
  }
  
  // 3. Procesar detección de NIÑO con temperatura alta
  if (temperaturaActual > TEMPERATURA_UMBRAL && movimientoDetectado == HIGH) {
    if (!estadoAlertaNino) {
      estadoAlertaNino = true;
      Serial.println("ALERTA: Nino detectado con temperatura alta!");
      ejecutarAlertaNino();
    }
  } else {
    if (estadoAlertaNino) {
      estadoAlertaNino = false;
      Serial.println("Condiciones de nino normalizadas");
    }
    // Apagar LED naranja si no hay alerta
    digitalWrite(PIN_LED_NARANJA, LOW);
  }
  
  // 4. Controlar tiempo del ventilador
  if (ventiladorActivo) {
    if (millis() - tiempoVentiladorOn > TIEMPO_VENTILACION) {
      apagarVentilador();
      Serial.println("Ventilador apagado por tiempo");
    }
  }
  
  // Controlar buzzer (si está activo)
  controlarBuzzer();
  
  delay(INTERVALO_LECTURA);
}

float leerTemperatura() {
  // Leer valor analógico (0-4095 para ESP32)
  int lectura = analogRead(PIN_TEMPERATURA);
  
  // CONVERSIÓN PARA SENSOR TMP36 (Ajustar según tu sensor)
  // TMP36: 10mV por °C, 500mV a 0°C
  // ESP32: 0-4095 corresponde a 0-3.3V
  float voltaje = (lectura / 4095.0) * 3.3;
  float temperatura = (voltaje - 0.5) * 100.0; // TMP36 específico
  
  // Para LM35: temperatura = voltaje * 100
  // Para NTC: usar tabla de conversión o fórmula específica
  
  // Limitar valores
  temperatura = constrain(temperatura, -10.0, 125.0);
  return temperatura;
}

void alertaGasHumo(bool activar) {
  if (activar) {
    // Activar alerta de gas/humo
    digitalWrite(PIN_LED_ROJO, HIGH);
    activarBuzzer(1000);  // Frecuencia de 1000 Hz
    apagarVentilador();   // Apagar ventilador por seguridad
  } else {
    // Desactivar alerta
    digitalWrite(PIN_LED_ROJO, LOW);
    desactivarBuzzer();
  }
}

void controlarVentiladorHumo(int lecturaGas) {
  // Detectar humo (sin gas)
  if (lecturaGas == LOW) {
    contadorHumo++;
    if (contadorHumo >= UMBRAL_HUMO && !ventiladorActivo) {
      humoDetectado = true;
      encenderVentilador();
      Serial.println("Humo detectado - Ventilador encendido");
    }
  } else {
    contadorHumo = 0;
    humoDetectado = false;
  }
}

void ejecutarAlertaNino() {
  unsigned long tiempoInicio = millis();
  Serial.println("INICIANDO ALERTA DE NINO - 5 segundos");
  
  while (millis() - tiempoInicio < TIEMPO_ALERTA) {
    // Secuencia intermitente
    digitalWrite(PIN_LED_NARANJA, HIGH);
    activarBuzzer(1500);  // Sonido agudo
    delay(250);
    
    digitalWrite(PIN_LED_NARANJA, LOW);
    activarBuzzer(800);   // Sonido grave
    delay(250);
    
    // Verificar si las condiciones siguen
    float temp = leerTemperatura();
    int movimiento = digitalRead(PIN_MOVIMIENTO);
    
    if (temp <= TEMPERATURA_UMBRAL || movimiento == LOW) {
      Serial.println("Condiciones cambiadas, deteniendo alerta...");
      break;
    }
  }
  
  // Apagar todo al final
  digitalWrite(PIN_LED_NARANJA, LOW);
  desactivarBuzzer();
  Serial.println("Alerta de nino finalizada");
}

void encenderVentilador() {
  if (!ventiladorActivo) {
    digitalWrite(PIN_VENTILADOR, HIGH);
    ventiladorActivo = true;
    tiempoVentiladorOn = millis();
    Serial.println("VENTILADOR ENCENDIDO - Extraccion de humo");
  }
}

void apagarVentilador() {
  if (ventiladorActivo) {
    digitalWrite(PIN_VENTILADOR, LOW);
    ventiladorActivo = false;
    Serial.println("VENTILADOR APAGADO");
  }
}

void activarBuzzer(int frecuencia) {
  frecuenciaBuzzer = frecuencia;
  buzzerActivo = true;
  tiempoBuzzer = millis();
  tone(PIN_BUZZER, frecuencia);
}

void desactivarBuzzer() {
  buzzerActivo = false;
  noTone(PIN_BUZZER);
}

void controlarBuzzer() {
  // Control adicional para el buzzer si es necesario
  if (buzzerActivo) {
    // Aquí puedes agregar patrones complejos si lo deseas
    // Por ahora, tone() mantiene el sonido
  }
}

void apagarTodo() {
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_NARANJA, LOW);
  digitalWrite(PIN_VENTILADOR, LOW);
  noTone(PIN_BUZZER);
}
