#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <vector>
using namespace std;


enum enUserPermissions { enTO_SHOW_CLIENT_LIST = 1, enTO_ADD_NEW_CLIENT = 2, enTO_DELETE_CLIENT = 4, enTO_UPDATE_CLIENT = 8, enTO_FIND_CLIENT = 16, enTO_MAKE_TRANSACTIONS = 32, enTO_MANAGE_USERS = 64 };

enum enClientChoice { enSHOW_CLIENT_LIST = 1, enADD_NEW_CLIENT = 2, enDELETE_CLIENT = 3, enUPDATE_CLIENT_INFO = 4, enFIND_CLIENT = 5, enTRANSACTION_LIST = 6, enMANAGE_USERS = 7, enLOG_OUT = 8 };

enum enUserChoice { enLIST_USERS = 1, enADD_NEW_USERS = 2, enDELETE_USERS = 3, enUPDATE_USERS = 4, enFIND_USERS = 5, enTO_MAIN_MENU };

enum enIsFound { enFOUND = 1, enNOT_FOUND = 2 };

enum enQuit { enEXIT_PROGRAM = 1, enSTAY = 2 };

enum enTransactionOption { enDEPOSIT = 1, enWITHDRAW = 2, enTOTAL_BALANCE = 3, enMAIN_MENU = 4 };

enum enWhichStruct { enstCLIENT = 0, enstUSER = 1 };


struct stClient {
    string accountNumber{};
    string pinCode{};
    string name{};
    string phonenumber{};
    double balance{};
    bool isDeleted{};
};


struct stUser {
    string name{};
    string password{};
    enUserPermissions permissions;
    int access{}; // get all permissions
    bool isDeleted{};
};

struct stFiles {
    string userFileName{ "users.txt" };
    string userDelimeter{ "&,&" };
    string clientFileName{ "clients.txt" };
    string clientDelimeter{ "/$/" };
};



void printClientCard(const stClient& CLIENT);
void printUserCard(const stUser& USER);
bool sayYes(string message);





string readString(string message) {
    cout << message << " ";
    string temp{};
    cin >> temp;
    return temp;
}

double readClientBalance(const string& message) {
    double temp{ 1 };
    bool isString{};
    do {
        cout << message << " ";
        cin >> temp;
        // validate string
        isString = cin.fail();

        if (isString) {
            // ignore
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "You can't enter a string!\n";
        }
        else if (temp < 0)
            cout << "You can't enter a negative number\n";
        else if (temp == 0)
            cout << "Balance can't be zero\n";
    } while (isString || temp <= 0);
    return temp;
}

// individual function
stClient readClientInfo(const string& accountNumber) {
    stClient client;
    client.accountNumber = accountNumber;
    cout << "Enter Pin Code: ";
    getline(cin >> ws, client.pinCode); // enable multiple reading
    cout << "Enter Name: ";
    getline(cin, client.name);
    cout << "Enter Phone Number: ";
    getline(cin, client.phonenumber);
    client.balance = readClientBalance("Enter Balance");

    return client;
}

void setUserPermissions(stUser& user) {
    user.access += (sayYes("Do you want him to show client list? (y-n)?")) ? 1 : 0;
    user.access += (sayYes("Do you want him to add a new client? (y-n)?")) ? 2 : 0;
    user.access += (sayYes("Do you want him to delete a client? (y-n)?")) ? 4 : 0;
    user.access += (sayYes("Do you want him to update a client? (y-n)?")) ? 8 : 0;
    user.access += (sayYes("Do you want him to search for a client? (y-n)?")) ? 16 : 0;
    user.access += (sayYes("Do you want him to make any transaction? (y-n)?")) ? 32 : 0;
    user.access += (sayYes("Do you want him to manage users? (y-n)?")) ? 64 : 0;
}

void setFullAccess(stUser& USER) {
    //USER.access += 1;
    //USER.access += 2;
    //USER.access += 4;
    //USER.access += 6;
    //USER.access += 8;
    //USER.access += 16;
    //USER.access += 32;
    //USER.access += 64;
    USER.access = 127;
}

// individual function
stUser readUserInfo(const string& userName) {
    stUser user{};
    user.name = userName;
    user.password = readString("Enter a password: ");

    if (sayYes("Do you want to give him full access?(y-n)? "))
        setFullAccess(user);
    else
        setUserPermissions(user);

    return user;
}


