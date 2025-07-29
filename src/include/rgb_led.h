#ifndef RGB_LED_H
#define RGB_LED_H

#include "dados.h" // Para DadosSistema_t e EstadoSistema

typedef enum {
    COR_LED_DESLIGADO,
    COR_LED_VERMELHO,
    COR_LED_VERDE,
    COR_LED_AZUL,
    COR_LED_AMARELO,
    COR_LED_CIANO,
    COR_LED_MAGENTA
} CorLed; // Renomeado para português

void rgb_led_init();
void rgb_led_set_color(CorLed cor); // Renomeado

/**
 * @brief Atualiza o LED RGB e, opcionalmente, o display, com base no estado do sistema.
 * Esta função gerencia o feedback visual principal do sistema.
 * @param dados_sistema Ponteiro para a estrutura de dados global do sistema.
 * @param ssd_instancia Ponteiro para a instância do display (ssd1306_t), se o display for usado.
 */
void rgb_led_atualizar_feedback_sistema(DadosSistema_t *dados_sistema); // Novo nome

#endif // RGB_LED_H