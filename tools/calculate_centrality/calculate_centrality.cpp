#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <igraph.h>
#include <tsv_parcing.h>
#include <residue.h>

std::string findPrefix(std::string file_name) {
    std::string result;
    // Finding the position of the last '/' symbol
    size_t lastSlashPos = file_name.find_last_of('/');

    // Finding the position of the first '_' symbol
    size_t firstUnderscorePos = file_name.find('_');

    size_t dotPos = file_name.find_last_of('.');

    if (lastSlashPos == std::string::npos && firstUnderscorePos == std::string::npos) {
        // If neither '/' nor '_' are found, return the whole string
        result = file_name.substr(0, dotPos) + "_";
    } else if (lastSlashPos == std::string::npos) {
        // If '/' not found, return the string untill the first '_'
        result = file_name.substr(0, firstUnderscorePos + 1);
    } else if (firstUnderscorePos == std::string::npos) {
        // If '_' not found, return the string after the last '/'
        result = file_name.substr(lastSlashPos + 1, dotPos - lastSlashPos - 1) + "_";
    } else if (lastSlashPos < firstUnderscorePos) {
        // If both symbols are found and '/' goes before '_', return the string berween them
        result = file_name.substr(lastSlashPos + 1, firstUnderscorePos - lastSlashPos);
    } else {
        // If '_' goes before '/', return an empty string (unexpected case)
        result = "";
    }
    return result;
}

// Function for reading vertices data from TSV file
std::map<ResidueID, int> getVerticesFromTSV(std::istream& input_stream, std::vector<ResidueID>& vertices) {
    TSVData tsv_data;
    parseTSVHeadersAndRows(input_stream, tsv_data);

    std::map<std::string, std::size_t> header_index;
    for (std::size_t i = 0; i < tsv_data.headers.size(); ++i) {
        if (header_index.count(tsv_data.headers[i]) > 0) {
            throw std::runtime_error("Column name repetition is not permitted in this program!");
        }
        header_index[tsv_data.headers[i]] = i;
    }

    std::map<ResidueID, int> vertex_map;
    int vertex_id = 0;

    for (const std::vector<std::string>& row : tsv_data.rows) {
        ResidueID id;
        id.chainID = row.at(header_index.at("ID_chainID"));
        id.resSeq = std::stoi(row.at(header_index.at("ID_resSeq")));
        if (vertex_map.find(id) == vertex_map.end()) {
            vertex_map[id] = vertex_id++;
            vertices.push_back(id);
        }
    }

    return vertex_map;
}

// Function for reading data from TSV file
std::vector<interaction> getInteractionsFromTSV(std::istream& input_stream) {
    TSVData tsv_data;
    parseTSVHeadersAndRows(input_stream, tsv_data);

    std::vector<interaction> edges;

    std::map<std::string, std::size_t> header_index;
    for (std::size_t i = 0; i < tsv_data.headers.size(); ++i) {
        if (header_index.count(tsv_data.headers[i]) > 0) {
            throw std::runtime_error("Column name repetition is not permitted in this program!");
        }
        header_index[tsv_data.headers[i]] = i;
    }

    for (const std::vector<std::string> tokens : tsv_data.rows) {
        if (tokens.size() != tsv_data.headers.size()) {
            throw std::runtime_error("Mismatch between number of headers and number of columns in a line");
        }

        interaction inter;
        ResidueID id1, id2;
        id1.chainID = tokens.at(header_index.at("ID1_chainID"));
        id1.resSeq = std::stoi(tokens.at(header_index.at("ID1_resSeq")));
        id2.chainID = tokens.at(header_index.at("ID2_chainID"));
        id2.resSeq = std::stoi(tokens.at(header_index.at("ID2_resSeq")));

        inter.edge = std::make_pair(id1, id2);

        inter.distance = std::stod(tokens.at(header_index.at("distance")));

        if (header_index.find("area") != header_index.end()) {
            inter.area = std::stod(tokens.at(header_index.at("area")));
        }

        edges.push_back(inter);
    }

    return edges;
}

