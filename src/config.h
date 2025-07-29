#ifndef CONFIG_H
#define CONFIG_H

// --- CONFIGURAÇÃO I2C ---
#define I2C_BAUDRATE        (400 * 1000)

// --- Barramento I2C 1: Para o Display OLED ---
#define DISPLAY_I2C_PORT    i2c1
#define DISPLAY_I2C_SDA_PIN 14 
#define DISPLAY_I2C_SCL_PIN 15 
#define DISPLAY_I2C_ADDR    0x3C
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64

// --- Barramento I2C 0: Para o Sensor MPU6050 ---
#define IMU_I2C_PORT        i2c0
#define IMU_I2C_SDA_PIN     0  // GPIO0
#define IMU_I2C_SCL_PIN     1  // GPIO1
#define MPU6050_I2C_ADDR    0x70 // Endereço padrão do MPU6050

// --- Configuração dos Pinos ---
// LED RGB
#define LED_RED_PIN         13
#define LED_GREEN_PIN       11
#define LED_BLUE_PIN        12

// Botões 
#define BUTTON_A_PIN        5 // Botão A: Iniciar / Parar a captura de dados.
#define BUTTON_B_PIN        6 // Botão B: Montar / Desmontar o cartão SD com segurança.
#define  BUTTON_JOYSTICK_PIN 22

// Buzzer
#define BUZZER_PWM_PIN      10 

// --- Configuração SPI para SD Card ---
#define SD_SPI_PORT         spi0
#define SD_SCK_PIN          18 // SCK: GPIO 18
#define SD_MOSI_PIN         19 // MOSI: GPIO 19
#define SD_MISO_PIN         16 // MISO: GPIO 16
#define SD_CS_PIN           17 // CS: GPIO 17

// --- Configuração de Temporização ---
#define IMU_READ_INTERVAL_MS 100 // Taxa de amostragem do IMU, 100ms = 10Hz
#define DISPLAY_UPDATE_INTERVAL_MS 500 // Atualiza display a cada 500ms
#define BUTTON_DEBOUNCE_US 200000 // 200ms para debounce

// --- Configurações dos Alertas ---
#define BUZZER_BEEP_FREQ_HZ 1500
#define BUZZER_BEEP_DURATION_MS 50 // Beep curto para feedback

#endif // CONFIG_H