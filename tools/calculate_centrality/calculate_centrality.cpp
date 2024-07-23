#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <igraph.h>
#include <tsv_parsing.h>
#include <residue.h>

// Function for reading vertices data from TSV file
std::map<ResidueID, int> getVerticesFromTSV(std::istream& input_stream, std::vector<ResidueID>& vertices) {
    TSVData tsv_data;
    parseTSVHeadersAndRows(input_stream, tsv_data);

    std::map<std::string, std::size_t> header_index;
    for (std::size_t i = 0; i < tsv_data.headers.size(); ++i) {
        header_index[tsv_data.headers[i]] = i;
    }

    std::map<ResidueID, int> vertex_map;
    int vertex_id = 0;

    for (const std::vector<std::string>& row : tsv_data.rows) {
        ResidueID id;
        id.chainID = row.at(header_index.at("ID_chainID"));
        id.resSeq = std::stoi(row.at(header_index.at("ID_resSeq")));
        id.interface_status = 0;
        if (vertex_map.find(id) == vertex_map.end()) {
            vertex_map[id] = vertex_id++;
            vertices.push_back(id);
        }
    }

    return vertex_map;
}

// Function for reading data from TSV file
std::vector<Interaction> getInteractionsFromTSV(std::istream& input_stream) {
    TSVData tsv_data;
    parseTSVHeadersAndRows(input_stream, tsv_data);

    std::vector<Interaction> edges;

    std::map<std::string, std::size_t> header_index;
    for (std::size_t i = 0; i < tsv_data.headers.size(); ++i) {
        header_index[tsv_data.headers[i]] = i;
    }

    for (const std::vector<std::string> tokens : tsv_data.rows) {
        Interaction inter;
        ResidueID id1, id2;
        inter.id1.chainID = tokens.at(header_index.at("ID1_chainID"));
        inter.id1.resSeq = std::stoi(tokens.at(header_index.at("ID1_resSeq")));
        inter.id2.chainID = tokens.at(header_index.at("ID2_chainID"));
        inter.id2.resSeq = std::stoi(tokens.at(header_index.at("ID2_resSeq")));
        inter.distance = std::stod(tokens.at(header_index.at("distance")));

        if (header_index.find("area") != header_index.end()) {
            inter.area = std::stod(tokens.at(header_index.at("area")));
        }

        edges.push_back(inter);
    }

    return edges;
}

void updateInterfaceStatus(const std::vector<Interaction>& edges, std::vector<ResidueID>& vertices, std::map<ResidueID, int>& vertex_map) {
    for (const Interaction& interaction : edges) {
        if (interaction.id1.chainID != interaction.id2.chainID) {
            // Update interface_status for both id1 and id2
            std::map<ResidueID, int>::iterator it1 = vertex_map.find(interaction.id1);
            std::map<ResidueID, int>::iterator it2 = vertex_map.find(interaction.id2);

            if (it1 != vertex_map.end()) {
                vertices[it1->second].interface_status = true;
            }

            if (it2 != vertex_map.end()) {
                vertices[it2->second].interface_status = true;
            }
        }
    }
}

std::vector<double> getDistanceWeights(std::vector<Interaction>& vect_inter) {
    std::vector<double> weights;
    for (const Interaction& inter : vect_inter) {
        weights.push_back(1. / (1 + inter.distance));
    }
    return weights;
}

std::vector<double> getAreaWeights(std::vector<Interaction>& vect_inter) {
    std::vector<double> weights;
    for (const Interaction& inter : vect_inter) {
        weights.push_back(inter.area);
    }
    return weights;
}

// GraphCentralityCalculator Class Definition
class GraphCentralityCalculator {
private:
    igraph_t graph;
    igraph_t area_graph;
    igraph_vector_int_t edge_vector;
    igraph_vector_t weight_vector;
    igraph_vector_int_t area_edge_vector;
    igraph_vector_t area_weight_vector;
    bool area_graph_initialized;

