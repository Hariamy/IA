#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <cstring>
#include <string>
#include <map>

#include "cores.h"

#define P_MUTACAO 90

#define SIMBOLO_RAINHA " ♛ "
#define ESPACO_BRANCO "   "

int tamTabuleiro = 0,
    passo = 0,
    QTD_GERACAO = 0,
    TAM_GERACAO = 0;

double pesoTotal = 0,
       passoPeso = 0;
std::map <double, int> sorteio;


class Tabuleiro {
    int *rainhas,
        qtdAtaque;
    double peso;

public:
    Tabuleiro () : peso(0), qtdAtaque(0), rainhas(new int[tamTabuleiro]) { }

    double& obterPeso () { return peso; }
    int* obterRainhas () { return rainhas; }
    int& obterQtdAtaque () { return qtdAtaque; }

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
        os << std::endl << std::endl;

        os << FUNDO_BRANCO << COR_AZUL;
        os << " ┏";
        for (int i = 0; i < tamTabuleiro; i++) {
            os << "━━━";
        }
        os << "┓ ";

        os << std::endl;
        for (int i = 0; i < tamTabuleiro; i++) {
            os << FUNDO_BRANCO << COR_AZUL;
            os << " ┃";
            for (int j = 0; j < tamTabuleiro; j++) {
                if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0)){
                    os << FUNDO_AZUL << COR_BRANCO;
                    os << ((i == tabuleiro.rainhas[j]) ? SIMBOLO_RAINHA : ESPACO_BRANCO) ;
                } else {
                    os << FUNDO_BRANCO << COR_AZUL;
                    os << ((i == tabuleiro.rainhas[j]) ? SIMBOLO_RAINHA : ESPACO_BRANCO) ;
                }
        		os << FUNDO_NORMAL << COR_NORMAL;

            }
            os << FUNDO_BRANCO << COR_AZUL;
            os << "┃ " << std::endl;
        }

        os << " ┗";
        for (int i = 0; i < tamTabuleiro; i++) {
            os << "━━━";
        }
        os << "┛ ";
        os << FUNDO_NORMAL << COR_NORMAL;
        os << std::endl;
        return os;
    }
};

