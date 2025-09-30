[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/ZhkyyTjF)

# RaioX - Sistema de Controle de Máquina de Raio‑X

Trabalho 2 da disciplina de Fundamentos de Sistemas Embarcados, focado no desenvolvimento de um sistema de controle para uma máquina de Raio-X de dois eixos.

## 1. Objetivo do Projeto

O objetivo deste trabalho foi aplicar conceitos de sistemas embarcados para desenvolver um sistema de controle completo para uma mesa de Raio-X. O sistema gerencia a movimentação nos eixos X e Y, implementa controle de posição preciso via PID, e interage com o operador através de um painel touch remoto.

## 2. Funcionalidades Implementadas

O sistema final implementa com sucesso as seguintes funcionalidades:

- **Controle de Posição PID:** Controle preciso da posição dos eixos X e Y utilizando controladores PID com constantes de ajuste independentes para cada eixo, garantindo um movimento estável e preciso.
- **Comunicação UART/Modbus:** Comunicação robusta com o painel de controle (ESP32) via UART, utilizando o protocolo Modbus para leitura de comandos do operador e envio de dados de status (posição, velocidade, etc.).
- **Múltiplos Modos de Operação:**
  - **Modo Manual:** Movimentação livre dos eixos através de botões físicos e dos botões de setas no painel touch.
  - **Modo Automático:** Posicionamento autônomo da máquina em locais pré-definidos.
- **Sistema de Presets:**
  - **Modo de Programação:** Uma interface dedicada permite ao usuário mover a máquina para uma posição desejada e salvá-la em um de quatro "slots" (P1, P2, P3, P4).
  - **Execução de Presets:** Ao selecionar um preset salvo, a máquina se move automaticamente para a posição armazenada usando o controle PID.
- **Sensores e Monitoramento:** Leitura em tempo real de sensores de temperatura e pressão (BMP280) e envio dos dados para o dashboard.
- **Segurança:** Implementação de parada de emergência via botão físico e tratamento do sinal `Ctrl+C` (`SIGINT`) para um desligamento seguro dos motores.

## 3. Estrutura do Projeto

O código foi organizado de forma modular para facilitar a manutenção e o entendimento:

- **`include/`**: Contém os arquivos de cabeçalho (`.h`) com as declarações de funções e estruturas de cada módulo.
- **`src/`**: Contém os arquivos de implementação (`.c`) com a lógica de cada módulo (motores, encoders, PID, presets, etc.).
- **`Makefile`**: Automatiza o processo de compilação e execução do projeto.
- **`main`**: O arquivo binário executável gerado após a compilação.

## 4. Instruções de Compilação e Execução

### Pré-requisitos

É necessário ter a biblioteca `wiringPi` instalada na Raspberry Pi.

### Compilação

Para compilar o projeto, execute o seguinte comando no terminal, na pasta raiz do projeto:

```bash
make build
```

### Execução

Devido à necessidade de acesso aos pinos GPIO, o programa deve ser executado com privilégios de superusuário:

```bash
make run
```

Este comando irá compilar o projeto (se necessário) e depois executá-lo com `sudo`. Para parar o programa de forma segura, pressione `Ctrl+C`.

### Limpeza

Para remover o arquivo binário gerado, execute:

```bash
make clean
```

## 5. Instruções de Uso

Após a execução do programa, o sistema opera da seguinte forma:

### Calibração Automática

Ao iniciar, a máquina executa uma rotina de calibração para encontrar os limites dos eixos X e Y.

### Modo de Operação

Após a calibração, a máquina entra em modo de operação normal, pronta para receber comandos.

### Para Salvar uma Posição (Ex: em P1)

1. **Mova a Máquina:** Utilize os botões de seta (físicos ou no painel touch) para posicionar a máquina no local exato que deseja salvar.
2. **Ative o Modo de Programação:** Pressione o botão *Programar* no painel touch. O sistema entrará no modo de programação, e os botões de movimento serão travados.
3. **Salve no Slot Desejado:** Pressione o botão *P1*. A posição atual será armazenada na memória do P1, e o sistema retornará automaticamente ao modo de operação normal.

### Para Ir a uma Posição Salva

1. No modo de operação normal, pressione o botão da posição desejada (ex: *P1*).
2. A máquina se moverá automaticamente para as coordenadas salvas.
3. Ao chegar ao destino, a máquina irá parar e aguardar o próximo comando.

## 6. Conclusão do Projeto

O desenvolvimento do software para o sistema de controle da máquina de Raio-X foi concluído com sucesso, atingindo os principais objetivos propostos no escopo do trabalho. O sistema final implementa um controle de posicionamento PID preciso, uma máquina de estados para gerenciar os modos de operação (manual, programação, automático), e uma interface de comunicação funcional com o painel touch via Modbus.

Entretanto, a fase de testes práticos apresentou desafios significativos relacionados ao hardware. Foi observado que uma parte considerável das placas de desenvolvimento e drivers de motor disponíveis apresentava falhas ou instabilidade, o que exigiu um esforço adicional na validação da plataforma física.

De forma notável, o eixo Y demonstrou um comportamento mecânico/elétrico inconsistente em todas as placas testadas. Mesmo com o software ajustado para compensar (como a inversão da lógica de direção), o eixo continuou apresentando uma resposta mais lenta e instável em comparação com o eixo X.

Apesar dos desafios de hardware, o projeto foi uma excelente experiência prática, permitindo aplicar e validar conceitos complexos de controle, programação concorrente e arquitetura de software em um sistema embarcado realista.

## 7. Autores

- Artur Rodrigues Sousa Alves (211043638)
- Johnny Da Ponte Lopes (190110066)

## Vídeo de Demonstração

[https://youtu.be/-K9s8bjxaFU?feature=shared](https://youtu.be/-K9s8bjxaFU?feature=shared)

---

## Referências

* Protocolo MODBUS‑RTU (Exercício UART-MODBUS)
* Biblioteca WiringPi: [http://wiringpi.com](http://wiringpi.com)
* Sensor BMP280: [https://github.com/BoschSensortec/BMP2-Sensor-API](https://github.com/BoschSensortec/BMP2-Sensor-API)
* Controle PID: [https://pt.wikipedia.org/wiki/Controlador\_proporcional\_integral\_derivativo](https://pt.wikipedia.org/wiki/Controlador_proporcional_integral_derivativo)
