#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h> // Para bool
#include <stdint.h>  // Para uint32_t
#include "dados.h"   // Para acessar a estrutura de estado 'DadosSistema_t'

// Funções para inicializar botões e lidar com eventos
void buttons_init();
/**
 * @brief Processa os eventos de clique dos botões no loop principal.
 * Esta função verifica as flags dos botões e executa a lógica de estado
 * e operações de E/S relacionadas aos botões.
 * @param dados_sistema Ponteiro para a estrutura de estado global do sistema.
 */
void handle_button_presses(DadosSistema_t *dados_sistema); // Usando DadosSistema_t

#endif // BUTTONS_H