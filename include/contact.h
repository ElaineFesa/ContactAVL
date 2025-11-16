#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <iostream>

class Contact {
public:
    Contact(const std::string& name = "", const std::string& phone = "", 
            const std::string& email = "", bool favorite = false);
    
    // Getters
    std::string getName() const;
    std::string getPhone() const;
    std::string getEmail() const;
    bool isFavorite() const;
    
    // Setters
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setFavorite(bool favorite);
    
    // Operadores para comparação
    bool operator<(const Contact& other) const;
    bool operator==(const Contact& other) const;
    bool operator>(const Contact& other) const;
    
    // Exibir contato
    void display() const;

private:
    std::string name;
    std::string phone;
    std::string email;
    bool favorite;
};

#endif