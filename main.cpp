#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

struct ResidueID{
    std::string chainID;
    int resSeq;

    bool operator<(const ResidueID& other) const {
        return chainID < other.chainID || (chainID == other.chainID && resSeq < other.resSeq);
    }
};

struct Atom {
    std::string atom_name;
    ResidueID residueID;
    float center_x;
    float center_y;
    float center_z;
};

float calculateDistance(Atom a1, Atom a2) {
    return std::sqrt(std::pow(a2.center_x - a1.center_x, 2) + std::pow(a2.center_y - a1.center_y, 2) + std::pow(a2.center_z - a1.center_z, 2));
}

// Function for obtaining values from the specified column and writing them to the Atom structure
std::vector< Atom > getAtomsFromTSV(std::istream& input_stream, std::vector<std::string>& headers) {
    std::vector< Atom > atoms;
    std::string line;
    
    // Read the first line to get the headers
    if (std::getline(input_stream, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        while (lineStream >> cell) {
            headers.push_back(cell);
        }
    }

    while (std::getline(input_stream, line)) {
        std::stringstream lineStream(line);
        std::string cell;
        std::vector< std::string > cells;

        // Splitting a row into cells
        while (lineStream >> cell) {
            cells.push_back(cell);
        }

        if (cells.size() >= 15) {
            Atom atom;
            atom.atom_name = cells[6];
            atom.residueID.chainID = cells[0];
            atom.residueID.resSeq = std::stoi(cells[1]);
            atom.center_x = std::stof(cells[11]);
            atom.center_y = std::stof(cells[12]);
            atom.center_z = std::stof(cells[13]);

            atoms.push_back(atom);
        }
    }

    return atoms;
}

std::map< ResidueID, std::vector< Atom > > groupAtoms(const std::vector<Atom>& atoms) {
    std::map< ResidueID, std::vector< Atom > > groupedAtoms;
    
    for (const Atom& atom : atoms) {
        groupedAtoms[atom.residueID].push_back(atom);
    }
    
    return groupedAtoms;
}

int main() {
    try {
        std::string filename;
        std::cout << "Enter the filename: ";
        std::cin >> filename;

        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + filename);
        }

        std::vector< std::string > headers;
        std::vector< Atom > atoms = getAtomsFromTSV(file, headers);
        file.close();

        std::map< ResidueID, std::vector< Atom > > groupedAtoms = groupAtoms(atoms);
        for (const auto& entry : groupedAtoms) {
        ResidueID key = entry.first;
        const std::vector<Atom>& value = entry.second;
        std::cout << "Key: (" << key.chainID << ", " << key.resSeq << ")\n";
        for (const Atom& atom : value) {
            std::cout << "  Atom: { name " << atom.atom_name << ", center_x: " << atom.center_x 
            << ", center_y: " << atom.center_y << ", center_z: " << atom.center_z << " }\n";
        }
        }


        // std::cout << "atoms.at(0).center_x = " << atoms.at(0).center_x << "\nCalculateDistance(atoms.at(0), atoms.at(4)) = " << calculateDistance(atoms.at(0), atoms.at(4)) << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}