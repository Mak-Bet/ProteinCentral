#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <igraph.h>

struct TSVData {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string> > rows;
};

struct ResidueID {
    std::string chainID;
    int resSeq;

    bool operator<(const ResidueID& other) const {
        return chainID < other.chainID || (chainID == other.chainID && resSeq < other.resSeq);
    }
};

// Checking whether the file is empty
bool is_empty(std::ifstream& pFile) {
    return pFile.peek() == std::ifstream::traits_type::eof();
}

// Function for parsing TSV headers and rows using the new structure
void parseTSVHeadersAndRows(std::istream& input_stream, TSVData& tsv_data) {
    std::string line;

    // Read the first line to get the headers
    if (std::getline(input_stream, line)) {
        std::stringstream lineStream(line);
        std::string token;
        while (std::getline(lineStream, token, '\t')) {
            tsv_data.headers.push_back(token);
        }
    }

    // Read the rest of the lines to get the rows
    while (std::getline(input_stream, line)) {
        std::vector<std::string> tokens;
        std::stringstream lineStream(line);
        std::string token;
        while (std::getline(lineStream, token, '\t')) {
            tokens.push_back(token);
        }
        tsv_data.rows.push_back(tokens);
    }
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
std::vector<std::pair<ResidueID, ResidueID> > getEdgesFromTSV(std::istream& input_stream) {
    TSVData tsv_data;
    parseTSVHeadersAndRows(input_stream, tsv_data);

    std::vector<std::pair<ResidueID, ResidueID> > edges;

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
        ResidueID id1, id2;
        id1.chainID = tokens.at(header_index.at("ID1_chainID"));
        id1.resSeq = std::stoi(tokens.at(header_index.at("ID1_resSeq")));
        id2.chainID = tokens.at(header_index.at("ID2_chainID"));
        id2.resSeq = std::stoi(tokens.at(header_index.at("ID2_resSeq")));

        edges.push_back(std::make_pair(id1, id2));
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

        std::vector<std::pair<ResidueID, ResidueID> > edges = getEdgesFromTSV(edges_input);
        edges_input.close();

        // Creating a graph
        igraph_t graph;
        igraph_vector_int_t edge_vector;
        igraph_vector_int_init(&edge_vector, edges.size() * 2);

        for (size_t i = 0; i < edges.size(); ++i) {
            VECTOR(edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map[edges[i].first]);
            VECTOR(edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map[edges[i].second]);
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

        output << "ID_chainID\tID_resSeq\tDegree\tCloseness\tBetweenness\tPageRank\tEigenvector\n";
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