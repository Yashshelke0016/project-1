#include <iostream>
#include <fstream>
#include <string>

using namespace std;
s
void writeToFile(const string& filename, const string& content) {
    // Open the file for writing. This will overwrite the existing file.
    ofstream outFile(filename);

    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        cout << "Data written to file: " << filename << endl;
    } else {
        cout << "Error opening file for writing!" << endl;
    }
}

void appendToFile(const string& filename, const string& content) {
    // Open the file for appending. This will not overwrite existing data.
    ofstream outFile(filename, ios::app);

    if (outFile.is_open()) {
        outFile << content;
        outFile.close();
        cout << "Data appended to file: " << filename << endl;
    } else {
        cout << "Error opening file for appending!" << endl;
    }
}

void readFromFile(const string& filename) {
    // Open the file for reading
    ifstream inFile(filename);

    if (inFile.is_open()) {
        string line;
        cout << "Reading data from file: " << filename << endl;
        while (getline(inFile, line)) {
            cout << line << endl;
        }
        inFile.close();
    } else {
        cout << "Error opening file for reading!" << endl;
    }
}

int main() {
    string filename = "example.txt";  // Name of the file
    string content;

    // Writing data to the file
    content = "This is the first line of the file.\n";
    writeToFile(filename, content);

    // Appending data to the file
    content = "This is an appended line.\n";
    appendToFile(filename, content);

    // Reading data from the file
    readFromFile(filename);

    return 0;
}
