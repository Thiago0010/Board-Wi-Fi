📡 ESP32 Web Utility Dashboard

Dashboard web utilitário para ESP32 (M5Stick C Plus 2), acessível diretamente pelo navegador, sem uso de componentes externos.

O projeto transforma o ESP32 em uma ferramenta de diagnóstico de rede e sistema, ideal para testes rápidos de Wi-Fi, estabilidade e status interno do dispositivo.

🚀 Funcionalidades

🌐 Wi-Fi Analyzer

Scan de redes

RSSI (força do sinal)

Canal

Segurança

Detecção 2.4 GHz / 5 GHz (scan)

📶 Teste de Rede

Latência (ping local)

Status da conexão

🧠 System Monitor

Uptime

Heap livre

Frequência da CPU

Motivo do último reset

IP local

🚦 Indicador de Qualidade Wi-Fi

Verde / Amarelo / Vermelho baseado no RSSI

🖥️ Web Console

Comandos básicos (status, wifi_scan, restart)

📝 Event Logger

Logs simples em memória (Wi-Fi, reboot, eventos)

🔧 Requisitos

ESP32 (testado no M5Stick C Plus 2)

Arduino IDE ou PlatformIO

Nenhum hardware externo

Rede Wi-Fi 2.4 GHz

📁 Estrutura do Projeto

Todo o projeto está em um único arquivo:

ESP32-Dashboard.ino


HTML, CSS e JavaScript estão embutidos no próprio código

Não usa SPIFFS, LittleFS ou arquivos externos

🔐 Acesso ao Dashboard

Senha padrão:

12345678


Após o ESP32 conectar ao Wi-Fi, ele exibirá o IP no Serial Monitor.

Para acessar:

Descubra o IP do ESP32 (ex: 192.168.1.50)

Abra o navegador

Acesse:

http://IP_DO_ESP32


📌 O acesso ao dashboard é feito exclusivamente pelo IP local do ESP32.

▶️ Como usar

Abra o arquivo .ino na Arduino IDE

Configure:

SSID do Wi-Fi

Senha do Wi-Fi

Compile e envie para o ESP32

Abra o Serial Monitor

Copie o IP exibido

Acesse pelo navegador

❌ O que este projeto NÃO faz

❌ Não mede voltagem externa

❌ Não usa sensores

❌ Não usa IR

❌ Não minera criptomoedas

❌ Não depende de hardware adicional