/*================================================================================================================*/

string convertstClientToLine(const stClient& client, const string& delimeter) {
    string record{ "" };
    record += client.accountNumber + delimeter;
    record += client.pinCode + delimeter;
    record += client.name + delimeter;
    record += client.phonenumber + delimeter;
    return record += to_string(client.balance);
}

string convertstUserToLine(const stUser& USER, const string& DELIMETER) {
    string temp{};
    temp += USER.name + DELIMETER;
    temp += USER.password + DELIMETER;
    return temp += to_string(USER.access);
}

/*================================================================================================================*/

bool writeClientsInFile(const vector<stClient>& vCLIENTS, const string& FILE_NAME, const string& DELIMETER) {
    // declare file
    fstream file;
    // open file in writing mode
    file.open(FILE_NAME, ios::out);

    string line{};
    if (file.is_open() && !vCLIENTS.empty()) {
        for (const stClient& CLIENT : vCLIENTS) {
            if (!CLIENT.isDeleted) {
                if ((line = convertstClientToLine(CLIENT, DELIMETER)) != "")
                    file << line << endl;
            }
        }
        file.close();
        return true;
    }
    else
        return false;
}

bool writeUsersInFile(const vector<stUser>& vUSERS, const string& FILE_NAME, const string& DELIMETER) {
    // declare file
    fstream file;
    // open file in writing mode
    file.open(FILE_NAME, ios::out);

    string line{};
    if (file.is_open() && !vUSERS.empty()) {
        for (const stUser& USER : vUSERS) {
            if (!USER.isDeleted) {
                if ((line = convertstUserToLine(USER, DELIMETER)) != "")
                    file << line << endl;
            }
        }
        file.close();
        return true;
    }
    else
        return false;
}

/*=================================================================================================================*/

vector<string> splitString(string line, const string& delimeter) {
    // define a container
    vector<string> vTokens;
    // locate delimeter
    int delimPos{};
    string token{};
    while ((delimPos = line.find(delimeter)) != std::string::npos) {
        // get token
        token = line.substr(0, delimPos);
        // ignore adjacent delimeters
        if (token != "")
            vTokens.push_back(token);
        // remove old token
        line.erase(0, delimPos + delimeter.length());
    }
    // ignore post last element delimeters
    if (line != "")
        vTokens.push_back(line);

    return vTokens;
}

// individual
stClient convertLineTostClient(vector<string>& vRecord) {
    vector<string>::iterator ptr = vRecord.begin();
    stClient client;
    client.accountNumber = *ptr++;
    client.pinCode = *ptr++;
    client.name = *ptr++;
    client.phonenumber = *ptr++;
    if (ptr != vRecord.end())
        client.balance = stod(*ptr);
    // no need for know isDelete state since we only need to delete at the run time, and if so, this record won't be even in the file!
    return client;
}

// individual
stUser convertLineTostUser(vector<string>& vRecord) {
    vector<string>::iterator ptr = vRecord.begin();
    stUser tempUser{};
    tempUser.name = *ptr++;
    tempUser.password = *ptr++;
    if (ptr != vRecord.end())
        tempUser.access = stoi(*ptr);
    return tempUser;
}

/*=================================== Reading =====================================================================*/

vector<stClient> readClientFile(const string& FILE_NAME, const string& DELIMETER) {
    fstream file;
    vector<string> vRecords{};
    // open file in reading mode
    file.open(FILE_NAME, ios::in);
    // define a container
    vector<stClient> vClients{};
    // begin reading
    string line{};
    if (file.is_open()) {
        while (getline(file, line)) {
            // safety measurement
            if (line != "") {
                vRecords = splitString(line, DELIMETER);
                vClients.push_back(convertLineTostClient(vRecords));
            }
        }
        file.close();
    }
    return vClients;
}

vector<stUser> readUserFile(const string& FILE_NAME, const string& DELIMETER) {
    fstream file;
    vector<string> vRecords{};
    file.open(FILE_NAME, ios::in);
    // define a container
    vector<stUser> vUsers{};
    // begin reading
    string line{};
    if (file.is_open()) {
        while (getline(file, line)) {
            // safety measurement
            if (line != "") {
                vRecords = splitString(line, DELIMETER);
                vUsers.push_back(convertLineTostUser(vRecords));
            }
        }
        file.close();
    }
    return vUsers;
}

/*====================================== validation ===============================================================*/

