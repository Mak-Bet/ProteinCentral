//To do list: refine the getEdgesFromTSV function to call capitals by their name instead of number,    
//think of a way to name graph vertices by a pair (string, int) and not only by number,    
//think about writing information to a file
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <igraph.h>

// Function for separating a string by delimiter
std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Function for generating a unique vertex identifier based on chain and number
std::string generateVertexID(const std::string& chainID, int resSeq) {
    return chainID + "_" + std::to_string(resSeq);
}

// Function for reading data from tsv file
std::vector<std::pair<std::string, std::string> > getEdgesFromTSV(std::istream& input_stream) {
    std::vector<std::pair<std::string, std::string> > edges;
    std::string line;

    // Read the first line to get the headers
    if (std::getline(input_stream, line)) {
       // Do nothing, just skip the header
    }

    while (std::getline(input_stream, line)) {
        std::vector<std::string> tokens = split(line, '\t');
        if (tokens.size() >= 4) {
            std::string id1 = generateVertexID(tokens[0], std::stoi(tokens[1]));
            std::string id2 = generateVertexID(tokens[2], std::stoi(tokens[3]));
            edges.push_back(std::make_pair(id1, id2));
        }
    }
    return edges;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.tsv>" << std::endl;
        return 1;
    }

    std::string file_path = argv[1];
    std::ifstream input_file(file_path);
    if (!input_file) {
        std::cerr << "Unable to open file: " << file_path << std::endl;
        return 1;
    }

    std::vector<std::pair<std::string, std::string> > edges = getEdgesFromTSV(input_file);

    // Creating a mapping for unique vertices
    std::unordered_map<std::string, int> vertex_map;
    std::vector<std::string> vertex_names;
    int vertex_id = 0;
    for (const std::pair<std::string, std::string>& edge : edges) {
        if (vertex_map.find(edge.first) == vertex_map.end()) {
            vertex_map[edge.first] = vertex_id++;
            vertex_names.push_back(edge.first);
        }
        if (vertex_map.find(edge.second) == vertex_map.end()) {
            vertex_map[edge.second] = vertex_id++;
            vertex_names.push_back(edge.second);
        }
    }

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
    std::cout << "Degree Centrality:" << std::endl;
    for (int i = 0; i < igraph_vector_int_size(&degree); i++) {
        std::cout << "Degree Centrality of vertex " << i << " = " << VECTOR(degree)[i] << "\n";
    }
    std::cout << std::endl;

    std::cout << "Closeness Centrality:" << std::endl;
    for (int i = 0; i < igraph_vector_size(&closeness); i++) {
        std::cout << "Closeness Centrality of vertex " << i << " = " << VECTOR(closeness)[i] << "\n";
    }
    std::cout << std::endl;

    std::cout << "Betweenness Centrality:" << std::endl;
    for (int i = 0; i < igraph_vector_size(&betweenness); i++) {
        std::cout << "Betweenness Centrality of vertex " << i << " = " << VECTOR(betweenness)[i] << "\n";
    }
    std::cout << std::endl;

    std::cout << "PageRank:" << std::endl;
    for (int i = 0; i < igraph_vector_size(&pagerank); i++) {
        std::cout << "PageRank Centrality of vertex " << i << " = " << VECTOR(pagerank)[i] << "\n";
    }
    std::cout << std::endl;

    std::cout << "Eigenvector Centrality:" << std::endl;
    for (int i = 0; i < igraph_vector_size(&eigenvector); i++) {
        std::cout << "Eigenvector Centrality of vertex " << i << " = " << VECTOR(eigenvector)[i] << "\n";
    }
    std::cout << "\nMaximum values of each algorithm and the vertex associated to it:\nDegree Centrality maximum: " << igraph_vector_int_max(&degree) << ", at vertex " << igraph_vector_int_which_max(&degree)
                                                                                   << "\nCloseness Centrality maximum: " << igraph_vector_max(&closeness) << ", at vertex " << igraph_vector_which_max(&closeness)
                                                                                   << "\nBetweenness Centrality maximum: " << igraph_vector_max(&betweenness) << ", at vertex " << igraph_vector_which_max(&betweenness)
                                                                                   << "\nPageRank Centrality maximum: " << igraph_vector_max(&pagerank) << ", at vertex " << igraph_vector_which_max(&pagerank)
                                                                                   << "\nEigenvector Centrality maximum: " << igraph_vector_max(&eigenvector) << ", at vertex " << igraph_vector_which_max(&eigenvector)
                                                                                   << std::endl;

    // Release of resources
    igraph_vector_int_destroy(&degree);
    igraph_vector_destroy(&closeness);
    igraph_vector_destroy(&betweenness);
    igraph_vector_destroy(&pagerank);
    igraph_vector_destroy(&eigenvector);
    igraph_destroy(&graph);

    return 0;
}