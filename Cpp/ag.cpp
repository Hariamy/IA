#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cstring>
#include <string>

#define TAM_GERACAO 1000
#define QTD_GERACAO 10000
#define P_MUTACAO 90

int tamTabuleiro = 0;

class Tabuleiro {
    int peso,
        *rainhas;
public:
    Tabuleiro () : peso(0), rainhas(new int[tamTabuleiro]) { }

    int& obterPeso () { return peso; }
    int* obterRainhas () { return rainhas; }

    void redimencionar (int redi) {
        if (rainhas != NULL){
            delete(rainhas);
        }
        rainhas = new int[redi];
    }

    int& operator [] (int i) {
        return rainhas[i];
    }

    void operator = (int *novasRainhas) {
        if (rainhas != NULL){
            delete(rainhas);
        }
        rainhas = novasRainhas;
    }

    friend std::ostream& operator << (std::ostream& os, const Tabuleiro &tabuleiro) {
        os << std::endl << "\u254b";	
        for (int i = 0; i < tamTabuleiro; i++) {
                    os << "\u2501\u2501\u254b";
            }
        os << std::endl;
        for (int i = 0; i < tamTabuleiro; i++) {
            os << "\u2503";
            for (int j = 0; j < tamTabuleiro; j++) {
                if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0)){
                    os << ((i == tabuleiro.rainhas[j]) ? "\u265B " : "\u2588\u2588") << "\u2503";
                } else {
                    os << ((i == tabuleiro.rainhas[j]) ? "\u265B " : "  ") << "\u2503";
                }
            }
            os << std::endl << "\u254b";
            for (int i = 0; i < tamTabuleiro; i++) {
                os << "\u2501\u2501\u254b";
            }
            os << std::endl;
        }
        os << std::endl;
        return os;
    }
};

void calcularPeso (Tabuleiro &tabuleiro) {
    for (int i = 0; i < tamTabuleiro; i++) {
        for (int j = i+1; j < tamTabuleiro; j++) {
            if (abs(i - j) == abs(tabuleiro[i] - tabuleiro[j]) ||
                tabuleiro[i] == tabuleiro[j]) {
                tabuleiro.obterPeso()++;
            }
        }
    }
}

void mutar (Tabuleiro &filho) {
    std::random_device rd;
    int qtd = (rd() % (int)(tamTabuleiro / 2)) + 1,
        pos1,
        pos2,
        valAux;

    for (int i = 0; i < qtd; i++) {
        pos1 = rd() % tamTabuleiro;
        pos2 = rd() % tamTabuleiro;
        valAux = filho[pos1];

        filho[pos1] = filho[pos2];
        filho[pos2] = valAux;
    }
}

Tabuleiro cruzar (Tabuleiro &pai, Tabuleiro &mae) {
    int metade = tamTabuleiro / 2,
        *rainhas = new int[tamTabuleiro];
    Tabuleiro filho;
    std::random_device rd;
    std::vector <int> posicoes;
    std::vector <bool> valores(tamTabuleiro);

    fill(valores.begin(), valores.end(), true);

    for (int i = 0; i <= metade; i++) {
        rainhas[i] = pai[i];
        valores[rainhas[i]] = false;
    }

    for (int i = metade+1; i < tamTabuleiro; i++) {
        if (valores[mae[i]] == true) {
            rainhas[i] = mae[i];
            valores[rainhas[i]] = false;
        } else {
            posicoes.push_back(i);
        }
    }

    for (int p : posicoes) {
        auto iterator = std::find(valores.begin(), valores.end(), true);
        int posDispo = (iterator - valores.begin());
        rainhas[p] = posDispo;
        *iterator = false;
    }

    filho = rainhas;
    calcularPeso(filho);

    
    if (P_MUTACAO < (rd() % 100)) {
        mutar(filho);
    }

    return filho;
}

void gerarTabuleiroRandomico (Tabuleiro &tabuleiro) {
    int *rainhas = new int[tamTabuleiro],
        posSorteado;
    std::random_device rd;
    bool valores[tamTabuleiro];

    memset(valores, true, tamTabuleiro);
    
    for (int i = 0; i < tamTabuleiro; i++) {
        posSorteado = rd() % tamTabuleiro; 
        while (!valores[posSorteado]) {
            posSorteado = (posSorteado + 1) % tamTabuleiro;
        }
        rainhas[i] = posSorteado;
        valores[posSorteado] = false;
    }
    tabuleiro = rainhas;
    calcularPeso(tabuleiro);
}

std::vector <Tabuleiro> criarGeracao () {
    std::vector <Tabuleiro> geracao(TAM_GERACAO);

    for_each(geracao.begin(), geracao.end(), gerarTabuleiroRandomico);

    return geracao;
}

int main () {
    int geracao = -1;
    bool achou = false;
    std::vector <Tabuleiro> geracaoAtual,
                            proxGeracao;
    Tabuleiro *pai,
              *mae;
    std::random_device rd;

    std::cout << "Tamanho do tabuleiro: ";
    std::cin >> tamTabuleiro;
    std::cout << std::endl;

    proxGeracao = criarGeracao();

    do {
        geracao++;

        std::cout << "\033[1A\033[KGeração: " << geracao << std::endl;

        geracaoAtual = proxGeracao;
        proxGeracao.clear();

        for (int i = 0; i < TAM_GERACAO / 2; i++) {
            pai = &geracaoAtual[(int)log10(rd()) % TAM_GERACAO];
            mae = &geracaoAtual[(int)log10(rd()) % TAM_GERACAO];

            proxGeracao.push_back(cruzar(*pai, *mae));
            proxGeracao.push_back(cruzar(*mae, *pai));
        }

        std::sort(proxGeracao.begin(), proxGeracao.end(), [] (Tabuleiro &t1, Tabuleiro &t2) { return t1.obterPeso() < t2.obterPeso(); });
        achou = proxGeracao[0].obterPeso() == 0;
    } while (geracao < QTD_GERACAO && !achou);
    std::cout << proxGeracao[0] << "Peso: " << proxGeracao[0].obterPeso() << std::endl;

    return 0;
}
