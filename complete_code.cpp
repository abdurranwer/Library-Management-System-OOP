#include <iostream>
#include <string>
using namespace std;

// ========== Book Class ==========
class Book {
public:
    string title;
    string author;
    int copies;

    void addBook(string t, string a, int c) {
        title = t;
        author = a;
        copies = c;
    }

    void showBook() {
        cout << "Title: " << title << ", Author: " << author << ", Copies: " << copies << endl;
    }
};

// ========== User Base Class ==========
class User {
public:
    string name;
    User(string n) {
        name = n;
    }

    virtual void menu() {
        cout << "User menu\n";
    }
};

// ========== Admin Class ==========
class Admin : public User {
public:
    Admin(string n) : User(n) {}

    void menu() {
        cout << "Admin Menu:\n1. Add Book\n2. View Books\n";
    }
};

// ========== Librarian Class ==========
class Librarian : public User {
public:
    Librarian(string n) : User(n) {}

    void menu() {
        cout << "Librarian Menu:\n1. Issue Book\n2. Return Book\n";
    }
};

// ========== Member Class ==========
class Member : public User {
public:
    Member(string n) : User(n) {}

    void menu() {
        cout << "Member Menu:\n1. View Books\n2. Borrow Book\n";
    }
};

// ========== Guest Class ==========
class Guest : public User {
public:
    Guest(string n) : User(n) {}

    void menu() {
        cout << "Guest Menu:\n1. View Book Catalog (Read Only)\n";
    }
};

// ========== Main Function ==========
int main() {
    // Book creation
    Book b1;
    b1.addBook("C++ Basics", "Bjarne", 4);

    Book b2;
    b2.addBook("Object-Oriented Design", "Grady Booch", 2);

    // Users
    Admin admin("Ali");
    Librarian librarian("Sara");
    Member member("Ahmed");
    Guest guest("Zara");

    // Display Menus
    cout << "\n--- Role Menus ---\n";
    admin.menu();
    librarian.menu();
    member.menu();
    guest.menu();

    // Show books
    cout << "\n--- Book Catalog ---\n";
    b1.showBook();
    b2.showBook();

    return 0;
}
