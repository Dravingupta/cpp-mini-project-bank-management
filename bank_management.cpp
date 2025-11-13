    #include <iostream>
    #include <fstream>
    #include <cstring>
    #include <iomanip>
    #include <limits>
    #include <cctype>   // for isdigit

    using namespace std;

    class Account {
    private:
        int accountNumber;
        char name[50];
        double balance;
        char pin[5]; // store PIN as string to preserve leading zeros (4 digits + '\0')

    public:
        // Constructor
        Account() {
            accountNumber = 0;
            strcpy(name, "");
            balance = 0.0;
            pin[0] = '\0';
        }

        // Function to create new account (auto-generate account number)
        void createAccount() {
            cout << "\n\t=== CREATE NEW ACCOUNT ===\n";

            // Find highest existing account number in file and set new one as highest+1
            int newAccNo = 1001; // default start
            fstream fileCheck("accounts.dat", ios::binary | ios::in);
            if (fileCheck) {
                Account tmp;
                int maxAcc = 0;
                while (fileCheck.read((char*)&tmp, sizeof(tmp))) {
                    if (tmp.accountNumber > maxAcc) maxAcc = tmp.accountNumber;
                }
                if (maxAcc >= newAccNo) newAccNo = maxAcc + 1;
                fileCheck.close();
            }

            accountNumber = newAccNo;
            cout << "\tYour generated Account Number: " << accountNumber << endl;

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\tEnter Account Holder Name: ";
            cin.getline(name, 50);

            cout << "\tEnter Initial Balance: Rs. ";
            cin >> balance;
            if(balance < 0) {
                cout << "\n\tError: Balance cannot be negative! Setting to 0.\n";
                balance = 0;
            }

            // PIN creation and confirmation (4-digit as string to preserve leading zeros)
            string p1, p2;
            while (true) {
                cout << "\tCreate a 4-digit PIN for withdrawal: ";
                cin >> p1;
                // validate length and digits
                if (p1.length() != 4) {
                    cout << "\n\tError: PIN must be exactly 4 digits.\n";
                    continue;
                }
                bool allDigits = true;
                for (char c : p1) if (!isdigit((unsigned char)c)) { allDigits = false; break; }
                if (!allDigits) {
                    cout << "\n\tError: PIN must contain only digits (0-9).\n";
                    continue;
                }

                cout << "\tConfirm PIN: ";
                cin >> p2;
                if (p1 != p2) {
                    cout << "\n\tError: PINs do not match. Try again.\n";
                    continue;
                }
                // ok
                break;
            }
            // copy into char array (safe, we already validated length == 4)
            strcpy(pin, p1.c_str());

            cout << "\n\t✓ Account Created Successfully!\n";
            cout << "\t(Keep your PIN secret!)\n";
        }

        // Function to deposit money (interactive)
        void deposit() {
            double amount;
            cout << "\n\tEnter amount to deposit: Rs. ";
            cin >> amount;

            if(amount <= 0) {
                cout << "\n\tError: Invalid amount!\n";
                return;
            }

            balance += amount;
            cout << "\n\t✓ Amount deposited successfully!\n";
            cout << "\tNew Balance: Rs. " << fixed << setprecision(4) << balance << endl;
        }

        // Function to withdraw money (interactive, requires PIN)
        void withdraw() {
            char enteredPin[5];
            int attempts = 0;
            const int maxAttempts = 3;

            // ask for pin and allow up to 3 tries
            while(attempts < maxAttempts) {
                cout << "\n\tEnter your 4-digit PIN: ";
                cin >> enteredPin;
                // basic validation: length and digits
                if (strlen(enteredPin) != 4) {
                    cout << "\n\tError: PIN must be exactly 4 digits.\n";
                } else {
                    bool allDigits = true;
                    for (int i = 0; i < 4; ++i)
                        if (!isdigit((unsigned char)enteredPin[i])) { allDigits = false; break; }
                    if (!allDigits) {
                        cout << "\n\tError: PIN must contain only digits.\n";
                    } else {
                        if (strcmp(enteredPin, pin) == 0) {
                            // correct PIN
                            break;
                        } else {
                            ++attempts;
                            cout << "\n\tError: Incorrect PIN. Attempts left: " << (maxAttempts - attempts) << endl;
                        }
                    }
                }
                // invalid format does not increment attempts (you can change that behavior)
            }

            if(attempts == maxAttempts) {
                cout << "\n\tError: Too many incorrect PIN attempts. Withdrawal cancelled.\n";
                return;
            }

            double amount;
            cout << "\n\tEnter amount to withdraw: Rs. ";
            cin >> amount;

            if(amount <= 0) {
                cout << "\n\tError: Invalid amount!\n";
                return;
            }

            if(amount > balance) {
                cout << "\n\tError: Insufficient balance!\n";
                cout << "\tAvailable Balance: Rs. " << fixed << setprecision(2) << balance << endl;
                return;
            }

            balance -= amount;
            cout << "\n\t✓ Amount withdrawn successfully!\n";
            cout << "\tRemaining Balance: Rs. " << fixed << setprecision(2) << balance << endl;
        }

        // non-interactive helpers for transfer
        bool checkPin(const char *entered) const {
            if (entered == nullptr) return false;
            return (strcmp(entered, pin) == 0);
        }

        bool withdrawAmountNoPrompt(double amount) {
            if (amount <= 0) return false;
            if (amount > balance) return false;
            balance -= amount;
            return true;
        }

        void depositAmountNoPrompt(double amount) {
            if (amount > 0) balance += amount;
        }

        // Function to display account details (do NOT show PIN)
        void displayAccount() const {
            cout << "\n\t=== ACCOUNT DETAILS ===\n";
            cout << "\tAccount Number: " << accountNumber << endl;
            cout << "\tAccount Holder: " << name << endl;
            cout << "\tBalance: Rs. " << fixed << setprecision(2) << balance << endl;
            cout << "\t=======================\n";
        }

        // Function to display account in list format (do NOT show PIN)
        void displayList() const {
            cout << "\t" << setw(10) << accountNumber
                << setw(25) << name
                << setw(15) << fixed << setprecision(2) << balance << endl;
        }

        // Getter for account number
        int getAccountNumber() const {
            return accountNumber;
        }

        // Function to write account to file
        void writeToFile(fstream &file) {
            file.write((char*)this, sizeof(*this));
        }

        // Function to read account from file
        void readFromFile(fstream &file) {
            file.read((char*)this, sizeof(*this));
        }
    };

    // Function declarations
    void createNewAccount();
    void depositMoney();
    void withdrawMoney();
    void transferMoney();        
    void checkBalance();
    void displayAllAccounts();
    void showMenu();

    int main() {
        int choice;

        cout << "\n\n";
        cout << "\t******************************************\n";
        cout << "\t*   BANK MANAGEMENT SYSTEM               *\n";
        cout << "\t*   Guru Nanak Dev University Amritsar   *\n";
        cout << "\t*   Department of CSE                    *\n";
        cout << "\t******************************************\n";
        cout << "\n\tDeveloped by:\n";
        cout << "\tDravin Gupta (92)\n";
        cout << "\tHarwinder Singh (90)\n";
        cout << "\n\tPress Enter to continue...";
        cin.get();

        do {
            showMenu();
            cout << "\n\tEnter your choice: ";
            cin >> choice;

            switch(choice) {
                case 1:
                    createNewAccount();
                    break;
                case 2:
                    depositMoney();
                    break;
                case 3:
                    withdrawMoney();
                    break;
                case 4:
                    transferMoney();   
                    break;
                case 5:
                    checkBalance();
                    break;
                case 6:
                    displayAllAccounts();
                    break;
                case 7:
                    cout << "\n\n\t✓ Thank you for using Bank Management System!\n";
                    cout << "\t✓ Exiting...\n\n";
                    break;
                default:
                    cout << "\n\tError: Invalid choice! Please try again.\n";
            }

            if(choice != 7) {
                cout << "\n\tPress Enter to continue...";
                cin.ignore();
                cin.get();
            }

        } while(choice != 7);

        return 0;
    }

    void showMenu() {
        system("clear"); // Use "cls" for Windows
        cout << "\n\n";
        cout << "\t******************************************\n";
        cout << "\t*     BANK MANAGEMENT SYSTEM - MENU      *\n";
        cout << "\t******************************************\n\n";
        cout << "\t[1] Create New Account\n";
        cout << "\t[2] Deposit Money\n";
        cout << "\t[3] Withdraw Money\n";
        cout << "\t[4] Transfer Money (to another account)\n";
        cout << "\t[5] Check Account Balance\n";
        cout << "\t[6] Display All Accounts\n";
        cout << "\t[7] Exit\n";
        cout << "\n\t****************************************\n";
    }

    void createNewAccount() {
        Account acc;
        acc.createAccount();

        fstream file;
        file.open("accounts.dat", ios::binary | ios::app);

        if(!file) {
            cout << "\n\tError: Unable to open file!\n";
            return;
        }

        acc.writeToFile(file);
        file.close();
    }

    void depositMoney() {
        int accNum;
        bool found = false;

        cout << "\n\t=== DEPOSIT MONEY ===\n";
        cout << "\tEnter Account Number: ";
        cin >> accNum;

        fstream file;
        file.open("accounts.dat", ios::binary | ios::in | ios::out);

        if(!file) {
            cout << "\n\tError: No accounts found!\n";
            return;
        }

        Account acc;
        while(file.read((char*)&acc, sizeof(acc))) {
            if(acc.getAccountNumber() == accNum) {
                acc.displayAccount();
                acc.deposit();

                int pos = -1 * (int)sizeof(acc);
                file.seekp(pos, ios::cur);
                file.write((char*)&acc, sizeof(acc));

                found = true;
                break;
            }
        }

        file.close();

        if(!found) {
            cout << "\n\tError: Account not found!\n";
        }
    }

    void withdrawMoney() {
        int accNum;
        bool found = false;

        cout << "\n\t=== WITHDRAW MONEY ===\n";
        cout << "\tEnter Account Number: ";
        cin >> accNum;

        fstream file;
        file.open("accounts.dat", ios::binary | ios::in | ios::out);

        if(!file) {
            cout << "\n\tError: No accounts found!\n";
            return;
        }

        Account acc;
        while(file.read((char*)&acc, sizeof(acc))) {
            if(acc.getAccountNumber() == accNum) {
                acc.displayAccount();
                acc.withdraw();

                int pos = -1 * (int)sizeof(acc);
                file.seekp(pos, ios::cur);
                file.write((char*)&acc, sizeof(acc));

                found = true;
                break;
            }
        }

        file.close();

        if(!found) {
            cout << "\n\tError: Account not found!\n";
        }
    }

    //  transferMoney 
    void transferMoney() {
        int srcAccNum, destAccNum;
        cout << "\n\t=== TRANSFER MONEY ===\n";
        cout << "\tEnter Source Account Number: ";
        cin >> srcAccNum;
        cout << "\tEnter Destination Account Number: ";
        cin >> destAccNum;

        if (srcAccNum == destAccNum) {
            cout << "\n\tError: Source and destination accounts must be different.\n";
            return;
        }

        fstream file("accounts.dat", ios::binary | ios::in | ios::out);
        if(!file) {
            cout << "\n\tError: No accounts found!\n";
            return;
        }

        Account acc;
        streampos srcPos = -1, destPos = -1;
        Account srcAcc, destAcc;

        // Find both accounts 
        while(file.read((char*)&acc, sizeof(acc))) {
            streampos pos = file.tellg();
            pos -= (streamoff)sizeof(acc); // start of this record
            if(acc.getAccountNumber() == srcAccNum) {
                srcPos = pos;
                srcAcc = acc;
            }
            if(acc.getAccountNumber() == destAccNum) {
                destPos = pos;
                destAcc = acc;
            }
            if(srcPos != -1 && destPos != -1) break;
        }

        if(srcPos == -1) {
            cout << "\n\tError: Source account not found!\n";
            file.close();
            return;
        }
        if(destPos == -1) {
            cout << "\n\tError: Destination account not found!\n";
            file.close();
            return;
        }

        // Verify PIN for source account
        string entered;
        int attempts = 0;
        const int maxAttempts = 3;
        bool pinOk = false;
        while(attempts < maxAttempts) {
            cout << "\n\tEnter 4-digit PIN for source account: ";
            cin >> entered;
            if (entered.length() != 4) {
                cout << "\n\tError: PIN must be exactly 4 digits.\n";
                ++attempts;
                cout << "\tAttempts left: " << (maxAttempts - attempts) << endl;
                continue;
            }
            bool allDigits = true;
            for (char c : entered) if (!isdigit((unsigned char)c)) { allDigits = false; break; }
            if (!allDigits) {
                cout << "\n\tError: PIN must contain only digits.\n";
                ++attempts;
                cout << "\tAttempts left: " << (maxAttempts - attempts) << endl;
                continue;
            }
            if(srcAcc.checkPin(entered.c_str())) {
                pinOk = true;
                break;
            } else {
                ++attempts;
                cout << "\n\tError: Incorrect PIN. Attempts left: " << (maxAttempts - attempts) << endl;
            }
        }

        if(!pinOk) {
            cout << "\n\tError: Too many incorrect PIN attempts. Transfer cancelled.\n";
            file.close();
            return;
        }

        double amount;
        cout << "\n\tEnter amount to transfer: Rs. ";
        cin >> amount;
        if(amount <= 0) {
            cout << "\n\tError: Invalid amount!\n";
            file.close();
            return;
        }
        if(amount > /* available */ 1e18) { /* just a sanity check, not necessary */ }

        if(amount > /* check balance */ 0 && amount > /* placeholder */ 0) {
            // We'll check accurately against srcAcc.balance by trying withdrawAmountNoPrompt
        }

        // Check sufficient funds
        // We can't access balance directly here; simulate by attempting withdrawNoPrompt
        if (!srcAcc.withdrawAmountNoPrompt(amount)) {
            cout << "\n\tError: Insufficient balance in source account!\n";
            file.close();
            return;
        }

        // If we reach here, srcAcc has been decremented in-memory. Add amount to destAcc.
        destAcc.depositAmountNoPrompt(amount);

        // Write both records back to file at their positions
        // Write source
        file.clear(); // clear eof flag if set
        file.seekp(srcPos);
        file.write((char*)&srcAcc, sizeof(srcAcc));

        // Write destination
        file.seekp(destPos);
        file.write((char*)&destAcc, sizeof(destAcc));

        file.close();

        cout << "\n\t✓ Transfer successful!\n";
        cout << "\tTransferred Rs. " << fixed << setprecision(2) << amount << " from account "
            << srcAccNum << " to account " << destAccNum << ".\n";
    }

    // checkBalance and displayAllAccounts unchanged
    void checkBalance() {
        int accNum;
        bool found = false;

        cout << "\n\t=== CHECK BALANCE ===\n";
        cout << "\tEnter Account Number: ";
        cin >> accNum;

        fstream file;
        file.open("accounts.dat", ios::binary | ios::in);

        if(!file) {
            cout << "\n\tError: No accounts found!\n";
            return;
        }

        Account acc;
        while(file.read((char*)&acc, sizeof(acc))) {
            if(acc.getAccountNumber() == accNum) {
                acc.displayAccount();
                found = true;
                break;
            }
        }

        file.close();

        if(!found) {
            cout << "\n\tError: Account not found!\n";
        }
    }

    void displayAllAccounts() {
        fstream file;
        file.open("accounts.dat", ios::binary | ios::in);

        if(!file) {
            cout << "\n\tError: No accounts found!\n";
            return;
        }

        cout << "\n\t=== ALL ACCOUNTS ===\n\n";
        cout << "\t" << setw(10) << "Acc No"
            << setw(25) << "Name"
            << setw(15) << "Balance" << endl;
        cout << "\t" << string(50, '-') << endl;

        Account acc;
        bool hasAccounts = false;

        while(file.read((char*)&acc, sizeof(acc))) {
            acc.displayList();
            hasAccounts = true;
        }

        if(!hasAccounts) {
            cout << "\n\tNo accounts available.\n";
        }

        cout << "\t" << string(50, '-') << endl;

        file.close();
    }