int getClientLocation(const vector<stClient>& vCLINETS, const string& ACCOUNT_NUMBER) {
    for (int i = 0; i < vCLINETS.size(); i++) {
        if (vCLINETS.at(i).accountNumber == ACCOUNT_NUMBER && !vCLINETS.at(i).isDeleted)
            return i;
    }
    return -1;
}

bool isClientExist(const vector<stClient>& vCLINETS, const string& ACCOUNT_NUMBER) {
    return (getClientLocation(vCLINETS, ACCOUNT_NUMBER) != -1);
}

int getUserLocation(const vector<stUser>& vUSERS, const string& USER_NAME) {
    for (int i = 0; i < vUSERS.size(); i++) {
        if (vUSERS.at(i).name == USER_NAME)
            return i;
    }
    return -1;
}

bool isUserExist(const vector<stUser>& vUSERS, const string& USER_NAME) {
    return (getUserLocation(vUSERS, USER_NAME) != -1);
}

bool findClient(const vector<stClient>& vCLINETS, const string& ACCOUNT_NUMBER) {
    int idx{};
    if ((idx = getClientLocation(vCLINETS, ACCOUNT_NUMBER)) != -1)
        printClientCard(vCLINETS.at(idx));
    else
        cout << "Client with account number " << ACCOUNT_NUMBER << " doesn't exist!\n";
    return idx != -1;
}

bool findUser(const vector<stUser>& vUSERS, const string& USER_NAME) {
    int idx{};
    if ((idx = getUserLocation(vUSERS, USER_NAME)) != -1)
        printUserCard(vUSERS.at(idx));
    else
        cout << "User with name " << USER_NAME << " doesn't exist!\n";
    return (idx != -1);
}

bool find(enWhichStruct whichStruct, const stFiles& FILES) {
    switch (whichStruct) {
    case enWhichStruct::enstCLIENT:
        return findClient(readClientFile(FILES.clientFileName, FILES.clientDelimeter), readString("Enter account number? "));
    case enWhichStruct::enstUSER:
        return findUser(readUserFile(FILES.userFileName, FILES.userDelimeter), readString("Enter user name? "));
    }
    return false;
}

/*================================================================================================================*/

// case insinsitive
bool isUpperChar(const char& c) {
    return (c >= 65 && c <= 90);
}

char toLowerChar(char c) {
    if (isUpperChar(c))
        return c += 32;
    else
        return c;
}

string toLowerString(string str) {
    string temp{};
    for (char c : str)
        temp += toLowerChar(c);
    return temp;
}

bool sayYes(string message) {
    cout << message << " ";
    char ans;
    cin >> ans;
    return((toLowerChar(ans)) == 'y');
}

/*=============================================== adding =====================================================================*/

bool addNewClient(vector<stClient> vClients, const string& FILE_NAME, const string& DELIMETER) {
    string key{};
    bool exist{ false };
    // make sure to add a new client
    do {
        // validate client doesn't exist
        key = readString("Enter an account number:");

        if (!(exist = isClientExist(vClients, key))) {
            // read a new client info
            vClients.push_back(readClientInfo(key));
            writeClientsInFile(vClients, FILE_NAME, DELIMETER);
        }
        else
            cout << "Client with account number " << key << " is already exist!\n";


    } while (exist || sayYes("Do you want to add more clietns?(y-n) "));

    return !exist;
}

bool addNewUser(vector<stUser> vUsers, const string& FILE_NAME, const string& DELIMETER) {
    string key{};
    bool exist{ false };
    // make sure to add a new client
    do {
        // validate client doesn't exist
        key = readString("Enter user name:");

        if (!(exist = isUserExist(vUsers, key))) {
            // read a new client info
            vUsers.push_back(readUserInfo(key));
            writeUsersInFile(vUsers, FILE_NAME, DELIMETER);
        }
        else
            cout << "User with name " << key << " is already exist!\n";


    } while (exist || sayYes("Do you want to add more clietns?(y-n) "));

    return !exist;
}

bool add(enWhichStruct whichStruct, const stFiles& FILES) {
    //stContainer tempContainer{};
    string key{};
    bool exist{};
    switch (whichStruct) {
    case enWhichStruct::enstCLIENT:
    {
        return addNewClient(readClientFile(FILES.clientFileName, FILES.clientDelimeter), FILES.clientFileName, FILES.clientDelimeter);
    }
    case enWhichStruct::enstUSER:
    {
        return addNewUser(readUserFile(FILES.userFileName, FILES.userDelimeter), FILES.userFileName, FILES.userDelimeter);

    }
    }
    return false;
}

