#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <windows.h> 

using namespace std;

struct Student {
    string name;
    string id;
    int marks;
    int age;
};

void printStudent(const Student& record) {
    cout << "Name: " << record.name << endl;
    cout << "ID: " << record.id << endl;
    cout << "Marks: " << record.marks << endl;
    cout << "Age: " << record.age << endl;
}

void intializeDatabase() {
    ifstream checkFile("student.csv");
    if (checkFile) {
        cout << "Database verified." << endl;
        checkFile.close();
        return;
    }
    ofstream createFile("student.csv");
    createFile << "Name,ID,Marks,Age\n"; 
    createFile.close();
    cout << "Database created successfully." << endl;
}

bool isIDunique(string targetID){
    ifstream inFile("student.csv");
    if (!inFile) {
        cerr << "Error opening file!" << endl;
        return false;
    }
    string line;
    getline(inFile, line); 
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, id, marksStr, ageStr;
        if (getline(ss, name, ',') && getline(ss, id, ',') && getline(ss, marksStr, ',') && getline(ss, ageStr)) {
            if(id == targetID){
                inFile.close();
                return false; 
            }
        }
    }
    inFile.close();
    return true; 
}

void appendRecord(Student newRecord) {
    if (!isIDunique(newRecord.id)) {
        cout << "Error: Student ID must be unique. Record not added." << endl;
        return;
    }
    ofstream outFile("student.csv", ios::app); 
    outFile << newRecord.name << "," << newRecord.id << "," << newRecord.marks << "," << newRecord.age << "\n";
    outFile.close();
    cout << "Record added successfully." << endl;
}

void searchById(string searchID){
    ifstream inFile("student.csv");
    string line;
    getline(inFile, line); 
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, id, marksStr, ageStr;
        if (getline(ss, name, ',') && getline(ss, id, ',') && getline(ss, marksStr, ',') && getline(ss, ageStr)) {
            if(id == searchID){
                Student foundRecord = {name, id, stoi(marksStr), stoi(ageStr)};
                printStudent(foundRecord);
                inFile.close();
                return;
            }
        }
    }
    inFile.close();
    cout << "Student with ID " << searchID << " not found." << endl;
}

void updateRecord(string targetID, Student NewData){
    if(targetID != NewData.id && !isIDunique(NewData.id)) {
        cout << "Error: New student ID must be unique. Record not updated." << endl;
        return;
    }
    ifstream inFile("student.csv");
    ofstream tempFile("temp.csv");
    string line;
    bool recordFound = false;
    getline(inFile, line); 
    tempFile << line << endl;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, id, marksStr, ageStr;
        if (getline(ss, name, ',') && getline(ss, id, ',') && getline(ss, marksStr, ',') && getline(ss, ageStr)) {
            if(id == targetID){
                tempFile << NewData.name << "," << NewData.id << "," << NewData.marks << "," << NewData.age << "\n";
                recordFound = true;
            } else {
                tempFile << line << endl; 
            }
        }
    }
    inFile.close();
    tempFile.close();
    if (recordFound) {
        remove("student.csv"); 
        rename("temp.csv", "student.csv"); 
        cout << "Record updated successfully." << endl;
    } else {
        remove("temp.csv"); 
        cout << "Student with ID " << targetID << " not found. No records updated." << endl;
    }
}

void deleteRecord(string targetID){
    ifstream inFile("student.csv");
    ofstream tempFile("temp.csv");
    string line;
    bool recordFound = false;
    getline(inFile, line); 
    tempFile << line << endl;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, id, marksStr, ageStr;
        if (getline(ss, name, ',') && getline(ss, id, ',') && getline(ss, marksStr, ',') && getline(ss, ageStr)) {
            if(id == targetID){
                recordFound = true; 
            } else {
                tempFile << line << endl; 
            }
        }
    }
    inFile.close();
    tempFile.close();
    if (recordFound) {
        remove("student.csv"); 
        rename("temp.csv", "student.csv"); 
        cout << "Record deleted successfully." << endl;
    } else {
        remove("temp.csv"); 
        cout << "Student with ID " << targetID << " not found. No records deleted." << endl;
    }
}

void displayAllRecords(){
    ifstream inFile("student.csv");
    if(!inFile) {
        cerr << "Error opening file!" << endl;
        return; 
    }
    string line;
    getline(inFile, line); 
    while (getline(inFile, line)) {
        stringstream ss(line);
        string name, id, marksStr, ageStr;
        if (getline(ss, name, ',') && getline(ss, id, ',') && getline(ss, marksStr, ',') && getline(ss, ageStr)) {
            Student record = {name, id, stoi(marksStr), stoi(ageStr)};
            printStudent(record);
            cout << "-------------------" << endl;
        }
    }
    inFile.close();
}

