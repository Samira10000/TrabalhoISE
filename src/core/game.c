#include "game.h"
#include "hardware.h"
#include "player.h"
#include "enemy.h"
#include "projectiles.h"
#include "level.h"
#include "tela_inicio.h"

int camera_x = 0;
int nivel_largura = 1500;
EstadoJogo estado = ESTADO_JOGANDO;
int menu_opcao_selecionada = 0;

void inicializa_jogo(void) {
    inicializa_jogador();
    inicializa_inimigos();
    inicializa_projeteis();
    inicializa_gaps();
    inicializa_lavas();
    inicializa_plataformas();
    inicializa_tokens();
    tokens_coletados = 0;
    camera_x = 0;
    estado = ESTADO_JOGANDO;
}

int colide(int ax, int ay, int aw, int ah, int bx, int by, int bw, int bh) {
    if (ax + aw <= bx) return 0;
    if (ax >= bx + bw) return 0;
    if (ay + ah <= by) return 0;
    if (ay >= by + bh) return 0;
    return 1;
}

int esta_sobre_gap(int x, int largura) {
    int i;
    for (i = 0; i < MAX_GAPS; i++) {
        int centro = x + largura / 2;
        if (centro >= gaps[i].x && centro < gaps[i].x + gaps[i].largura) {
            return 1;
        }
    }
    return 0;
}

int esta_sobre_lava(int x, int largura) {
    int i;
    for (i = 0; i < MAX_LAVAS; i++) {
        int centro = x + largura / 2;
        if (centro >= lavas[i].x && centro < lavas[i].x + lavas[i].largura) {
            return 1;
        }
    }
    return 0;
}

void desenha_tela_inicio(void) {
    int x, y;
    for (y = 0; y < SCREEN_HEIGHT; y++) {
        for (x = 0; x < SCREEN_WIDTH; x++) {
            desenha_pixel(x, y, tela_inicio[y * SCREEN_WIDTH + x]);
        }
    }
}

void desenha_tela_gameover(void) {
    limpa_tela(COLOR_BLACK);
    desenha_retangulo(60, 90, 200, 60, 0x4000);
    desenha_retangulo(70, 100, 180, 40, COLOR_BLACK);
    int i;
    for (i = 0; i < 30; i++) {
        desenha_pixel(145 + i, 105 + i, 0xF800);
        desenha_pixel(145 + i, 135 - i, 0xF800);
    }
}

void desenha_tela_vitoria(void) {
    limpa_tela(COLOR_SKY);
    desenha_retangulo(60, 90, 200, 60, COLOR_TREASURE);
    desenha_retangulo(70, 100, 180, 40, COLOR_SKY);
    desenha_retangulo(150, 110, 20, 20, COLOR_TREASURE);
}

void desenha_tela_pausa(void) {
    int x, y;

    desenha_cenario();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();

    for (y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (x = 0; x < SCREEN_WIDTH; x += 2) {
            desenha_pixel(x, y, COLOR_BLACK);
        }
    }

    desenha_retangulo(70, 70, 180, 100, COLOR_STONE_DARK);
    desenha_retangulo(78, 78, 164, 84, COLOR_BLACK);

    desenha_retangulo(140, 90, 12, 30, COLOR_WHITE);
    desenha_retangulo(160, 90, 12, 30, COLOR_WHITE);

    desenha_retangulo(90, 130, 140, 4, COLOR_TREASURE);
    desenha_retangulo(90, 142, 100, 4, COLOR_ENEMY_EYE);
}

