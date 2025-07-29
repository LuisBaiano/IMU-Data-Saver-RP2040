#include "pico/stdlib.h"
#include "rgb_led.h"
#include "config.h"
#include "dados.h" // Para DadosSistema_t e EstadoSistema
#include "pico/time.h" // Para absolute_time_diff_us

// Variável estática para o tempo da última atualização do display, se o display for atualizado aqui
static absolute_time_t ultimo_update_display;


void rgb_led_init() {
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
}

void rgb_led_set_color(CorLed cor) {
    bool r=0, g=0, b=0;
    switch (cor) {
        case COR_LED_VERMELHO:  r = 1; break;
        case COR_LED_VERDE:     g = 1; break;
        case COR_LED_AZUL:      b = 1; break;
        case COR_LED_AMARELO:   r = 1; g = 1; break;
        case COR_LED_CIANO:     g = 1; b = 1; break;
        case COR_LED_MAGENTA:   r = 1; b = 1; break;
        case COR_LED_DESLIGADO: break;
    }
    gpio_put(LED_RED_PIN, r);
    gpio_put(LED_GREEN_PIN, g);
    gpio_put(LED_BLUE_PIN, b);
}

void rgb_led_atualizar_feedback_sistema(DadosSistema_t *dados_sistema) {
    // Controle do LED RGB conforme o estado do sistema
    switch (dados_sistema->estado_atual) {
        case ESTADO_INICIALIZANDO:
            rgb_led_set_color(COR_LED_AMARELO); // Amarelo
            break;
        case ESTADO_PRONTO:
            rgb_led_set_color(COR_LED_VERDE);  // Verde
            break;
        case ESTADO_GRAVANDO:
            rgb_led_set_color(COR_LED_VERMELHO); // Vermelho
            break;
        case ESTADO_ACESSANDO_SD:
            // Para piscar, precisaríamos de uma lógica de temporização mais complexa aqui
            // ou um LED PWM. Por simplicidade, deixaremos azul sólido por enquanto.
            rgb_led_set_color(COR_LED_AZUL); // Azul
            break;
        case ESTADO_ERRO:
            // Deixaremos roxo sólido por enquanto.
            rgb_led_set_color(COR_LED_MAGENTA); // Roxo
            break;
        default:
            rgb_led_set_color(COR_LED_DESLIGADO);
            break;
    }

}