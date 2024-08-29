#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <igraph.h>
#include <tsv_parsing.h>
#include <residue.h>

namespace protcentr{
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
std::vector<Interaction> getInteractionsFromTSV(std::istream& input_stream, int min_seq_separator) {
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
        if (inter.id1.chainID != inter.id2.chainID || std::abs(inter.id1.resSeq - inter.id2.resSeq) >= min_seq_separator){
            edges.push_back(inter);
        }
    }

    return edges;
}

void updateInterfaceStatus(const std::vector<Interaction>& edges, const std::map<ResidueID, int>& vertex_map, std::vector<ResidueID>& vertices) {
    for (const Interaction& interaction : edges) {
        if (interaction.id1.chainID != interaction.id2.chainID) {
            // Update interface_status for both id1 and id2
            std::map<ResidueID, int>::const_iterator it1 = vertex_map.find(interaction.id1);
            std::map<ResidueID, int>::const_iterator it2 = vertex_map.find(interaction.id2);

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
        weights.push_back(inter.distance);
    }
    return weights;
}

std::vector<double> getAreaWeights(std::vector<Interaction>& vect_inter) {
    std::vector<double> weights;
    for (const Interaction& inter : vect_inter) {
        weights.push_back(1. / (1 + inter.area));
    }
    return weights;
}

// GraphCentralityCalculator Class Definition
class GraphCentralityCalculator {
    public:
        GraphCentralityCalculator(const std::vector<Interaction>& edges,
                                const std::map<ResidueID, int>& vertex_map,
                                const std::vector<double>& dist_weights,
                                const std::vector<double>& area_weights)
            : area_graph_initialized(false) {
            // Initialize igraph vectors
            int error_code;
            error_code = igraph_vector_int_init(&edge_vector, edges.size() * 2);
            checkIgraphError(error_code, "Failed to initialize edge vector.");

            error_code = igraph_vector_init(&distance_weight_vector, dist_weights.size());
            checkIgraphError(error_code, "Failed to initialize weight vector.");

            for (size_t i = 0; i < edges.size(); ++i) {
                VECTOR(edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id1));
                VECTOR(edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id2));
                VECTOR(distance_weight_vector)[i] = (dist_weights[i]);
            }

            // Create the graph
            error_code = igraph_create(&graph, &edge_vector, 0, IGRAPH_UNDIRECTED);
            checkIgraphError(error_code, "Failed to create graph.");
            igraph_vector_int_destroy(&edge_vector);

            // Initialize centrality vectors
            error_code = igraph_vector_int_init(&unw_degree, 0);
            checkIgraphError(error_code, "Failed to initialize unw_degree vector.");

            error_code = igraph_vector_init(&unw_closeness, 0);
            checkIgraphError(error_code, "Failed to initialize unw_closeness vector.");

            error_code = igraph_vector_init(&unw_betweenness, 0);
            checkIgraphError(error_code, "Failed to initialize unw_betweenness vector.");

            error_code = igraph_vector_init(&unw_pagerank, 0);
            checkIgraphError(error_code, "Failed to initialize unw_pagerank vector.");

            error_code = igraph_vector_init(&unw_eigenvector, 0);
            checkIgraphError(error_code, "Failed to initialize unw_eigenvector vector.");

            error_code = igraph_vector_init(&unw_edge_betweenness, 0);
            checkIgraphError(error_code, "Failed to initialize unw_edge_betweenness vector.");


            error_code = igraph_vector_init(&dist_w_degree, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_degree vector.");

            error_code = igraph_vector_init(&dist_w_closeness, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_closeness vector.");

            error_code = igraph_vector_init(&dist_w_betweenness, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_betweenness vector.");

            error_code = igraph_vector_init(&dist_w_pagerank, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_pagerank vector.");

            error_code = igraph_vector_init(&dist_w_eigenvector, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_eigenvector vector.");

            error_code = igraph_vector_init(&dist_w_edge_betweenness, 0);
            checkIgraphError(error_code, "Failed to initialize dist_w_edge_betweenness vector.");


            // Check if area weights are valid and initialize area graph
            if (!area_weights.empty() && (edges[0].area > 0.0000000000000000001) && (edges[0].area < 100000)) {
                area_graph_initialized = true;
                error_code = igraph_vector_int_init(&area_edge_vector, edges.size() * 2);
                checkIgraphError(error_code, "Failed to initialize area_edge_vector.");

                error_code = igraph_vector_init(&area_weight_vector, area_weights.size());
                checkIgraphError(error_code, "Failed to initialize area_weight_vector.");

                for (size_t i = 0; i < edges.size(); ++i) {
                    VECTOR(area_edge_vector)[2 * i] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id1));
                    VECTOR(area_edge_vector)[2 * i + 1] = static_cast<igraph_integer_t>(vertex_map.at(edges[i].id2));
                    VECTOR(area_weight_vector)[i] = (area_weights[i]);
                }

                error_code = igraph_create(&area_graph, &area_edge_vector, 0, IGRAPH_UNDIRECTED);
                checkIgraphError(error_code, "Failed to create area graph.");
                igraph_vector_int_destroy(&area_edge_vector);

                error_code = igraph_vector_init(&area_w_degree, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_degree vector.");

                error_code = igraph_vector_init(&area_w_closeness, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_closeness vector.");

                error_code = igraph_vector_init(&area_w_betweenness, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_betweenness vector.");

                error_code = igraph_vector_init(&area_w_pagerank, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_pagerank vector.");

                error_code = igraph_vector_init(&area_w_eigenvector, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_eigenvector vector.");

                error_code = igraph_vector_init(&area_w_edge_betweenness, 0);
                checkIgraphError(error_code, "Failed to initialize area_w_edge_betweenness vector.");
            }
        }

        ~GraphCentralityCalculator() {
            // Destroy vectors and graphs
            igraph_vector_int_destroy(&unw_degree);
            igraph_vector_destroy(&unw_closeness);
            igraph_vector_destroy(&unw_betweenness);
            igraph_vector_destroy(&unw_pagerank);
            igraph_vector_destroy(&unw_eigenvector);
            igraph_vector_destroy(&unw_edge_betweenness);

            igraph_vector_destroy(&dist_w_degree);
            igraph_vector_destroy(&dist_w_closeness);
            igraph_vector_destroy(&dist_w_betweenness);
            igraph_vector_destroy(&dist_w_pagerank);
            igraph_vector_destroy(&dist_w_eigenvector);
            igraph_vector_destroy(&dist_w_edge_betweenness);
            igraph_vector_destroy(&distance_weight_vector);
            
            if (area_graph_initialized) {
                igraph_vector_destroy(&area_w_degree);
                igraph_vector_destroy(&area_w_closeness);
                igraph_vector_destroy(&area_w_betweenness);
                igraph_vector_destroy(&area_w_pagerank);
                igraph_vector_destroy(&area_w_eigenvector);
                igraph_vector_destroy(&area_w_edge_betweenness);
                igraph_vector_destroy(&area_weight_vector);
                igraph_destroy(&area_graph);
            }
            igraph_destroy(&graph);

        }
        void calculateCentrality() {
            int error_code;
            // Calculate centrality measures for unweighted and distance-weighted graphs
            error_code = igraph_degree(&graph, &unw_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
            checkIgraphError(error_code, "Failed to calculate unweighted degree.");

            error_code = igraph_closeness(&graph, &unw_closeness, NULL, NULL, igraph_vss_all(), IGRAPH_ALL, NULL, false);
            checkIgraphError(error_code, "Failed to calculate unweighted closeness.");

            error_code = igraph_betweenness(&graph, &unw_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, NULL);
            checkIgraphError(error_code, "Failed to calculate unweighted betweenness.");

            error_code = igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_ARPACK, &unw_pagerank, NULL, igraph_vss_all(), 1, 0.85, NULL, NULL);
            checkIgraphError(error_code, "Failed to calculate unweighted pagerank.");

            error_code = igraph_eigenvector_centrality(&graph, &unw_eigenvector, NULL, false, 0, NULL, NULL);
            checkIgraphError(error_code, "Failed to calculate unweighted eigenvector centrality.");

            error_code = igraph_edge_betweenness(&graph, &unw_edge_betweenness, IGRAPH_UNDIRECTED, NULL);
            checkIgraphError(error_code, "Failed to calculate unweighted edge betweenness centrality.");


            error_code = igraph_strength(&graph, &dist_w_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS, &distance_weight_vector);
            checkIgraphError(error_code, "Failed to calculate distance-weighted degree.");

            error_code = igraph_closeness(&graph, &dist_w_closeness, NULL, NULL, igraph_vss_all(), IGRAPH_ALL, &distance_weight_vector, false);
            checkIgraphError(error_code, "Failed to calculate distance-weighted closeness.");

            error_code = igraph_betweenness(&graph, &dist_w_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, &distance_weight_vector);
            checkIgraphError(error_code, "Failed to calculate distance-weighted betweenness.");

            error_code = igraph_pagerank(&graph, IGRAPH_PAGERANK_ALGO_ARPACK, &dist_w_pagerank, NULL, igraph_vss_all(), 1, 0.85, &distance_weight_vector, NULL);
            checkIgraphError(error_code, "Failed to calculate distance-weighted pagerank.");

            error_code = igraph_eigenvector_centrality(&graph, &dist_w_eigenvector, NULL, false, 0, &distance_weight_vector, NULL);
            checkIgraphError(error_code, "Failed to calculate distance-weighted eigenvector centrality.");

            error_code = igraph_edge_betweenness(&graph, &dist_w_edge_betweenness, IGRAPH_UNDIRECTED, &distance_weight_vector);
            checkIgraphError(error_code, "Failed to calculate distance-weighted edge betweenness centrality.");


            if (area_graph_initialized) {
                error_code = igraph_strength(&area_graph, &area_w_degree, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS, &area_weight_vector);
                checkIgraphError(error_code, "Failed to calculate area-weighted degree.");

                error_code = igraph_closeness(&area_graph, &area_w_closeness, NULL, NULL, igraph_vss_all(), IGRAPH_ALL, &area_weight_vector, false);
                checkIgraphError(error_code, "Failed to calculate area-weighted closeness.");

                error_code = igraph_betweenness(&area_graph, &area_w_betweenness, igraph_vss_all(), IGRAPH_UNDIRECTED, &area_weight_vector);
                checkIgraphError(error_code, "Failed to calculate area-weighted betweenness.");

                error_code = igraph_pagerank(&area_graph, IGRAPH_PAGERANK_ALGO_ARPACK, &area_w_pagerank, NULL, igraph_vss_all(), 1, 0.85, &area_weight_vector, NULL);
                checkIgraphError(error_code, "Failed to calculate area-weighted pagerank.");

                error_code = igraph_eigenvector_centrality(&area_graph, &area_w_eigenvector, NULL, false, 0, &area_weight_vector, NULL);
                checkIgraphError(error_code, "Failed to calculate area-weighted eigenvector centrality.");

                error_code = igraph_edge_betweenness(&graph, &area_w_edge_betweenness, IGRAPH_UNDIRECTED, &area_weight_vector);
                checkIgraphError(error_code, "Failed to calculate area-weighted edge betweenness centrality.");
            }

            // Calculate average edge betweenness for each vertex
            std::map<int, double> unw_vertex_betweenness_sum, dist_w_vertex_betweenness_sum;
            std::map<int, int> unw_vertex_edge_count, dist_w_vertex_edge_count;

            for (int i = 0; i < igraph_ecount(&graph); ++i) {
                igraph_integer_t from, to;
                igraph_edge(&graph, i, &from, &to);

                unw_vertex_betweenness_sum[from] += VECTOR(unw_edge_betweenness)[i];
                unw_vertex_betweenness_sum[to] += VECTOR(unw_edge_betweenness)[i];

                unw_vertex_edge_count[from]++;
                unw_vertex_edge_count[to]++;


                dist_w_vertex_betweenness_sum[from] += VECTOR(dist_w_edge_betweenness)[i];
                dist_w_vertex_betweenness_sum[to] += VECTOR(dist_w_edge_betweenness)[i];

                dist_w_vertex_edge_count[from]++;
                dist_w_vertex_edge_count[to]++;
            }

            for (const std::pair<const int, double>& pair : unw_vertex_betweenness_sum) {
                if (unw_vertex_edge_count[pair.first] <= 0){
                    unw_average_betweenness_per_vertex[pair.first] = 0;
                }
                else{
                    unw_average_betweenness_per_vertex[pair.first] = pair.second / unw_vertex_edge_count[pair.first];
                }
            }
            for (const std::pair<const int, double>& pair : dist_w_vertex_betweenness_sum) {
                if (dist_w_vertex_edge_count[pair.first] <= 0){
                    dist_w_average_betweenness_per_vertex[pair.first] = 0;
                }
                else{
                    dist_w_average_betweenness_per_vertex[pair.first] = pair.second / dist_w_vertex_edge_count[pair.first];
                }
            }

            if (area_graph_initialized) {
                std::map<int, double> area_w_vertex_betweenness_sum;
                std::map<int, int> area_w_vertex_edge_count;

                for (int i = 0; i < igraph_ecount(&graph); ++i) {
                    igraph_integer_t from, to;
                    igraph_edge(&graph, i, &from, &to);

                    area_w_vertex_betweenness_sum[from] += VECTOR(area_w_edge_betweenness)[i];
                    area_w_vertex_betweenness_sum[to] += VECTOR(area_w_edge_betweenness)[i];

                    area_w_vertex_edge_count[from]++;
                    area_w_vertex_edge_count[to]++;
                }

                for (const std::pair<const int, double>& pair : area_w_vertex_betweenness_sum) {
                    if (area_w_vertex_edge_count[pair.first] <= 0){
                        area_w_average_betweenness_per_vertex[pair.first] = 0;
                    }
                    else{
                        area_w_average_betweenness_per_vertex[pair.first] = pair.second / area_w_vertex_edge_count[pair.first];
                    }
                }
            }
        }

        void outputResults(std::ostream& output, const std::vector<ResidueID>& vertices, const std::string& prefix) const {
            // Output centrality measures to the given output stream
            output << "ID_chainID\tID_resSeq\t"
                << prefix << "Interface_status\t"
                << prefix << "unweighted_Degree\t"
                << prefix << "unweighted_Closeness\t"
                << prefix << "unweighted_Betweenness\t"
                << prefix << "unweighted_PageRank\t"
                << prefix << "unweighted_Eigenvector\t"
                << prefix << "unweighted_average_Edge_Betweenness\t"
                << prefix << "distance_weighted_Degree\t"
                << prefix << "distance_weighted_Closeness\t"
                << prefix << "distance_weighted_Betweenness\t"
                << prefix << "distance_weighted_PageRank\t"
                << prefix << "distance_weighted_Eigenvector\t"
                << prefix << "distance_weighted_average_Edge_Betweenness";
            if (area_graph_initialized) {
                output << "\t" << prefix << "area_weighted_Degree\t"
                    << prefix << "area_weighted_Closeness\t"
                    << prefix << "area_weighted_Betweenness\t"
                    << prefix << "area_weighted_PageRank\t"
                    << prefix << "area_weighted_Eigenvector\t"
                    << prefix << "area_weighted_average_Edge_Betweenness";
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
                    << unw_average_betweenness_per_vertex.at(i) << "\t"
                    << VECTOR(dist_w_degree)[i] << "\t"
                    << VECTOR(dist_w_closeness)[i] << "\t"
                    << VECTOR(dist_w_betweenness)[i] << "\t"
                    << VECTOR(dist_w_pagerank)[i] << "\t"
                    << VECTOR(dist_w_eigenvector)[i] << "\t"
                    << dist_w_average_betweenness_per_vertex.at(i);
                if (area_graph_initialized) {
                    output << "\t" << VECTOR(area_w_degree)[i] << "\t"
                                   << VECTOR(area_w_closeness)[i] << "\t"
                                   << VECTOR(area_w_betweenness)[i] << "\t"
                                   << VECTOR(area_w_pagerank)[i] << "\t"
                                   << VECTOR(area_w_eigenvector)[i] << "\t"
                                   << area_w_average_betweenness_per_vertex.at(i);
                }
                output << "\n";
            }
        }
    private:
        igraph_t graph;
        igraph_t area_graph;
        igraph_vector_int_t edge_vector;
        igraph_vector_t distance_weight_vector;
        igraph_vector_int_t area_edge_vector;
        igraph_vector_t area_weight_vector;
        bool area_graph_initialized;

        // Centrality measure vectors
        igraph_vector_t unw_closeness, unw_betweenness, unw_pagerank, unw_eigenvector, unw_edge_betweenness;
        igraph_vector_int_t unw_degree;
        igraph_vector_t dist_w_degree, dist_w_closeness, dist_w_betweenness, dist_w_pagerank, dist_w_eigenvector, dist_w_edge_betweenness;
        igraph_vector_t area_w_degree, area_w_closeness, area_w_betweenness, area_w_pagerank, area_w_eigenvector, area_w_edge_betweenness;

        std::map<int, double> unw_average_betweenness_per_vertex, dist_w_average_betweenness_per_vertex;
        std::map<int, double> area_w_average_betweenness_per_vertex;

        void checkIgraphError(int error_code, const std::string& message) {
        if (error_code != IGRAPH_SUCCESS) {
            throw std::runtime_error("igraph error (" + std::to_string(error_code) + "): " + message);
        }
    }
};
}

int main(int argc, char* argv[]) {
    try{
        if (argc != 6) {
            std::cerr << "Usage: " << argv[0] << " : too few arguments!\n";
            return 1;
        }

        std::string vertices_file = argv[1];
        std::string edges_file = argv[2];
        std::string output_file = argv[3];
        std::string prefix = argv[4];
        int min_seq_separator = std::stoi(argv[5]);
        std::ifstream vertices_input(vertices_file);
        if (!vertices_input) {
            throw std::runtime_error("Unable to open file: " + vertices_file);
        }
        if (is_empty(vertices_input)){
            throw std::runtime_error("The file " + vertices_file + " is empty!");
        }

        // Creating a mapping for unique vertices
        std::vector<protcentr::ResidueID> vertices;
        std::map<protcentr::ResidueID, int> vertex_map = protcentr::getVerticesFromTSV(vertices_input, vertices);

        std::ifstream edges_input(edges_file);
        if (!edges_input) {
            throw std::runtime_error("Unable to open file: " + edges_file);
        }
        if (is_empty(edges_input)) {
            throw std::runtime_error("The file " + edges_file + " is empty!");
        }

        std::vector<protcentr::Interaction> edges = protcentr::getInteractionsFromTSV(edges_input, min_seq_separator);

        std::vector<double> dist_weights = protcentr::getDistanceWeights(edges);
        std::vector<double> area_weights = protcentr::getAreaWeights(edges);

        protcentr::updateInterfaceStatus(edges, vertex_map, vertices);

        protcentr::GraphCentralityCalculator calculator(edges, vertex_map, dist_weights, area_weights);
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