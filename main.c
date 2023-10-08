#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Estrutura para armazenar informações do artista
typedef struct {
    char nome[100];
    char genero[100];
    char local[100];
    char **albuns;
    int num_albuns;
} Artista;

// Estrutura para armazenar uma lista de artistas
typedef struct {
    Artista *artistas;
    int num_artistas;
} ListaArtistas;

// Função para ler os registros do arquivo e criar a lista de artistas
void lerArquivo(ListaArtistas *lista) {
    FILE *arquivo = fopen("artistas.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    char linha[1000];
    char *token;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strcmp(linha, "==========\n") == 0) {
            continue; // Ignora linhas de separação
        }

        // Aloca memória para um novo artista na lista
        lista->artistas = realloc(lista->artistas, (lista->num_artistas + 1) * sizeof(Artista));
        Artista *artista = &lista->artistas[lista->num_artistas];

        // Lê nome, gênero e local
        strcpy(artista->nome, linha);
        fgets(artista->genero, sizeof(artista->genero), arquivo);
        fgets(artista->local, sizeof(artista->local), arquivo);

        // Remove caracteres de nova linha
        artista->nome[strlen(artista->nome) - 1] = '\0';
        artista->genero[strlen(artista->genero) - 1] = '\0';
        artista->local[strlen(artista->local) - 1] = '\0';

        // Lê os álbuns
        artista->num_albuns = 0;
        artista->albuns = NULL;

        while (fgets(linha, sizeof(linha), arquivo)) {
            if (strcmp(linha, "==========\n") == 0) {
                break; // Fim do registro do artista
            }

            // Aloca memória para o álbum
            artista->albuns = realloc(artista->albuns, (artista->num_albuns + 1) * sizeof(char *));
            artista->albuns[artista->num_albuns] = strdup(linha);
            artista->albuns[artista->num_albuns][strlen(artista->albuns[artista->num_albuns]) - 1] = '\0';
            artista->num_albuns++;
        }

        lista->num_artistas++;
    }

    fclose(arquivo);
}

