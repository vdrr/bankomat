#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace std;

const int VIEW_MENU = 1;
const int VIEW_WITHDRAW = 2;
const int VIEW_CHANGE_PIN = 3;
const int VIEW_EXIT = 4;

const string ERR_MSG_WRONG_CREDENTIALS = "Wrong Card Number or PIN, please try again...";
const string ERR_MSG_ACCOUNT_LOCKED = "This account has been locked. You are unable to access it.";
const string ERR_MSG_PIN_MISMATCH = "Numbers must match, try again...";
const string ERR_MSG_PIN_LENGTH = "PIN must be exactly 4 digits long. Please try again...";
const string ERR_MSG_PIN_DIGITS = "PIN must consist only of digits. Please try again...";

const string MSG_EXIT = "Have a nice day!";

const int CLIENT_DATA_COL_CARD_NUMBER = 0;
const int CLIENT_DATA_COL_PIN = 1;
const int CLIENT_DATA_COL_BALANCE = 2;
const int CLIENT_DATA_COL_LOCK = 3;

bool is_digits(const std::string &str);

class Client {
    public:
        string cardNumber;
        string cardPin;
        double balance;
        bool lock;
        int loginAttempts;

        Client() {

        }

        Client(string cardNumber, string cardPin, double balance, bool lock) {
            this->cardNumber = cardNumber;
            this->cardPin = cardPin;
            this->balance = balance;
            this->lock = lock;
            loginAttempts = 0;
        }
};

class Atm {
    public:
        Atm(string clientDataFileName, string banknotesDataFileName) {
            this->clientDataFileName = clientDataFileName;
            this->banknotesDataFileName = banknotesDataFileName;
            currentView = VIEW_MENU;
            loggedIn = false;
            readDataFiles();
        }

        ~Atm() {
            //delete currentClient;
        }

        void showMenu() {
            do {
                while (!loggedIn) {
                    loginScreen();
                    if (currentView == VIEW_EXIT)
                        break;
                }

                switch(currentView) {
                    case VIEW_MENU:
                        menuScreen();
                        break;
                    case VIEW_WITHDRAW:
                        withdrawScreen();
                        break;
                    case VIEW_CHANGE_PIN:
                        changePinScreen();
                        break;
                    case VIEW_EXIT:
                        cout << MSG_EXIT << endl;
                        break;
                }
            } while(currentView != VIEW_EXIT);
        }

    private:
        string clientDataFileName;
        string banknotesDataFileName;

        bool loggedIn;
        Client* currentClient;
        vector<Client> clients;
        int currentView;

        void menuScreen() {
            int option;

            cout << "Select option:" << endl;
            cout << "1. Withdraw money" << endl;
            cout << "2. Change PIN" << endl;
            cout << endl;
            cout << "0. Exit" << endl;

            cin >> option;

            switch(option) {
                case 0:
                    currentView = VIEW_EXIT;
                    break;
                case 1:
                    currentView = VIEW_WITHDRAW;
                    break;
                case 2:
                    currentView = VIEW_CHANGE_PIN;
                    break;
                default:
                    break;
            }
        }

        bool loginScreen() {
            string clientCardNumber,
                clientPIN;

            string option;

            cout << "Select option:" << endl;
            cout << "1. Enter card credentials" << endl;
            cout << endl;
            cout << "0. Exit" << endl;
            cin >> option;

            if (option == "0") {
                currentView = VIEW_EXIT;
                return false;
            }

            cout << "Enter Card Number:" << endl;
            cin >> clientCardNumber;

            cout << "Enter PIN:" << endl;
            cin >> clientPIN;

            currentClient = getClientByCardNumber(clientCardNumber);

            if (currentClient == NULL) {
                cout << ERR_MSG_WRONG_CREDENTIALS << endl;
                return false;
            }

            if (currentClient->lock) {
                cout << ERR_MSG_ACCOUNT_LOCKED << endl;
                return false;
            }

            if (currentClient->cardPin != clientPIN) {
                currentClient->loginAttempts++;
                if (currentClient->loginAttempts >= 3) {
                    currentClient->lock = true;
                    cout << ERR_MSG_ACCOUNT_LOCKED << endl;
                } else {
                    cout << ERR_MSG_WRONG_CREDENTIALS << endl;
                }
                return false;
            }

            loggedIn = true;
            return true;
        }

        void changePinScreen() {
            string pin1, pin2;

            cout << "Enter new PIN: ";
            cin >> pin1;
            cout << "Re-enter new PIN: ";
            cin >> pin2;

            if (pin1 == pin2) {
                if (pin1.size() != 4) {
                    cout << ERR_MSG_PIN_LENGTH << endl;
                    return;
                }

                if (!is_digits(pin1)) {
                    cout << ERR_MSG_PIN_DIGITS << endl;
                    return;
                }

                currentClient->cardPin = pin1;
                currentView = VIEW_MENU;
                return;
            }

            cout << ERR_MSG_PIN_MISMATCH << endl;
        }

        bool withdrawScreen() {
            cout << "Withdraw money" << endl;
        }

        Client* getClientByCardNumber(string cardNumber) {
            int size = static_cast<int>(clients.size());

            for (int i = 0; i < size; i++) {
                if (clients[i].cardNumber == cardNumber)
                    return &clients[i];
            }

            return NULL;
        }

        void readDataFiles() {
            ifstream clientsFile;
            clientsFile.open(clientDataFileName);

            string line;

            while (clientsFile) {
                getline(clientsFile, line);
                clients.push_back(parseClientDataString(line));
            }

            clientsFile.close();
        }

        Client parseClientDataString(string rawData) {
            Client c;
            char delimiter = '\t';

            size_t pos = 0;
            string token;
            int column = CLIENT_DATA_COL_CARD_NUMBER;

            while ((pos = rawData.find(delimiter)) != string::npos) {
                token = rawData.substr(0, pos);
                rawData.erase(0, pos + 1);

                switch (column)
                {
                case CLIENT_DATA_COL_CARD_NUMBER:
                    c.cardNumber = token;
                    break;

                case CLIENT_DATA_COL_PIN:
                    c.cardPin = token;
                    break;

                case CLIENT_DATA_COL_BALANCE:
                    c.balance = stod(token);
                    break;

                case CLIENT_DATA_COL_LOCK:
                    c.lock = (token == "TAK") ? true : false;
                    break;

                default:
                    break;
                }

                column++;
            }

            return c;
        }
};

int main(void)
{
    Atm bankomat("dane.txt","bankomat.txt");
    bankomat.showMenu();
    return 0;
}

bool is_digits(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}