/*================================================= printing ===================================================================*/

string repeatString(string str, int times) {
    string temp{};
    while (times--) {
        temp += str;
    }
    return temp;
}

void header(string message) {
    cout << repeatString("-", 20) << endl;
    cout << right << setw(15) << message << '\n';
    cout << repeatString("-", 20) << endl;
}

void clientListHeader(const int& numberOfClients) {
    cout << "\n" << right << setw(60) << "Client list " << "(" << numberOfClients << ") client(s)\n\n";
    cout << repeatString("_", 110) << "\n\n";
    cout << "| " << left << setw(15) << "Account number";
    cout << "| " << left << setw(15) << "Pin code";
    cout << "| " << left << setw(30) << "Name";
    cout << "| " << left << setw(25) << "Phone";
    cout << "| " << left << setw(15) << "Account balance\n";
    cout << repeatString("_", 110) << endl << endl;
}

void listClient(const stClient& client) {
    cout << "| " << left << setw(15) << client.accountNumber;
    cout << "| " << left << setw(15) << client.pinCode;
    cout << "| " << left << setw(30) << client.name;
    cout << "| " << left << setw(25) << client.phonenumber;
    cout << "| " << left << setw(15) << client.balance << endl;
}

void listClients(const vector<stClient>& vClients) {
    clientListHeader(vClients.size());
    for (const stClient& client : vClients) {
        listClient(client);
    }
    cout << "\n\n" << repeatString("_", 110) << "\n\n\n";

}

void printClientCard(const stClient& CLIENT) {
    cout << "\nThe following is the client details\n";
    cout << repeatString("-", 20) << endl;
    cout << left << setw(12) << "Account number" << ": " << CLIENT.accountNumber << '\n';
    cout << left << setw(14) << "Pin code" << ": " << CLIENT.pinCode << '\n';
    cout << left << setw(14) << "name" << ": " << CLIENT.name << '\n';
    cout << left << setw(14) << "Phone number" << ": " << CLIENT.phonenumber << '\n';
    cout << left << setw(14) << "Balance" << ": $" << CLIENT.balance << '\n';
    cout << repeatString("-", 20) << endl;
}

void userListHeader(const int& NUM_OF_USERS) {
    cout << "\n" << right << setw(60) << "User list " << "(" << NUM_OF_USERS << ") user(s)\n";
    cout << repeatString("_", 100) << "\n\n";
    cout << "| " << left << setw(15) << "User name";
    cout << "| " << left << setw(15) << "Password";
    cout << "| " << left << setw(10) << "Permissions\n";
    cout << repeatString("_", 100) << endl << endl;

}

void listUser(const stUser& USER) {
    cout << "| " << left << setw(15) << USER.name;
    cout << "| " << left << setw(15) << USER.password;
    cout << "| " << left << setw(15) << USER.access << repeatString("\n", 3);
}

void listUsers(const vector<stUser>& vUSERS) {
    userListHeader(vUSERS.size());
    for (const stUser& USER : vUSERS)
        listUser(USER);
}

void printUserCard(const stUser& USER) {
    cout << "\nThe following is the user details\n";
    cout << repeatString("-", 20) << endl;
    cout << left << setw(12) << "User name" << ": " << USER.name << endl;
    cout << left << setw(12) << "Password" << ": " << USER.password << endl;
    cout << left << setw(12) << "Permissions" << ": " << USER.access << endl;
    cout << repeatString("-", 20) << endl;
}

void list(enWhichStruct whichStruct, const stFiles& FILES) {
    switch (whichStruct) {
    case enWhichStruct::enstCLIENT:
        listClients(readClientFile(FILES.clientFileName, FILES.clientDelimeter));
        break;
    case enWhichStruct::enstUSER:
        listUsers(readUserFile(FILES.userFileName, FILES.userDelimeter));
        break;
    }
}

/*===================================================== delete =======================================================================*/

void markClientToDelete(stClient& client) {
    client.isDeleted = true;
}

void markUserToDelete(stUser& user) {
    user.isDeleted = true;
}

