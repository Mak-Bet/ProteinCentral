#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>


struct Atom {
    char chain;
    int resSeq;
    float center_x;
    float center_y;
    float center_z;
};

float findDistance(Atom a1, Atom a2) {
    return std::sqrt(std::pow(a2.center_x - a1.center_x, 2) + std::pow(a2.center_y - a1.center_y, 2) + std::pow(a2.center_z - a1.center_z, 2));
}

// Function for obtaining values from the specified column and writing them to the Atom structure
std::vector<Atom> getAtomsFromTSV(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Atom> atoms;

    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return atoms;
    }

    std::string line;
    bool isFirstLine = true;

    while (std::getline(file, line)) {
        if (isFirstLine) {
            // Skip the first line (header)
            isFirstLine = false;
            continue;
        }

        std::stringstream lineStream(line);
        std::string cell;
        std::vector<std::string> cells;

        // Splitting a row into cells
        while (lineStream >> cell) {
            cells.push_back(cell);
        }

        if (cells.size() >= 15) {
            Atom atom;
            atom.chain = cells[0][0];
            atom.resSeq = std::stoi(cells[1]);
            atom.center_x = std::stof(cells[11]);
            atom.center_y = std::stof(cells[12]);
            atom.center_z = std::stof(cells[13]);

            atoms.push_back(atom);
        }
    }

    return atoms;
}

std::map<std::pair<char, int>, std::vector<Atom>> groupAtoms(const std::vector<Atom>& atoms) {
    std::map<std::pair<char, int>, std::vector<Atom>> groupedAtoms;
    
    for (const Atom& atom : atoms) {
        std::pair<char, int> key = std::make_pair(atom.chain, atom.resSeq);
        groupedAtoms[key].push_back(atom);
    }
    
    return groupedAtoms;
}

int main() {
    std::string filename;
    std::cout << "Type a path to the file:\n";
    std::cin >> filename;

    // Getting values and writing to components of the Atom structure
    std::vector<Atom> atoms = getAtomsFromTSV(filename);

    // Accessing and displaying variables after a loop
    // for (size_t i = 0; i < atoms.size(); ++i) {
    //     std::cout << "Atom " << i + 1 << ": "
    //               << atoms[i].chain << " "
    //               << atoms[i].atom_index << " "
    //               << atoms[i].res_index << " "
    //               << atoms[i].center_x << " "
    //               << atoms[i].center_y << " "
    //               << atoms[i].center_z << std::endl;
    // }

    // std::map<std::pair<char, int>, std::vector<Atom>> groupedAtoms = groupAtoms(atoms);

    // for (const auto& entry : groupedAtoms) {
    //     std::pair<char, int> key = entry.first;
    //     const std::vector<Atom>& group = entry.second;
    //     std::cout << "Key: (" << key.first << ", " << key.second << ")\n";
    //     for (const Atom& atom : group) {
    //         std::cout << "  Atom: { chain: " << atom.chain << ", resSeq: " << atom.resSeq
    //                   << ", center_x: " << atom.center_x << ", center_y: " << atom.center_y
    //                   << ", center_z: " << atom.center_z << " }\n";
    //     }
    // }

    std::cout << "the distance between atom 1 and atom 2 = " << findDistance(atoms[1], atoms[2]) <<"\n";
    
    return 0;
}