#!/usr/bin/Rscript

# Get command-line arguments
args <- commandArgs(trailingOnly = TRUE)

# Set file paths
# Use the provided path or the current working directory if no path is provided
base_path <- ifelse(length(args) > 0, args[1], getwd())

# Construct file paths
file1_path <- file.path(base_path, "dist_based_centrality_measures.tsv")
file2_path <- file.path(base_path, "tes_based_centrality_measures.tsv")

# Function to check if file exists and is readable
check_file <- function(file_path) {
  if (!file.exists(file_path)) {
    stop(paste("Error: File does not exist: ", file_path))
  }
  if (!file.access(file_path, 4) == 0) {
    stop(paste("Error: File is not readable: ", file_path))
  }
}

# Check both files
check_file(file1_path)
check_file(file2_path)

# Function to read and check if data frame is non-empty and has necessary columns
read_and_check <- function(file_path, required_columns) {
  # Read the file
  data <- read.table(file_path, sep = "\t", header = TRUE, stringsAsFactors = FALSE)
  
  # Check if the data frame is empty
  if (nrow(data) == 0) {
    stop(paste("Error: File is empty: ", file_path))
  }
  
  # Check for required columns
  missing_cols <- setdiff(required_columns, names(data))
  if (length(missing_cols) > 0) {
    stop(paste("Error: Missing columns in", file_path, ":", paste(missing_cols, collapse = ", ")))
  }
  
  return(data)
}

# Define the required columns for merging
required_columns <- c("ID_chainID", "ID_resSeq", "Interface_status")

# Read and check both TSV files
data1 <- read_and_check(file1_path, required_columns)
data2 <- read_and_check(file2_path, required_columns)

# Merge the dataframes based on common columns
merged_data <- merge(data1, data2, by = required_columns)

# Check if the merge result is non-empty
if (nrow(merged_data) == 0) {
  stop("Error: Merged data is empty. Check if the key columns match between files.")
}

# Order the merged dataframe by ID_chainID and ID_resSeq
merged_data <- merged_data[order(merged_data$ID_chainID, merged_data$ID_resSeq), ]

# Write the merged dataframe to a new TSV file in the same directory
output_file_path <- file.path(base_path, "merged_centrality_measures.tsv")
write.table(merged_data, output_file_path, sep = "\t", row.names = FALSE, quote = FALSE)
