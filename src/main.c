#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico/binary_info.h"
#include <string.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"

#include "config.h"
#include "dados.h"
#include "include/rgb_led.h"
#include "include/buzzer.h"
#include "include/buttons.h"
#include "include/display.h"
#include "include/external/sd__card.h"

#define ACEL_FATOR_G  16384.0f
#define GIRO_FATOR_DPS 131.0f

/**
 * @brief Reseta e acorda o MPU6050.
 */
static void main_imu_reset() {
    int addr = MPU6050_I2C_ADDR;

    uint8_t buf[] = {0x6B, 0x80}; // Reset
    i2c_write_blocking(IMU_I2C_PORT, addr, buf, 2, false);
    sleep_ms(100);
    
    buf[1] = 0x00; // Sair do modo sleep
    i2c_write_blocking(IMU_I2C_PORT, addr, buf, 2, false);
    sleep_ms(10);
}

/**
 * @brief Lê os dados brutos do MPU6050 via I2C e converte para unidades físicas.
 */
static void main_imu_read_and_process(DadosSistema_t *dados_ptr) {
    int16_t accel_raw[3]; // Usa um array dedicado para os dados brutos de aceleração
    int16_t gyro_raw[3];  // Usa um array dedicado para os dados brutos de giroscópio
    uint8_t buffer[6]; 
    uint8_t val;
    int addr = MPU6050_I2C_ADDR;

    // --- Etapa 1: Leitura da ACELERAÇÃO ---
    val = 0x3B; // Endereço do primeiro registrador do acelerômetro (ACCEL_XOUT_H)
    i2c_write_blocking(IMU_I2C_PORT, addr, &val, 1, true);   // Aponta para o registrador
    i2c_read_blocking(IMU_I2C_PORT, addr, buffer, 6, false); // Lê os 6 bytes de aceleração
    // Converte os bytes lidos e armazena no array de aceleração
    for (int i = 0; i < 3; i++) {
        accel_raw[i] = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
    }

    // --- Etapa 2: Leitura do GIROSCÓPIO ---
    val = 0x43; // Endereço do primeiro registrador do giroscópio (GYRO_XOUT_H)
    i2c_write_blocking(IMU_I2C_PORT, addr, &val, 1, true);   // Aponta para o NOVO registrador
    i2c_read_blocking(IMU_I2C_PORT, addr, buffer, 6, false); // Lê 6 NOVOS bytes, sobrescrevendo o buffer
    // Converte os novos bytes e armazena no array de giroscópio
    for (int i = 0; i < 3; i++) {
        gyro_raw[i] = (buffer[i * 2] << 8) | buffer[i * 2 + 1];
    }

    dados_ptr->acel_x_g = (float)accel_raw[0] / ACEL_FATOR_G;
    dados_ptr->acel_y_g = (float)accel_raw[1] / ACEL_FATOR_G;
    dados_ptr->acel_z_g = (float)accel_raw[2] / ACEL_FATOR_G;
    
    dados_ptr->giro_x_dps = (float)gyro_raw[0] / GIRO_FATOR_DPS;
    dados_ptr->giro_y_dps = (float)gyro_raw[1] / GIRO_FATOR_DPS;
    dados_ptr->giro_z_dps = (float)gyro_raw[2] / GIRO_FATOR_DPS;
}


// Variáveis globais
DadosSistema_t dados;
ssd1306_t ssd;

