#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;
typedef map<char, string> TabelaDeCompactacao;

class No {
public:
    char valor;
    int contagem;
    map<char, No*> filhos;

    No(char valor) : valor(valor) {
        contagem = 0;
    }

    bool folha() {
        if (filhos.find('0') == filhos.end() && filhos.find('1') == filhos.end()) {
            return true;
        }
        return false;
    }
};


class Trie {
public:
    No* raiz;
    Trie(char valorRaiz) {
        raiz = new No(valorRaiz);
    }
    ~Trie() {
        if (raiz->filhos.find('0') != raiz->filhos.end()) {
            deletarNos(raiz->filhos.at('0'));
        }
        if (raiz->filhos.find('1') != raiz->filhos.end()) {
            deletarNos(raiz->filhos.at('1'));
        }
        delete raiz;
    }

    void juntar(Trie& trie) {
        No* novoNo = new No(0);
        novoNo->contagem = trie.raiz->contagem + raiz->contagem;
        novoNo->filhos.insert(pair<char, No*>('0', raiz));
        novoNo->filhos.insert(pair<char, No*>('1', trie.raiz));
        raiz = novoNo;
    }

    TabelaDeCompactacao gerarTabelaDeCompactacao() {
        TabelaDeCompactacao resultado;

        gerarTabelaDeCompactacao(raiz->filhos.at('0'), resultado, "0");
        gerarTabelaDeCompactacao(raiz->filhos.at('1'), resultado, "1");

        return resultado;
    }

    static void gerarTabelaDeCompactacao(No* no, TabelaDeCompactacao &tabela, string prefixo) {
        if (no == nullptr) {
            return;
        }

        if (no->folha()) {
            tabela.insert(TabelaDeCompactacao::value_type(no->valor, prefixo));
            return;
        }

        gerarTabelaDeCompactacao(no->filhos.at('0'), tabela, prefixo + "0");
        gerarTabelaDeCompactacao(no->filhos.at('1'), tabela, prefixo + "1");
    }

    bool operator < (const Trie& trie) const
    {
        return (raiz->contagem < trie.raiz->contagem);
    }

    static bool compararContagem(Trie& a, Trie& b) {
        if (a.raiz->contagem > b.raiz->contagem) {
            return false;
        }
        return true;
    }

private:
    static void deletarNos(No* no) {
        if (no->filhos.find('0') == no->filhos.end()) {
            deletarNos(no->filhos.at('0'));
        }
        if (no->filhos.find('1') == no->filhos.end()) {
            deletarNos(no->filhos.at('1'));
        }
        delete no;
    }
};

struct NoBuffer {
    char valor;
    NoBuffer* proximo;
};

class BufferCircular {
public:
    int tamanhoMaximo;
    BufferCircular(int tamanho) {
        tamanhoMaximo = tamanho;
        noInicial = new NoBuffer;
        final = noInicial;

        for (int i{}; i<tamanho; i++) {
            final->proximo = new NoBuffer;
            final = final->proximo;
        }

        final->proximo = noInicial;
        inicio = noInicial;
        final = noInicial;
    }
    ~BufferCircular() {
        NoBuffer* noAtual = noInicial->proximo;
        NoBuffer* proximoNo = noAtual->proximo;

        while (noAtual != noInicial) {
            delete noAtual;
            noAtual = proximoNo;
            proximoNo = noAtual->proximo;
        }

        delete noAtual;
    }

    bool cheio() {
        if (final->proximo == inicio) {
            return true;
        }
        return false;
    }

    bool vazio() {
        if (inicio == final) {
            return true;
        }
        return false;
    }

    int nosUsados() {
        if (cheio()) {
            tamanhoMaximo - 1;
        }

        NoBuffer* noAtual = inicio;
        int contagem{};

        while(noAtual != final) {
            contagem++;
            noAtual = noAtual->proximo;
        }
        return contagem;
    }

    char ler() {
        if (vazio()) {
            return -1;
        }

        NoBuffer* noLido = inicio;
        inicio = inicio->proximo;

        return noLido->valor;
    }

    void escrever(string valor) {
        for (char& c : valor) {
            escrever(c);
        }
    }

    void escrever(char valor) {
        if (cheio()) {
            return;
        }

        final->valor = valor;
        final = final->proximo;
    }

private:
    NoBuffer* noInicial;
    NoBuffer* inicio;
    NoBuffer* final;
};

