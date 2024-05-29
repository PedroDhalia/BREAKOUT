#include "keyboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#define LARGURA_TELA 40
#define ALTURA_TELA 20
#define NUM_BLOCOS_LINHA 10
#define NUM_BLOCOS_COLUNA 2
#define NUM_BLOCOS (NUM_BLOCOS_LINHA * NUM_BLOCOS_COLUNA)
#define NUM_BLOCOS_DIFICIL 20

struct Paleta {
    int x, y;
};

struct Bola {
    int x, y;
    int dx, dy;
};

struct Jogador {
    char nome[50];
    int pontuacao;
};

struct Bloco {
    int x, y;
    int quebrado;
    int interativo;
};

char tela[ALTURA_TELA][LARGURA_TELA];

void inicializarBlocos(struct Bloco blocos[], int quantidadeTijolos) {
    for (int i = 0; i < NUM_BLOCOS; i++) {
        blocos[i].quebrado = 0;
        blocos[i].interativo = 1;
    }

    int numFileiras = quantidadeTijolos / NUM_BLOCOS_LINHA;
    int larguraBloco = LARGURA_TELA / NUM_BLOCOS_LINHA;
    int alturaBloco = 1;
    int espacamentoVertical = 2;

    for (int i = 0; i < numFileiras; i++) {
        for (int j = 0; j < NUM_BLOCOS_LINHA; j++) {
            int indiceBloco = i * NUM_BLOCOS_LINHA + j;

            blocos[indiceBloco].x = j * larguraBloco;
            blocos[indiceBloco].y = i * espacamentoVertical;
            blocos[indiceBloco].quebrado = 0;
        }
    }
}

void limparTela() {
    printf("\033[H\033[J");
}

void atualizarTela(struct Paleta *paleta, struct Bola *bola, struct Bloco blocos[]) {
    limparTela();

    int larguraBloco = LARGURA_TELA / NUM_BLOCOS_LINHA;
    char linha[LARGURA_TELA + 1];

    for (int i = 0; i < ALTURA_TELA; i++) {
        memset(linha, ' ', LARGURA_TELA);
        linha[LARGURA_TELA] = '\0';

        if (i == paleta->y) {
            for (int j = paleta->x; j < paleta->x + 5 && j < LARGURA_TELA; j++) {
                linha[j] = '-';
            }
        } else if (i == bola->y) {
            linha[bola->x] = 'O';
        } else if (i < ALTURA_TELA / 2) {
            int colunaBloco = i / 2;
            int indiceBloco = colunaBloco * NUM_BLOCOS_LINHA;

            for (int j = 0; j < NUM_BLOCOS_LINHA; j++) {
                if (!blocos[indiceBloco + j].quebrado && blocos[indiceBloco + j].interativo) {
                    int posInicial = j * larguraBloco;
                    int posFinal = (j + 1) * larguraBloco - 1;

                    for (int k = posInicial; k <= posFinal && k < LARGURA_TELA; k++) {
                        linha[k] = '#';
                    }
                }
            }
        }

        printf("%s\n", linha);
    }

    for (int i = 0; i < LARGURA_TELA; i++) {
        printf("-");
    }
    printf("\n");

    fflush(stdout);
}