bool deleteClientFromFile(vector<stClient> vClients, const string& FILE_NAME, const string& DELIMETER) {

    if (vClients.empty()) {
        cout << "Vector is empty\n";
        return false;
    }




    string key{};
    int idx{};
    bool ansYes{};
    do {
        // get client account number
        key = readString("Enter an account number: ");
        // validate
        if ((idx = getClientLocation(vClients, key)) != -1) {

            printClientCard(vClients.at(idx));
            if (sayYes("Are you sure you wanna delete this clietn? (y-n) ")) {
                markClientToDelete(vClients.at(idx));
                // update file
                writeClientsInFile(vClients, FILE_NAME, DELIMETER);
                cout << "Client got deleted successfully, ";

            }
            else
                cout << "Nothing get deleted from file!\n";

            ansYes = sayYes("Do you want to delete more clietns?(y-n)");
        }
        else
            cout << "Client with account number " << key << " was not found\n";

    } while (idx == -1 || ansYes);

    return idx != -1;
}

bool deleteUserFromFile(vector<stUser> vUsers, const string& FILE_NAME, const string& DELIMETER) {
    if (vUsers.empty()) {
        cout << "Vector is empty!\n";
        return false;
    }

    string key{};
    int idx{};
    bool ansYes{};
    do {
        key = readString("Enter a user name ");
        if ((idx = getUserLocation(vUsers, key)) != -1) {

            // after knowing admin is recorded
            if (toLowerString(vUsers.at(idx).name) == "admin") {
                cout << "Admin can't be deleted!\n";
                return false;
            }

            printUserCard(vUsers.at(idx));

            if (sayYes("Are you sure you wanna delete this clietn? (y-n) ")) {
                markUserToDelete(vUsers.at(idx));
                // update file
                writeUsersInFile(vUsers, FILE_NAME, DELIMETER);
                cout << "User got deleted successfully, ";

            }
            else
                cout << "Nothing get deleted from file!\n";

            ansYes = sayYes("Do you want to delete more users?(y-n) ");
        }
        else
            cout << "User with name " << key << " was not found\n";
    } while (idx == -1 || ansYes);

    return idx != -1;
}

bool deleteFromFile(enWhichStruct whichStruct, const stFiles& FILES) {
    header("Deletion screen");

    switch (whichStruct) {
    case enWhichStruct::enstCLIENT: {
        // update clients
        return deleteClientFromFile(readClientFile(FILES.clientFileName, FILES.clientDelimeter), FILES.clientFileName, FILES.clientDelimeter);
    }
    case enWhichStruct::enstUSER: {
        return deleteUserFromFile(readUserFile(FILES.userFileName, FILES.userDelimeter), FILES.userFileName, FILES.userDelimeter);
    }

    }
    // for non-explicable reason
    return false;
}

/*=================================================== update =============================================================================*/

bool updateClientInfo(const stFiles& FILES) {
    string accountNumber = readString("Enter account number ");
    vector<stClient> vClietns = readClientFile(FILES.clientFileName, FILES.clientDelimeter);
    int location = getClientLocation(vClietns, accountNumber);
    if (location != -1) {
        // print client card
        printClientCard(vClietns.at(location));
        if (sayYes("Are you sure you want to update this client?(y-n) ")) {
            // update client info
            vClietns.at(location) = readClientInfo(accountNumber);
            // write new clients in file
            writeClientsInFile(vClietns, FILES.clientFileName, FILES.clientDelimeter);
        }
        else {
            "nothing happened!\n";
            return false;
        }
    }
    else
        cout << "Client with account number " << accountNumber << " doesn't exist!\n";

    return (location != -1);
}

bool updateUserInfo(const stFiles& FILES) {
    string userName = readString("Enter user name ");
    vector<stUser> vUsers = readUserFile(FILES.userFileName, FILES.userDelimeter);
    int location = getUserLocation(vUsers, userName);
    if (location != -1) {
        printUserCard(vUsers.at(location));
        if (sayYes("Are you sure you want to update this user?(y-n) ")) {
            vUsers.at(location) = readUserInfo(userName);
            writeUsersInFile(vUsers, FILES.userFileName, FILES.userDelimeter);

        }
        else {
            cout << "Nothing happened!\n";
            return false;
        }
    }
    else
        cout << "USer with name " << userName << " doesn't exist!\n";
    return (location != -1);
}