char pow(unsigned char num, unsigned char power) {
    if (power == 0) {
        return 1;
    }

    char result{1};
    for (int i=1; i<=power; i++) {
        result *= num;
    }

    return result;
}

char strBinariosParaByte(string bin) {
    unsigned char resultado{};

    for (char i=0; i < 8; i++) {
        if (bin[i] == '1') {
            resultado += pow(2, 7 - i);
        }
    }
    return (char) resultado;
}

string byteParaStrBinarios(unsigned char byte) {
    char quotient=1;
    string bin;

    for (int i = 7; quotient != 0; i--) {

        quotient = byte / 2;
        bin = to_string( byte % 2) + bin;
        byte /= 2;
    }

    int casasFaltando = 8 - bin.size();
    for (int i = 0; i < casasFaltando; ++i) {
        bin = "0" + bin;
    }

    return bin;
}

void salvarByteEmArquivo(ofstream& arquivo, BufferCircular* buffer) {
    string strBinarios;

    for (int i = 0; i < 8; ++i) {
        strBinarios += buffer->ler();
    }

    arquivo << strBinariosParaByte(strBinarios);
}

int compactar(Trie& trie, ifstream& arquivoEntrada) {
    auto buffer = new BufferCircular(33);
    TabelaDeCompactacao tabelaDeCompactacao = trie.gerarTabelaDeCompactacao();
    char letra;
    int contagem{};
    ofstream arquivoSaida("huffman.freq");

    while (arquivoEntrada.get(letra)) {
        buffer->escrever(tabelaDeCompactacao.at(letra));

        if (buffer->nosUsados() >= 8) {
            salvarByteEmArquivo(arquivoSaida, buffer);
            contagem++;
        }
    }

    if (buffer->nosUsados() >= 8) {
        salvarByteEmArquivo(arquivoSaida, buffer);
        contagem++;
    }

    int nosRestantes = 8 - buffer->nosUsados();
    for (int i = 0; i < nosRestantes; ++i) {
        buffer->escrever(0);
    }

    salvarByteEmArquivo(arquivoSaida, buffer);
    contagem++;
    arquivoSaida.close();
    delete buffer;

    return contagem;
}

void descompactar(Trie& trie, int contagemChar) {
    ifstream arquivoEntrada("huffman.freq");
    ofstream arquivoSaida("huffman.defreq");
    char byte;
    auto buffer = new BufferCircular(33);
    No* noAtual = trie.raiz;
    int charEscritos{};

    while (charEscritos < contagemChar) {
        if (buffer->vazio()) {
            if (arquivoEntrada.get(byte)){
                buffer->escrever(byteParaStrBinarios(byte));
            }
        }

        noAtual = noAtual->filhos.at(buffer->ler());

        if (noAtual->folha()) {
            arquivoSaida << noAtual->valor;
            noAtual = trie.raiz;
            charEscritos++;
        }
    }

    arquivoEntrada.close();
    arquivoSaida.close();
    delete buffer;
}

void calcularTaxaDeCompactacao(int contagemChar, int tamanhoCompactado) {
    cout << "Taxa de Compactacao: " << (float) tamanhoCompactado / (float) contagemChar << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Informe o nome do arquivo como argumento.\n");
        return 1;
    }

    int contagemChar{};
    ifstream arquivoEntrada(argv[1]);
    char letra{};
    vector<Trie> tries;

    while (arquivoEntrada.get(letra)) {
        contagemChar++;
        bool trieEncontrada = false;

        for (auto trie : tries) {
            No* raiz = trie.raiz;
            if (raiz->valor == letra) {
                trieEncontrada = true;
                raiz->contagem++;
                break;
            }
        }

        if (!trieEncontrada) {
            Trie novaTrie(letra);
            novaTrie.raiz->contagem++;
            tries.push_back(novaTrie);
        }
    }

    while (tries.size() != 1) {
        sort(tries.begin(), tries.end(), less<Trie>());
        Trie trieDireita = tries[1];
        tries[0].juntar(trieDireita);
        tries.erase(tries.begin() + 1);
    }

    Trie trieCompleta = tries.front();
    arquivoEntrada.clear();
    arquivoEntrada.seekg(0);
    int tamanhoCompactado = compactar(trieCompleta, arquivoEntrada);
    arquivoEntrada.close();
    calcularTaxaDeCompactacao(contagemChar, tamanhoCompactado);
    descompactar(trieCompleta, contagemChar);

    return 0;
}