int jogarJogo(int velocidade) {
    struct Paleta paleta = {LARGURA_TELA / 2 - 2, ALTURA_TELA - 2};
    struct Bola bola = {LARGURA_TELA / 2, ALTURA_TELA / 2, 1, -1};
    struct Bloco blocos[NUM_BLOCOS];

    inicializarBlocos(blocos, NUM_BLOCOS_DIFICIL);
    int larguraBloco = LARGURA_TELA / NUM_BLOCOS_LINHA;
        keyboardInit();

        int blocosQuebrados = 0;
        int jogoAtivo = 1;

        while (jogoAtivo) {
            if (keyhit()) {
                char tecla = readch();
                if ((tecla == 'a' || tecla == 'A') && paleta.x > 0) {
                    paleta.x--;
                }
                if ((tecla == 'd' || tecla == 'D') && paleta.x < LARGURA_TELA - 5) {
                    paleta.x++;
                }
            }
            bola.x += bola.dx;
            bola.y += bola.dy;

            if (bola.x <= 0 || bola.x >= LARGURA_TELA - 1) {
                bola.dx = -bola.dx;
            }

            if (bola.y <= 0) {
                bola.dy = -bola.dy;
            }

            for (int i = 0; i < NUM_BLOCOS; i++) {
                if (!blocos[i].quebrado &&
                    bola.y >= blocos[i].y && bola.y <= blocos[i].y + 1 &&
                    bola.x >= blocos[i].x && bola.x <= blocos[i].x + larguraBloco) {
                    blocos[i].quebrado = 1;
                    bola.dy = -bola.dy;
                    blocosQuebrados++;
                    break;
                }
            }

            if (bola.y == paleta.y && bola.x >= paleta.x && bola.x < paleta.x + 5) {
                bola.dy = -bola.dy;
            }

            atualizarTela(&paleta, &bola, blocos);

            if (bola.y >= ALTURA_TELA - 1) {
                jogoAtivo = 0;
            }

            usleep(velocidade); // Ajustando a velocidade da bola
        }

        keyboardDestroy();
        return blocosQuebrados;
    }

    void exibirMenu() {
        limparTela();
        printf("===================================\n");
        printf("           BREAKOUT GAME\n");
        printf("===================================\n");
        printf("1. Iniciar Jogo\n");
        printf("2. Ver Ranking\n");
        printf("3. Sair\n");
        printf("===================================\n");
        printf("Escolha uma opcao: ");
        fflush(stdout);
    }

    int obterEscolhaMenu() {
        keyboardInit();
        int escolha = 0;
        while (!escolha) {
            if (keyhit()) {
                char tecla = readch();
                if (tecla >= '1' && tecla <= '3') {
                    escolha = tecla - '0';
                }
            }
            usleep(10000);
        }
        keyboardDestroy();
        return escolha;
    }

    void digitarNome(char nome[]) {
        printf("Digite o seu nome: ");
        fflush(stdout);

        int i = 0;
        while (1) {
            if (keyhit()) {
                char tecla = readch();
                if (tecla == '\n') {
                    break;
                } else if (tecla == 127) {
                    if (i > 0) {
                        i--;
                        nome[i] = '\0';
                        printf("\b \b");
                        fflush(stdout);
                    }
                } else if (i < 49) {
                    nome[i] = tecla;
                    i++;
                    nome[i] = '\0';
                    printf("%c", tecla);
                    fflush(stdout);
                }
            }
            usleep(10000);
        }
    }

    void exibirRanking(struct Jogador *jogadores, int numJogadores) {
        limparTela();
        printf("===================================\n");
        printf("           RANKING\n");
        printf("===================================\n");
        for (int i = 0; i < numJogadores; i++) {
            printf("%d. %s: %d\n", i + 1, jogadores[i].nome, jogadores[i].pontuacao);
        }
        printf("===================================\n");
        printf("Pressione Enter para voltar ao menu...");
        fflush(stdout);

        while (1) {
            if (keyhit()) {
                char tecla = readch();
                if (tecla == '\n') {
                    break;
                }
            }
            usleep(10000);
        }
    }

    int main() {
        srand(time(NULL));
        struct Jogador *jogadores = malloc(50 * sizeof(struct Jogador));
        if (jogadores == NULL) {
            fprintf(stderr, "Erro ao alocar memória.\n");
            return 1;
        }
        int numJogadores = 0;
        int velocidadeInicial = 150000; // Velocidade inicial da bola

        keyboardInit();

        while (1) {
            exibirMenu();
            int escolha = obterEscolhaMenu();

            switch (escolha) {
                case 1: {
                    char nome[50];
                    digitarNome(nome);

                    limparTela();
                    printf("Preparando o jogo...");
                    fflush(stdout);
                    sleep(3);

                    strcpy(jogadores[numJogadores].nome, nome);
                    jogadores[numJogadores].pontuacao = jogarJogo(velocidadeInicial); // Passando a velocidade inicial
                    numJogadores++;

                    break;
                }
                case 2: {
                    exibirRanking(jogadores, numJogadores);
                    break;
                }
                case 3:
                    printf("\nJogo finalizado. Obrigado por jogar!\n");
                    free(jogadores);
                    keyboardDestroy();
                    exit(0);
            }
        }

        keyboardDestroy();
        free(jogadores);
        return 0;
    }

