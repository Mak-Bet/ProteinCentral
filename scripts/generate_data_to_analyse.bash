#!/bin/bash

cd "$(dirname "$0")/.."

rm -rf ./output/graphs_data

input_file="input/SKEMPI/list_of_pdb_ids_with_single_mutations_in_skempi_v2.txt"

if [ ! -f "$input_file" ]; then
  echo "File $input_file does not exist."
  exit 1
fi

cat "$input_file" | xargs -L 1 -P 8  ./scripts/process_pdb_id.bash output/graphs_data
