#!/bin/bash

FOLDER="analysis_data"

OUTPUT_FILE="/Users/makarbetlei/Documents/ProteinCentral/output/AUC_PR_results.tsv"

for filepath in $(find "/Users/makarbetlei/Documents/ProteinCentral/output/$FOLDER" -type f -name "*.tsv"); do
    filename=$(basename "$filepath" .tsv)

    auc_pr_line=$(grep "AUC_PR" "$filepath")

    auc_pr_values=($(echo "$auc_pr_line" | cut -f2-))

    header_line=$(head -n 1 "$filepath")
    centrality_measures=($(echo "$header_line" | cut -f2-))

    for i in "${!centrality_measures[@]}"; do
        centrality="${centrality_measures[$i]}"
        auc_pr="${auc_pr_values[$i]}"
        
        echo -e "${filename}_${centrality}\t${auc_pr}" >> $OUTPUT_FILE
    done
done

sort -k2,2nr $OUTPUT_FILE -o $OUTPUT_FILE