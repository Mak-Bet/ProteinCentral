#!/bin/bash
base_path="/Users/makarbetlei/Documents/ProteinCentral"

find "$base_path/output/graphs_data" -type f -name "atom_graph_nodes.tsv" | while read -r input_file; do
    foldername=$(basename "$(dirname "$input_file")")
    output_file="output/graphs_data/${foldername}/dist_based_residue_graph_links.tsv"
    
    echo "$input_file" "$output_file"
done | xargs -L 1 -P 8 ./generate_list_graph