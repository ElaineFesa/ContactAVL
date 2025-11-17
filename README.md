#  Sistema de Agenda de Contatos com Árvore AVL

##  Descrição do Projeto
teste
Sistema completo de gerenciamento de contatos implementado em C++ utilizando a estrutura de dados **Árvore AVL** para garantir eficiência nas operações de inserção, remoção e busca. Desenvolvido como trabalho acadêmico para a disciplina de Estrutura de Dados.

##  Objetivos

- Implementar uma árvore AVL funcional para armazenamento de contatos
- Garantir complexidade O(log n) para operações básicas
- Desenvolver interface amigável de linha de comando (CLI)
- Implementar funcionalidades completas de CRUD
- Fornecer sistema de validação e testes

##  Funcionalidades

###  Operações Básicas
-  **Adicionar contato** - Inserção balanceada na AVL
-  **Remover contato** - Remoção com rebalanceamento
-  **Buscar contato** - Busca eficiente O(log n)
-  **Listar contatos** - Ordem alfabética automática
-  **Gerenciar favoritos** - Marcar/desmarcar contatos importantes
-  **Estatísticas da árvore** - Verficação de balanceamento
-  **Importação/Exportação CSV** - Backup dos dados
-  **Testes unitários** - Validação do sistema
-  **Interface intuitiva** - Menu com emojis e formatação


##  Estrutura do Projeto

```
ContactAVL/
├── include/
│   ├── avl_tree.h          # Implementação completa da Árvore AVL
│   └── contact.h           # Classe Contato com todos os atributos
├── src/
│   ├── contact.cpp         # Implementação dos métodos do Contato
│   └── main_console.cpp    # Programa principal com interface CLI
├── tests/
│   └── test_avl.cpp        # Testes unitários completos
├── compilar.bat           # Script de compilação automática
└── README.md              # Este arquivo
```

##  Estruturas de Dados Implementadas

###  Árvore AVL (avl_tree.h)
```cpp
template<typename T>
class AVLTree {
    // Operações principais
    void insert(const T& value);      // Inserção balanceada
    void remove(const T& value);      // Remoção com rebalanceamento  
    bool contains(const T& value);    // Busca O(log n)
    T* search(const T& value);        // Retorna ponteiro para o elemento
    
    // Travessias
    std::vector<T> inOrder();         // Listagem ordenada
    std::vector<T> getFavorites();    // Apenas favoritos
    
    // Verificações
    bool isBalanced();                // Valida propriedades AVL
    bool isEmpty();                   // Verifica se está vazia
    int size();                       // Retorna quantidade de elementos
};
```

###  Classe Contato (contact.h)
```cpp
class Contact {
private:
    std::string name;     // Nome do contato (chave de ordenação)
    std::string phone;    // Número de telefone
    std::string email;    // Endereço de email
    bool favorite;        // Indicador de favorito

public:
    // Getters e Setters
    std::string getName() const;
    void setPhone(const std::string& phone);
    // ... outros métodos
    
    // Operadores para árvore
    bool operator<(const Contact& other) const;
    bool operator==(const Contact& other) const;
};
```

##  Como Compilar e Executar

### Pré-requisitos
- Compilador C++ com suporte a C++17 (GCC 7+ ou MinGW)
- Windows, Linux ou macOS

### Compilação Automática (Windows)
```cmd
compilar.bat
```

### Compilação Manual
```bash
# Compilar
g++ src/main_console.cpp src/contact.cpp -Iinclude -o agenda_avl.exe -std=c++17

# Executar
./agenda_avl.exe
```

### Compilação dos Testes
```bash
g++ tests/test_avl.cpp src/contact.cpp -Iinclude -o test_avl.exe -std=c++17
./test_avl.exe
```

##  Como Usar o Sistema

### Menu Principal
```
==================================================
         SISTEMA DE AGENDA - ÁRVORE AVL
==================================================
1. 📝 Adicionar contato
2. 🗑️  Remover contato  
3. 🔍 Buscar contato
4. 📋 Listar todos os contatos (ordem alfabética)
5. ⭐ Listar favoritos
6. ★  Marcar/desmarcar favorito
7. 📊 Estatísticas da árvore
8. 🧪 Executar testes
9. 💾 Exportar para CSV
10. 📥 Importar de CSV
11. 🚪 Sair
==================================================
Escolha uma opção: 
```

