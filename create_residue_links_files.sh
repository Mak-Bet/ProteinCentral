#!/bin/bash
base_path="/Users/makarbetlei/Documents/Internship/voronota_1.29.4190/output"
current_dir=$(pwd)
cd "$current_dir" || exit

find "$base_path" -type f -name "atom_graph_nodes.tsv" | while read -r input_file; do
    foldername=$(basename "$(dirname "$input_file")")
    output_file="../graph_generation_result/${foldername}_residue_links.txt"
    
    ./my_graph_creation "$input_file" "$output_file"
done