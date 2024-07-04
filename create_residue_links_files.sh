#!/bin/bash
base_path="/Users/makarbetlei/Documents/Internship-project-ljk-1/voronota_results"
current_dir=$(pwd)
cd "$current_dir" || exit

find "$base_path" -type f -name "atom_graph_nodes.tsv" | while read -r input_file; do
    foldername=$(basename "$(dirname "$input_file")")
    output_file="../graph_generation_results/${foldername}_residue_links.tsv"
    
    ./my_graph_creation "$input_file" "$output_file"
done