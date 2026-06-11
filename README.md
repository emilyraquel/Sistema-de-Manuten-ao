# Sistema de Manutenção de Contas Bancárias em C

Exercício de manipulação de arquivos binários com registros de tamanho fixo em linguagem C, usando acesso direto via `fseek()`, leitura/escrita com `fread()`/`fwrite()` e releitura com `rewind()`.

---

## Estrutura do registro

Cada correntista é representado pela struct abaixo, com **tamanho fixo de 167 bytes**:

```c
typedef struct Correntista {
    int   codigo;       // 4 bytes  — identificador / posição no arquivo
    char  apelido[155]; // 155 bytes — nome do titular
    int   conta;        // 4 bytes  — número da conta
    float balanco;      // 4 bytes  — saldo atual
} correntista;
```

O tamanho fixo é o que permite calcular o offset de qualquer registro diretamente:

```
offset = posição × sizeof(correntista)
```

Isso elimina a necessidade de percorrer o arquivo inteiro para encontrar um registro.

---

## Arquivo binário

O arquivo `correntistas.dat` é aberto no modo `rb+` (leitura e escrita binária sem truncar):

```c
registros = fopen("correntistas.dat", "rb+");
```

Todos os dados são lidos e gravados como blocos de bytes brutos, sem conversão de texto, o que garante que o tamanho de cada registro seja sempre o mesmo independente do conteúdo.

---

## Funções de arquivo utilizadas

| Função | Onde é usada | Finalidade |
|---|---|---|
| `fseek()` | Casos 1, 2, 3 e 4 | Posiciona o ponteiro no offset do registro desejado |
| `fread()` | Casos 1, 2, 3 e 5 | Lê um registro completo da posição atual |
| `fwrite()` | Casos 1, 3 e 4 | Grava um registro completo na posição atual |
| `rewind()` | Casos 5 e 6 | Reposiciona o ponteiro para o byte 0 do arquivo |

---

## Opções do menu

### 1. Inserir correntista em slot específico

O usuário informa o número do slot (posição). O programa usa `fseek()` para ir até aquela posição e tenta ler com `fread()`. Se a leitura retornar dados (slot ocupado), informa que a posição já está preenchida. Caso contrário, coleta o nome e grava o novo registro com `fwrite()`.

```c
fseek(registros, sizeof(correntista) * slot, SEEK_SET);
int leitura = fread(&novoReg, sizeof(correntista), 1, registros);
```

### 2. Buscar correntista pelo código da conta

Usa `fseek()` para ir diretamente ao offset do código informado e lê o registro com `fread()`. Se o campo `codigo` for diferente de zero, exibe nome, saldo e número da conta. Caso contrário, informa que nenhum registro foi encontrado.

### 3. Modificar saldo (crédito ou débito)

Localiza o registro com `fseek()` + `fread()`, pergunta se a operação é crédito ou débito, ajusta o campo `balanco` e regrava na mesma posição com `fseek()` + `fwrite()`.

### 4. Deletar correntista

Cria um registro em branco (todos os campos zerados/nulos) e o grava na posição do registro a remover. Isso sobrescreve os dados sem reduzir o tamanho do arquivo, mantendo a integridade dos offsets dos demais registros.

```c
regVazio.codigo     = 0;
regVazio.apelido[0] = '\0';
regVazio.balanco    = 0;
regVazio.conta      = 0;

fseek(registros, sizeof(correntista) * slotRemover, SEEK_SET);
fwrite(&regVazio, sizeof(correntista), 1, registros);
```

### 5. Listar todos os correntistas

Chama `rewind()` para garantir que a leitura começa do início do arquivo e percorre todos os registros sequencialmente com `fread()` em loop. Exibe apenas os registros cujo campo `codigo` seja diferente de zero (ou seja, não deletados).

### 6. Reiniciar leitura com `rewind()`

Reposiciona explicitamente o ponteiro de leitura para o byte 0 do arquivo. Isso permite repetir a listagem do início sem reabrir o arquivo.

```c
rewind(registros);
```

### 7. Sair

Encerra o laço `do...while`, fecha o arquivo com `fclose()` e finaliza o programa.

---

## Tratamento de entrada inválida

A função `alertaEntradaInvalida()` é chamada sempre que `scanf()` não consegue converter a entrada para o tipo esperado. Ela exibe uma mensagem de erro e limpa o buffer de entrada com um loop `while (getchar() != '\n')`, evitando que caracteres inválidos sejam reaproveitados na próxima leitura.

---

## Como compilar e executar

```bash
# Compilar
gcc -o banco banco.c

# Criar o arquivo binário vazio antes de usar (necessário para rb+)
touch correntistas.dat

# Executar
./banco
```

> **Observação:** o modo `rb+` exige que o arquivo já exista. Se `correntistas.dat` não existir, `fopen()` retornará `NULL` e o programa exibirá uma mensagem de erro. Crie o arquivo vazio com `touch` antes da primeira execução.

---

## Arquivos do projeto

```
.
├── banco.c              # Código-fonte principal
├── correntistas.dat     # Arquivo binário de dados (gerado em execução)
└── README.md            # Este arquivo
```