// Função para inserção ordenada de um novo artista na lista e no arquivo
void inserirArtista(ListaArtistas *lista) {
    Artista novo;
    printf("Nome do novo artista: ");
    fgets(novo.nome, sizeof(novo.nome), stdin);
    printf("Gênero do novo artista: ");
    fgets(novo.genero, sizeof(novo.genero), stdin);
    printf("Local de origem do novo artista: ");
    fgets(novo.local, sizeof(novo.local), stdin);

    novo.nome[strlen(novo.nome) - 1] = '\0';
    novo.genero[strlen(novo.genero) - 1] = '\0';
    novo.local[strlen(novo.local) - 1] = '\0';

    // Lê os álbuns do novo artista
    printf("Número de álbuns: ");
    scanf("%d", &novo.num_albuns);
    getchar(); // Consumir a nova linha

    novo.albuns = malloc(novo.num_albuns * sizeof(char *));
    for (int i = 0; i < novo.num_albuns; i++) {
        printf("Álbum %d: ", i + 1);
        novo.albuns[i] = malloc(100);
        fgets(novo.albuns[i], 100, stdin);
        novo.albuns[i][strlen(novo.albuns[i]) - 1] = '\0';
    }

    // Inserir o novo artista na lista
    lista->artistas = realloc(lista->artistas, (lista->num_artistas + 1) * sizeof(Artista));
    lista->artistas[lista->num_artistas] = novo;
    lista->num_artistas++;

    // Abrir o arquivo para adicionar o novo registro
    FILE *arquivo = fopen("artistas.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fprintf(arquivo, "%s\n%s\n%s\n", novo.nome, novo.genero, novo.local);
    for (int i = 0; i < novo.num_albuns; i++) {
        fprintf(arquivo, "%s\n", novo.albuns[i]);
    }
    fprintf(arquivo, "==========\n");

    fclose(arquivo);
}

// Função para remover um artista da lista e do arquivo
void removerArtista(ListaArtistas *lista, const char *nome) {
    int indice = -1;
    for (int i = 0; i < lista->num_artistas; i++) {
        if (strcmp(lista->artistas[i].nome, nome) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Artista não encontrado.\n");
        return;
    }

    // Liberar memória dos álbuns
    for (int i = 0; i < lista->artistas[indice].num_albuns; i++) {
        free(lista->artistas[indice].albuns[i]);
    }
    free(lista->artistas[indice].albuns);

    // Remover o artista da lista
    for (int i = indice; i < lista->num_artistas - 1; i++) {
        lista->artistas[i] = lista->artistas[i + 1];
    }
    lista->num_artistas--;

    // Abrir o arquivo para reescrever os registros
    FILE *arquivo = fopen("artistas.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < lista->num_artistas; i++) {
        fprintf(arquivo, "%s\n%s\n%s\n", lista->artistas[i].nome, lista->artistas[i].genero, lista->artistas[i].local);
        for (int j = 0; j < lista->artistas[i].num_albuns; j++) {
            fprintf(arquivo, "%s\n", lista->artistas[i].albuns[j]);
        }
        fprintf(arquivo, "==========\n");
    }

    fclose(arquivo);
    printf("Artista removido com sucesso.\n");
}

// Função para buscar um artista pelo nome usando busca binária
void buscarArtistaBinaria(ListaArtistas *lista, const char *nome) {
    int inicio = 0;
    int fim = lista->num_artistas - 1;
    int meio;

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        int comparacao = strcmp(lista->artistas[meio].nome, nome);

        if (comparacao == 0) {
            printf("Artista encontrado:\n");
            printf("Nome: %s\n", lista->artistas[meio].nome);
            printf("Gênero: %s\n", lista->artistas[meio].genero);
            printf("Local: %s\n", lista->artistas[meio].local);
            printf("Álbuns:\n");
            for (int i = 0; i < lista->artistas[meio].num_albuns; i++) {
                printf("%s\n", lista->artistas[meio].albuns[i]);
            }
            return;
        } else if (comparacao < 0) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    printf("Artista não encontrado.\n");
}

// Função para buscar um álbum de um artista usando busca sequencial
void buscarAlbumSequencial(ListaArtistas *lista, const char *album) {
    for (int i = 0; i < lista->num_artistas; i++) {
        for (int j = 0; j < lista->artistas[i].num_albuns; j++) {
            if (strstr(lista->artistas[i].albuns[j], album) != NULL) {
                printf("Artista: %s\n", lista->artistas[i].nome);
                printf("Álbum: %s\n", lista->artistas[i].albuns[j]);
            }
        }
    }
}

// Função para liberar a memória alocada para a lista de artistas
void liberarLista(ListaArtistas *lista) {
    for (int i = 0; i < lista->num_artistas; i++) {
        for (int j = 0; j < lista->artistas[i].num_albuns; j++) {
            free(lista->artistas[i].albuns[j]);
        }
        free(lista->artistas[i].albuns);
    }
    free(lista->artistas);
}

void criarMenu() {
	printf("\nMenu:\n");
    printf("1. Inserir novo artista\n");
    printf("2. Remover artista\n");
    printf("3. Buscar artista por nome\n");
    printf("4. Buscar álbum por nome\n");
	printf("5. Editar\n");
    printf("6. Sair\n");
    printf("Escolha uma opção: ");
}

// Função para editar as informações de um artista
void editarArtista(ListaArtistas *lista, const char *nome) {
    int indice = -1;
    for (int i = 0; i < lista->num_artistas; i++) {
        if (strcmp(lista->artistas[i].nome, nome) == 0) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("Artista não encontrado.\n");
        return;
    }

    Artista *artista = &lista->artistas[indice];

    // Editar informações do artista
    printf("Novo nome do artista: ");
    fgets(artista->nome, sizeof(artista->nome), stdin);
    artista->nome[strlen(artista->nome) - 1] = '\0';

    printf("Novo gênero do artista: ");
    fgets(artista->genero, sizeof(artista->genero), stdin);
    artista->genero[strlen(artista->genero) - 1] = '\0';

    printf("Novo local de origem do artista: ");
    fgets(artista->local, sizeof(artista->local), stdin);
    artista->local[strlen(artista->local) - 1] = '\0';

    // Editar os álbuns
    printf("Número de álbuns: ");
    scanf("%d", &artista->num_albuns);
    getchar(); // Consumir a nova linha

    artista->albuns = realloc(artista->albuns, artista->num_albuns * sizeof(char *));
    for (int i = 0; i < artista->num_albuns; i++) {
        printf("Novo nome do álbum %d: ", i + 1);
        fgets(artista->albuns[i], 100, stdin);
        artista->albuns[i][strlen(artista->albuns[i]) - 1] = '\0';
    }

    // Abrir o arquivo para reescrever os registros
    FILE *arquivo = fopen("artistas.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    for (int i = 0; i < lista->num_artistas; i++) {
        fprintf(arquivo, "%s\n%s\n%s\n", lista->artistas[i].nome, lista->artistas[i].genero, lista->artistas[i].local);
        for (int j = 0; j < lista->artistas[i].num_albuns; j++) {
            fprintf(arquivo, "%s\n", lista->artistas[i].albuns[j]);
        }
        fprintf(arquivo, "==========\n");
    }

    fclose(arquivo);
    printf("Artista editado com sucesso.\n");
}

int main() {
	setlocale(LC_ALL,"");
    ListaArtistas lista = {NULL, 0};
    lerArquivo(&lista);

    int escolha;
    char nome[100];
    char album[100];

    do {
        criarMenu();
        scanf("%d", &escolha);
        getchar(); // Consumir a nova linha

        switch (escolha) {
            case 1:
                inserirArtista(&lista);
                break;
            case 2:
                printf("Nome do artista a ser removido: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strlen(nome) - 1] = '\0';
                removerArtista(&lista, nome);
                break;
            case 3:
                printf("Nome do artista a ser buscado: ");
                fgets(nome, sizeof(nome), stdin);
                nome[strlen(nome) - 1] = '\0';
                buscarArtistaBinaria(&lista, nome);
                break;
            case 4:
                printf("Nome do álbum a ser buscado: ");
                fgets(album, sizeof(album), stdin);
                album[strlen(album) - 1] = '\0';
                buscarAlbumSequencial(&lista, album);
                break;
            case 5:
            	printf("Nome do artista a ser editado: ");
    			fgets(nome, sizeof(nome), stdin);
    			nome[strlen(nome) - 1] = '\0';
    			editarArtista(&lista, nome);
                break;
            case 6:
    			printf("Saindo...\n");
    			break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    } while (escolha != 6);

    liberarLista(&lista);
    return 0;
}