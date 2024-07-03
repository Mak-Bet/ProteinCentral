base_path="/Users/makarbetlei/Documents/Internship/voronota_1.29.4190/output"

find "$base_path" -type f -name "atom_graph_nodes.tsv" | while read -r input_file; do
    foldername=$(basename "$(dirname "$input_file")")
    output_file="../${foldername}_residue_links.txt"
    
    ./residue_links_creation "$input_file" "$output_file"
done