#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "display.h"
#include "config.h"
#include "dados.h"

/**
 * @brief Desenha a tela principal do Datalogger (Status, SD, Tempo, Amostras).
 */
static void display_internal(ssd1306_t *ssd, DadosSistema_t *sistema) {
    char buffer[32];

    switch (sistema->estado_atual) {
        case ESTADO_INICIALIZANDO: ssd1306_draw_string(ssd, "Inicializando...", 2, 0); break;
        case ESTADO_PRONTO:        ssd1306_draw_string(ssd, "PRONTO!", 2, 0);            break;
        case ESTADO_GRAVANDO:      ssd1306_draw_string(ssd, "GRAVANDO...", 2, 0);        break;
        case ESTADO_ACESSANDO_SD:  ssd1306_draw_string(ssd, "SD ACESSO...", 2, 0);       break;
        case ESTADO_ERRO:          ssd1306_draw_string(ssd, "ERRO!", 2, 0);              break;
    }

    snprintf(buffer, sizeof(buffer), "SD: %s", sistema->sd_montado ? "Montado" : "Desmontado");
    ssd1306_draw_string(ssd, buffer, 2, 16);

    if (sistema->esta_gravando) {
        snprintf(buffer, sizeof(buffer), "T: %lus", sistema->tempo_gravacao_ms / 1000);
        ssd1306_draw_string(ssd, buffer, 2, 32);
        snprintf(buffer, sizeof(buffer), "Amostras: %lu", sistema->contagem_amostras);
        ssd1306_draw_string(ssd, buffer, 2, 48);
    } else {
        ssd1306_draw_string(ssd, "A: Gravar/Parar", 2, 32);
        ssd1306_draw_string(ssd, "Joy: Alts Tela", 2, 48);
    }
}

/**
 * @brief Desenha a tela com os dados em tempo real do Acelerômetro e Giroscópio.
 */
static void display_imu_data_internal(ssd1306_t *ssd, DadosSistema_t *sistema) {
    char buffer[20];
    
    // Títulos
    ssd1306_draw_string(ssd, "ACEL", 8, 0);
    ssd1306_draw_string(ssd, "GIRO", 64, 0);
    
    // Acessando os campos diretamente da estrutura principal
    snprintf(buffer, sizeof(buffer), "X: %-5.2f", sistema->acel_x_g);
    ssd1306_draw_string(ssd, buffer, 2, 16);
    snprintf(buffer, sizeof(buffer), "Y: %-5.2f", sistema->acel_y_g);
    ssd1306_draw_string(ssd, buffer, 2, 32);
    snprintf(buffer, sizeof(buffer), "Z: %-5.2f", sistema->acel_z_g);
    ssd1306_draw_string(ssd, buffer, 2, 48);

    snprintf(buffer, sizeof(buffer), "X: %-5.1f", sistema->giro_x_dps);
    ssd1306_draw_string(ssd, buffer, 64, 16);
    snprintf(buffer, sizeof(buffer), "Y: %-5.1f", sistema->giro_y_dps);
    ssd1306_draw_string(ssd, buffer, 64, 32);
    snprintf(buffer, sizeof(buffer), "Z: %-5.1f", sistema->giro_z_dps);
    ssd1306_draw_string(ssd, buffer, 64, 48);
}

void display_init(ssd1306_t *ssd) {
    i2c_init(DISPLAY_I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(DISPLAY_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_I2C_SDA_PIN);
    gpio_pull_up(DISPLAY_I2C_SCL_PIN);
    
    ssd1306_init(ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, DISPLAY_I2C_ADDR, DISPLAY_I2C_PORT);
    ssd1306_config(ssd);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
    printf("Display inicializado.\n");
}

void display_startup_screen(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    const char *line1 = "EMBARCATECH";
    const char *line2 = "PROJETO";
    const char *line3 = "IMU & SD CARD";
    uint8_t center_x = ssd->width / 2;
    uint8_t pos_x1 = center_x - (strlen(line1) * 8) / 2;
    uint8_t pos_x2 = center_x - (strlen(line2) * 8) / 2;
    uint8_t pos_x3 = center_x - (strlen(line3) * 8) / 2;
    ssd1306_draw_string(ssd, line1, pos_x1, 8);
    ssd1306_draw_string(ssd, line2, pos_x2, 28);
    ssd1306_draw_string(ssd, line3, pos_x3, 48);
    ssd1306_send_data(ssd);
    sleep_ms(2000);
    ssd1306_fill(ssd, false);
    ssd1306_send_data(ssd);
}


void display_data_screen(ssd1306_t *ssd, DadosSistema_t *sistema) {
    ssd1306_fill(ssd, false);
    switch (sistema->tela_atual) {
        case TELA_DADOS_IMU:
            display_imu_data_internal(ssd, sistema);
            break;

        case TELA_LOG:
        default:
            display_internal(ssd, sistema);
            break;
    }
    ssd1306_send_data(ssd);
}


void display_message(ssd1306_t *ssd, const char *line1, const char *line2) {
    ssd1306_fill(ssd, false);
    uint8_t center_x = ssd->width / 2;
    if (line1 && strlen(line1) > 0) {
        uint8_t pos_x1 = center_x - (strlen(line1) * 8) / 2;
        ssd1306_draw_string(ssd, line1, pos_x1, 24);
    }
    if (line2 && strlen(line2) > 0) {
        uint8_t pos_x2 = center_x - (strlen(line2) * 8) / 2;
        ssd1306_draw_string(ssd, line2, pos_x2, 36);
    }
    ssd1306_send_data(ssd);
}