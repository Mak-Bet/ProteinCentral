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

bool is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

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

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            std::cerr << "Usage " << argv[0] << " : too few arguments!\n";
            return 1;
        }

        std::string infile = argv[1];
        std::string outfile = argv[2];

        // Open the input file in read mode
        std::ifstream input_file(infile);

        if (!input_file) {
            throw std::runtime_error("Unable to open file: " + infile);
        }
        if (is_empty(input_file)) {
            throw std::runtime_error("The file " + infile + " is empty!");
        }

        std::vector<std::string> headers;
        std::vector<Atom> atoms = getAtomsFromTSV(input_file, headers);
        input_file.close();

        std::map<ResidueID, std::vector<Atom> > groupedAtoms = groupAtoms(atoms);

        // Open the output file in write mode
        std::ofstream output_file(outfile);

        if (!output_file) {
            throw std::runtime_error("Unable to open file: " + outfile);
        }

        typedef std::map< ResidueID, std::vector< Atom > >::const_iterator GroupIterator;
        for (GroupIterator it1 = groupedAtoms.begin(); it1 != groupedAtoms.end(); ++it1) {
            for (GroupIterator it2 = std::next(it1); it2 != groupedAtoms.end(); ++it2) {
                if (it1->first.chainID == it2->first.chainID) {
                    const std::vector<Atom>& atoms1 = it1->second;
                    const std::vector<Atom>& atoms2 = it2->second;

                    for (const Atom& atom1 : atoms1) {
                        for (const Atom& atom2 : atoms2) {
                            float distance = calculateDistance(atom1, atom2);
                            if (distance <= 5.0) {
                                output_file << "Distance between (" << it1->first.chainID << ", " << it1->first.resSeq 
                                            << ") and (" << it2->first.chainID << ", " << it2->first.resSeq 
                                            << ") atoms " << atom1.atom_name << " and " << atom2.atom_name 
                                            << " is: " << distance << "\n";
                            }
                        }
                    }
                }
            }
        }

        output_file.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}