// Definición de pines
const int PIN_GAS_HUMO = 23;
const int PIN_LED_ROJO = 22;
const int PIN_LED_NARANJA = 21;
const int PIN_TEMPERATURA = 19;
const int PIN_MOVIMIENTO = 18;
const int PIN_BUZZER = 5;

// Variables
int temperatura = 0;
bool gasHumoDetectado = false;
bool movimientoDetectado = false;
bool ventiladorEncendido = false;

// Constantes
const int TEMPERATURA_UMBRAL = 30; // 30°C
const unsigned long TIEMPO_ALERTA = 5000; // 5 segundos

void setup() {
  // Configurar pines
  pinMode(PIN_GAS_HUMO, INPUT);
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_NARANJA, OUTPUT);
  pinMode(PIN_MOVIMIENTO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  
  // El pin de temperatura es analógico, no necesita pinMode
  
  // Inicializar estados
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_NARANJA, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  
  // Iniciar comunicación serial para debugging
  Serial.begin(115200);
  Serial.println("Sistema de Cocina Segura Iniciado");
}

void loop() {
  // Leer sensores
  leerSensores();
  
  // Procesar lógica de gas/humo
  procesarGasHumo();
  
  // Procesar lógica de temperatura y movimiento
  procesarTemperaturaMovimiento();
  
  delay(100); // Pequeña pausa para estabilidad
}

void leerSensores() {
  // Leer sensor de gas/humo (digital)
  gasHumoDetectado = digitalRead(PIN_GAS_HUMO);
  
  // Leer sensor de temperatura (analógico)
  int lecturaAnalogica = analogRead(PIN_TEMPERATURA);
  // Convertir a temperatura (ajusta esta conversión según tu sensor)
  temperatura = map(lecturaAnalogica, 0, 4095, 0, 100);
  
  // Leer sensor de movimiento (digital)
  movimientoDetectado = digitalRead(PIN_MOVIMIENTO);
  
  // Debugging (opcional)
  Serial.print("Gas/Humo: ");
  Serial.print(gasHumoDetectado);
  Serial.print(" | Temp: ");
  Serial.print(temperatura);
  Serial.print("°C | Movimiento: ");
  Serial.println(movimientoDetectado);
}

void procesarGasHumo() {
  if (gasHumoDetectado) {
    // Si hay gas/humo detectado
    digitalWrite(PIN_LED_ROJO, HIGH);
    tone(PIN_BUZZER, 1000); // Sonido continuo
    
    // El ventilador NO se enciende si hay gas
    ventiladorEncendido = false;
    
    Serial.println("ALERTA: Gas/Humo detectado!");
  } else {
    // Si no hay gas/humo
    digitalWrite(PIN_LED_ROJO, LOW);
    noTone(PIN_BUZZER);
    
    // Aquí podrías agregar lógica para encender ventilador si solo hay humo
    // ventiladorEncendido = true;
  }
}

void procesarTemperaturaMovimiento() {
  if (temperatura > TEMPERATURA_UMBRAL && movimientoDetectado) {
    // Si temperatura > 30°C y hay movimiento
    Serial.println("ALERTA: Temperatura alta y niño detectado!");
    activarAlertaNino();
  } else {
    // Apagar LEDs naranja si no hay alerta
    digitalWrite(PIN_LED_NARANJA, LOW);
  }
}

void activarAlertaNino() {
  unsigned long tiempoInicio = millis();
  
  while (millis() - tiempoInicio < TIEMPO_ALERTA) {
    // LED naranja intermitente
    digitalWrite(PIN_LED_NARANJA, HIGH);
    tone(PIN_BUZZER, 2000); // Sonido de alerta
    delay(500);
    
    digitalWrite(PIN_LED_NARANJA, LOW);
    noTone(PIN_BUZZER);
    delay(500);
    
    // Verificar si todavía se cumple la condición
    leerSensores();
    if (!(temperatura > TEMPERATURA_UMBRAL && movimientoDetectado)) {
      break; // Salir si la condición ya no se cumple
    }
  }
  
  // Asegurarse de apagar todo al final
  digitalWrite(PIN_LED_NARANJA, LOW);
  noTone(PIN_BUZZER);
}

// Función para controlar ventilador (si lo tienes conectado)
void controlarVentilador(bool encender) {
  if (encender && !gasHumoDetectado) {
    // Solo encender ventilador si no hay gas
    // Aquí conectarías el pin del ventilador
    ventiladorEncendido = true;
    Serial.println("Ventilador ENCENDIDO");
  } else {
    ventiladorEncendido = false;
    Serial.println("Ventilador APAGADO");
  }
}
