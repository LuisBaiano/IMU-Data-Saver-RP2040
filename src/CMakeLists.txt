# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)
project(IMU_SSD C CXX ASM)
pico_sdk_init()


# Adiciona seus arquivos fonte ao executável
add_executable(${PROJECT_NAME}
    main.c
    hw_config.c     # Contém a configuração SPI para o SD Card
    include/rgb_led.c       # Biblioteca do LED RGB
    include/buzzer.c        # Biblioteca do Buzzer
    include/buttons.c       # Biblioteca dos Botões
    include/debouncer.c     # Dependência da biblioteca de Botões
    include/display.c       # Biblioteca do Display OLED
    include/external/sd__card.c     # Biblioteca do Logger de SD Card (refatorada de sd_card_menu.c)
    include/lib/ssd1306/ssd1306.c
)

# Adiciona diretórios de inclusão para seus cabeçalhos
target_include_directories(${PROJECT_NAME} PUBLIC
    .                                # Para os cabeçalhos do seu projeto (config.h, dados.h, etc.)
    include                          # Para os cabeçalhos em 'include/' (rgb_led.h, buzzer.h, buttons.h, display.h)
    include/external                 # Para os cabeçalhos em 'include/external/' (imu.h, sd__card.h)
    include/lib/ssd1306                      # Para a biblioteca ssd1306 (display)
)

# Adiciona a subpasta da biblioteca FatFs_SPI
# Assume que 'lib/FatFs_SPI' contém um CMakeLists.txt que configura a biblioteca FatFs_SPI
add_subdirectory(include/lib/FatFs_SPI)

# Vincula as bibliotecas necessárias
target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    FatFs_SPI           # Conforme sua especificação, vincula a biblioteca FatFs_SPI
    hardware_clocks     # Para clock_get_hz (usado no buzzer)
    hardware_adc        # Para adc_init (usado na hw_config/sd_card_menu se não foi removido totalmente)
    hardware_i2c        # Para MPU6050 e Display OLED
    hardware_spi        # Para comunicação SPI com o cartão SD
    hardware_pwm        # Para o Buzzer
    hardware_sync
    hardware_resets
    hardware_watchdog   # Para funcionalidades como watchdog_reboot (usado no main para BOOTSEL)
    pico_bootrom        # Para reset_usb_boot (usado no main para BOOTSEL)
)


pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
