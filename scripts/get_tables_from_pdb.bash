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

tests_folder_path=$(realpath ${DIRNAME}/${PDBID})

${generate_list_graph_app_path}/generate_list_graph \
  "${tests_folder_path}/atom_graph_nodes.tsv" \
  "${tests_folder_path}/dist_based_residue_graph_links.tsv"

${calculate_centrality_app_path}/calculate_centrality \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/dist_based_residue_graph_links.tsv" \
  "${tests_folder_path}/dist_based_centrality_measures.tsv" \
  dist_ 1

${calculate_centrality_app_path}/calculate_centrality \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/residue_graph_links.tsv" \
  "${tests_folder_path}/tes_based_centrality_measures.tsv" \
  tes_ 1


  ${calculate_centrality_app_path}/calculate_centrality \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/dist_based_residue_graph_links.tsv" \
  "${tests_folder_path}/sep2_dist_based_centrality_measures.tsv" \
  sep2_dist_ 2

${calculate_centrality_app_path}/calculate_centrality \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/residue_graph_links.tsv" \
  "${tests_folder_path}/sep2_tes_based_centrality_measures.tsv" \
  sep2_tes_ 2
