#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <memory>
#include <limits>

using namespace std;

// Forward declarations
class Fine;
class User;
class Book;
class BorrowRecord;


// Constants
const int MAX_BOOKS = 1000;
const double DAILY_FINE = 0.50;
const int BORROW_DAYS = 14;

// Utility functions
time_t getCurrentTime() {
    return time(nullptr);
}

string timeToString(time_t time) {
    char buffer[80];
    tm* timeinfo = localtime(&time);
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

// Abstract base class for User
class User {
protected:
    string username;
    string password;
    string name;
    string email;
    static int totalUsers; // Static data member

public:
    User(const string& uname, const string& pwd, const string& n, const string& e)
        : username(uname), password(pwd), name(n), email(e) {
        totalUsers++;
    }

    // Pure virtual function making this an abstract class
    virtual void displayDashboard() = 0;

    // Virtual destructor
    virtual ~User() {
        totalUsers--;
    }

    // Static member function
    static int getTotalUsers() {
        return totalUsers;
    }

    // Getters with const
    string getUsername() const { return username; }
    string getName() const { return name; }
    string getEmail() const { return email; }

    // Friend function declaration
    friend void displayUserInfo(const User& user);

protected:
    // Protected method for derived classes
    void changePassword(const string& newPassword) {
        password = newPassword;
    }
};

int User::totalUsers = 0;

// Friend function definition
void displayUserInfo(const User& user) {
    cout << "User Info (via friend function):" << endl;
    cout << "Username: " << user.username << endl;
    cout << "Name: " << user.name << endl;
    cout << "Email: " << user.email << endl;
}

// Book class
class Book {
private:
    string title;
    string author;
    string isbn;
    string genre;
    int totalCopies;
    int availableCopies;
    static int totalBooks; // Static data member

public:
    // Constructor overloading
    Book() : title(""), author(""), isbn(""), genre(""), totalCopies(0), availableCopies(0) {}
    Book(const string& t, const string& a, const string& i, const string& g, int copies)
        : title(t), author(a), isbn(i), genre(g), totalCopies(copies), availableCopies(copies) {
        totalBooks++;
    }

    // Destructor
    ~Book() {
        totalBooks--;
    }

    // Static member function
    static int getTotalBooks() {
        return totalBooks;
    }

    // Getters with const
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getIsbn() const { return isbn; }
    string getGenre() const { return genre; }
    int getTotalCopies() const { return totalCopies; }
    int getAvailableCopies() const { return availableCopies; }

    // Setters
    void setTitle(const string& t) { title = t; }
    void setAuthor(const string& a) { author = a; }
    void setGenre(const string& g) { genre = g; }
    void setTotalCopies(int copies) {
        totalCopies = copies;
        availableCopies += (copies - totalCopies);
    }

    // Member functions
    void borrowCopy() {
        if (availableCopies > 0) {
            availableCopies--;
        }
    }

    void returnCopy() {
        if (availableCopies < totalCopies) {
            availableCopies++;
        }
    }

    // Operator overloading
    bool operator==(const Book& other) const {
        return isbn == other.isbn;
    }

    // Display book info
    void display() const {
        cout << "Title: " << title << "\nAuthor: " << author
             << "\nISBN: " << isbn << "\nGenre: " << genre
             << "\nCopies: " << availableCopies << "/" << totalCopies << endl;
    }
};

int Book::totalBooks = 0;

// BorrowRecord class
class Fine {
private:
    double amount;
    time_t daysOverdue;

public:
    Fine(time_t secondsOverdue) {
        daysOverdue = secondsOverdue / (24 * 60 * 60);
        amount = daysOverdue * DAILY_FINE;
    }

    // Getters with const
    double getAmount() const { return amount; }
    time_t getDaysOverdue() const { return daysOverdue; }

    // Pay fine
    void payFine(double payment) {
        amount -= payment;
        if (amount < 0) amount = 0;
    }
};
class BorrowRecord {
private:
    string userId;
    string bookIsbn;
    time_t borrowDate;
    time_t dueDate;
    time_t returnDate;
    bool returned;
    Fine* fine; // Association

public:
    BorrowRecord(const string& uid, const string& isbn, time_t bDate)
        : userId(uid), bookIsbn(isbn), borrowDate(bDate),
          dueDate(bDate + (BORROW_DAYS * 24 * 60 * 60)),
          returned(false), fine(nullptr) {}

    // Destructor
    ~BorrowRecord() {
        if (fine != nullptr) {
            delete fine;
        }
    }

    // Getters with const
    string getUserId() const { return userId; }
    string getBookIsbn() const { return bookIsbn; }
    time_t getBorrowDate() const { return borrowDate; }
    time_t getDueDate() const { return dueDate; }
    time_t getReturnDate() const { return returnDate; }
    bool isReturned() const { return returned; }
    Fine* getFine() const { return fine; }

    // Return the book
    void returnBook(time_t rDate) {
        returnDate = rDate;
        returned = true;
        if (returnDate > dueDate) {
            // Create fine if returned late
            fine = new Fine(returnDate - dueDate);
        }
    }

    // Display record
    void display() const {
        cout << "User ID: " << userId << "\nBook ISBN: " << bookIsbn
             << "\nBorrowed: " << timeToString(borrowDate)
             << "\nDue: " << timeToString(dueDate);
        if (returned) {
            cout << "\nReturned: " << timeToString(returnDate);
            if (fine != nullptr) {
                cout << "\nFine: $" << fine->getAmount();
            }
        } else {
            cout << "\nStatus: Not returned";
            if (getCurrentTime() > dueDate) {
                cout << "\nOverdue by " << (getCurrentTime() - dueDate) / (24 * 60 * 60) << " days";
            }
        }
        cout << endl;
    }
};

// Fine class


// User derived classes
class Admin : public User {
private:
    static Admin* instance; // For Singleton pattern

    // Private constructor for Singleton
    Admin(const string& uname, const string& pwd, const string& n, const string& e)
        : User(uname, pwd, n, e) {}

public:
    // Singleton implementation
    static Admin* getInstance(const string& uname, const string& pwd,
                             const string& n, const string& e) {
        if (instance == nullptr) {
            instance = new Admin(uname, pwd, n, e);
        }
        return instance;
    }

    // Destructor
    ~Admin() {
        instance = nullptr;
    }

    // Function overriding
    void displayDashboard() override {
        cout << "\n=== ADMIN DASHBOARD ===\n";
        cout << "Welcome, " << name << "!\n";
        cout << "1. Manage Users\n2. Generate Reports\n3. System Settings\n4. Logout\n";
    }

    // Admin specific functions
    void addUser(vector<User*>& users, User* newUser) {
        users.push_back(newUser);
    }

    void removeUser(vector<User*>& users, const string& username) {
        auto it = find_if(users.begin(), users.end(),
            [&username](User* u) { return u->getUsername() == username; });
        if (it != users.end()) {
            delete *it; // Free memory
            users.erase(it);
        }
    }

    void generateReport(const vector<Book*>& books, const vector<BorrowRecord*>& records) {
        cout << "\n=== LIBRARY REPORT ===\n";
        cout << "Total Books: " << Book::getTotalBooks() << endl;
        cout << "Total Users: " << User::getTotalUsers() << endl;

        int overdue = count_if(records.begin(), records.end(),
            [](BorrowRecord* r) { return !r->isReturned() && getCurrentTime() > r->getDueDate(); });
        cout << "Overdue Books: " << overdue << endl;
    }
};

Admin* Admin::instance = nullptr;

class Librarian : public User {
public:
    Librarian(const string& uname, const string& pwd, const string& n, const string& e)
        : User(uname, pwd, n, e) {}

    // Function overriding
    void displayDashboard() override {
        cout << "\n=== LIBRARIAN DASHBOARD ===\n";
        cout << "Welcome, " << name << "!\n";
        cout << "1. Manage Books\n2. Issue Books\n3. Accept Returns\n4. Track Overdues\n5. Logout\n";
    }

    // Librarian specific functions
    void addBook(vector<Book*>& books, Book* newBook) {
        books.push_back(newBook);
    }

    void editBook(Book* book, const string& title, const string& author, const string& genre, int copies) {
        book->setTitle(title);
        book->setAuthor(author);
        book->setGenre(genre);
        book->setTotalCopies(copies);
    }

    void deleteBook(vector<Book*>& books, const string& isbn) {
        auto it = find_if(books.begin(), books.end(),
            [&isbn](Book* b) { return b->getIsbn() == isbn; });
        if (it != books.end()) {
            delete *it; // Free memory
            books.erase(it);
        }
    }

    BorrowRecord* issueBook(const string& userId, Book* book) {
        if (book->getAvailableCopies() > 0) {
            book->borrowCopy();
            return new BorrowRecord(userId, book->getIsbn(), getCurrentTime());
        }
        return nullptr;
    }

    void acceptReturn(Book* book, BorrowRecord* record) {
        book->returnCopy();
        record->returnBook(getCurrentTime());
    }
};

class Member : public User {
private:
    vector<BorrowRecord*> borrowingHistory;

public:
    Member(const string& uname, const string& pwd, const string& n, const string& e)
        : User(uname, pwd, n, e) {}

    // Function overriding
    void displayDashboard() override {
        cout << "\n=== MEMBER DASHBOARD ===\n";
        cout << "Welcome, " << name << "!\n";
        cout << "1. Search Books\n2. Borrow Books\n3. Return Books\n4. View History\n5. Logout\n";
    }

    // Member specific functions
    void searchBooks(const vector<Book*>& books, const string& query) {
        cout << "\n=== SEARCH RESULTS ===\n";
        for (const auto& book : books) {
            if (book->getTitle().find(query) != string::npos ||
                book->getAuthor().find(query) != string::npos ||
                book->getGenre().find(query) != string::npos) {
                book->display();
                cout << "-------------------\n";
            }
        }
    }

    void borrowBook(Book* book, Librarian& librarian) {
        BorrowRecord* record = librarian.issueBook(username, book);
        if (record != nullptr) {
            borrowingHistory.push_back(record);
            cout << "Book borrowed successfully!\n";
        } else {
            cout << "No available copies of this book.\n";
        }
    }

    void returnBook(Book* book, Librarian& librarian) {
        auto it = find_if(borrowingHistory.begin(), borrowingHistory.end(),
            [book](BorrowRecord* r) { return r->getBookIsbn() == book->getIsbn() && !r->isReturned(); });

        if (it != borrowingHistory.end()) {
            librarian.acceptReturn(book, *it);
            cout << "Book returned successfully!\n";
        } else {
            cout << "You haven't borrowed this book.\n";
        }
    }

    void viewHistory() const {
        cout << "\n=== BORROWING HISTORY ===\n";
        for (const auto& record : borrowingHistory) {
            record->display();
            cout << "-------------------\n";
        }
    }

    // Destructor
    ~Member() {
        for (auto record : borrowingHistory) {
            delete record;
        }
    }
};

class Guest : public User {
public:
    Guest() : User("guest", "", "Guest", "") {}

    // Function overriding
    void displayDashboard() override {
        cout << "\n=== GUEST ACCESS ===\n";
        cout << "1. Search Books\n2. Exit\n";
    }

    // Guest specific functions
    void searchBooks(const vector<Book*>& books, const string& query) {
        cout << "\n=== SEARCH RESULTS ===\n";
        for (const auto& book : books) {
            if (book->getTitle().find(query) != string::npos ||
                book->getAuthor().find(query) != string::npos ||
                book->getGenre().find(query) != string::npos) {
                cout << "Title: " << book->getTitle() << "\nAuthor: " << book->getAuthor()
                     << "\nGenre: " << book->getGenre() << "\n";
                cout << "-------------------\n";
            }
        }
    }
};

// UserFactory class (Factory pattern)
class UserFactory {
public:
    static User* createUser(const string& role, const string& uname,
                          const string& pwd, const string& name, const string& email) {
        if (role == "admin") {
            return Admin::getInstance(uname, pwd, name, email);
        } else if (role == "librarian") {
            return new Librarian(uname, pwd, name, email);
        } else if (role == "member") {
            return new Member(uname, pwd, name, email);
        } else if (role == "guest") {
            return new Guest();
        }
        return nullptr;
    }
};

// LibraryManager class (Singleton)
class LibraryManager {
private:
    static LibraryManager* instance;
    vector<User*> users;
    vector<Book*> books;
    vector<BorrowRecord*> records;
    User* currentUser;

    // Private constructor for Singleton
    LibraryManager() : currentUser(nullptr) {
        // Initialize with some data
        users.push_back(Admin::getInstance("admin", "admin123", "System Admin", "admin@library.com"));
        users.push_back(new Librarian("lib1", "lib123", "John Librarian", "john@library.com"));
        users.push_back(new Member("member1", "mem123", "Alice Member", "alice@example.com"));

        books.push_back(new Book("The C++ Programming Language", "Bjarne Stroustrup", "9780321563842", "Programming", 5));
        books.push_back(new Book("Design Patterns", "Erich Gamma", "9780201633610", "Computer Science", 3));
        books.push_back(new Book("Clean Code", "Robert Martin", "9780132350884", "Programming", 4));
    }

public:
    // Singleton implementation
    static LibraryManager* getInstance() {
        if (instance == nullptr) {
            instance = new LibraryManager();
        }
        return instance;
    }

    // Destructor
    ~LibraryManager() {
        for (auto user : users) {
            delete user;
        }
        for (auto book : books) {
            delete book;
        }
        for (auto record : records) {
            delete record;
        }
    }

    // Login function
    bool login(const string& username, const string& password) {
        auto it = find_if(users.begin(), users.end(),
            [&username, &password](User* u) {
                return u->getUsername() == username &&
                       (dynamic_cast<Guest*>(u) == nullptr || password == "");
            });

        if (it != users.end()) {
            currentUser = *it;
            return true;
        }
        return false;
    }

    void logout() {
        currentUser = nullptr;
    }

    User* getCurrentUser() const {
        return currentUser;
    }

    vector<Book*>& getBooks() {
        return books;
    }

    vector<User*>& getUsers() {
        return users;
    }

    vector<BorrowRecord*>& getRecords() {
        return records;
    }

    // Save data to files
    void saveData() {
        ofstream userFile("users.txt"), bookFile("books.txt"), recordFile("records.txt");

        for (const auto& user : users) {
            if (dynamic_cast<Admin*>(user)) {
                userFile << "admin," << user->getUsername() << "," << user->getName() << "," << user->getEmail() << "\n";
            } else if (dynamic_cast<Librarian*>(user)) {
                userFile << "librarian," << user->getUsername() << "," << user->getName() << "," << user->getEmail() << "\n";
            } else if (dynamic_cast<Member*>(user)) {
                userFile << "member," << user->getUsername() << "," << user->getName() << "," << user->getEmail() << "\n";
            }
        }

        for (const auto& book : books) {
            bookFile << book->getTitle() << "," << book->getAuthor() << ","
                     << book->getIsbn() << "," << book->getGenre() << ","
                     << book->getTotalCopies() << "\n";
        }

        for (const auto& record : records) {
            recordFile << record->getUserId() << "," << record->getBookIsbn() << ","
                       << record->getBorrowDate() << "," << record->getDueDate() << ","
                       << record->getReturnDate() << "," << record->isReturned() << "\n";
        }
    }

    // Load data from files
    void loadData() {
        ifstream userFile("users.txt"), bookFile("books.txt"), recordFile("records.txt");
        string line;

        // Clear existing data
        for (auto user : users) {
            if (dynamic_cast<Admin*>(user) == nullptr) { // Don't delete the admin
                delete user;
            }
        }
        users.clear();

        for (auto book : books) {
            delete book;
        }
        books.clear();

        for (auto record : records) {
            delete record;
        }
        records.clear();

        // Add the admin back
        users.push_back(Admin::getInstance("admin", "admin123", "System Admin", "admin@library.com"));

        // Load users
        while (getline(userFile, line)) {
            size_t pos = 0;
            string token;
            vector<string> tokens;

            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            if (tokens.size() == 4) {
                string role = tokens[0];
                string username = tokens[1];
                string name = tokens[2];
                string email = tokens[3];

                User* user = UserFactory::createUser(role, username, "", name, email);
                if (user != nullptr) {
                    users.push_back(user);
                }
            }
        }

        // Load books
        while (getline(bookFile, line)) {
            size_t pos = 0;
            string token;
            vector<string> tokens;

            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            if (tokens.size() == 5) {
                string title = tokens[0];
                string author = tokens[1];
                string isbn = tokens[2];
                string genre = tokens[3];
                int copies = stoi(tokens[4]);

                books.push_back(new Book(title, author, isbn, genre, copies));
            }
        }

        // Load records
        while (getline(recordFile, line)) {
            size_t pos = 0;
            string token;
            vector<string> tokens;

            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);

            if (tokens.size() == 6) {
                string userId = tokens[0];
                string bookIsbn = tokens[1];
                time_t borrowDate = stol(tokens[2]);
                time_t dueDate = stol(tokens[3]);
                time_t returnDate = stol(tokens[4]);
                bool returned = (tokens[5] == "1");

                BorrowRecord* record = new BorrowRecord(userId, bookIsbn, borrowDate);
                record->returnBook(returnDate);

                records.push_back(record);
            }
        }
    }
};

