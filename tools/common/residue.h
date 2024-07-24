#ifndef RESIDUE_H
#define RESIDUE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <map>

namespace protcentr{
struct ResidueID {
    std::string chainID;
    int resSeq;
    bool interface_status;

    ResidueID(): resSeq(0), interface_status(0){

    }

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

struct Interaction{
    ResidueID id1;
    ResidueID id2;
    double distance;
    double area;

    Interaction(): distance(0.0), area(0.0){

    }
};

double calculateDistance(Atom a1, Atom a2) {
    return std::sqrt(std::pow(a2.center_x - a1.center_x, 2) + std::pow(a2.center_y - a1.center_y, 2) + std::pow(a2.center_z - a1.center_z, 2));
}

std::map< ResidueID, std::vector< Atom > > groupAtoms(const std::vector<Atom>& atoms) {
    std::map< ResidueID, std::vector< Atom > > groupedAtoms;

    for (const Atom& atom : atoms) {
        groupedAtoms[atom.residueID].push_back(atom);
    }

    return groupedAtoms;
}
}

#endif // RESIDUE_H