bool update(enWhichStruct whichStruct, const stFiles& FILES) {
    header("Update screen");
    switch (whichStruct) {
    case enWhichStruct::enstCLIENT:
        return updateClientInfo(FILES);
    case enWhichStruct::enstUSER:
        return updateUserInfo(FILES);
    }
    return false;
}

/*=============================================== transaction ====================================================================================*/

void mainMenuScreenTitle(const string& TITLE) {
    cout << repeatString("=", 45) << endl;
    cout << right << setw(32) << TITLE << endl;
    cout << repeatString("=", 45) << endl;
}

void mainMenuScreen(const stUser& whoLogedIn) {
    mainMenuScreenTitle("user " + whoLogedIn.name + " logged in");
    cout << repeatString("\t", 2) << "[1] Show client list." << endl;
    cout << repeatString("\t", 2) << "[2] Add new client." << endl;
    cout << repeatString("\t", 2) << "[3] Delete client." << endl;
    cout << repeatString("\t", 2) << "[4] Update client info." << endl;
    cout << repeatString("\t", 2) << "[5] Find client." << endl;
    cout << repeatString("\t", 2) << "[6] Transactions." << endl;
    cout << repeatString("\t", 2) << "[7] Manage users\n";
    cout << repeatString("\t", 2) << "[8] Logout." << endl;
    cout << repeatString("=", 45) << endl;
}

/*
* module 2 - list of options
*/

// read choice
int readInRange(const string& MESSAGE, const int& FROM, const int& TO) {
    int temp{ 1 };
    // validate string
    bool isString{};
    do {
        cout << MESSAGE << " ";
        cin >> temp;
        isString = cin.fail();

        if (isString) {
            // ignore error
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "You entered a string which is not valid!";
        }
        else if (temp < FROM || temp > TO)
            cout << "Number is out of range. Enter a number between (" << FROM << " and " << TO << ")\n";

    } while ((temp < FROM || temp > TO) || isString);

    return temp;
}

// int to enum
enClientChoice getUserOption(const string& MESSAGE, const int& FROM, const int& TO) {
    return (enClientChoice)readInRange(MESSAGE, FROM, TO);
}

enIsFound isFound(const stClient& client, const string& accountNumber) {
    return (client.accountNumber == accountNumber) ? enIsFound::enFOUND : enIsFound::enNOT_FOUND;
}

/*
* show transaction menu
*/

void transactionMenu() {
    mainMenuScreenTitle("Transactions Menu Screen");
    cout << repeatString("\t", 2) << "[1] Deposit." << endl;
    cout << repeatString("\t", 2) << "[2] Withdraw." << endl;
    cout << repeatString("\t", 2) << "[3] Total Balance." << endl;
    cout << repeatString("\t", 2) << "[4] Main menu." << endl;
}

enTransactionOption getTransactionOption(const int& FROM, const int& TO) {
    return (enTransactionOption)readInRange("Choose what do you want to do? (1 - 4)?", 1, 4);
}

/*
* Deposit
*/

// if account number exists, make a deposit

void makeADeposit(stClient& client, const double& AMOUNT) {
    client.balance += AMOUNT;
}

void depositOrWithdraw(vector<stClient>& vClients, const string& SCREEN_MESSAGE, bool deposit = true) {
    header(SCREEN_MESSAGE);
    string accountNumber{}, message{ "Enter account number?" };
    int index{};
    double amount{};
    do {
        accountNumber = readString(message);
        if ((index = getClientLocation(vClients, accountNumber)) != -1) {
            printClientCard(vClients.at(index));
            // make a withdrawal
            if (!deposit) {
                while ((amount = readClientBalance("Please, Enter amount?")) > vClients.at(index).balance) {
                    cout << "Amount exceeds the balance. You can withdraw up to " << vClients.at(index).balance << "\n";
                }
                amount *= -1;
            }
            else // make a deposit
                amount = readClientBalance("Please, Enter amount?");

            if (sayYes("Are you sure you want to perform this transaction? (y-n)?")) {
                makeADeposit(vClients.at(index), amount);
                cout << "Done successfully. New balance becomes $" << vClients.at(index).balance << "\n";
            }

        }
        else {
            cout << "Client with account number (" << accountNumber << ") doesn't exist. ";
            message = "Enter another account number";
        }


    } while (index == -1);
}

/*
* get total balance
*/

