#include <stdio.h>
#include <string.h>
 
void alertaEntradaInvalida() {
  printf("\n Entrada invalida: era esperado um numero. \n");
  while (getchar() != '\n');
}
 
int exibirMenu() {
  int cmd;
  printf("\n1. Inserir novo correntista em uma posicao especifica."
         "\n2. Buscar correntista pelo codigo da conta."
         "\n3. Modificar o saldo do correntista."
         "\n4. Deletar correntista."
         "\n5. Exibir todos os correntistas."
         "\n6. Reiniciar leitura do arquivo com rewind()."
         "\n7. Sair do sistema.\n");
  if ((scanf("%d%*c", &cmd)) != 1) {
    alertaEntradaInvalida();
  }
  return cmd;
}
 
typedef struct Correntista {
  int    codigo;       
  char   apelido[155]; 
  int    conta;        
  float  balanco;      
} correntista;
 
int main(void) {
  int acao;
  FILE *registros;
  registros = fopen("correntistas.dat", "rb+");
 
  if (registros == NULL) {
    printf("Falha ao acessar o arquivo de dados. \n");
  } else {
    do {
      acao = exibirMenu();
 
      switch (acao) {
      case 1: {
        int slot;
        correntista novoReg;
 
        printf("Informe o slot de cadastro: \n");
        if ((scanf("%d%*c", &slot)) != 1) {
          alertaEntradaInvalida();
          break;
        }
 
        fseek(registros, sizeof(correntista) * slot, SEEK_SET);
        int leitura = fread(&novoReg, sizeof(correntista), 1, registros);
        if (leitura != 0) {
          printf("Slot ja ocupado. Escolha outro. \n");
          break;
        } else {
          printf("Informe o nome do correntista: \n");
          fgets(novoReg.apelido, sizeof(novoReg.apelido), stdin);
          novoReg.apelido[strcspn(novoReg.apelido, "\n")] = '\0';
 
          novoReg.codigo  = slot;
          novoReg.conta   = slot;
          novoReg.balanco = 0;
 
          fseek(registros, sizeof(correntista) * slot, SEEK_SET);
          fwrite(&novoReg, sizeof(correntista), 1, registros);
          break;
        }
      }
 
      case 2: {
        int codBusca;
        correntista dadosBusca;
 
        printf("Informe o codigo da conta para consulta: \n");
        if ((scanf("%d%*c", &codBusca)) != 1) {
          alertaEntradaInvalida();
          break;
        }
 
        fseek(registros, sizeof(correntista) * codBusca, SEEK_SET);
        if (fread(&dadosBusca, sizeof(correntista), 1, registros) == 1 &&
            dadosBusca.codigo != 0) {
          printf("\n Nome: %s \n Balanco: R$%.2f \n Conta: %d. \n",
                 dadosBusca.apelido, dadosBusca.balanco, dadosBusca.conta);
        } else {
          printf("Nenhum registro encontrado para este codigo. \n");
        }
        break;
      }
 
      case 3: {
        float ajuste;
        correntista regAlvo;
        int tipoOp;
        int codAlvo;
 
        printf("Informe o codigo da conta para alterar saldo: \n");
        if ((scanf("%d%*c", &codAlvo)) != 1) {
          alertaEntradaInvalida();
          break;
        }
 
        fseek(registros, sizeof(correntista) * codAlvo, SEEK_SET);
        if (fread(&regAlvo, sizeof(correntista), 1, registros) != 0) {
          printf(" 1 - Creditar \n 2 - Debitar \n");
          if ((scanf("%d%*c", &tipoOp)) != 1) {
            alertaEntradaInvalida();
            break;
          }
 
          if (tipoOp == 1) {
            printf("Valor a creditar: ");
            if ((scanf("%f%*c", &ajuste)) != 1) {
              alertaEntradaInvalida();
              break;
            }
            regAlvo.balanco += ajuste;
            fseek(registros, sizeof(correntista) * codAlvo, SEEK_SET);
            fwrite(&regAlvo, sizeof(correntista), 1, registros);
 
          } else if (tipoOp == 2) {
            printf("Valor a debitar: ");
            if ((scanf("%f%*c", &ajuste)) != 1) {
              alertaEntradaInvalida();
              break;
            }
            regAlvo.balanco -= ajuste;
            fseek(registros, sizeof(correntista) * codAlvo, SEEK_SET);
            fwrite(&regAlvo, sizeof(correntista), 1, registros);
 
          } else {
            printf("Operacao invalida. \n");
          }
        } else {
          printf("Codigo de conta nao localizado. \n");
        }
        break;
      }
 
      case 4: {
        int slotRemover;
        correntista regVazio;
 
        printf("Informe o codigo da conta a ser removida: \n");
        if ((scanf("%d%*c", &slotRemover)) != 1) {
          alertaEntradaInvalida();
          break;
        }
 
        regVazio.codigo      = 0;
        regVazio.apelido[0]  = '\0';
        regVazio.balanco     = 0;
        regVazio.conta       = 0;
 
        fseek(registros, sizeof(correntista) * slotRemover, SEEK_SET);
        fwrite(&regVazio, sizeof(correntista), 1, registros);
        break;
      }
 
      case 5: {
        rewind(registros);
        correntista item;
        while (fread(&item, sizeof(correntista), 1, registros) == 1) {
          if (item.codigo != 0) {
            printf("Correntista %d: %s \n", item.conta, item.apelido);
          }
        }
        break;
      }
 
      case 6: {
        char confirmacao;
        printf("Deseja reiniciar a leitura do arquivo? S/N \n");
        scanf(" %c", &confirmacao);
 
        if (confirmacao == 'S' || confirmacao == 's') {
          rewind(registros);
          printf("Leitura reiniciada com sucesso. \n");
        } else if (confirmacao == 'N' || confirmacao == 'n') {
          printf("Leitura mantida na posicao atual. \n");
        } else {
          printf("Resposta nao reconhecida. \n");
        }
        break;
      }
 
      case 7:
        printf("Sistema encerrado. Ate logo! \n");
        break;
 
      default:
        printf("\n Opcao fora do intervalo valido. \n");
        break;
      }
    } while (acao != 7);
  }
 
  fclose(registros);
  return 0;
}