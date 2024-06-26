//
//  main.cpp
//  Internship-project-ljk
//
//  Created by Mak-Bet on 26/06/2024.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

// Function to retrieve values from a specified column
vector<string> getColumnValuesFromTSV(const string& filename, int col) {
    ifstream file(filename);
    vector<string> columnValues;

    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return columnValues;
    }

    string line;
    bool isFirstLine = true;

    while (getline(file, line)) {
        if (isFirstLine) {
            // Skip the first line (header)
            isFirstLine = false;
            continue;
        }

        stringstream lineStream(line);
        string cell;
        int currentCol = 0;

        while (lineStream >> cell) {
            if (currentCol == col) {
                columnValues.push_back(cell);
                break;
            }
            currentCol++;
        }
    }

    return columnValues;
}

//Initialisation of the atom structure (can be further developed)
struct Atom {
    char chain;
    int atom_index;
    int res_index;
    float center_x;
    float center_y;
    float center_z;
};

int main() {
    string filename;
    int n;
    Atom ar[n];
    cout <<"Type the path to the file:"<<endl;
    cin >> filename;

    vector<float> center_x_Variables;
    vector<float> center_y_Variables;
    vector<float> center_z_Variables;

    int col = 10; // coloumn number
    vector<string> values = getColumnValuesFromTSV(filename, col);

    // Assigning values to variables and adding them to the vector
    for (vector<string>::iterator i = values.begin(); i != values.end(); ++i) {
        // Here we can actually perform any operations with each column value
        string value = *i;
        center_x_Variables.push_back(stof(value));
    }

    col = 11; 
    values = getColumnValuesFromTSV(filename, col);

    for (vector<string>::iterator i = values.begin(); i != values.end(); ++i) {
        string value = *i;
        center_y_Variables.push_back(stof(value));
    }

    col = 12; 
    values = getColumnValuesFromTSV(filename, col);

    for (vector<string>::iterator i = values.begin(); i != values.end(); ++i) {
        string value = *i;
        center_z_Variables.push_back(stof(value));
    }

    n = center_x_Variables.size();

    //Ideally, it should assign the value of the vector element to a specific atom in the ar array, but this does not happen, an EXC_BAD_ACCESS error occurs. 
    for (int i = 0; i < n; ++i) {
        ar[i].atom_index = i;
        ar[i].center_x = center_x_Variables[i];
        ar[i].center_y = center_y_Variables[i];
        ar[i].center_z = center_z_Variables[i];
    }

    // Testing on random examples (still need to create more interesting ones)
    // cout <<"ar[10].center_x = "<< ar[10].center_x <<"\n"<<"ar[695].center_z = "<< ar[695].center_z <<"\n"<<"ar[808].center_y = "<< ar[808].center_y <<"\n"<<"ar[21].atom_index = "<< ar[21].atom_index<<endl;

    return 0;
}