void calcularPeso (Tabuleiro &tabuleiro) {
    for (int i = 0; i < tamTabuleiro; i++) {
        for (int j = i+1; j < tamTabuleiro; j++) {
            if (abs(i - j) == abs(tabuleiro[i] - tabuleiro[j]) ||
                tabuleiro[i] == tabuleiro[j]) {
                tabuleiro.obterQtdAtaque()++;
            }
        }
    }

    if (tabuleiro.obterQtdAtaque() == 0){
        tabuleiro.obterPeso() = 1.0f / tabuleiro.obterQtdAtaque();
    } else {
        tabuleiro.obterPeso() = 1.0f / 0.5f;
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

    
    if (P_MUTACAO < (rd() % 100)) {
        mutar(filho);
    }

    calcularPeso(filho);
    pesoTotal += filho.obterPeso();

    return filho;
}

Tabuleiro cruzarRandomico (Tabuleiro &pai, Tabuleiro &mae) {
    int metade = tamTabuleiro / 2,
        qtdPai = metade,
        qtdMae = tamTabuleiro - metade,
        *rainhas = new int[tamTabuleiro];
    Tabuleiro filho,
              *tabuleiroEscolhido;
    std::random_device rd;
    std::vector <int> posicoes;
    std::vector <bool> valores(tamTabuleiro);

    fill(valores.begin(), valores.end(), true);

    for (int i = 0; qtdPai > 0 && qtdMae > 0; i++) {
        if (rd() % 2 == 0) {
            qtdPai--;
            tabuleiroEscolhido = &pai;
        } else {
            qtdMae--;
            tabuleiroEscolhido = &mae;
        }

        if (valores[tabuleiroEscolhido->operator[](i)] == true) {
            rainhas[i] = tabuleiroEscolhido->operator[](i);
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

    
    if (P_MUTACAO < (rd() % 100)) {
        mutar(filho);
    }

    calcularPeso(filho);
    pesoTotal += filho.obterPeso();

    return filho;
}

Tabuleiro cruzarIntercalado (Tabuleiro &pai, Tabuleiro &mae) {
    int *rainhas = new int[tamTabuleiro];
    Tabuleiro filho,
              *tabuleiroEscolhido;
    std::random_device rd;
    std::vector <int> posicoes;
    std::vector <bool> valores(tamTabuleiro);

    fill(valores.begin(), valores.end(), true);

    for (int i = 0; i < tamTabuleiro; i++) {
        if (i % 2 == 0) {
            tabuleiroEscolhido = &pai;
        } else {
            tabuleiroEscolhido = &mae;
        }

        if (valores[tabuleiroEscolhido->operator[](i)] == true) {
            rainhas[i] = tabuleiroEscolhido->operator[](i);
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

    
    if (P_MUTACAO < (rd() % 100)) {
        mutar(filho);
    }

    calcularPeso(filho);
    pesoTotal += filho.obterPeso();

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
    
    pesoTotal += tabuleiro.obterPeso();
}

std::vector <Tabuleiro> criarGeracao () {
    std::vector <Tabuleiro> geracao(TAM_GERACAO);

    for_each(geracao.begin(), geracao.end(), gerarTabuleiroRandomico);

    return geracao;
}

int main () {
    int geracao = -1,
        melhorResultado = -1;
    bool achou = false;
    std::vector <Tabuleiro> geracaoAtual,
                            proxGeracao;
    Tabuleiro *pai,
              *mae;
    std::random_device rd;

    std::cout << "\nTamanho do tabuleiro: ";
    std::cin >> tamTabuleiro;
    
    std::cout << "Quantidade máxima de gerações: ";
    std::cin >> QTD_GERACAO;

    std::cout << "Tamanho da população por geração: ";
    std::cin >> TAM_GERACAO;

    std::cout << std::endl << std::endl;
    proxGeracao = criarGeracao();

    do {
        geracao++;

        //if (geracao % 10  == 0){
            std::cout << "\033[1A\033[KGeração: " << geracao << " - Peso: " << melhorResultado << std::endl;
        //}

        for_each(geracaoAtual.begin(), geracaoAtual.end(), [] (Tabuleiro &tabuleiro) { delete(tabuleiro.obterRainhas()); });
        geracaoAtual = proxGeracao;
        proxGeracao.clear();
        passo = 0;
        passoPeso = 0;
        for_each(proxGeracao.begin(), proxGeracao.end(), [] (Tabuleiro &tabuleiro) { sorteio[passoPeso] = passo;
                                                                                     passo++;
                                                                                     passoPeso += tabuleiro.obterPeso() / pesoTotal; });

        for (int i = 0; i < TAM_GERACAO / 2; i++) {
            //pai = &geracaoAtual[(int)logf64(rd()) % TAM_GERACAO];
            //mae = &geracaoAtual[(int)logf64(rd()) % TAM_GERACAO];
            pai = &geracaoAtual[sorteio.lower_bound(rd()/rd.max())->second];
            mae = &geracaoAtual[sorteio.lower_bound(rd()/rd.max())->second];
            
            //proxGeracao.push_back(cruzar(*pai, *mae));
            //proxGeracao.push_back(cruzar(*mae, *pai));

            //proxGeracao.push_back(cruzarRandomico(*pai, *mae));
            //proxGeracao.push_back(cruzarRandomico(*mae, *pai));

            proxGeracao.push_back(cruzarIntercalado(*pai, *mae));
            proxGeracao.push_back(cruzarIntercalado(*mae, *pai));
        }
        pesoTotal = 0;
        std::sort(proxGeracao.begin(), proxGeracao.end(), [] (Tabuleiro &t1, Tabuleiro &t2) { return t1.obterQtdAtaque() < t2.obterQtdAtaque(); });
        melhorResultado = proxGeracao[0].obterQtdAtaque();
        achou = melhorResultado == 0;
    } while (geracao < QTD_GERACAO && !achou);
    std::cout << "\033[1A\033[KGeração: " << geracao << " - Peso: " << melhorResultado << std::endl;
    std::cout << proxGeracao[0] << "\n\nQuantidade de ataques: " << proxGeracao[0].obterQtdAtaque() << std::endl;

    return 0;
}
