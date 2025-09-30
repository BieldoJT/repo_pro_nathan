# get_next_line

## Descrição

O **get_next_line** é um projeto que tem como objetivo criar uma função capaz de ler uma linha de um descritor de arquivo por vez, independentemente do tamanho do buffer ou do arquivo.

## Funcionalidades

- Lê uma linha por chamada da função.
- Funciona com qualquer descritor de arquivo (stdin, arquivos, sockets, etc.).
- Gerencia corretamente a memória para evitar vazamentos.
- Suporta qualquer tamanho de **BUFFER_SIZE**.

## Como usar

1. Clone este repositório:
   ```sh
   git clone https://github.com/seu-usuario/get_next_line.git
   cd get_next_line
   ```

2. Compile a biblioteca:
   ```sh
   gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c -o gnl
   ```

3. Teste a função em um arquivo:
   ```sh
   ./gnl < arquivo.txt
   ```

## Estrutura do Projeto

- `get_next_line.c` - Contém a implementação principal da função.
- `get_next_line_utils.c` - Funções auxiliares (manipulação de strings, alocação de memória, etc.).
- `get_next_line.h` - Arquivo de cabeçalho com as definições e protótipos das funções.

## Regras e Restrições

- O código deve seguir as normas da **42 School**.
- Nenhuma função proibida pode ser utilizada (malloc, free, read, e funções de manipulação de strings são permitidas).
- O uso de variáveis globais não é permitido.

## Exemplo de Uso

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int fd = open("arquivo.txt", O_RDONLY);
    char *linha;

    while ((linha = get_next_line(fd)))
    {
        printf("%s", linha);
        free(linha);
    }
    close(fd);
    return (0);
}
```

## Autores
- [Nathan-N7](https://github.com/Nathan-N7)
