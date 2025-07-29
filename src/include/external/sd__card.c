#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

#include "sd__card.h"
#include "config.h" // Para os pinos do SPI
#include "ff.h"      // Biblioteca principal FatFs
#include "f_util.h"  // Funções utilitárias FatFs
#include "hw_config.h" // Para sd_get_by_num, que gerencia os drives

// A biblioteca FatFs precisa de uma estrutura FATFS para cada drive montado.
static FATFS fatfs;

// A biblioteca FatFs precisa de uma estrutura FIL para cada arquivo aberto.
static FIL fil;

// Flag para saber se um arquivo está aberto.
static bool file_is_open = false;

// Contador para sincronização periódica
static int write_count = 0;


bool sd_init_spi() {
    if (!sd_init_driver()) {
        printf("ERRO: Falha ao inicializar o driver do SD Card.\n");
        return false;
    }
    printf("Driver do SD Card inicializado.\n");
    return true;
}

bool sd_mount(DadosSistema_t *dados_sistema) {
    const char *drive_path = "0:";

    // Tenta montar o sistema de arquivos
    FRESULT fr = f_mount(&fatfs, drive_path, 1);
    if (fr != FR_OK) {
        printf("ERRO: f_mount falhou (%d): %s\n", fr, FRESULT_str(fr));
        dados_sistema->sd_montado = false;
        return false;
    }

    printf("SD Card montado com sucesso.\n");
    dados_sistema->sd_montado = true;
    return true;
}

bool sd_unmount(DadosSistema_t *dados_sistema) {
    const char *drive_path = "0:";
    f_unmount(drive_path); // f_unmount não retorna erros na implementação padrão
    
    printf("SD Card desmontado.\n");
    dados_sistema->sd_montado = false;
    if (file_is_open) {
        sd_close_file();
    }
    return true;
}

bool sd_open_file(const char *filename, bool append) {
    if (file_is_open) {
        printf("AVISO: Já havia um arquivo aberto. Fechando-o primeiro.\n");
        sd_close_file();
    }

    // Define o modo de abertura:
    // FA_WRITE: permissão de escrita
    // FA_CREATE_ALWAYS: cria um novo arquivo. Se já existe, ele é sobrescrito.
    // FA_OPEN_APPEND: abre um arquivo existente e posiciona no final para adicionar dados.
    BYTE mode = FA_WRITE | (append ? FA_OPEN_APPEND : FA_CREATE_ALWAYS);

    FRESULT fr = f_open(&fil, filename, mode);
    if (fr != FR_OK) {
        printf("ERRO: f_open falhou (%d): %s\n", fr, FRESULT_str(fr));
        file_is_open = false;
        return false;
    }
    
    printf("Arquivo '%s' aberto com sucesso.\n", filename);
    file_is_open = true;
    write_count = 0; // Reseta o contador de sincronização
    return true;
}

bool sd_write_line(const char *line) {
    if (!file_is_open) {
        printf("ERRO: Tentativa de escrever em arquivo sem estar aberto.\n");
        return false;
    }

    if (f_printf(&fil, "%s\n", line) < 0) {
        printf("ERRO: f_printf falhou.\n");
        return false;
    }
    write_count++;
    if (write_count >= 10) {
        FRESULT fr = f_sync(&fil);
        if (fr != FR_OK) {
            printf("ERRO: f_sync falhou (%d): %s\n", fr, FRESULT_str(fr));
            return false;
        }
        write_count = 0; // Reseta o contador
    }

    return true;
}

void sd_close_file() {
    if (!file_is_open) {
        return;
    }

    FRESULT fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERRO: f_close falhou (%d): %s\n", fr, FRESULT_str(fr));
    } else {
        printf("Arquivo fechado com sucesso.\n");
    }

    file_is_open = false;
}