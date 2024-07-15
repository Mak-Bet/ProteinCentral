#!/bin/bash

voronota_path="/my_graph_creation/voronota_1.29.4198"
graph_creation_app_path="/my_graph_creation/build"
centrality_calculation_app_path="/centrality_calculation/build"

if [ -z "$1" ] || [ -n "$2" ]
then
  echo "Error: invalid arguments, must provide a single input PDB ID"
  exit 1
fi

readonly PDBID="$1"

mkdir -p tests

./${voronota_path}/voronota-js-lt-graph --input "PDBs/${PDBID}.pdb" --output-dir "tests"

tests_folder_path=$(realpath tests/${PDBID})

./${graph_creation_app_path}/my_graph_creation \
  "${tests_folder_path}/atom_graph_nodes.tsv" \
  "${tests_folder_path}/my_residue_graph_links.tsv"

./${centrality_calculation_app_path}/centrality_calculation \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/my_residue_graph_links.tsv" \
  "${tests_folder_path}/my_centrality_measures.tsv"

./${centrality_calculation_app_path}/centrality_calculation \
  "${tests_folder_path}/residue_graph_nodes.tsv" \
  "${tests_folder_path}/residue_graph_links.tsv" \
  "${tests_folder_path}/voronota_centrality_measures.tsv"