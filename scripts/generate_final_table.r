
# Reading arguments from the command line
args <- commandArgs(trailingOnly = TRUE)

# Check if all required arguments are entered
if (length(args) < 2) {
  stop("Error: too few arguments!")
}

# Variables folder_name and pdb_id from arguments
folder_name <- args[1]
pdb_id <- args[2]

# Files' paths
file_skempi <- "input/SKEMPI/skempi_v2_single_mutations_with_ddG.tsv"
file_z_scores <- paste0(folder_name, "/", pdb_id, "/z_scores_merged_centrality_measures.tsv")

# Checking the existence of files
if (!file.exists(file_skempi)) {
  stop(paste("File not found:", file_skempi))
}
if (!file.exists(file_z_scores)) {
  stop(paste("File not found:", file_z_scores))
}

# Reading of the tables
skempi_data <- read.delim(file_skempi, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
z_scores_data <- read.delim(file_z_scores, sep = "\t", header = TRUE, stringsAsFactors = FALSE)

# Filtering of the first table by pdb_id
filtered_skempi <- skempi_data[skempi_data$pdb_id == pdb_id, ]

# Renaming columns in the z_scores_data table
colnames(filtered_skempi)[3:4] <- c("ID_chainID", "ID_resSeq")

# Merge tables by “ID_chainID” and “ID_resSeq” columns
merged_data <- merge(filtered_skempi, z_scores_data, by = c("ID_chainID", "ID_resSeq"))

merged_data <- merged_data[order(merged_data$ID_chainID, merged_data$ID_resSeq), ]

# Saving the result to a new file
output_file <- paste0(folder_name, "/", pdb_id, "/final_table.tsv")
write.table(merged_data, file = output_file, sep = "\t", row.names = FALSE, quote = FALSE)
