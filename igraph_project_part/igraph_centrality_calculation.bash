#!/bin/bash
base_path="/Users/makarbetlei/Documents/Internship-project-ljk-1/voronota_results"

mkdir ../../igraph_centrality_results

find "$base_path" -type f -name "residue_graph_nodes.tsv" | while read -r vertices_file; do
    foldername=$(basename "$(dirname "$vertices_file")")
    edges_file="../../graph_generation_results/${foldername}_residue_links.tsv"
    output_file="../../igraph_centrality_results/${foldername}_centrality_measures.tsv"
    
    ./igraph_part "$vertices_file" "$edges_file" "$output_file"
done | xargs -L 1 -P 8 ./igraph_part