void desenha_engrenagem(int cx, int cy, short int cor) {
    /* Engrenagem pixelart 12x12 centrada em (cx, cy) */
    int ox = cx - 6;
    int oy = cy - 6;
    /* Circulo central */
    desenha_retangulo(ox + 4, oy + 2, 4, 8, cor);
    desenha_retangulo(ox + 2, oy + 4, 8, 4, cor);
    desenha_retangulo(ox + 3, oy + 3, 6, 6, cor);
    /* Buraco central */
    desenha_retangulo(ox + 5, oy + 5, 2, 2, COLOR_BLACK);
    /* Dentes da engrenagem */
    desenha_retangulo(ox + 5, oy + 0, 2, 2, cor); /* topo */
    desenha_retangulo(ox + 5, oy + 10, 2, 2, cor); /* baixo */
    desenha_retangulo(ox + 0, oy + 5, 2, 2, cor); /* esquerda */
    desenha_retangulo(ox + 10, oy + 5, 2, 2, cor); /* direita */
    desenha_retangulo(ox + 1, oy + 1, 2, 2, cor); /* diag sup-esq */
    desenha_retangulo(ox + 9, oy + 1, 2, 2, cor); /* diag sup-dir */
    desenha_retangulo(ox + 1, oy + 9, 2, 2, cor); /* diag inf-esq */
    desenha_retangulo(ox + 9, oy + 9, 2, 2, cor); /* diag inf-dir */
}

void desenha_letra(int x, int y, char c, short int cor) {
    /* Letras pixelart 5x5 para o menu - simplificado */
    /* Cada letra eh desenhada com retangulos */
    switch (c) {
        case 'C':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'O':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'N':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_pixel(x+1, y+1, cor);
            desenha_pixel(x+2, y+2, cor);
            desenha_pixel(x+3, y+3, cor);
            break;
        case 'F':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y+2, 4, 1, cor);
            break;
        case 'I':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x+2, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'G':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            desenha_retangulo(x+4, y+2, 1, 3, cor);
            desenha_retangulo(x+2, y+2, 3, 1, cor);
            break;
        case 'T':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x+2, y, 1, 5, cor);
            break;
        case 'R':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y, 4, 1, cor);
            desenha_retangulo(x+4, y, 1, 3, cor);
            desenha_retangulo(x, y+2, 5, 1, cor);
            desenha_pixel(x+3, y+3, cor);
            desenha_pixel(x+4, y+4, cor);
            break;
        case 'A':
            desenha_retangulo(x+1, y, 3, 1, cor);
            desenha_retangulo(x, y+1, 1, 4, cor);
            desenha_retangulo(x+4, y+1, 1, 4, cor);
            desenha_retangulo(x, y+2, 5, 1, cor);
            break;
        case 'E':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y+2, 4, 1, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'S':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 3, cor);
            desenha_retangulo(x, y+2, 5, 1, cor);
            desenha_retangulo(x+4, y+2, 1, 3, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'L':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'U':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        case 'D':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y, 4, 1, cor);
            desenha_retangulo(x+4, y+1, 1, 3, cor);
            desenha_retangulo(x, y+4, 4, 1, cor);
            break;
        case 'P':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x+4, y, 1, 3, cor);
            desenha_retangulo(x, y+2, 5, 1, cor);
            break;
        case 'W':
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_pixel(x+2, y+3, cor);
            desenha_pixel(x+1, y+4, cor);
            desenha_pixel(x+3, y+4, cor);
            break;
        case 'J':
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            desenha_pixel(x, y+3, cor);
            break;
        case 'V':
            desenha_retangulo(x, y, 1, 4, cor);
            desenha_retangulo(x+4, y, 1, 4, cor);
            desenha_pixel(x+1, y+3, cor);
            desenha_pixel(x+3, y+3, cor);
            desenha_pixel(x+2, y+4, cor);
            break;
        case '/':
            desenha_pixel(x+4, y, cor);
            desenha_pixel(x+3, y+1, cor);
            desenha_pixel(x+2, y+2, cor);
            desenha_pixel(x+1, y+3, cor);
            desenha_pixel(x, y+4, cor);
            break;
        case '=':
            desenha_retangulo(x, y+1, 5, 1, cor);
            desenha_retangulo(x, y+3, 5, 1, cor);
            break;
        case '0':
            desenha_retangulo(x, y, 5, 1, cor);
            desenha_retangulo(x, y, 1, 5, cor);
            desenha_retangulo(x+4, y, 1, 5, cor);
            desenha_retangulo(x, y+4, 5, 1, cor);
            break;
        default:
            break;
    }
}

void desenha_texto(int x, int y, const char *texto, short int cor) {
    int i = 0;
    while (texto[i] != '\0') {
        if (texto[i] != ' ') {
            desenha_letra(x, y, texto[i], cor);
        }
        x += 6;
        i++;
    }
}

