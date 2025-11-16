#include <iostream>
#include <string>
#include <limits>
#include <cctype>
#include <fstream>
#include <sstream>
#include "contact.h"
#include "avl_tree.h"

using namespace std;

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pause() {
    cout << "\nPressione Enter para continuar...";
    clearInput();
}

void displayMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "         SISTEMA DE AGENDA - ÁRVORE AVL" << endl;
    cout << string(50, '=') << endl;
    cout << "1. 📝 Adicionar contato" << endl;
    cout << "2. 🗑️  Remover contato" << endl;
    cout << "3. 🔍 Buscar contato" << endl;
    cout << "4. 📋 Listar todos os contatos (ordem alfabética)" << endl;
    cout << "5. ⭐ Listar favoritos" << endl;
    cout << "6. ★  Marcar/desmarcar favorito" << endl;
    cout << "7. 📊 Estatísticas da árvore" << endl;
    cout << "8. 🧪 Executar testes" << endl;
    cout << "9. 💾 Exportar para CSV" << endl;
    cout << "10. 📥 Importar de CSV" << endl;
    cout << "11. 🚪 Sair" << endl;
    cout << string(50, '=') << endl;
    cout << "Escolha uma opção: ";
}

void addContact(AVLTree<Contact>& agenda) {
    string nome, telefone, email;
    char favorito;
    
    cout << "\n--- ADICIONAR CONTATO ---" << endl;
    cout << "Nome: ";
    getline(cin, nome);
    
    if (nome.empty()) {
        cout << " Erro: Nome é obrigatório!" << endl;
        return;
    }
    
    cout << "Telefone: ";
    getline(cin, telefone);
    cout << "Email: ";
    getline(cin, email);
    cout << "Favorito (s/n): ";
    cin >> favorito;
    clearInput();
    
    Contact novoContato(nome, telefone, email, toupper(favorito) == 'S');
    
    if (agenda.contains(novoContato)) {
        cout << " Erro: Contato já existe!" << endl;
    } else {
        agenda.insert(novoContato);
        cout << " Contato adicionado com sucesso!" << endl;
    }
}

void removeContact(AVLTree<Contact>& agenda) {
    string nome;
    cout << "\n--- REMOVER CONTATO ---" << endl;
    cout << "Nome do contato a remover: ";
    getline(cin, nome);
    
    if (nome.empty()) {
        cout << " Erro: Digite um nome!" << endl;
        return;
    }
    
    Contact temp(nome, "", "");
    
    if (agenda.contains(temp)) {
        agenda.remove(temp);
        cout << " Contato removido com sucesso!" << endl;
    } else {
        cout << " Erro: Contato não encontrado!" << endl;
    }
}

void searchContact(AVLTree<Contact>& agenda) {
    string nome;
    cout << "\n--- BUSCAR CONTATO ---" << endl;
    cout << "Nome: ";
    getline(cin, nome);
    
    Contact temp(nome, "", "");
    Contact* encontrado = agenda.search(temp);
    
    if (encontrado) {
        cout << "\n Contato encontrado:" << endl;
        cout << "────────────────────" << endl;
        encontrado->display();
    } else {
        cout << "Contato não encontrado!" << endl;
    }
}

void listAllContacts(AVLTree<Contact>& agenda) {
    cout << "\n--- TODOS OS CONTATOS ---" << endl;
    auto contatos = agenda.inOrder();
    
    if (contatos.empty()) {
        cout << "Nenhum contato cadastrado." << endl;
    } else {
        cout << "Total: " << contatos.size() << " contatos" << endl;
        cout << "────────────────────" << endl;
        for (const auto& contato : contatos) {
            contato.display();
        }
    }
}

void listFavorites(AVLTree<Contact>& agenda) {
    cout << "\n--- CONTATOS FAVORITOS ---" << endl;
    auto favoritos = agenda.getFavorites();
    
    if (favoritos.empty()) {
        cout << "Nenhum contato favorito." << endl;
    } else {
        cout << "Total: " << favoritos.size() << " favoritos" << endl;
        cout << "────────────────────" << endl;
        for (const auto& contato : favoritos) {
            contato.display();
        }
    }
}

void toggleFavorite(AVLTree<Contact>& agenda) {
    string nome;
    cout << "\n--- ALTERAR FAVORITO ---" << endl;
    cout << "Nome do contato: ";
    getline(cin, nome);
    
    Contact temp(nome, "", "");
    Contact* encontrado = agenda.search(temp);
    
    if (encontrado) {
        Contact atualizado = *encontrado;
        atualizado.setFavorite(!atualizado.isFavorite());
        
        agenda.remove(*encontrado);
        agenda.insert(atualizado);
        
        cout << " Contato " << (atualizado.isFavorite() ? "marcado" : "desmarcado") 
             << " como favorito!" << endl;
    } else {
        cout << " Erro: Contato não encontrado!" << endl;
    }
}

void showStatistics(AVLTree<Contact>& agenda) {
    cout << "\n--- ESTATÍSTICAS DA ÁRVORE ---" << endl;
    cout << "✅ Árvore balanceada: " << (agenda.isBalanced() ? "Sim" : "Não") << endl;
    cout << "📊 Total de contatos: " << agenda.size() << endl;
    cout << "⭐ Total de favoritos: " << agenda.getFavorites().size() << endl;
    cout << "🌳 Árvore vazia: " << (agenda.isEmpty() ? "Sim" : "Não") << endl;
}