LibraryManager* LibraryManager::instance = nullptr;

// Main application
int main() {
    LibraryManager* library = LibraryManager::getInstance();

    // Load data from files
    library->loadData();

    while (true) {
        if (library->getCurrentUser() == nullptr) {
            // Login screen
            cout << "\n=== LIBRARY MANAGEMENT SYSTEM ===\n";
            cout << "1. Login\n2. Continue as Guest\n3. Exit\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                string username, password;
                cout << "Username: ";
                getline(cin, username);
                cout << "Password: ";
                getline(cin, password);

                if (!library->login(username, password)) {
                    cout << "Invalid credentials!\n";
                }
            } else if (choice == 2) {
                library->login("guest", "");
            } else if (choice == 3) {
                library->saveData();
                delete library;
                return 0;
            }
        } else {
            User* currentUser = library->getCurrentUser();
            currentUser->displayDashboard();

            if (Admin* admin = dynamic_cast<Admin*>(currentUser)) {
                cout << "Enter choice: ";
                int choice;
                cin >> choice;
                cin.ignore();

                if (choice == 1) {
                    // Manage Users
                    cout << "\n=== MANAGE USERS ===\n";
                    cout << "1. Add User\n2. Remove User\n3. View Users\n4. Back\n";
                    cout << "Enter choice: ";
                    cin >> choice;
                    cin.ignore();

                    if (choice == 1) {
                        string role, username, password, name, email;
                        cout << "Role (librarian/member): ";
                        getline(cin, role);
                        cout << "Username: ";
                        getline(cin, username);
                        cout << "Password: ";
                        getline(cin, password);
                        cout << "Name: ";
                        getline(cin, name);
                        cout << "Email: ";
                        getline(cin, email);

                        User* newUser = UserFactory::createUser(role, username, password, name, email);
                        admin->addUser(library->getUsers(), newUser);
                        cout << "User added successfully!\n";
                    } else if (choice == 2) {
                        string username;
                        cout << "Enter username to remove: ";
                        getline(cin, username);
                        admin->removeUser(library->getUsers(), username);
                        cout << "User removed if existed.\n";
                    } else if (choice == 3) {
                        cout << "\n=== USER LIST ===\n";
                        for (const auto& user : library->getUsers()) {
                            displayUserInfo(*user); // Using friend function
                            cout << "-------------------\n";
                        }
                    }
                } else if (choice == 2) {
                    // Generate Reports
                    admin->generateReport(library->getBooks(), library->getRecords());
                } else if (choice == 4) {
                    library->logout();
                }
            } else if (Librarian* librarian = dynamic_cast<Librarian*>(currentUser)) {
                cout << "Enter choice: ";
                int choice;
                cin >> choice;
                cin.ignore();

                if (choice == 1) {
                    // Manage Books
                    cout << "\n=== MANAGE BOOKS ===\n";
                    cout << "1. Add Book\n2. Edit Book\n3. Delete Book\n4. View Books\n5. Back\n";
                    cout << "Enter choice: ";
                    cin >> choice;
                    cin.ignore();

                    if (choice == 1) {
                        string title, author, isbn, genre;
                        int copies;
                        cout << "Title: ";
                        getline(cin, title);
                        cout << "Author: ";
                        getline(cin, author);
                        cout << "ISBN: ";
                        getline(cin, isbn);
                        cout << "Genre: ";
                        getline(cin, genre);
                        cout << "Copies: ";
                        cin >> copies;
                        cin.ignore();

                        Book* newBook = new Book(title, author, isbn, genre, copies);
                        librarian->addBook(library->getBooks(), newBook);
                        cout << "Book added successfully!\n";
                    } else if (choice == 2) {
                        string isbn, title, author, genre;
                        int copies;
                        cout << "Enter ISBN of book to edit: ";
                        getline(cin, isbn);

                        auto it = find_if(library->getBooks().begin(), library->getBooks().end(),
                            [&isbn](Book* b) { return b->getIsbn() == isbn; });

                        if (it != library->getBooks().end()) {
                            cout << "New Title: ";
                            getline(cin, title);
                            cout << "New Author: ";
                            getline(cin, author);
                            cout << "New Genre: ";
                            getline(cin, genre);
                            cout << "New Copies: ";
                            cin >> copies;
                            cin.ignore();

                            librarian->editBook(*it, title, author, genre, copies);
                            cout << "Book updated successfully!\n";
                        } else {
                            cout << "Book not found!\n";
                        }
                    } else if (choice == 3) {
                        string isbn;
                        cout << "Enter ISBN of book to delete: ";
                        getline(cin, isbn);
                        librarian->deleteBook(library->getBooks(), isbn);
                        cout << "Book deleted if existed.\n";
                    } else if (choice == 4) {
                        cout << "\n=== BOOK CATALOG ===\n";
                        for (const auto& book : library->getBooks()) {
                            book->display();
                            cout << "-------------------\n";
                        }
                    }
                } else if (choice == 2) {
                    // Issue Books
                    string userId, isbn;
                    cout << "Enter Member Username: ";
                    getline(cin, userId);
                    cout << "Enter Book ISBN: ";
                    getline(cin, isbn);

                    auto userIt = find_if(library->getUsers().begin(), library->getUsers().end(),
                        [&userId](User* u) { return u->getUsername() == userId && dynamic_cast<Member*>(u) != nullptr; });

                    auto bookIt = find_if(library->getBooks().begin(), library->getBooks().end(),
                        [&isbn](Book* b) { return b->getIsbn() == isbn; });

                    if (userIt != library->getUsers().end() && bookIt != library->getBooks().end()) {
                        BorrowRecord* record = librarian->issueBook(userId, *bookIt);
                        if (record != nullptr) {
                            library->getRecords().push_back(record);
                            cout << "Book issued successfully!\n";
                        } else {
                            cout << "No available copies!\n";
                        }
                    } else {
                        cout << "Invalid user or book!\n";
                    }
                } else if (choice == 3) {
                    // Accept Returns
                    string userId, isbn;
                    cout << "Enter Member Username: ";
                    getline(cin, userId);
                    cout << "Enter Book ISBN: ";
                    getline(cin, isbn);

                    auto userIt = find_if(library->getUsers().begin(), library->getUsers().end(),
                        [&userId](User* u) { return u->getUsername() == userId && dynamic_cast<Member*>(u) != nullptr; });

                    auto bookIt = find_if(library->getBooks().begin(), library->getBooks().end(),
                        [&isbn](Book* b) { return b->getIsbn() == isbn; });

                    if (userIt != library->getUsers().end() && bookIt != library->getBooks().end()) {
                        auto recordIt = find_if(library->getRecords().begin(), library->getRecords().end(),
                            [&userId, &isbn](BorrowRecord* r) {
                                return r->getUserId() == userId && r->getBookIsbn() == isbn && !r->isReturned();
                            });

                        if (recordIt != library->getRecords().end()) {
                            librarian->acceptReturn(*bookIt, *recordIt);
                            cout << "Book returned successfully!\n";
                        } else {
                            cout << "No matching active borrowing record found!\n";
                        }
                    } else {
                        cout << "Invalid user or book!\n";
                    }
                } else if (choice == 4) {
                    // Track Overdues
                    cout << "\n=== OVERDUE BOOKS ===\n";
                    for (const auto& record : library->getRecords()) {
                        if (!record->isReturned() && getCurrentTime() > record->getDueDate()) {
                            record->display();
                            cout << "-------------------\n";
                        }
                    }
                } else if (choice == 5) {
                    library->logout();
                }
            } else if (Member* member = dynamic_cast<Member*>(currentUser)) {
                cout << "Enter choice: ";
                int choice;
                cin >> choice;
                cin.ignore();

                if (choice == 1) {
                    // Search Books
                    string query;
                    cout << "Enter search term (title/author/genre): ";
                    getline(cin, query);
                    member->searchBooks(library->getBooks(), query);
                } else if (choice == 2) {
                    // Borrow Books
                    string isbn;
                    cout << "Enter ISBN of book to borrow: ";
                    getline(cin, isbn);

                    auto bookIt = find_if(library->getBooks().begin(), library->getBooks().end(),
                        [&isbn](Book* b) { return b->getIsbn() == isbn; });

                    if (bookIt != library->getBooks().end()) {
                        // Find any librarian
                        auto librarianIt = find_if(library->getUsers().begin(), library->getUsers().end(),
                            [](User* u) { return dynamic_cast<Librarian*>(u) != nullptr; });

                        if (librarianIt != library->getUsers().end()) {
                            Librarian* librarian = dynamic_cast<Librarian*>(*librarianIt);
                            member->borrowBook(*bookIt, *librarian);
                        } else {
                            cout << "No librarian available to process your request!\n";
                        }
                    } else {
                        cout << "Book not found!\n";
                    }
                } else if (choice == 3) {
                    // Return Books
                    string isbn;
                    cout << "Enter ISBN of book to return: ";
                    getline(cin, isbn);

                    auto bookIt = find_if(library->getBooks().begin(), library->getBooks().end(),
                        [&isbn](Book* b) { return b->getIsbn() == isbn; });

                    if (bookIt != library->getBooks().end()) {
                        // Find any librarian
                        auto librarianIt = find_if(library->getUsers().begin(), library->getUsers().end(),
                            [](User* u) { return dynamic_cast<Librarian*>(u) != nullptr; });

                        if (librarianIt != library->getUsers().end()) {
                            Librarian* librarian = dynamic_cast<Librarian*>(*librarianIt);
                            member->returnBook(*bookIt, *librarian);
                        } else {
                            cout << "No librarian available to process your request!\n";
                        }
                    } else {
                        cout << "Book not found!\n";
                    }
                } else if (choice == 4) {
                    // View History
                    member->viewHistory();
                } else if (choice == 5) {
                    library->logout();
                }
            } else if (Guest* guest = dynamic_cast<Guest*>(currentUser)) {
                cout << "Enter choice: ";
                int choice;
                cin >> choice;
                cin.ignore();

                if (choice == 1) {
                    // Search Books
                    string query;
                    cout << "Enter search term (title/author/genre): ";
                    getline(cin, query);
                    guest->searchBooks(library->getBooks(), query);
                } else if (choice == 2) {
                    library->logout();
                }
            }
        }
    }

    return 0;
}