void briefClientListHeader(const int& NUM_OF_CLIENTS) {
    cout << "\n" << right << setw(60) << "Client list " << "(" << NUM_OF_CLIENTS << ") client(s)\n\n";
    cout << repeatString("_", 110) << "\n\n";
    cout << "| " << left << setw(15) << "Account number";
    cout << "| " << left << setw(30) << "Name";
    cout << "| " << left << setw(15) << "Account balance\n";
    cout << repeatString("_", 110) << endl << endl;
}

void showClientBriefInfo(const stClient& CLIENT) {
    cout << "| " << left << setw(15) << CLIENT.accountNumber;
    cout << "| " << left << setw(30) << CLIENT.name;
    cout << "| $" << left << setw(15) << CLIENT.balance << endl;
}

void showTotalBalance(const vector<stClient>& V_CLIENTS) {
    double sum{};
    briefClientListHeader(V_CLIENTS.size());
    for (const stClient& CLIENT : V_CLIENTS) {
        showClientBriefInfo(CLIENT);
        sum += CLIENT.balance;
    }
    cout << "\n\n" << right << setw(45) << "Total balance = $" << sum << endl;

}

/*
* start transactions
*/

void goBackSCreen() {
    cout << "Press any key to go back to the main menu... ";
    system("pause>0");
    system("cls");
}

void runTransaction(vector<stClient>& vClients, const string& FILE_NAME, const string& DELIMETER) {
    enQuit quit = enQuit::enSTAY;
    while (quit == enQuit::enSTAY) {
        // show transaction menu
        transactionMenu();
        switch (getTransactionOption(1, 4)) {
        case enTransactionOption::enDEPOSIT:
            system("cls");
            depositOrWithdraw(vClients, "Deposit screen");
            break;
        case enTransactionOption::enWITHDRAW:
            system("cls");
            depositOrWithdraw(vClients, "Withdrawal screen", false);
            break;
        case enTransactionOption::enTOTAL_BALANCE:
            system("cls");
            showTotalBalance(vClients);
            break;
        case enTransactionOption::enMAIN_MENU:
            system("cls");
            quit = enQuit::enEXIT_PROGRAM;
            break;
        default:
            return;
        }
        // update file
        writeClientsInFile(vClients, FILE_NAME, DELIMETER);

        if (quit != enQuit::enEXIT_PROGRAM)
            goBackSCreen();
    }
}
/*==================================================== logn in ================================================================================*/

// make it cnt 3 times to login!
bool login(const stFiles& FILES, stUser& whoLogedIn) {
    header("Login screen");
    int idx{};
    bool isHim{};
    string userName{}, password{};
    vector<stUser> vUsers = readUserFile(FILES.userFileName, FILES.userDelimeter);
    do {
        userName = readString("Enter user name ");
        password = readString("Enter password ");
        // exception may occur!
        /*if ((idx = getUserLocation(vUsers, userName)) != -1 && (vUsers.at(idx).password == password))
            return true;*/

        if ((idx = getUserLocation(vUsers, userName)) != -1) {
            if (isHim = (vUsers.at(idx).password == password)) {
                whoLogedIn = vUsers.at(idx);
                return true;
            }

        }

        if (idx == -1 || !isHim)
            cout << "Invalid userName or password!\n\n";

    } while (idx == -1 || !isHim);

    return idx != -1;

}
/*=========================================== manage user screen ========================================================================*/

void userChoicesMenu() {
    header("Manage user screen");
    cout << repeatString("\t", 2) << "[1] List users\n";
    cout << repeatString("\t", 2) << "[2] Add New Users\n";
    cout << repeatString("\t", 2) << "[3] Delete Users\n";
    cout << repeatString("\t", 2) << "[4] Update users\n";
    cout << repeatString("\t", 2) << "[5] Find users\n";
    cout << repeatString("\t", 2) << "[6] Main Menu\n";
}

enUserChoice getUserChoice() {
    return (enUserChoice)readInRange("Make a choice (1-6)", 1, 6);
}


