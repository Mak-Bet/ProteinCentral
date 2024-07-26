
# Define the path to the merged data file
args <- commandArgs(trailingOnly = TRUE)

# Use the provided path or the current working directory if no path is provided
base_path <- ifelse(length(args) > 0, args[1], getwd())

# Construct the file path for the merged TSV file
merged_file_path <- file.path(base_path, "merged_centrality_measures.tsv")

# Function to calculate z-scores for a vector
calculate_z_scores <- function(x) {
  # Calculate the mean and standard deviation
  mu <- mean(x, na.rm = TRUE)
  sigma <- sd(x, na.rm = TRUE)
  
  # Calculate z-scores
  z_scores <- (x - mu) / sigma
  
  return(z_scores)
}

# Read the merged TSV file
merged_data <- read.table(merged_file_path, sep = "\t", header = TRUE, stringsAsFactors = FALSE)

# Identify columns related to centrality measures
centrality_columns <- grep("degree|closeness|betweenness|pagerank|eigenvector", names(merged_data), ignore.case = TRUE, value = TRUE)

# Calculate z-scores for each centrality column and replace the original column with z-scores
for (col in centrality_columns) {
  merged_data[[col]] <- calculate_z_scores(merged_data[[col]])
}

# Construct the file path for the output TSV file
output_file_path <- file.path(base_path, "z_scores_merged_centrality_measures.tsv")

# Write the modified data with z-scores to a new TSV file
write.table(merged_data, output_file_path, sep = "\t", row.names = FALSE, quote = FALSE)
