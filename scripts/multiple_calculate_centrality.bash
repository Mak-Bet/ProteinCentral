#!/bin/bash
base_path="/Users/makarbetlei/Documents/ProteinCentral"

find "$base_path/output/graphs_data" -type f -name "residue_graph_nodes.tsv" | while read -r vertices_file; do
    foldername=$(basename "$(dirname "$vertices_file")")
    edges_file="output/graphs_data/${foldername}/dist_based_residue_graph_links.tsv"
    output_file="output/graphs_data/${foldername}/dist_based_centrality_measures.tsv"
    
    echo "$vertices_file" "$edges_file" "$output_file"
done | xargs -L 1 -P 8 ./calculate_centrality