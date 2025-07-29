#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

#include "buttons.h"
#include "config.h"
#include "debouncer.h"
#include "dados.h"

static volatile bool btn_a_pressed = false;
static volatile bool btn_b_pressed = false;
static volatile bool btn_joystick_pressed = false;

static uint32_t last_press_time_A = 0;
static uint32_t last_press_time_B = 0;
static uint32_t last_press_time_JOY = 0;

// Variável para a trava (lockout) que resolve o problema do "repique"
static absolute_time_t button_a_lockout_until;

static void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_A_PIN) {
        // A interrupção só aceita o clique se o tempo de trava já passou
        if (check_debounce(&last_press_time_A, BUTTON_DEBOUNCE_US) && absolute_time_diff_us(get_absolute_time(), button_a_lockout_until) < 0) {
            btn_a_pressed = true;
        }
    } else if (gpio == BUTTON_B_PIN) {
        if (check_debounce(&last_press_time_B, BUTTON_DEBOUNCE_US)) btn_b_pressed = true;
    } else if (gpio == BUTTON_JOYSTICK_PIN) {
        if (check_debounce(&last_press_time_JOY, BUTTON_DEBOUNCE_US)) btn_joystick_pressed = true;
    }
}

void buttons_init() {
    // Inicializa a trava como já "vencida"
    button_a_lockout_until = get_absolute_time();
    
    // Código de inicialização dos pinos
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);
    gpio_init(BUTTON_JOYSTICK_PIN);
    gpio_set_dir(BUTTON_JOYSTICK_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_JOYSTICK_PIN);

    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_JOYSTICK_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void handle_button_presses(DadosSistema_t *dados_sistema) {
    if (btn_a_pressed) {
        btn_a_pressed = false;

        // Inverte o estado da gravação
        dados_sistema->esta_gravando = !dados_sistema->esta_gravando;
        
        // Se a gravação foi INICIADA, ativa a trava
        if (dados_sistema->esta_gravando) {
            button_a_lockout_until = make_timeout_time_ms(500);
        }
        
        dados_sistema->display_precisa_atualizar = true;
    }
    
    // Lógica do botão B e Joystick (sem alterações)
    if (btn_b_pressed) {
        btn_b_pressed = false;
        if (!dados_sistema->esta_gravando) {
            dados_sistema->sd_montado = !dados_sistema->sd_montado;
            dados_sistema->estado_atual = ESTADO_ACESSANDO_SD;
            dados_sistema->display_precisa_atualizar = true;
        }
    }
    if (btn_joystick_pressed) {
        btn_joystick_pressed = false;
        dados_sistema->tela_atual = (dados_sistema->tela_atual == TELA_LOG) ? TELA_DADOS_IMU : TELA_LOG;
        dados_sistema->display_precisa_atualizar = true;
    }
}