void manageUsers(const stFiles& FILES, const stUser& whoLoggedIn) {
    enQuit quit = enQuit::enSTAY;
    while (quit != enQuit::enEXIT_PROGRAM) {
        userChoicesMenu();
        switch (getUserChoice()) {
        case enUserChoice::enLIST_USERS:
            system("cls");
            listUsers(readUserFile(FILES.userFileName, FILES.userDelimeter));
            break;
        case enUserChoice::enADD_NEW_USERS:
            system("cls");
            add(enWhichStruct::enstUSER, FILES);
            break;
        case enUserChoice::enDELETE_USERS:
            system("cls");
            deleteFromFile(enWhichStruct::enstUSER, FILES);
            break;
        case enUserChoice::enUPDATE_USERS:
            system("cls");
            update(enWhichStruct::enstUSER, FILES);
            break;
        case enUserChoice::enFIND_USERS:
            system("cls");
            find(enWhichStruct::enstUSER, FILES);
            break;
        case enUserChoice::enTO_MAIN_MENU: {
            system("cls");
            mainMenuScreen(whoLoggedIn);
            quit = enQuit::enEXIT_PROGRAM;
        }
                                         break;
        }
        if (quit != enQuit::enEXIT_PROGRAM)
            goBackSCreen();
    }
}

/*================================================================= user permissions ===========================================================================*/

//int convertEnumToNum(enUserPermissions userPermission) {
//    return int(userPermission);
//}


bool canUserDoThat(const int USER_ACCESS, int PERMISSION) {
    return (USER_ACCESS & PERMISSION) == PERMISSION;
}


/*================================================================ final ======================================================================================*/

void run(const stFiles& FILES, stUser whoLogedIn) {
    enQuit quit = enQuit::enSTAY;
    bool isintransactionmenu{};

    while (quit != enQuit::enEXIT_PROGRAM) {

        mainMenuScreen(whoLogedIn);

        switch (getUserOption("enter your choice", 1, 8))
        {
        case enClientChoice::enSHOW_CLIENT_LIST: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_SHOW_CLIENT_LIST)) {
                system("cls");
                list(enWhichStruct::enstCLIENT, FILES);
            }
            else
                cout << "Sorry, but you're not allowed to show clients list!\n";
        }
                                               break;

        case enClientChoice::enADD_NEW_CLIENT: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_ADD_NEW_CLIENT)) {
                system("cls");
                add(enWhichStruct::enstCLIENT, FILES);
            }
            else
                cout << "Sorry, but you're not allowed to add a new client!\n";
        }
                                             break;

        case enClientChoice::enDELETE_CLIENT: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_DELETE_CLIENT)) {
                system("cls");
                deleteFromFile(enWhichStruct::enstCLIENT, FILES);
            }
            else
                cout << "Sorry, but you're not allowed to delete any client!\n";
        }

                                            break;

        case enClientChoice::enUPDATE_CLIENT_INFO: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_UPDATE_CLIENT)) {
                system("cls");
                update(enWhichStruct::enstCLIENT, FILES);
            }
            else
                cout << "Sorry, but you're not allowed to update any client!\n";
        }

                                                 break;

        case enClientChoice::enFIND_CLIENT: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_FIND_CLIENT)) {
                system("cls");
                find(enWhichStruct::enstCLIENT, FILES);
            }
            else
                cout << "Sorry, but you're not allowed to search for clients!\n";
        }
                                          break;
        case enClientChoice::enTRANSACTION_LIST: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_MAKE_TRANSACTIONS)) {
                system("cls");
                vector<stClient> vClietns = readClientFile(FILES.clientFileName, FILES.clientDelimeter);
                runTransaction(vClietns, FILES.clientFileName, FILES.clientDelimeter);
                isintransactionmenu = true;
            }
            else
                cout << "Sorry, but you're not allowed to make transactions!\n";
        }
                                               break;

        case enClientChoice::enMANAGE_USERS: {
            if (canUserDoThat(whoLogedIn.access, enUserPermissions::enTO_MANAGE_USERS)) {
                system("cls");
                manageUsers(FILES, whoLogedIn);
            }
            else
                cout << "Sorry, but you're not allowed to manage users on the system!\n";
        }
                                           break;

        case enClientChoice::enLOG_OUT:
            system("cls");
            quit = enQuit::enEXIT_PROGRAM;
            break;

        default:
            return;
        }

        if (quit != enQuit::enEXIT_PROGRAM && !isintransactionmenu)
            goBackSCreen();

        isintransactionmenu = false;
    }
}


void start(stFiles files, stUser whoLoggedIn) {
    while (login(files, whoLoggedIn)) {
        system("cls");
        // get which user
        run(files, whoLoggedIn);
    }
}

int main()
{

    stFiles files{};
    stUser whoLogedIn{};
    start(files, whoLogedIn);


    system("pause > 0");
}
