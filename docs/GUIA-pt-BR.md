# xLogger — guia passo a passo

[English](../README.md) · **Português**

Este guia leva você de um projeto vazio até um firmware com logs de debug que somem
da versão final sem precisar comentar nenhuma linha.

- [1. Instalar o PlatformIO](#1-instalar-o-platformio)
- [2. Criar o projeto](#2-criar-o-projeto)
- [3. Adicionar o xLogger](#3-adicionar-o-xlogger)
- [4. Primeiro log](#4-primeiro-log)
- [5. Níveis de log](#5-níveis-de-log)
- [6. Ambientes de debug e de release](#6-ambientes-de-debug-e-de-release)
- [7. Recursos extras](#7-recursos-extras)
- [8. Migrando da classe `Logger` antiga](#8-migrando-da-classe-logger-antiga)
- [9. Problemas comuns](#9-problemas-comuns)
- [10. Usando com IA (ChatGPT, Claude, Gemini, Copilot…)](#10-usando-com-ia-chatgpt-claude-gemini-copilot)
- [11. Sugestões, dúvidas e bugs](#11-sugestões-dúvidas-e-bugs)
- [12. Para o mantenedor: publicando uma nova versão](#12-para-o-mantenedor-publicando-uma-nova-versão)

---

## 1. Instalar o PlatformIO

1. Instale o [VS Code](https://code.visualstudio.com/).
2. Na aba *Extensions* (`Ctrl+Shift+X`), procure **PlatformIO IDE** e instale.
3. Reinicie o VS Code. Aparece o ícone do PlatformIO (uma cabeça de formiga) na barra lateral.

## 2. Criar o projeto

1. Ícone do PlatformIO → *PIO Home* → **New Project**.
2. Dê um nome, escolha a placa (ex.: *Arduino Uno* ou *Espressif ESP32 Dev Module*)
   e o framework **Arduino**.
3. O PlatformIO cria esta estrutura:

```
meu-projeto/
├── include/
├── lib/
├── src/
│   └── main.cpp
└── platformio.ini
```

## 3. Adicionar o xLogger

Abra o `platformio.ini` e adicione `lib_deps` e `monitor_speed`:

```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/YuriCirino/xlogger.git#v1.1.0
```

O `#v1.1.0` fixa a versão. Sem ele, o PlatformIO baixa o que estiver no branch
principal, e uma atualização da lib pode mudar o comportamento do seu firmware sem você perceber.

Na próxima compilação (✓ na barra inferior), a biblioteca é baixada para `.pio/libdeps/`.

## 4. Primeiro log

Em `src/main.cpp`:

```cpp
#include <Arduino.h>
#include <xLogger.h>

XLogger logger(Serial);

void setup() {
  Serial.begin(115200);
  XLOG_INFO(logger, F("boot ok"));
}

void loop() {
  int adc = analogRead(A0);
  float v = adc * 5.0f / 1023.0f;

  XLOG_DEBUG(logger, F("adc="), adc, F(" v="), v);
  delay(1000);
}
```

Compile e grave (→ na barra inferior), depois abra o monitor serial (🔌):

```
[I] boot ok
[D] adc=512 v=2.50
[D] adc=514 v=2.51
```

Repare que não há `String(...)` nem `+`: cada argumento separado por vírgula é impresso
em sequência. O `F("...")` mantém o texto na memória flash em vez da RAM, o que importa
no Uno (2 KB de RAM).

## 5. Níveis de log

| Macro | Uso típico | Saída |
|---|---|---|
| `XLOG_ERROR` | Algo falhou e o sistema não consegue seguir normalmente | `[E] ...` |
| `XLOG_WARN`  | Situação anormal, mas o sistema continua | `[W] ...` |
| `XLOG_INFO`  | Eventos importantes: boot, conexão, mudança de estado | `[I] ...` |
| `XLOG_DEBUG` | Valores de variáveis, detalhes para depuração | `[D] ...` |
| `XLOG_HERE`  | "Passou por aqui": imprime função e linha | `[H] loop:42` |

O nível do logger define o que aparece. Uma mensagem é impressa se o nível dela for
menor ou igual ao do logger:

```cpp
logger.setLevel(XLogLevel::Warn);   // só [W] e [E]
logger.setLevel(XLogLevel::Debug);  // tudo (padrão)
logger.setLevel(XLogLevel::None);   // nada
```

Isso pode mudar com o firmware rodando, por exemplo por um comando recebido pela serial
(veja o exemplo [`LevelsAndTags`](../examples/LevelsAndTags/LevelsAndTags.ino)).

## 6. Ambientes de debug e de release

Este é o ponto principal da biblioteca. O `setLevel()` só esconde a mensagem: o texto e o
código continuam ocupando flash. Para remover de verdade, defina o nível máximo
**em tempo de compilação** com `build_flags`.

Crie dois ambientes no `platformio.ini`:

```ini
[env]
platform = atmelavr
board = uno
framework = arduino
monitor_speed = 115200
lib_deps =
    https://github.com/YuriCirino/xlogger.git#v1.1.0

[env:debug]
build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_DEBUG

[env:release]
build_flags = -D XLOGGER_MAX_LEVEL=XLOG_LEVEL_WARN
```

A seção `[env]` sem nome vale para todos os ambientes. Agora:

- **Debug:** `pio run -e debug -t upload` — todos os logs.
- **Release:** `pio run -e release -t upload` — só `XLOG_WARN` e `XLOG_ERROR` existem no binário.

No VS Code, escolha o ambiente na barra inferior (botão `env:...`).

Para ver a diferença, compile os dois e compare a linha `Flash:` no fim da saída da compilação.

> ⚠️ **Não coloque efeitos colaterais dentro das macros.** No release, a linha inteira
> desaparece, incluindo os argumentos:
>
> ```cpp
> XLOG_DEBUG(logger, F("lido: "), lerSensor());  // no release, lerSensor() NÃO é chamada
> XLOG_DEBUG(logger, contador++);                // no release, contador NÃO incrementa
> ```
>
> Leia o valor antes, em uma variável, e logue a variável.

Valores aceitos em `XLOGGER_MAX_LEVEL`: `XLOG_LEVEL_NONE` (0), `XLOG_LEVEL_ERROR` (1),
`XLOG_LEVEL_WARN` (2), `XLOG_LEVEL_INFO` (3) e `XLOG_LEVEL_DEBUG` (4, o padrão).

## 7. Recursos extras

**Timestamp:**

```cpp
logger.setTimestamp(true);
// 10234 [I] boot ok
```

**Um logger por módulo, com tag:**

```cpp
XLogger imuLog(Serial);
XLogger wifiLog(Serial);

void setup() {
  imuLog.setTag("IMU");
  wifiLog.setTag("WIFI");
  wifiLog.setLevel(XLogLevel::Warn);   // silencia o Wi-Fi sem afetar o IMU
}
// [D][IMU] ax=0.02 ay=-0.98
// [W][WIFI] rssi baixo: -85
```

**Outra saída:** qualquer objeto `Print` serve.

```cpp
XLogger logger(Serial1);         // UART 1
logger.setOutput(Serial);        // troca em runtime
```

**Evitar cálculo caro quando o nível está desligado:**

```cpp
if (logger.enabled(XLogLevel::Debug)) {
  float rms = calcularRMS(buffer, 512);
  XLOG_DEBUG(logger, F("rms="), rms);
}
```

## 8. Migrando da classe `Logger` antiga

A API antiga continua disponível, então dá para migrar aos poucos:

| Antes | Depois |
|---|---|
| `#include "Logger.h"` | `#include <xLogger.h>` |
| `Logger logger(Serial);` | `XLogger logger(Serial, XLogLevel::None);` (desligado, como antes) |
| `logger.setDebugMode(true);` | `logger.setLevel(XLogLevel::Debug);` (o antigo ainda funciona) |
| `logger.println("x=" + String(x));` | `XLOG_DEBUG(logger, F("x="), x);` |
| `logger.print(...)` / `println(...)` | Continuam funcionando, sem prefixo |
| `logger.setSerial(Serial1);` | `logger.setOutput(Serial1);` |
| `logger.line();` | `XLOG_HERE(logger);` (agora mostra a linha certa) |

## 9. Problemas comuns

**`xLogger.h: No such file or directory`**
O `lib_deps` está no ambiente errado ou com erro de digitação. Confira se ele está no
mesmo `[env:...]` que você está compilando (ou em `[env]`, que vale para todos).

**Nada aparece no monitor serial**
- O `monitor_speed` precisa ser igual ao `Serial.begin(...)`.
- Em placas com USB nativo (Leonardo, ESP32-S3, RP2040), as primeiras mensagens saem
  antes de o PC abrir a porta. Espere com um limite de tempo no `setup()`:
  `while (!Serial && millis() < 3000) {}`. Sem o limite (`while (!Serial) {}`), a placa
  trava para sempre quando roda sem computador conectado.
- Confira se o nível do logger e o `XLOGGER_MAX_LEVEL` permitem a mensagem.

**`call of overloaded 'print(long long ...)' is ambiguous` no AVR**
O core AVR não sabe imprimir `int64_t`/`uint64_t`. Converta para `long` ou imprima em duas partes.

**Mudei `XLOGGER_MAX_LEVEL` com `#define` e não fez efeito**
O `#define` precisa vir **antes** do `#include <xLogger.h>` e só vale para aquele arquivo.
Use `build_flags` no `platformio.ini` para valer no projeto inteiro.

**Mudei `build_flags` e o comportamento não mudou**
Faça um *Clean* (🗑 na barra inferior, ou `pio run -t clean`) e compile de novo.

## 10. Usando com IA (ChatGPT, Claude, Gemini, Copilot…)

Uma IA não conhece o xLogger de antemão. Sem contexto, ela tende a inventar funções ou
voltar para o `Serial.print()` com `String`. Por isso a biblioteca traz uma referência
escrita para IA: regras, API completa, padrões e erros comuns, com todos os exemplos
compilados automaticamente no CI. Há três formas de usar, da mais simples à mais integrada.

### Opção 1 — Anexar um arquivo no chat (funciona em qualquer IA)

1. Baixe o **[llms-full.txt](https://raw.githubusercontent.com/YuriCirino/xlogger/main/llms-full.txt)**
   (botão direito → *Salvar link como…*).
2. Anexe o arquivo na conversa e faça o pedido. Exemplo:

   > Use a biblioteca xLogger, conforme o arquivo anexo, para adicionar logs a este
   > firmware de ESP32. Quero logs de debug nas leituras do sensor e de erro nas falhas de
   > Wi-Fi, e um `platformio.ini` com ambientes de debug e de release.

Se a sua IA consegue abrir links, basta colar a URL do arquivo no lugar de anexar.

### Opção 2 — Agent Skill (Claude, Copilot, Cursor, Codex, Gemini CLI…)

O mesmo conteúdo está empacotado como **Agent Skill**, um formato aberto que vários
assistentes de código carregam sozinhos quando percebem que o projeto usa o xLogger, sem
você precisar anexar nada a cada pedido.

Copie a pasta [`skills/xlogger`](../skills/xlogger) do repositório para a pasta de skills da
sua ferramenta. No Claude Code, por exemplo, ela vai em `.claude/skills/xlogger/` dentro do
seu projeto. Para as outras ferramentas, veja a lista de compatíveis em
[agentskills.io](https://agentskills.io/).

### Opção 3 — O arquivo já está no seu projeto PlatformIO

Quando o PlatformIO baixa a biblioteca, o `llms-full.txt` vem junto, em
`.pio/libdeps/<ambiente>/xLogger/`. Um agente que trabalha dentro do seu projeto pode
lê-lo direto. Basta pedir:

> Antes de mexer nos logs, leia `.pio/libdeps/debug/xLogger/llms-full.txt`.

## 11. Sugestões, dúvidas e bugs

Sua opinião ajuda a melhorar a biblioteca. Pode escrever em português ou em inglês.

- 🐛 **Encontrou um bug?** [Abra um relato de bug](https://github.com/YuriCirino/xlogger/issues/new?template=bug_report.yml).
  Informe a placa, a versão do xLogger, um trecho mínimo de código que reproduza o problema
  e a saída do monitor serial.
- 💡 **Tem uma ideia ou sugestão?** [Sugira um recurso](https://github.com/YuriCirino/xlogger/issues/new?template=feature_request.yml).
- 🔀 **Quer contribuir com código?** Veja o [CONTRIBUTING.md](../CONTRIBUTING.md) e abra um pull request.

Antes de abrir, procure nas [issues existentes](https://github.com/YuriCirino/xlogger/issues)
para ver se alguém já relatou a mesma coisa. Se sim, deixe um 👍 ou um comentário com
informações novas. Todas as interações seguem o [Código de Conduta](../CODE_OF_CONDUCT.md).

Você precisa de uma conta gratuita no GitHub para abrir issues.

## 12. Para o mantenedor: publicando uma nova versão

### A cada nova versão

1. Atualize a versão em **quatro** lugares: `library.json`, `library.properties`,
   `XLOGGER_VERSION` em `src/xLogger.h` e `metadata.version` em `skills/xlogger/SKILL.md`.
   Troque também o `#vX.Y.Z` dos exemplos de `lib_deps` na documentação.
   Confira com `python3 extras/tools/check_version.py`.
2. Se a API mudou, atualize o `SKILL.md` e regenere a referência para IA com
   `python3 extras/tools/ai_docs.py build` (nunca edite o `llms-full.txt` à mão).
3. No `CHANGELOG.md`, transforme `[Unreleased]` em `[X.Y.Z] - data` e atualize os links
   de comparação no fim do arquivo.
4. Commit e push no `main`. Espere o CI ficar verde.
5. No GitHub: *Releases* → **Draft a new release** → *Choose a tag* → digite `vX.Y.Z` →
   *Create new tag on publish*. Título `xLogger X.Y.Z`; na descrição, cole a seção da versão
   do `CHANGELOG.md`. Clique em **Publish release**.

A partir daí é automático:
- o workflow **Publish to PlatformIO Registry** confere se a tag bate com a versão e
  publica no registro do PlatformIO (se o segredo abaixo estiver configurado);
- o **Arduino Library Manager** detecta a tag nova sozinho, em até um dia
  (depois da inscrição inicial, abaixo).

Siga versionamento semântico: `1.0.1` para correções, `1.1.0` para novos recursos
compatíveis, `2.0.0` quando quebrar a API.

### Configuração única: registro do PlatformIO

Permite que usuários instalem com `lib_deps = <seu-usuario-platformio>/xLogger@^1.1.0`
e que a lib apareça na busca do PlatformIO.

1. Crie uma conta em [platformio.org](https://platformio.org/).
2. No seu computador:

   ```bash
   pip install platformio
   pio account login
   pio account token          # copie o token exibido
   ```

3. No GitHub: *Settings* → *Secrets and variables* → *Actions* → **New repository secret**.
   Nome `PLATFORMIO_AUTH_TOKEN`, valor: o token copiado.
4. Para publicar uma versão que já existe (sem criar release nova): *Actions* →
   *Publish to PlatformIO Registry* → **Run workflow**.

O token dá acesso à sua conta PlatformIO: nunca o coloque em arquivos do repositório.

### Configuração única: Arduino Library Manager

1. Abra [repositories.txt no arduino/library-registry](https://github.com/arduino/library-registry/edit/main/repositories.txt).
2. Adicione no fim uma linha com `https://github.com/YuriCirino/xlogger`.
3. **Propose changes** → **Create pull request**.
4. Um bot valida a biblioteca (é o mesmo `arduino-lint` que roda no nosso CI) e aceita
   a inclusão automaticamente se estiver tudo certo. Se ele comentar algum problema,
   corrija no repositório e responda no pull request.
