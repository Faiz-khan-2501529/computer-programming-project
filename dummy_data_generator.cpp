#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // For random numbers
#include <ctime>   // For random seed

using namespace std;

int main() {
    // Seed the random number generator
    srand(time(0)); 

    ofstream outFile("student.csv");

    if (!outFile) {
        cerr << "Error: Could not create file!" << endl;
        return 1;
    }

    // 1. Write the mandatory CSV Header
    outFile << "Name,ID,Marks,Age\n";

    // 2. Write your specific Hardware Key test record first!
    outFile << "Faiz Khan,123,69,21\n";
    cout << "Added: Faiz Khan (Hardware Key Record)" << endl;

    // 3. Loop to generate 100 random students
    for (int i = 1; i <= 100; i++) {
        // Skip ID 123 so we don't accidentally make a duplicate
        if (i == 123) continue; 

        string name = "Student_" + to_string(i);
        string id = to_string(i);
        int marks = rand() % 101;       // Random marks between 0 and 100
        int age = (rand() % 5) + 18;    // Random age between 18 and 22

        outFile << name << "," << id << "," << marks << "," << age << "\n";
    }

    outFile.close();
    cout << "\nSuccess! student.csv is now populated with 101 records." << endl;
    cout << "You can now return to your main Gradebook program." << endl;

    return 0;
}