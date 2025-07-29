
# Datalogger de Movimento com IMU MPU6050 e Cartão SD

## Visão Geral

Este projeto é um datalogger de movimento portátil e autônomo, desenvolvido como parte da Residência em Sistemas Embarcados (Embarcatech/CEPEDI). O sistema utiliza um Raspberry Pi Pico (em uma placa de desenvolvimento BitDogLab) para capturar dados de 6 eixos (aceleração e giroscópio) de um sensor IMU MPU6050. Os dados coletados são salvos em formato `.csv` em um cartão MicroSD.

O dispositivo possui uma interface de usuário rica e interativa, fornecendo feedback em tempo real através de um Display OLED, um LED RGB e um Buzzer, e é controlado por botões físicos. Acompanha o projeto um script em Python para a visualização e análise posterior dos dados coletados.

## Funcionalidades Principais

- **Captura de Dados de 6 Eixos:** Leitura contínua dos eixos X, Y e Z do acelerômetro e do giroscópio.
- **Armazenamento em CSV:** Gravação dos dados em formato de texto (`.csv`) em um cartão MicroSD, com cabeçalho e amostras periódicas (a cada 500ms).
- **Interface de Usuário Interativa:**
  - **Display OLED:** Exibe em tempo real o status do sistema, a condição do cartão SD, o tempo de gravação e a contagem de amostras. Uma tela secundária exibe os dados numéricos do IMU.
  - **LED RGB:** Fornece feedback visual claro sobre o estado atual do sistema (Pronto, Gravando, Acesso ao SD, Erro).
  - **Buzzer:** Emite alertas sonoros para confirmar ações importantes, como início e fim da gravação.
- **Controle por Botões:** O sistema é totalmente controlado por botões físicos, gerenciados por interrupções e lógica de debounce por software para garantir a precisão.
- **Análise de Dados Externa:** Um script em Python (`analise_imu.py`) permite a fácil visualização dos dados coletados em gráficos separados para aceleração и giroscópio.

## Hardware Necessário

- Placa de desenvolvimento com RP2040 (ex: BitDogLab ou Raspberry Pi Pico)
- Sensor IMU MPU6050
- Módulo para Cartão MicroSD (com interface SPI)
- Cartão MicroSD formatado em FAT32
- Display OLED SSD1306 (I2C)
- LED RGB
- Buzzer
- Botões de pressão (3 unidades)

## Software e Bibliotecas

- **Firmware:** C/C++ usando o Raspberry Pi Pico SDK.
  - Biblioteca FatFs (fornecida pela SDK) para gerenciamento do sistema de arquivos.
  - Bibliotecas customizadas para os periféricos (`display.c`, `buttons.c`, `rgb_led.c`, etc.).
- **Análise:** Python 3.
  - `pandas`: Para leitura e manipulação dos dados do CSV.
  - `matplotlib`: Para a plotagem dos gráficos.

## Como Compilar e Gravar

1. **Clone o Repositório:**

   ```bash
   git clone [link_do_seu_repositório]
   ```
2. **Configure o Ambiente:** Certifique-se de que o [Pico SDK](https://github.com/raspberrypi/pico-sdk) esteja configurado corretamente no seu sistema.
3. **Compile o Projeto:**

   ```bash
   cd [pasta_do_projeto]
   mkdir build
   cd build
   cmake ..
   make
   ```
4. **Grave na Pico:** Conecte a Raspberry Pi Pico ao computador enquanto segura o botão BOOTSEL. Arraste o arquivo `seu_projeto.uf2` gerado na pasta `build` para a unidade `RPI-RP2` que aparece.

## Modo de Operação

Após ligar, o dispositivo executa uma sequência de inicialização e entra no modo "Pronto".

- **Display:**

  - **Tela de Status (Padrão):** Mostra o estado (`PRONTO`, `GRAVANDO`, `ERRO`), status do SD (`Montado`/`Desmontado`), tempo de gravação (`T: Xs`) e contagem de amostras.
  - **Tela de Dados:** Acessada com o **Botão do Joystick**. Mostra uma grade com os valores numéricos dos 6 eixos do IMU em tempo real. Pressione novamente para voltar à tela de Status.
- **Botões:**

  - **Botão A:** Um clique curto para **iniciar** a gravação. Um segundo clique curto para **parar**. Uma trava de software ("lockout") previne paradas acidentais.
  - **Botão B:** Quando a gravação não está ativa, um clique **desmonta** o cartão SD (para remoção segura). Um novo clique **monta** o cartão novamente. A ação é bloqueada se o SD não for detectado.
- **LED RGB:**

  - **Verde:** `ESTADO_PRONTO` - Pronto para gravar.
  - **Vermelho:** `ESTADO_GRAVANDO` - Gravando dados.
  - **Azul:** `ESTADO_ACESSANDO_SD` - Escrevendo no cartão.
  - **Roxo:** `ESTADO_ERRO` - Falha na inicialização ou operação com o SD.
- **Buzzer:**

  - **Um beep curto:** Gravação iniciada.
  - **Dois beeps curtos:** Gravação parada.

## Análise dos Dados com Python

Após a coleta, o arquivo `LOG_IMU.CSV` pode ser analisado com o script `analise_imu.py`.

1. **Instale as Dependências:**

   ```bash
   # Para sistemas Debian/Ubuntu (método recomendado)
   sudo apt install python3-pandas python3-matplotlib

   # Ou usando pip (preferencialmente em um ambiente virtual)
   # pip install pandas matplotlib
   ```
2. **Copie o Arquivo:** Coloque o arquivo `LOG_IMU.CSV` do cartão SD na mesma pasta do script `analise_imu.py`.
3. **Execute o Script:**

   ```bash
   python3 analise_imu.py
   ```

   Duas janelas de gráfico serão exibidas, uma para aceleração e outra para giroscópio.

## Estrutura do Código

O projeto é altamente modularizado para facilitar a manutenção e o entendimento.

`.
├── build/                 # Arquivos de compilação (gerado)
├── include/               # Arquivos de cabeçalho (.h)
│   ├── buzzer.h
│   ├── buttons.h
│   ├── config.h           # Configurações de pinos e constantes
│   ├── dados.h            # Definição da estrutura de estado global
│   ├── debouncer.h
│   ├── display.h
│   ├── rgb_led.h
│   └── external/          # Cabeçalhos de bibliotecas externas (sd_card, etc.)
├── src/                   # Arquivos de código-fonte (.c)
│   ├── buzzer.c
│   ├── buttons.c
│   ├── debouncer.c
│   ├── display.c
│   ├── rgb_led.c
│   └── external/
│       ├── sd_card.c
│       └── ... (outros arquivos da lib FatFs)
├── analise_imu.py         # Script Python para análise
├── CMakeLists.txt         # Arquivo de build do CMake
├── pico_sdk_import.cmake  # Padrão do Pico SDK
└── main.c                 # Arquivo principal com o loop e a lógica de aplicação`