int main(int argc, char* argv[]) {
    try{
        if (argc != 4) {
            std::cerr << "Usage: " << argv[0] << " : too few arguments!\n";
            return 1;
        }

        std::string vertices_file = argv[1];
        std::string edges_file = argv[2];
        std::string output_file = argv[3];
        std::ifstream vertices_input(vertices_file);
        std::string prefix = findPrefix(output_file);
        if (!vertices_input) {
            throw std::runtime_error("Unable to open file: " + vertices_file);
        }
        if (is_empty(vertices_input)){
            throw std::runtime_error("The file " + vertices_file + " is empty!");
        }

        // Creating a mapping for unique vertices
        std::vector<ResidueID> vertices;
        std::map<ResidueID, int> vertex_map = getVerticesFromTSV(vertices_input, vertices);

        std::ifstream edges_input(edges_file);
        if (!edges_input) {
            throw std::runtime_error("Unable to open file: " + edges_file);
        }
        if (is_empty(edges_input)) {
            throw std::runtime_error("The file " + edges_file + " is empty!");
        }

        std::vector<interaction> edges = getInteractionsFromTSV(edges_input);
        edges_input.close();

        // Creating a graph
        igraph_t graph;
        igraph_vector_int_t edge_vector;
        igraph_vector_int_init(&edge_vector, edges.size() * 2);

        for (size_t i = 0; i < edges.size(); ++i) {
            VECTOR(edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map[edges[i].edge.first]);
            VECTOR(edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map[edges[i].edge.second]);
        }

        igraph_create(&graph, &edge_vector, 0, IGRAPH_UNDIRECTED);
        igraph_vector_int_destroy(&edge_vector);

        // Applying centrality measures
        igraph_vector_t closeness, betweenness, pagerank, eigenvector;
        igraph_vector_int_t degree;
        igraph_vector_int_init(&degree, 0);
        igraph_vector_init(&closeness, 0);
        igraph_vector_init(&betweenness, 0);
        igraph_vector_init(&pagerank, 0);
        igraph_vector_init(&eigenvector, 0);

        igraph_degree(&graph, &degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
        igraph_closeness(&graph, &closeness, NULL, NULL, igraph_vss_all(), IGRAPH_ALL, NULL, false);
        igraph_betweenness(&graph, &betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, NULL);
        igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_ARPACK, &pagerank, NULL, igraph_vss_all(), 1, 0.85, NULL, NULL);
        igraph_eigenvector_centrality(&graph, &eigenvector, NULL, false, 0, NULL, NULL);

        // result output
        std::ofstream output(output_file);
        if (!output) {
            throw std::runtime_error("Unable to open output file: " + output_file);
        }

        output << "ID_chainID\tID_resSeq\t" << prefix << "Degree\t" << prefix << "Closeness\t" << prefix << "Betweenness\t" << prefix << "PageRank\t" << prefix << "Eigenvector\n";
        for (int i = 0; i < igraph_vector_int_size(&degree); i++) {
            output << vertices[i].chainID << "\t"
                   << vertices[i].resSeq << "\t"
                   << VECTOR(degree)[i] << "\t"
                   << VECTOR(closeness)[i] << "\t"
                   << VECTOR(betweenness)[i] << "\t"
                   << VECTOR(pagerank)[i] << "\t"
                   << VECTOR(eigenvector)[i] << "\n";
        }

        igraph_vector_int_destroy(&degree);
        igraph_vector_destroy(&closeness);
        igraph_vector_destroy(&betweenness);
        igraph_vector_destroy(&pagerank);
        igraph_vector_destroy(&eigenvector);
        igraph_destroy(&graph);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...){
        std::cerr << "Unrecognizable error" << std::endl;
    }

    return 0;
}