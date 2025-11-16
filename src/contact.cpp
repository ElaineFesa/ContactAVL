#include "contact.h"

Contact::Contact(const std::string& name, const std::string& phone, 
                 const std::string& email, bool favorite)
    : name(name), phone(phone), email(email), favorite(favorite) {}

std::string Contact::getName() const { return name; }
std::string Contact::getPhone() const { return phone; }
std::string Contact::getEmail() const { return email; }
bool Contact::isFavorite() const { return favorite; }

void Contact::setPhone(const std::string& phone) { this->phone = phone; }
void Contact::setEmail(const std::string& email) { this->email = email; }
void Contact::setFavorite(bool favorite) { this->favorite = favorite; }

bool Contact::operator<(const Contact& other) const {
    return name < other.name;
}

bool Contact::operator==(const Contact& other) const {
    return name == other.name;
}

bool Contact::operator>(const Contact& other) const {
    return name > other.name;
}

void Contact::display() const {
    std::cout << "📞 " << name << " | " << phone << " | " << email;
    if (favorite) std::cout << " ⭐";
    std::cout << std::endl;
}