void exportToCSV(const AVLTree<Contact>& agenda) {
    ofstream file("contatos.csv");
    if (!file.is_open()) {
        cout << " Erro ao criar arquivo contatos.csv!" << endl;
        return;
    }
    
    // Cabeçalho
    file << "Nome,Telefone,Email,Favorito\n";
    
    // Dados
    auto contacts = agenda.inOrder();
    for (const auto& contact : contacts) {
        file << contact.getName() << ","
             << contact.getPhone() << ","
             << contact.getEmail() << ","
             << (contact.isFavorite() ? "true" : "false") << "\n";
    }
    
    file.close();
    cout << " " << contacts.size() << " contatos exportados para contatos.csv" << endl;
}

void importFromCSV(AVLTree<Contact>& agenda) {
    ifstream file("contatos.csv");
    if (!file.is_open()) {
        cout << " Arquivo contatos.csv não encontrado!" << endl;
        cout << " Exporte primeiro alguns contatos para criar o arquivo." << endl;
        return;
    }
    
    string line;
    getline(file, line); // Pular cabeçalho
    
    int imported = 0;
    int skipped = 0;
    
    while (getline(file, line)) {
        stringstream ss(line);
        string name, phone, email, favoriteStr;
        
        getline(ss, name, ',');
        getline(ss, phone, ',');
        getline(ss, email, ',');
        getline(ss, favoriteStr, ',');
        
        if (!name.empty()) {
            bool favorite = (favoriteStr == "true");
            Contact newContact(name, phone, email, favorite);
            
            if (!agenda.contains(newContact)) {
                agenda.insert(newContact);
                imported++;
            } else {
                skipped++;
            }
        }
    }
    
    file.close();
    cout << " " << imported << " contatos importados!" << endl;
    if (skipped > 0) {
        cout << " " << skipped << " contatos duplicados foram ignorados." << endl;
    }
}

void runTests() {
    cout << "\n--- EXECUTANDO TESTES ---" << endl;
    
    AVLTree<Contact> testTree;
    
    // Teste de inserção
    testTree.insert(Contact("Alice", "111", "alice@email.com", true));
    testTree.insert(Contact("Carlos", "222", "carlos@email.com"));
    testTree.insert(Contact("Beatriz", "333", "beatriz@email.com", true));
    
    cout << " Teste de inserção: " << (testTree.size() == 3 ? "PASSOU" : "FALHOU") << endl;
    
    // Teste de busca
    Contact temp("Alice", "", "");
    bool found = testTree.contains(temp);
    cout << " Teste de busca: " << (found ? "PASSOU" : "FALHOU") << endl;
    
    // Teste de favoritos
    auto favorites = testTree.getFavorites();
    cout << " Teste de favoritos: " << (favorites.size() == 2 ? "PASSOU" : "FALHOU") << endl;
    
    // Teste de balanceamento
    cout << " Teste de balanceamento: " << (testTree.isBalanced() ? "PASSOU" : "FALHOU") << endl;
    
    // Teste de remoção
    testTree.remove(temp);
    cout << " Teste de remoção: " << (!testTree.contains(temp) ? "PASSOU" : "FALHOU") << endl;
    
    // Teste de múltiplas operações
    for (int i = 0; i < 10; i++) {
        testTree.insert(Contact("Test" + to_string(i), "123", "test@email.com"));
    }
    cout << " Teste múltiplas inserções: " << (testTree.isBalanced() ? "PASSOU" : "FALHOU") << endl;
    
    cout << " Todos os testes concluídos!" << endl;
}

int main() {
    AVLTree<Contact> agenda;
    int opcao;
    
    // Adicionar alguns contatos de exemplo
    agenda.insert(Contact("Ana Silva", "11-1111-1111", "ana@email.com", true));
    agenda.insert(Contact("Carlos Oliveira", "11-2222-2222", "carlos@email.com"));
    agenda.insert(Contact("Beatriz Santos", "11-3333-3333", "beatriz@email.com", true));
    agenda.insert(Contact("Daniel Costa", "11-4444-4444", "daniel@email.com"));
    agenda.insert(Contact("Eduarda Lima", "11-5555-5555", "eduarda@email.com", true));
    
    cout << " SISTEMA DE AGENDA AVL INICIADO!" << endl;
    cout << " " << agenda.size() << " contatos de exemplo carregados" << endl;
    
    do {
        displayMenu();
        cin >> opcao;
        clearInput();
        
        switch (opcao) {
            case 1: addContact(agenda); break;
            case 2: removeContact(agenda); break;
            case 3: searchContact(agenda); break;
            case 4: listAllContacts(agenda); break;
            case 5: listFavorites(agenda); break;
            case 6: toggleFavorite(agenda); break;
            case 7: showStatistics(agenda); break;
            case 8: runTests(); break;
            case 9: exportToCSV(agenda); break;
            case 10: importFromCSV(agenda); break;
            case 11: cout << " Saindo... Até logo!" << endl; break;
            default: cout << " Opção inválida!" << endl;
        }
        
        if (opcao != 11) pause();
        
    } while (opcao != 11);
    
    return 0;
}