void desenha_tela_configuracao(void) {
    int x, y, i;
    const char *opcoes[3];
    opcoes[0] = "CONTINUAR";
    opcoes[1] = "REINICIAR";
    opcoes[2] = "CONTROLES";

    /* Desenha o cenario congelado ao fundo */
    desenha_cenario();
    desenha_plataformas();
    desenha_tokens();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();

    /* Overlay escuro (padrao xadrez) */
    for (y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (x = 0; x < SCREEN_WIDTH; x += 2) {
            desenha_pixel(x, y, COLOR_BLACK);
        }
    }

    /* Caixa do menu */
    desenha_retangulo(60, 50, 200, 140, COLOR_STONE_DARK);
    desenha_retangulo(62, 52, 196, 136, COLOR_BLACK);
    /* Borda dourada */
    desenha_retangulo(60, 50, 200, 2, COLOR_TREASURE);
    desenha_retangulo(60, 188, 200, 2, COLOR_TREASURE);
    desenha_retangulo(60, 50, 2, 140, COLOR_TREASURE);
    desenha_retangulo(258, 50, 2, 140, COLOR_TREASURE);

    /* Engrenagem e titulo */
    desenha_engrenagem(90, 67, COLOR_TREASURE);
    desenha_texto(102, 64, "CONFIG", COLOR_WHITE);

    /* Linha separadora */
    desenha_retangulo(72, 80, 176, 1, COLOR_STONE_DARK);

    /* Opcoes do menu */
    for (i = 0; i < MENU_NUM_OPCOES; i++) {
        int oy = 92 + i * 25;
        short int cor_opcao;

        if (i == menu_opcao_selecionada) {
            cor_opcao = COLOR_TREASURE;
            /* Cursor triangular */
            desenha_retangulo(82, oy + 1, 1, 5, COLOR_TREASURE);
            desenha_retangulo(83, oy + 2, 1, 3, COLOR_TREASURE);
            desenha_pixel(84, oy + 3, COLOR_TREASURE);
            /* Barra de destaque */
            desenha_retangulo(72, oy - 2, 176, 11, COLOR_STONE_DARK);
        } else {
            cor_opcao = COLOR_WHITE;
        }

        desenha_texto(92, oy, opcoes[i], cor_opcao);
    }

    /* Instrucoes no rodape */
    desenha_texto(72, 158, "W/S = NAVEGAR", COLOR_STONE);
    desenha_texto(72, 170, "J   = SELECIONAR", COLOR_STONE);
}

void desenha_tela_controles(void) {

    /* Fundo escuro */
    limpa_tela(COLOR_BLACK);

    /* Caixa */
    desenha_retangulo(40, 30, 240, 180, COLOR_STONE_DARK);
    desenha_retangulo(42, 32, 236, 176, COLOR_BLACK);
    /* Borda dourada */
    desenha_retangulo(40, 30, 240, 2, COLOR_TREASURE);
    desenha_retangulo(40, 208, 240, 2, COLOR_TREASURE);
    desenha_retangulo(40, 30, 2, 180, COLOR_TREASURE);
    desenha_retangulo(278, 30, 2, 180, COLOR_TREASURE);

    /* Titulo */
    desenha_texto(100, 44, "CONTROLES", COLOR_TREASURE);
    desenha_retangulo(52, 55, 216, 1, COLOR_STONE_DARK);

    /* Teclas e descricoes */
    desenha_texto(60, 68, "A / ESQUERDA", COLOR_WHITE);
    desenha_texto(188, 68, "= ESQUERDA", COLOR_STONE);

    desenha_texto(60, 84, "D / DIREITA", COLOR_WHITE);
    desenha_texto(188, 84, "= DIREITA", COLOR_STONE);

    desenha_texto(60, 100, "W / CIMA", COLOR_WHITE);
    desenha_texto(188, 100, "= PULAR", COLOR_STONE);

    desenha_texto(60, 116, "J / ESPACO", COLOR_WHITE);
    desenha_texto(188, 116, "= ATIRAR", COLOR_STONE);

    desenha_texto(60, 132, "R", COLOR_WHITE);
    desenha_texto(188, 132, "= REINICIAR", COLOR_STONE);

    desenha_texto(60, 148, "SW0", COLOR_WHITE);
    desenha_texto(188, 148, "= CONFIG", COLOR_STONE);

    /* Linha separadora */
    desenha_retangulo(52, 164, 216, 1, COLOR_STONE_DARK);

    /* Instrucao de voltar */
    desenha_texto(88, 178, "J = VOLTAR", COLOR_TREASURE);
}