    // Centrality measure vectors
    igraph_vector_t unw_closeness, unw_betweenness, unw_pagerank, unw_eigenvector;
    igraph_vector_int_t unw_degree;
    igraph_vector_t dist_w_closeness, dist_w_betweenness, dist_w_pagerank, dist_w_eigenvector;
    igraph_vector_int_t dist_w_degree;
    igraph_vector_t area_w_closeness, area_w_betweenness, area_w_pagerank, area_w_eigenvector;
    igraph_vector_int_t area_w_degree;

public:
    GraphCentralityCalculator(const std::vector<Interaction>& edges,
                              const std::map<ResidueID, int>& vertex_map,
                              const std::vector<double>& dist_weights,
                              const std::vector<double>& area_weights)
        : area_graph_initialized(false) {
        // Initialize igraph vectors
        igraph_vector_int_init(&edge_vector, edges.size() * 2);
        igraph_vector_init(&weight_vector, dist_weights.size());

        for (size_t i = 0; i < edges.size(); ++i) {
            VECTOR(edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id1));
            VECTOR(edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id2));
            VECTOR(weight_vector)[i] = (dist_weights[i]);
        }

        // Create the graph
        igraph_create(&graph, &edge_vector, 0, IGRAPH_UNDIRECTED);
        igraph_vector_int_destroy(&edge_vector);

        // Initialize centrality vectors
        igraph_vector_int_init(&unw_degree, 0);
        igraph_vector_init(&unw_closeness, 0);
        igraph_vector_init(&unw_betweenness, 0);
        igraph_vector_init(&unw_pagerank, 0);
        igraph_vector_init(&unw_eigenvector, 0);
        igraph_vector_int_init(&dist_w_degree, 0);
        igraph_vector_init(&dist_w_closeness, 0);
        igraph_vector_init(&dist_w_betweenness, 0);
        igraph_vector_init(&dist_w_pagerank, 0);
        igraph_vector_init(&dist_w_eigenvector, 0);

        // Check if area weights are valid and initialize area graph
        if (!area_weights.empty() && (edges[0].area > 0.001) && (edges[0].area < 1000)) {
            area_graph_initialized = true;
            igraph_vector_int_init(&area_edge_vector, edges.size() * 2);
            igraph_vector_init(&area_weight_vector, area_weights.size());

            for (size_t i = 0; i < edges.size(); ++i) {
                VECTOR(area_edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id1));
                VECTOR(area_edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id2));
                VECTOR(area_weight_vector)[i] = (area_weights[i]);
            }

            igraph_create(&area_graph, &area_edge_vector, 0, IGRAPH_UNDIRECTED);
            igraph_vector_int_destroy(&area_edge_vector);

            igraph_vector_int_init(&area_w_degree, 0);
            igraph_vector_init(&area_w_closeness, 0);
            igraph_vector_init(&area_w_betweenness, 0);
            igraph_vector_init(&area_w_pagerank, 0);
            igraph_vector_init(&area_w_eigenvector, 0);
        }
    }

    ~GraphCentralityCalculator() {
        // Destroy vectors and graphs
        igraph_vector_int_destroy(&unw_degree);
        igraph_vector_destroy(&unw_closeness);
        igraph_vector_destroy(&unw_betweenness);
        igraph_vector_destroy(&unw_pagerank);
        igraph_vector_destroy(&unw_eigenvector);
        igraph_vector_int_destroy(&dist_w_degree);
        igraph_vector_destroy(&dist_w_closeness);
        igraph_vector_destroy(&dist_w_betweenness);
        igraph_vector_destroy(&dist_w_pagerank);
        igraph_vector_destroy(&dist_w_eigenvector);
        igraph_vector_destroy(&weight_vector);

        if (area_graph_initialized) {
            igraph_vector_int_destroy(&area_w_degree);
            igraph_vector_destroy(&area_w_closeness);
            igraph_vector_destroy(&area_w_betweenness);
            igraph_vector_destroy(&area_w_pagerank);
            igraph_vector_destroy(&area_w_eigenvector);
            igraph_vector_destroy(&area_weight_vector);
            igraph_destroy(&area_graph);
        }
        igraph_destroy(&graph);
    }
    void calculateCentrality() {
        igraph_degree(&graph, &unw_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
        igraph_closeness(&graph, &unw_closeness, nullptr, nullptr, igraph_vss_all(), IGRAPH_ALL, nullptr, false);
        igraph_betweenness(&graph, &unw_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, nullptr);
        igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_ARPACK, &unw_pagerank, nullptr, igraph_vss_all(), 1, 0.85, nullptr, nullptr);
        igraph_eigenvector_centrality(&graph, &unw_eigenvector, nullptr, false, 0, nullptr, nullptr);

        igraph_degree(&graph, &dist_w_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
        igraph_closeness(&graph, &dist_w_closeness, nullptr, nullptr, igraph_vss_all(), IGRAPH_ALL, &weight_vector, false);
        igraph_betweenness(&graph, &dist_w_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, &weight_vector);
        igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_ARPACK, &dist_w_pagerank, nullptr, igraph_vss_all(), 1, 0.85, &weight_vector, nullptr);
        igraph_eigenvector_centrality(&graph, &dist_w_eigenvector, nullptr, false, 0, &weight_vector, nullptr);

        if (area_graph_initialized) {
            igraph_degree(&area_graph, &area_w_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
            igraph_closeness(&area_graph, &area_w_closeness, nullptr, nullptr, igraph_vss_all(), IGRAPH_ALL, &area_weight_vector, false);
            igraph_betweenness(&area_graph, &area_w_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, &area_weight_vector);
            igraph_pagerank(&area_graph, IGRAPH_PAGERANK_ALGO_ARPACK, &area_w_pagerank, nullptr, igraph_vss_all(), 1, 0.85, &area_weight_vector, nullptr);
            igraph_eigenvector_centrality(&area_graph, &area_w_eigenvector, nullptr, false, 0, &area_weight_vector, nullptr);
        }
    }

    void outputResults(std::ostream& output, const std::vector<ResidueID>& vertices, const std::string& prefix) const {
        // Output centrality measures to the given output stream
        output << "ID_chainID\tID_resSeq\tInterface_status\t"
            << prefix << "unweighted_Degree\t"
            << prefix << "unweighted_Closeness\t"
            << prefix << "unweighted_Betweenness\t"
            << prefix << "unweighted_PageRank\t"
            << prefix << "unweighted_Eigenvector\t"
            << prefix << "distance_weighted_Degree\t"
            << prefix << "distance_weighted_Closeness\t"
            << prefix << "distance_weighted_Betweenness\t"
            << prefix << "distance_weighted_PageRank\t"
            << prefix << "distance_weighted_Eigenvector";
        if (area_graph_initialized) {
            output << "\t" << prefix << "area_weighted_Degree\t"
                << prefix << "area_weighted_Closeness\t"
                << prefix << "area_weighted_Betweenness\t"
                << prefix << "area_weighted_PageRank\t"
                << prefix << "area_weighted_Eigenvector";
        }
        output << "\n";

        for (int i = 0; i < igraph_vector_int_size(&unw_degree); i++) {
            output << vertices[i].chainID << "\t"
                << vertices[i].resSeq << "\t"
                << vertices[i].interface_status << "\t"
                << VECTOR(unw_degree)[i] << "\t"
                << VECTOR(unw_closeness)[i] << "\t"
                << VECTOR(unw_betweenness)[i] << "\t"
                << VECTOR(unw_pagerank)[i] << "\t"
                << VECTOR(unw_eigenvector)[i] << "\t"
                << VECTOR(dist_w_degree)[i] << "\t"
                << VECTOR(dist_w_closeness)[i] << "\t"
                << VECTOR(dist_w_betweenness)[i] << "\t"
                << VECTOR(dist_w_pagerank)[i] << "\t"
                << VECTOR(dist_w_eigenvector)[i];
            if (area_graph_initialized) {
                output << "\t" << VECTOR(area_w_degree)[i] << "\t"
                    << VECTOR(area_w_closeness)[i] << "\t"
                    << VECTOR(area_w_betweenness)[i] << "\t"
                    << VECTOR(area_w_pagerank)[i] << "\t"
                    << VECTOR(area_w_eigenvector)[i];
            }
            output << "\n";
        }
    }
};

int main(int argc, char* argv[]) {
    try{
        if (argc != 5) {
            std::cerr << "Usage: " << argv[0] << " : too few arguments!\n";
            return 1;
        }

        std::string vertices_file = argv[1];
        std::string edges_file = argv[2];
        std::string output_file = argv[3];
        std::string prefix = argv[4];
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

        std::vector<Interaction> edges = getInteractionsFromTSV(edges_input);

        std::vector<double> dist_weights = getDistanceWeights(edges);
        std::vector<double> area_weights = getAreaWeights(edges);

        updateInterfaceStatus(edges, vertices, vertex_map);
        
        GraphCentralityCalculator calculator(edges, vertex_map, dist_weights, area_weights);
        calculator.calculateCentrality();

        std::ofstream output(output_file);
        if (!output) {
        throw std::runtime_error("Unable to open output file: " + output_file);
        }
        calculator.outputResults(output, vertices, prefix);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...){
        std::cerr << "Unrecognizable error" << std::endl;
    }

    return 0;
}