#!/bin/bash
base_path="/Users/makarbetlei/Documents/Internship-project-ljk-1"

mkdir ../../results/centrality_calculation_results

find "$base_path/results/voronota_results" -type f -name "residue_graph_nodes.tsv" | while read -r vertices_file; do
    foldername=$(basename "$(dirname "$vertices_file")")
    edges_file="../../results/graph_creation_results/${foldername}_residue_links.tsv"
    output_file="../../results/centrality_calculation_results/${foldername}_centrality_measures.tsv"
    
    echo "$vertices_file" "$edges_file" "$output_file"
done | xargs -L 1 -P 8 ./centrality_calculation