### Exemplo de Uso

1. **Adicionar Contato**
   ```
   Nome: João Silva
   Telefone: 11-9999-8888  
   Email: joao@email.com
   Favorito (s/n): s
    Contato adicionado com sucesso!
   ```

2. **Listar Contatos**
   ```
   --- TODOS OS CONTATOS ---
   Total: 5 contatos
   ────────────────────
   📞 Ana Silva | 11-1111-1111 | ana@email.com ⭐
   📞 Beatriz Santos | 11-3333-3333 | beatriz@email.com ⭐
   📞 Carlos Oliveira | 11-2222-2222 | carlos@email.com
   ```

3. **Verificar Estatísticas**
   ```
   --- ESTATÍSTICAS DA ÁRVORE ---
   ✅ Árvore balanceada: Sim
   📊 Total de contatos: 5
   ⭐ Total de favoritos: 2
   🌳 Árvore vazia: Não
   ```

## 📊 Formatos de Arquivo

### Exportação CSV
```csv
Nome,Telefone,Email,Favorito
Ana Silva,11-1111-1111,ana@email.com,true
Carlos Oliveira,11-2222-2222,carlos@email.com,false
Beatriz Santos,11-3333-3333,beatriz@email.com,true
```

### Importação CSV
- Formato: `Nome,Telefone,Email,Favorito`
- Encoding: UTF-8
- Delimitador: Vírgula
- Cabeçalho obrigatório

##  Testes e Validação

### Suite de Testes Implementada
```cpp
void runTests() {
    // ✅ Teste de inserção básica
    // ✅ Teste de remoção  
    // ✅ Teste de busca
    // ✅ Teste de favoritos
    // ✅ Teste de balanceamento
    // ✅ Teste de múltiplas operações
    // ✅ Teste de árvore vazia
    // ✅ Teste de diferentes formas de leitura
}
```

### Validação das Propriedades AVL
O sistema verifica automaticamente:
- **Fator de balanceamento** entre -1 e 1
- **Altura correta** dos nós
- **Rebalanceamento** após inserções/remoções
- **Preservação da ordem** dos elementos

##  Complexidade das Operações

| Operação | Complexidade | Descrição |
|----------|--------------|-----------|
| Inserção | O(log n) | Inserção com rebalanceamento |
| Remoção | O(log n) | Remoção com rebalanceamento |
| Busca | O(log n) | Busca binária na árvore |
| Listagem | O(n) | Travessia in-order |
| Favoritos | O(n) | Filtragem durante travessia |

##  Características Técnicas

### Implementação da AVL
- **Smart Pointers**: Gerenciamento automático de memória
- **Templates**: Código genérico e reutilizável  
- **Recursividade**: Algoritmos recursivos elegantes
- **Balanceamento**: Rotações LL, RR, LR, RL

### Tratamento de Erros
- Entrada de dados validada
- Prevenção de contatos duplicados
- Verificação de arquivos CSV
- Mensagens de erro descritivas


##  Exemplos de Código

### Inserção na AVL
```cpp
AVLTree<Contact> agenda;
agenda.insert(Contact("Maria", "11-1234-5678", "maria@email.com", true));
```

### Busca Eficiente
```cpp
Contact temp("Maria", "", "");
Contact* encontrado = agenda.search(temp);
if (encontrado) {
    encontrado->display();
}
```

### Exportação de Dados
```cpp
exportToCSV(agenda);  // Cria arquivo "contatos.csv"
```

## 🎓 Aprendizados do Projeto

### Estruturas de Dados
- Implementação prática de árvores balanceadas
- Compreensão de algoritmos de balanceamento
- Manipulação de ponteiros e memória


### Algoritmos
- Análise de complexidade assintótica
- Recursividade e divisão e conquista
- Manipulação de estruturas complexas

## 🔧 Possíveis Melhorias Futuras

- [ ] Interface gráfica (Qt)
- [ ] Persistência em banco de dados
- [ ] Sincronização com nuvem
- [ ] Busca avançada (por telefone, email)
- [ ] Grupos de contatos
- [ ] Backup automático
- [ ] Histórico de operações


---
