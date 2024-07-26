#!/bin/bash

cd "$(dirname $0)/.."

voronota_path="./tools/voronota_1.29.4198"
generate_list_graph_app_path="./tools/generate_list_graph/build"
calculate_centrality_app_path="./tools/calculate_centrality/build"

if [ -z "$1" ] || [ -n "$3" ]
then
  echo "Error: invalid arguments, must provide to inputs: PDB ID and directory where the results will be"
  exit 1
fi

readonly DIRNAME="$1"

readonly PDBID="$2"

mkdir -p ${DIRNAME}

${voronota_path}/voronota-js-lt-graph --input "input/PDB/${PDBID}.pdb" --output-dir ${DIRNAME}

output_folder_path=$(realpath ${DIRNAME}/${PDBID})

${generate_list_graph_app_path}/generate_list_graph \
  "${output_folder_path}/atom_graph_nodes.tsv" \
  "${output_folder_path}/dist_based_residue_graph_links.tsv"

${calculate_centrality_app_path}/calculate_centrality \
  "${output_folder_path}/residue_graph_nodes.tsv" \
  "${output_folder_path}/dist_based_residue_graph_links.tsv" \
  "${output_folder_path}/sep1_dist_based_centrality_measures.tsv" \
  sep1_dist_ 1

${calculate_centrality_app_path}/calculate_centrality \
  "${output_folder_path}/residue_graph_nodes.tsv" \
  "${output_folder_path}/residue_graph_links.tsv" \
  "${output_folder_path}/sep1_tes_based_centrality_measures.tsv" \
  sep1_tes_ 1


${calculate_centrality_app_path}/calculate_centrality \
  "${output_folder_path}/residue_graph_nodes.tsv" \
  "${output_folder_path}/dist_based_residue_graph_links.tsv" \
  "${output_folder_path}/sep2_dist_based_centrality_measures.tsv" \
  sep2_dist_ 2

${calculate_centrality_app_path}/calculate_centrality \
  "${output_folder_path}/residue_graph_nodes.tsv" \
  "${output_folder_path}/residue_graph_links.tsv" \
  "${output_folder_path}/sep2_tes_based_centrality_measures.tsv" \
  sep2_tes_ 2

Rscript scripts/merge_tables.r \
  "${output_folder_path}"

Rscript scripts/generate_z_score_table.r \
  "${output_folder_path}"

Rscript scripts/generate_final_table.r \
  "${DIRNAME}" \
  "${PDBID}"
