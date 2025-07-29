import pandas as pd
import matplotlib.pyplot as plt
import sys

# --- CONFIGURAÇÕES ---
NOME_ARQUIVO_CSV = 'LOG_IMU.CSV'
INTERVALO_AMOSTRAGEM_SEGUNDOS = 0.5

def plotar_dados_imu(nome_arquivo, intervalo_s):
    """
    Lê um arquivo CSV de dados do IMU de forma robusta e plota os gráficos
    de aceleração e giroscópio em JANELAS SEPARADAS.
    """
    try:
        nomes_das_colunas = ['numero_amostra', 'accel_x', 'accel_y', 'accel_z', 'giro_x', 'giro_y', 'giro_z']
        dados = pd.read_csv(nome_arquivo, sep=',', names=nomes_das_colunas, skiprows=1)
        
        print("--- SCRIPT DE ANÁLISE PYTHON ---")
        print(f"Arquivo '{nome_arquivo}' lido com sucesso.")
        print("As 5 primeiras linhas de dados, como o Python as interpretou, são:")
        print(dados.head())
        print("-----------------------------------")

    except FileNotFoundError:
        print(f"ERRO: O arquivo '{nome_arquivo}' não foi encontrado!")
        sys.exit(1)
    except Exception as e:
        print(f"Ocorreu um erro inesperado ao ler o arquivo: {e}")
        sys.exit(1)

    if dados.empty:
        print("ERRO: O arquivo CSV está vazio ou não pôde ser lido corretamente.")
        return

    # --- Criação do Eixo de Tempo (sem alterações) ---
    dados['tempo_s'] = (dados['numero_amostra'] - 1) * intervalo_s
    
    # ==============================================================================
    # <<< BLOCO DE PLOTAGEM MODIFICADO >>>
    # ==============================================================================

    # --- Figura 1: Gráfico de Aceleração ---
    plt.figure(1, figsize=(12, 6)) # Cria e ativa a Figura 1
    plt.plot(dados['tempo_s'], dados['accel_x'], label='Aceleração X', color='r')
    plt.plot(dados['tempo_s'], dados['accel_y'], label='Aceleração Y', color='g')
    plt.plot(dados['tempo_s'], dados['accel_z'], label='Aceleração Z', color='b')
    plt.title('Análise de Aceleração')
    plt.ylabel('Aceleração (g)')
    plt.xlabel('Tempo (segundos)')
    plt.grid(True)
    plt.legend()
    plt.tight_layout() # Ajusta o layout para a Figura 1

    # --- Figura 2: Gráfico de Giroscópio ---
    plt.figure(2, figsize=(12, 6)) # Cria e ativa a Figura 2
    plt.plot(dados['tempo_s'], dados['giro_x'], label='Giroscópio X', color='r')
    plt.plot(dados['tempo_s'], dados['giro_y'], label='Giroscópio Y', color='g')
    plt.plot(dados['tempo_s'], dados['giro_z'], label='Giroscópio Z', color='b')
    plt.title('Análise de Giroscópio')
    plt.ylabel('Velocidade Angular (dps)')
    plt.xlabel('Tempo (segundos)')
    plt.grid(True)
    plt.legend()
    plt.tight_layout() # Ajusta o layout para a Figura 2

    # --- Exibe TODAS as figuras criadas ---
    plt.show()


if __name__ == "__main__":
    plotar_dados_imu(NOME_ARQUIVO_CSV, INTERVALO_AMOSTRAGEM_SEGUNDOS)