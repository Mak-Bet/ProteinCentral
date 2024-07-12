#!/bin/bash
base_path="/Users/makarbetlei/Documents/Internship-project-ljk-1"

mkdir ../../results/graph_creation_results

find "$base_path/results/voronota_results" -type f -name "atom_graph_nodes.tsv" | while read -r input_file; do
    foldername=$(basename "$(dirname "$input_file")")
    output_file="../../results/graph_creation_results/${foldername}_residue_links.tsv"
    
    echo "$input_file" "$output_file"
done | xargs -L 1 -P 8 ./my_graph_creation