string getIDFromESP32(const char* portName) {
    HANDLE hSerial = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "\n[!] Error: Could not open serial port. Is the ESP32 plugged in?" << endl;
        return "";
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    GetCommState(hSerial, &dcbSerialParams);
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    SetCommState(hSerial, &dcbSerialParams);

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50; 
    timeouts.ReadTotalTimeoutConstant = 50; 
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(hSerial, &timeouts);

    Sleep(2000); // Give the ESP32 time to boot

    DWORD bytesWritten;
    char request = 'R';
    WriteFile(hSerial, &request, 1, &bytesWritten, NULL);

    char szBuff[11] = { 0 };
    DWORD bytesRead;
    
    if (ReadFile(hSerial, szBuff, 10, &bytesRead, NULL)) {
        CloseHandle(hSerial);
        return string(szBuff); 
    }

    CloseHandle(hSerial);
    return "";
}

// ==========================================
//          NEW SECURE MAIN LOOP
// ==========================================
int main() {
    intializeDatabase();
    
    const char* myComPort = "\\\\.\\COM3"; 
    
    while(true) {
        cout << "\n=========================================" << endl;
        cout << "   SYSTEM LOCKED. PLEASE PLUG IN ESP32   " << endl;
        cout << "   Press Enter when device is connected. " << endl;
        cout << "=========================================" << endl;
        
        // Wait for the user to hit enter before scanning
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Scanning hardware key... Waiting for ESP32..." << endl;
        
        string currentSessionID = getIDFromESP32(myComPort);
        currentSessionID.erase(currentSessionID.find_last_not_of(" \n\r\t\0") + 1);

        // ==========================================
        //         ADMINISTRATOR LOOP (ID: 999)
        // ==========================================
        if (currentSessionID == "999") {
            cout << "\n[!] ADMIN LOGIN SUCCESSFUL [!]" << endl;
            bool loggedIn = true;
            
            while(loggedIn) {
                cout<<"\n--- ADMIN MENU ---"<<endl;
                cout<<"1: Add Student | 2: Search | 3: Update | 4: Delete | 5: View All | 6: Logout | 7: SHUTDOWN"<<endl;
                cout<<"Choice: ";
                
                int choice; 
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                
                switch(choice) {
                    case 1: {
                        Student newRecord;
                        cout << "Enter name: ";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        getline(cin, newRecord.name);
                        cout << "Enter ID: ";
                        cin >> newRecord.id;
                        cout << "Enter marks: ";
                        cin >> newRecord.marks;
                        cout << "Enter age: ";
                        cin >> newRecord.age;
                        appendRecord(newRecord);
                        break;
                    }
                    case 2: {
                        cout << "Enter ID to search: "; 
                        string searchID; 
                        cin >> searchID; 
                        searchById(searchID); 
                        break;
                    }
                    case 3: {
                        string targetID;
                        Student updatedData;
                        cout << "Enter ID of the record to update: ";
                        cin >> targetID;
                        cout << "Enter new name: ";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                        getline(cin, updatedData.name);
                        cout << "Enter new ID: ";
                        cin >> updatedData.id;
                        cout << "Enter new marks: ";
                        cin >> updatedData.marks;
                        cout << "Enter new age: ";
                        cin >> updatedData.age;
                        updateRecord(targetID, updatedData);
                        break;
                    }
                    case 4: {
                        string targetID;
                        cout << "Enter ID of the record to delete: ";
                        cin >> targetID;
                        deleteRecord(targetID);
                        break;
                    }
                    case 5:
                        displayAllRecords(); 
                        break;
                    case 6: 
                        cout << "Logging out Admin..." << endl; 
                        // ---> ADD THIS TO FIX THE BUG <---
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                        loggedIn = false; 
                        break;
                    case 7: 
                        cout << "[!] INITIATING COMPLETE SYSTEM SHUTDOWN [!]" << endl;
                        cout << "Closing database connections... Goodbye." << endl;
                        return 0; // This instantly kills the entire C++ program
                }
            }
        }
        
        // ==========================================
        //         STUDENT LOOP (ID: 123)
        // ==========================================
        else if (currentSessionID == "123") {
            cout << "\n[STUDENT LOGIN SUCCESSFUL]" << endl;
            cout << "Welcome, Faiz. You have restricted access." << endl;
            bool loggedIn = true;
            
            while(loggedIn) {
                cout<<"\n--- STUDENT MENU ---"<<endl;
                cout<<"1: View My Grades | 2: Logout"<<endl;
                cout<<"Choice: ";
                
                int choice; 
                if (!(cin >> choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                
                switch(choice) {
                    case 1: 
                        searchById("123"); 
                        break;
                    case 2: 
                        cout << "Logging out..." << endl; 
                        // ---> ADD THIS TO FIX THE BUG <---
                        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                        loggedIn = false; 
                        break;
                    default: 
                        cout << "Invalid choice." << endl;
                }
            }
        } 
        
        // ==========================================
        //         UNAUTHORIZED
        // ==========================================
        else if (!currentSessionID.empty()) {
            cout << "\n[ACCESS DENIED] Unrecognized hardware key: " << currentSessionID << endl;
        }
    }
    return 0;
}