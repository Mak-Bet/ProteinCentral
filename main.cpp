#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


struct Atom {
    char chain;
    int atom_index;
    int res_index;
    float center_x;
    float center_y;
    float center_z;
};

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
            atom.atom_index = std::stoi(cells[7]);
            atom.res_index = std::stoi(cells[8]);
            atom.center_x = std::stof(cells[11]);
            atom.center_y = std::stof(cells[12]);
            atom.center_z = std::stof(cells[13]);

            atoms.push_back(atom);
        }
    }

    return atoms;
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

    std::cout <<"atoms[141].center_x = "<< atoms[141].center_x <<"\natoms[811].chain = "<< atoms[811].chain <<"\natoms[2].center_z = "<< atoms[2].center_z <<"\natoms[2].res_index = "<< atoms[2].res_index;

    return 0;
}