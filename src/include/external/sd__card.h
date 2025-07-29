#ifndef SD__CARD_H
#define SD__CARD_H

#include <stdbool.h>
#include "dados.h" // Dependência da estrutura global

/**
 * @brief Inicializa o periférico SPI para comunicação com o cartão SD.
 * Deve ser chamada uma vez na inicialização do sistema.
 * @return true em caso de sucesso, false em caso de falha.
 */
bool sd_init_spi();

/**
 * @brief Monta o sistema de arquivos do cartão SD.
 * @param dados_sistema Ponteiro para a estrutura de dados global.
 * @return true em caso de sucesso, false em caso de falha.
 */
bool sd_mount(DadosSistema_t *dados_sistema);

/**
 * @brief Desmonta o sistema de arquivos do cartão SD.
 * @param dados_sistema Ponteiro para a estrutura de dados global.
 * @return true em caso de sucesso, false em caso de falha.
 */
bool sd_unmount(DadosSistema_t *dados_sistema);

/**
 * @brief Abre um arquivo para escrita, criando-o se não existir.
 * @param filename O nome do arquivo a ser aberto (ex: "LOG_IMU.CSV").
 * @param append Se true, tenta mover o ponteiro para o final do arquivo. Se false, o arquivo é sobrescrito.
 * @return true em caso de sucesso, false em caso de falha.
 */
bool sd_open_file(const char *filename, bool append);

/**
 * @brief Escreve uma única linha de texto no arquivo atualmente aberto.
 * Adiciona automaticamente um caractere de nova linha.
 * @param line A string de texto a ser escrita.
 * @return true em caso de sucesso, false em caso de falha.
 */
bool sd_write_line(const char *line);

/**
 * @brief Fecha o arquivo que está atualmente aberto.
 * Essencial para garantir que todos os dados em buffer sejam salvos no cartão.
 */
void sd_close_file();

#endif // SD_CARD_H