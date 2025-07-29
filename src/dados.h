#ifndef DADOS_H
#define DADOS_H

#include <stdint.h>
#include <stdbool.h>

// Enum para os estados de operação do sistema
typedef enum {
    ESTADO_INICIALIZANDO,
    ESTADO_PRONTO,
    ESTADO_GRAVANDO,
    ESTADO_ACESSANDO_SD,
    ESTADO_ERRO
} EstadoSistema;

// Enum para controlar a tela ativa no display
typedef enum {
    TELA_LOG,
    TELA_DADOS_IMU
} TelaAtiva;

typedef struct {
    // Status Operacional
    EstadoSistema estado_atual;
    bool sd_montado;
    bool esta_gravando;


    float acel_x_g;
    float acel_y_g;
    float acel_z_g;
    float giro_x_dps;
    float giro_y_dps;
    float giro_z_dps;

    // Informações de Log
    uint32_t contagem_amostras;
    uint32_t tempo_gravacao_ms;

    // Flags de Controle
    bool display_precisa_atualizar;
    TelaAtiva tela_atual;

} DadosSistema_t;

#endif // DADOS_H