void processa_menu_configuracao(void) {
    static int cima_anterior = 0;
    static int baixo_anterior = 0;
    static int atirar_anterior = 0;

    if (estado == ESTADO_CONFIGURACAO) {
        /* Navegacao com edge detection (so muda na transicao 0->1) */
        if (tecla_cima && !cima_anterior) {
            menu_opcao_selecionada--;
            if (menu_opcao_selecionada < 0) menu_opcao_selecionada = MENU_NUM_OPCOES - 1;
        }
        if (tecla_baixo && !baixo_anterior) {
            menu_opcao_selecionada++;
            if (menu_opcao_selecionada >= MENU_NUM_OPCOES) menu_opcao_selecionada = 0;
        }

        /* Confirmar opcao */
        if (tecla_atirar && !atirar_anterior) {
            if (menu_opcao_selecionada == MENU_CONTINUAR) {
                estado = ESTADO_JOGANDO;
            } else if (menu_opcao_selecionada == MENU_REINICIAR) {
                inicializa_jogo();
            } else if (menu_opcao_selecionada == MENU_CONTROLES) {
                estado = ESTADO_CONTROLES;
            }
        }
    } else if (estado == ESTADO_CONTROLES) {
        /* Na tela de controles, J/Space volta para o menu */
        if (tecla_atirar && !atirar_anterior) {
            estado = ESTADO_CONFIGURACAO;
        }
    }

    cima_anterior = tecla_cima;
    baixo_anterior = tecla_baixo;
    atirar_anterior = tecla_atirar;
}

void renderiza_cena(void) {
    if (estado == ESTADO_INICIO) {
        desenha_tela_inicio();
        return;
    }
    if (estado == ESTADO_GAMEOVER) {
        desenha_tela_gameover();
        return;
    }
    if (estado == ESTADO_VITORIA) {
        desenha_tela_vitoria();
        return;
    }
    if (estado == ESTADO_PAUSADO) {
        desenha_tela_pausa();
        return;
    }
    if (estado == ESTADO_CONFIGURACAO) {
        desenha_tela_configuracao();
        return;
    }
    if (estado == ESTADO_CONTROLES) {
        desenha_tela_controles();
        return;
    }

    desenha_cenario();
    desenha_plataformas();
    desenha_bandeira();
    desenha_tokens();
    desenha_inimigos();
    desenha_projeteis();
    desenha_jogador();
}

void atualiza_estado(void) {
    if (estado != ESTADO_JOGANDO) return;

    atualiza_jogador();
    atualiza_projeteis();
    atualiza_inimigos();
}


void processa_pausa_e_reinicio(void) {
    static int chave_anterior = 0;
    int chave_atual = chave_sw0_ligada();

    if (chave_atual && !chave_anterior) {
        if (estado == ESTADO_INICIO) {
            estado = ESTADO_JOGANDO;
        } else if (estado == ESTADO_JOGANDO) {
            estado = ESTADO_CONFIGURACAO;
            menu_opcao_selecionada = 0;
        } else if (estado == ESTADO_CONFIGURACAO || estado == ESTADO_CONTROLES) {
            estado = ESTADO_JOGANDO;
        }
    }
    chave_anterior = chave_atual;
}

void le_entrada(void) {
    processa_teclado();
    processa_pausa_e_reinicio();
    processa_menu_configuracao();
}

void atraso_frame(void) {
    volatile int i;
#ifdef CPULATOR
    for (i = 0; i < 50000; i++) {
#else
    for (i = 0; i < 8000; i++) {
#endif
    }
}
