#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace gsa{
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
        double center_x;
        double center_y;
        double center_z;
    };

    bool is_empty(std::ifstream& pFile)
    {
        return pFile.peek() == std::ifstream::traits_type::eof();
    }

    double calculateDistance(Atom a1, Atom a2) {
        return std::sqrt(std::pow(a2.center_x - a1.center_x, 2) + std::pow(a2.center_y - a1.center_y, 2) + std::pow(a2.center_z - a1.center_z, 2));
    }

    // Function for obtaining values from the specified column and writing them to the Atom structure
    std::vector<Atom> getAtomsFromTSV(std::istream& input_stream) {
        std::vector<std::string> headers;
        std::vector<Atom> atoms;
        std::string line;

        // Read the first line to get the headers
        if (std::getline(input_stream, line)) {
            std::stringstream lineStream(line);
            std::string token;
            while (std::getline(lineStream, token, '\t')) {
                headers.push_back(token);
            }
        }

        std::map<std::string, std::size_t> header_index;
        for (std::size_t i = 0; i < headers.size(); ++i) {
            if(header_index.count(headers[i]) > 0){
                throw std::runtime_error("Coloumn name repetition is not permitted in this program!");
            }
            header_index[headers[i]] = i;
        }

        while (std::getline(input_stream, line)) {
            std::vector<std::string> tokens;
            {
            std::stringstream lineStream(line);
            std::string token;

            // Splitting a row into tokens
            while (std::getline(lineStream, token, '\t')) {
                tokens.push_back(token);
            }
            }

            if (tokens.size() != headers.size()) {
                throw std::runtime_error("Mismatch between number of headers and number of columns in a line");
            }

            Atom atom;
            atom.atom_name = tokens.at(header_index.at("ID_name"));
            atom.residueID.chainID = tokens.at(header_index.at("ID_chainID"));
            atom.residueID.resSeq = std::stoi(tokens.at(header_index.at("ID_resSeq")));
            atom.center_x = std::stod(tokens.at(header_index.at("center_x")));
            atom.center_y = std::stod(tokens.at(header_index.at("center_y")));
            atom.center_z = std::stod(tokens.at(header_index.at("center_z")));

            atoms.push_back(atom);
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

    void findAndSaveShortestDistances(const std::map<ResidueID, std::vector<Atom> >& groupedAtoms, std::ostream& output_file) {
        if (groupedAtoms.empty()) {
            throw std::runtime_error("The file is empty!");
        }

        // Write headers to the output file
        output_file << "ID1_chainID\tID1_resSeq\tID2_chainID\tID2_resSeq\tdistance\n";
        typedef std::map< ResidueID, std::vector< Atom > >::const_iterator GroupIterator;

        for (GroupIterator it1 = groupedAtoms.begin(); it1 != groupedAtoms.end(); ++it1) {
            for (GroupIterator it2 = std::next(it1); it2 != groupedAtoms.end(); ++it2) {
                {
                    {
                    const std::vector<Atom>& atoms1 = it1->second;
                    const std::vector<Atom>& atoms2 = it2->second;

                    double min_distance = std::numeric_limits<double>::max();

                    for (const Atom& atom1 : atoms1) {
                        for (const Atom& atom2 : atoms2) {
                            double distance = calculateDistance(atom1, atom2);
                            min_distance=std::min(min_distance, distance);
                        }
                    }
                        if (min_distance <= 5.0) {
                            output_file << it1->first.chainID << "\t" << it1->first.resSeq << "\t"
                                << it2->first.chainID << "\t" << it2->first.resSeq << "\t"
                                << min_distance << "\n";
                        }
                    }
                }
            }
        }
    }
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
        if (gsa::is_empty(input_file)) {
            throw std::runtime_error("The file " + infile + " is empty!");
        }

        std::vector<std::string> headers;
        std::vector<gsa::Atom> atoms = gsa::getAtomsFromTSV(input_file);

        std::map<gsa::ResidueID, std::vector<gsa::Atom> > groupedAtoms = gsa::groupAtoms(atoms);
        {
            std::ofstream output_file(outfile);
            if(!output_file){
                throw std::runtime_error("Unable to open file: " + outfile);
            }
            findAndSaveShortestDistances(groupedAtoms, output_file);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...){
        std::cerr << "Unrecognizable error" << std::endl;
    }

    return 0;
}