int main() {
    stdio_init_all();
    sleep_ms(2000);

    memset(&dados, 0, sizeof(DadosSistema_t));
    dados.estado_atual = ESTADO_INICIALIZANDO;
    dados.tela_atual = TELA_LOG;
    
    // Configuração dos pinos (bi_decl)
    bi_decl(bi_program_description("Savamento de dados de Movimento com MPU6050 e SD Card"));
    bi_decl(bi_2pins_with_func(IMU_I2C_SDA_PIN, IMU_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_2pins_with_func(DISPLAY_I2C_SDA_PIN, DISPLAY_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_4pins_with_func(SD_SCK_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_CS_PIN, GPIO_FUNC_SPI));
    bi_decl(bi_1pin_with_name(BUTTON_A_PIN, "Botao A (Iniciar/Parar)"));
    bi_decl(bi_1pin_with_name(BUTTON_B_PIN, "Botao B (Controle SD)"));
    bi_decl(bi_3pins_with_names(LED_RED_PIN, "LED_VERMELHO", LED_GREEN_PIN, "LED_VERDE", LED_BLUE_PIN, "LED_AZUL"));
    bi_decl(bi_1pin_with_name(BUZZER_PWM_PIN, "Buzzer PWM"));
    
    // Inicialização dos periféricos
    rgb_led_init();
    buzzer_init();
    buttons_init();
    display_init(&ssd);
    
    // Inicialização do I2C do IMU e Reset do sensor
    i2c_init(IMU_I2C_PORT, 400 * 1000);
    gpio_set_function(IMU_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(IMU_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_I2C_SDA_PIN);
    gpio_pull_up(IMU_I2C_SCL_PIN);
    main_imu_reset();
    
    display_startup_screen(&ssd);

    // Inicialização do SD
    if (!sd_init_spi() || !sd_mount(&dados)) {
        dados.estado_atual = ESTADO_ERRO;
    } else {
        dados.estado_atual = ESTADO_PRONTO;
    }

    // Variáveis de controle para o loop
    bool flag_gravacao_iniciada = false;
    absolute_time_t inicio_gravacao_ts = get_absolute_time(); // Inicializado para evitar erro
    int contador_para_salvar = 0;

    while (true) {
        // Tarefas executadas em todos os ciclos (aprox. a cada 100ms)
        handle_button_presses(&dados);
        main_imu_read_and_process(&dados);
        display_data_screen(&ssd, &dados);
        rgb_led_atualizar_feedback_sistema(&dados);
        
        // --- Bloco de Gerenciamento da Gravação ---
        if (dados.esta_gravando) {
            // Executa UMA VEZ ao iniciar a gravação
            if (!flag_gravacao_iniciada) {
                buzzer_beep(BUZZER_BEEP_FREQ_HZ, BUZZER_BEEP_DURATION_MS); // Beep de início
                if (sd_open_file("LOG_IMU.CSV", false)) {
                    sd_write_line("numero_amostra,accel_x,accel_y,accel_z,giro_x,giro_y,giro_z"); // Cabeçalho do enunciado
                    dados.contagem_amostras = 0;
                    contador_para_salvar = 0;
                    inicio_gravacao_ts = get_absolute_time();
                    flag_gravacao_iniciada = true;
                } else {
                    dados.esta_gravando = false; 
                    dados.estado_atual = ESTADO_ERRO;
                }
            }

            // Executa em TODOS os ciclos enquanto estiver gravando
            if (flag_gravacao_iniciada) {
                dados.tempo_gravacao_ms = absolute_time_diff_us(get_absolute_time(), inicio_gravacao_ts) / 1000;
                contador_para_salvar++;
                
                if (contador_para_salvar >= 5) { 
                    contador_para_salvar = 0; // Zera o contador

                    dados.contagem_amostras++;
                    char linha_csv[128];
                    snprintf(linha_csv, sizeof(linha_csv), "%lu,%.4f,%.4f,%.4f,%.2f,%.2f,%.2f",
                             dados.contagem_amostras,
                             dados.acel_x_g, dados.acel_y_g, dados.acel_z_g,
                             dados.giro_x_dps, dados.giro_y_dps, dados.giro_z_dps);
                    
                    if (!sd_write_line(linha_csv)) {
                        dados.esta_gravando = false; 
                        dados.estado_atual = ESTADO_ERRO;
                    }
                }
            }
        } else {
            // Executa UMA VEZ ao parar a gravação
            if (flag_gravacao_iniciada) {
                sd_close_file();
                flag_gravacao_iniciada = false;
                
                // Feedback sonoro e visual
                buzzer_beep(BUZZER_BEEP_FREQ_HZ, BUZZER_BEEP_DURATION_MS); sleep_ms(100);
                buzzer_beep(BUZZER_BEEP_FREQ_HZ, BUZZER_BEEP_DURATION_MS);
                display_message(&ssd, "Dados Salvos!", "");
                sleep_ms(2000);
            }
        }
        
        // Pausa no final do loop
        sleep_ms(IMU_READ_INTERVAL_MS);
    }
    return 0;
}