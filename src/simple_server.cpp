#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "avl_tree.h"
#include "contact.h"

using namespace std;

class SimpleWebServer {
private:
    SOCKET serverSocket;
    AVLTree<Contact> agenda;

public:
    SimpleWebServer() : serverSocket(INVALID_SOCKET) {
        // Dados de exemplo
        agenda.insert(Contact("Ana Silva", "11-1111-1111", "ana@email.com", true));
        agenda.insert(Contact("Carlos Oliveira", "11-2222-2222", "carlos@email.com"));
        agenda.insert(Contact("Beatriz Santos", "11-3333-3333", "beatriz@email.com", true));
        agenda.insert(Contact("Daniel Costa", "11-4444-4444", "daniel@email.com"));
        agenda.insert(Contact("Eduarda Lima", "11-5555-5555", "eduarda@email.com", true));
    }

    bool start(int port = 8080) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "Falha ao inicializar Winsock" << endl;
            return false;
        }

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            cerr << "Erro ao criar socket" << endl;
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            cerr << "Erro no bind" << endl;
            return false;
        }

        if (listen(serverSocket, 10) == SOCKET_ERROR) {
            cerr << "Erro no listen" << endl;
            return false;
        }

        cout << "Servidor rodando na porta " << port << endl;
        cout << "Acesse: http://localhost:" << port << endl;
        return true;
    }

    string readFile(const string& filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            return "";
        }
        
        string content;
        file.seekg(0, ios::end);
        content.resize(file.tellg());
        file.seekg(0, ios::beg);
        file.read(&content[0], content.size());
        file.close();
        
        return content;
    }

    string getContentType(const string& filename) {
        if (filename.find(".html") != string::npos) return "text/html; charset=utf-8";
        if (filename.find(".css") != string::npos) return "text/css";
        if (filename.find(".js") != string::npos) return "application/javascript";
        if (filename.find(".json") != string::npos) return "application/json";
        return "text/plain";
    }

    void handleRequests() {
        while (true) {
            SOCKET clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "Erro no accept" << endl;
                continue;
            }

            char buffer[4096];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            
            if (bytesReceived > 0) {
                string request(buffer, bytesReceived);
                string response = handleRequest(request);
                send(clientSocket, response.c_str(), response.length(), 0);
            }

            closesocket(clientSocket);
        }
    }

    string handleRequest(const string& request) {
        cout << "Requisição: " << request.substr(0, request.find('\n')) << endl;

        // Servir arquivos estáticos
        if (request.find("GET / ") != string::npos || request.find("GET /index.html") != string::npos) {
            return serveStaticFile("web/index.html");
        }
        else if (request.find("GET /style.css") != string::npos) {
            return serveStaticFile("web/style.css");
        }
        else if (request.find("GET /script.js") != string::npos) {
            return serveStaticFile("web/script.js");
        }
        else if (request.find("GET /api/contacts") != string::npos) {
            return generateContactsJSON();
        }
        else if (request.find("POST /api/add") != string::npos) {
            return handleAddContact(request);
        }
        else if (request.find("POST /api/remove") != string::npos) {
            return handleRemoveContact(request);
        }
        else if (request.find("POST /api/toggle-favorite") != string::npos) {
            return handleToggleFavorite(request);
        }
        else if (request.find("GET /api/statistics") != string::npos) {
            return generateStatisticsJSON();
        }
        else {
            return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 - Pagina nao encontrada";
        }
    }

    string serveStaticFile(const string& filename) {
        string content = readFile(filename);
        if (content.empty()) {
            return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nArquivo nao encontrado: " + filename;
        }

        string contentType = getContentType(filename);
        string response = "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Content-Length: " + to_string(content.length()) + "\r\n";
        response += "Connection: close\r\n\r\n";
        response += content;

        return response;
    }

    string generateContactsJSON() {
        auto contacts = agenda.inOrder();
        string json = "{\"success\":true,\"contacts\":[";
        
        for (size_t i = 0; i < contacts.size(); i++) {
            if (i > 0) json += ",";
            json += "{";
            json += "\"name\":\"" + escapeJSON(contacts[i].getName()) + "\",";
            json += "\"phone\":\"" + escapeJSON(contacts[i].getPhone()) + "\",";
            json += "\"email\":\"" + escapeJSON(contacts[i].getEmail()) + "\",";
            json += "\"favorite\":" + string(contacts[i].isFavorite() ? "true" : "false");
            json += "}";
        }
        
        json += "]}";
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + 
               to_string(json.length()) + "\r\n\r\n" + json;
    }

    string generateStatisticsJSON() {
        auto contacts = agenda.inOrder();
        auto favorites = agenda.getFavorites();
        
        string json = "{\"success\":true,\"statistics\":{";
        json += "\"total\":" + to_string(contacts.size()) + ",";
        json += "\"favorites\":" + to_string(favorites.size()) + ",";
        json += "\"balanced\":" + string(agenda.isBalanced() ? "true" : "false");
        json += "}}";
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + 
               to_string(json.length()) + "\r\n\r\n" + json;
    }

    string handleAddContact(const string& request) {
        size_t jsonStart = request.find("\r\n\r\n");
        if (jsonStart == string::npos) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"JSON nao encontrado\"}";
        }
        
        string jsonBody = request.substr(jsonStart + 4);
        string name = extractJSONValue(jsonBody, "name");
        string phone = extractJSONValue(jsonBody, "phone");
        string email = extractJSONValue(jsonBody, "email");
        string favoriteStr = extractJSONValue(jsonBody, "favorite");
        bool favorite = (favoriteStr == "true");
        
        if (name.empty()) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Nome e obrigatorio\"}";
        }
        
        Contact newContact(name, phone, email, favorite);
        
        if (agenda.contains(newContact)) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Contato ja existe\"}";
        }
        
        agenda.insert(newContact);
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true,\"message\":\"Contato adicionado com sucesso\"}";
    }

    string handleRemoveContact(const string& request) {
        size_t jsonStart = request.find("\r\n\r\n");
        if (jsonStart == string::npos) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"JSON nao encontrado\"}";
        }
        
        string jsonBody = request.substr(jsonStart + 4);
        string name = extractJSONValue(jsonBody, "name");
        
        if (name.empty()) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Nome e obrigatorio\"}";
        }
        
        Contact temp(name, "", "");
        
        if (!agenda.contains(temp)) {
            return "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Contato nao encontrado\"}";
        }
        
        agenda.remove(temp);
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true,\"message\":\"Contato removido com sucesso\"}";
    }

    string handleToggleFavorite(const string& request) {
        size_t jsonStart = request.find("\r\n\r\n");
        if (jsonStart == string::npos) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"JSON nao encontrado\"}";
        }
        
        string jsonBody = request.substr(jsonStart + 4);
        string name = extractJSONValue(jsonBody, "name");
        
        if (name.empty()) {
            return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Nome e obrigatorio\"}";
        }
        
        Contact temp(name, "", "");
        Contact* found = agenda.search(temp);
        
        if (!found) {
            return "HTTP/1.1 404 Not Found\r\nContent-Type: application/json\r\n\r\n{\"success\":false,\"message\":\"Contato nao encontrado\"}";
        }
        
        Contact updated = *found;
        updated.setFavorite(!updated.isFavorite());
        
        agenda.remove(*found);
        agenda.insert(updated);
        
        return "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n{\"success\":true,\"message\":\"Favorito atualizado\"}";
    }

    string extractJSONValue(const string& json, const string& key) {
        string searchStr = "\"" + key + "\":\"";
        size_t start = json.find(searchStr);
        if (start == string::npos) {
            // Tentar encontrar valor boolean
            searchStr = "\"" + key + "\":";
            start = json.find(searchStr);
            if (start == string::npos) return "";
            
            start += searchStr.length();
            size_t end = json.find_first_of(",}", start);
            if (end == string::npos) return "";
            
            return json.substr(start, end - start);
        }
        
        start += searchStr.length();
        size_t end = json.find("\"", start);
        if (end == string::npos) return "";
        
        return json.substr(start, end - start);
    }

    string escapeJSON(const string& input) {
        string output;
        for (char c : input) {
            if (c == '"') output += "\\\"";
            else if (c == '\\') output += "\\\\";
            else if (c == '\b') output += "\\b";
            else if (c == '\f') output += "\\f";
            else if (c == '\n') output += "\\n";
            else if (c == '\r') output += "\\r";
            else if (c == '\t') output += "\\t";
            else output += c;
        }
        return output;
    }

    ~SimpleWebServer() {
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
        }
        WSACleanup();
    }
};

int main() {
    SimpleWebServer server;
    
    if (server.start(8080)) {
        cout << "Servidor iniciado com sucesso!" << endl;
        cout << "Acesse: http://localhost:8080" << endl;
        server.handleRequests();
    } else {
        cerr << "Falha ao iniciar servidor" << endl;
        return 1;
    }
    
    return 0;
}