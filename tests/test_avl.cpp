#include <cstdio>
#include <cassert>
#include "../include/avl_tree.h"
#include "../include/contact.h"

void runTests() {
    std::cout << "Iniciando testes da Árvore AVL...\n" << std::endl;
    
    // Teste 1: Inserção básica
    std::cout << "Teste 1: Inserção básica... ";
    AVLTree<Contact> tree1;
    Contact c1("Alice", "123", "alice@email.com");
    Contact c2("Bob", "456", "bob@email.com");
    Contact c3("Charlie", "789", "charlie@email.com");
    
    tree1.insert(c1);
    tree1.insert(c2);
    tree1.insert(c3);
    
    assert(tree1.contains(c1));
    assert(tree1.contains(c2));
    assert(tree1.contains(c3));
    assert(tree1.isBalanced());
    std::cout << "OK!" << std::endl;
    
    // Teste 2: Remoção
    std::cout << "Teste 2: Remoção... ";
    AVLTree<Contact> tree2;
    tree2.insert(c1);
    tree2.insert(c2);
    tree2.insert(c3);
    
    tree2.remove(c2);
    assert(!tree2.contains(c2));
    assert(tree2.contains(c1));
    assert(tree2.contains(c3));
    assert(tree2.isBalanced());
    std::cout << "OK!" << std::endl;
    
    // Teste 3: Busca
    std::cout << "Teste 3: Busca... ";
    AVLTree<Contact> tree3;
    tree3.insert(c1);
    tree3.insert(c2);
    
    Contact* found = tree3.search(c1);
    assert(found != nullptr);
    assert(found->getName() == "Alice");
    
    found = tree3.search(Contact("Unknown", "", ""));
    assert(found == nullptr);
    std::cout << "OK!" << std::endl;
    
    // Teste 4: Ordenação
    std::cout << "Teste 4: Ordenação... ";
    AVLTree<Contact> tree4;
    Contact c4("Charlie", "123", "c@email.com");
    Contact c5("Alice", "456", "a@email.com");
    Contact c6("Bob", "789", "b@email.com");
    
    tree4.insert(c4);
    tree4.insert(c5);
    tree4.insert(c6);
    
    auto contacts = tree4.inOrder();
    assert(contacts[0].getName() == "Alice");
    assert(contacts[1].getName() == "Bob");
    assert(contacts[2].getName() == "Charlie");
    std::cout << "OK!" << std::endl;
    
    // Teste 5: Favoritos
    std::cout << "Teste 5: Favoritos... ";
    AVLTree<Contact> tree5;
    Contact c7("Alice", "123", "a@email.com", false);
    Contact c8("Bob", "456", "b@email.com", true);
    Contact c9("Charlie", "789", "c@email.com", true);
    
    tree5.insert(c7);
    tree5.insert(c8);
    tree5.insert(c9);
    
    auto favorites = tree5.getFavorites();
    assert(favorites.size() == 2);
    assert(favorites[0].getName() == "Bob");
    assert(favorites[1].getName() == "Charlie");
    assert(tree5.isBalanced());
    std::cout << "OK!" << std::endl;
    
    // Teste 6: Múltiplas inserções e remoções mantendo propriedades AVL
    std::cout << "Teste 6: Múltiplas inserções e remoções... ";
    AVLTree<Contact> tree6;
    
    // Inserir muitos contatos
    for (int i = 0; i < 50; i++) {
        std::string name = "Contact" + std::to_string(i);
        Contact c(name, "123", "email@test.com", i % 3 == 0);
        tree6.insert(c);
        assert(tree6.isBalanced());
    }
    
    // Verificar que todos estão presentes
    for (int i = 0; i < 50; i++) {
        std::string name = "Contact" + std::to_string(i);
        Contact temp(name, "", "");
        assert(tree6.contains(temp));
    }
    assert(tree6.isBalanced());
    
    // Remover alguns contatos
    for (int i = 0; i < 50; i += 2) {
        std::string name = "Contact" + std::to_string(i);
        Contact temp(name, "", "");
        tree6.remove(temp);
        assert(tree6.isBalanced());
    }
    
    // Verificar que os removidos não estão mais presentes
    for (int i = 0; i < 50; i += 2) {
        std::string name = "Contact" + std::to_string(i);
        Contact temp(name, "", "");
        assert(!tree6.contains(temp));
    }
    
    // Verificar que os não removidos ainda estão presentes
    for (int i = 1; i < 50; i += 2) {
        std::string name = "Contact" + std::to_string(i);
        Contact temp(name, "", "");
        assert(tree6.contains(temp));
    }
    assert(tree6.isBalanced());
    std::cout << "OK!" << std::endl;
    
    // Teste 7: Diferentes formas de leitura
    std::cout << "Teste 7: Diferentes formas de leitura... ";
    AVLTree<Contact> tree7;
    Contact c10("D", "1", "d@email.com");
    Contact c11("B", "2", "b@email.com");
    Contact c12("F", "3", "f@email.com");
    Contact c13("A", "4", "a@email.com");
    Contact c14("C", "5", "c@email.com");
    Contact c15("E", "6", "e@email.com");
    Contact c16("G", "7", "g@email.com");
    
    tree7.insert(c10);
    tree7.insert(c11);
    tree7.insert(c12);
    tree7.insert(c13);
    tree7.insert(c14);
    tree7.insert(c15);
    tree7.insert(c16);
    
    auto inOrder = tree7.inOrder();
    assert(inOrder.size() == 7);
    assert(inOrder[0].getName() == "A");
    assert(inOrder[6].getName() == "G");
    
    auto preOrder = tree7.preOrder();
    assert(preOrder.size() == 7);
    assert(preOrder[0].getName() == "D"); // Raiz
    
    auto postOrder = tree7.postOrder();
    assert(postOrder.size() == 7);
    assert(postOrder[6].getName() == "D"); // Raiz por último
    
    assert(tree7.isBalanced());
    std::cout << "OK!" << std::endl;
    
    // Teste 8: Árvore vazia
    std::cout << "Teste 8: Árvore vazia... ";
    AVLTree<Contact> tree8;
    assert(tree8.isEmpty());
    assert(tree8.isBalanced());
    assert(tree8.inOrder().empty());
    assert(tree8.getFavorites().empty());
    std::cout << "OK!" << std::endl;
    
    std::cout << "\nTodos os testes passaram!" << std::endl;
}

int main() {
    try {
        runTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Erro nos testes: " << e.what() << std::endl;
        return 1;
    }
}