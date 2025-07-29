#ifndef DISPLAY_H
#define DISPLAY_H

#include "dados.h" // Inclui a estrutura de estado 'DadosSistema_t'
#include "lib/ssd1306/ssd1306.h" // Inclusão explícita
#include "lib/ssd1306/font.h"  // Inclusão explícita

/**
 * @brief Inicializa o display OLED via I2C.
 * @param ssd Ponteiro para a instância do objeto ssd1306_t.
 */
void display_init(ssd1306_t *ssd);

/**
 * @brief Exibe uma tela de inicialização/boas-vindas no display.
 * @param ssd Ponteiro para a instância do objeto ssd1306_t.
 */
void display_startup_screen(ssd1306_t *ssd);

/**
 * @brief Exibe duas linhas de mensagem genéricas no display.
 * @param ssd Ponteiro para a instância do objeto ssd1306_t.
 * @param line1 Primeira linha de texto.
 * @param line2 Segunda linha de texto.
 */
void display_message(ssd1306_t *ssd, const char *line1, const char *line2);

/**
 * @brief Atualiza a tela principal do datalogger com base no estado atual.
 * Esta função será responsável por exibir o status, tempo de gravação e amostras.
 * @param ssd Ponteiro para a instância do objeto ssd1306_t.
 * @param sistema Ponteiro para a estrutura de estado do sistema.
 */
void display_data_screen(ssd1306_t *ssd, DadosSistema_t *sistema); // Renomeado 'state' para 'sistema'

void display_message(ssd1306_t *ssd, const char *line1, const char *line2) ;

#endif // DISPLAY_H