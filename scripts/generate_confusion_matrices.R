# Load necessary libraries
library(rpart)
library(rpart.plot)

# Define file path
file_path <- "/Users/makarbetlei/Documents/ProteinCentral/output/concatenated_final_table.tsv"

# Load the data using base R functions
tryCatch({
  data <- read.table(file_path, header = TRUE, sep = "\t")
}, error = function(e) {
  stop("Error loading data: ", e$message)
})

# Inspect the data
tryCatch({
  print("Data structure:")
  print(str(data))
}, error = function(e) {
  stop("Error inspecting data: ", e$message)
})

# User input for ddG column name
ddG_col <- "ddG"

# Ensure ddG_col exists in the data
tryCatch({
  if (!ddG_col %in% names(data)) stop(paste("Column", ddG_col, "not found in the data"))
}, error = function(e) {
  stop("Error checking columns: ", e$message)
})

# Define variables
threshold <- 0.592

# Identify columns containing centrality measures
centrality_measures <- c("Degree", "Closeness", "Betweenness", "PageRank", "Eigenvector")
centrality_cols <- grep(paste(centrality_measures, collapse="|"), names(data), value = TRUE)

# Extract unique prefixes
prefixes <- unique(sub(paste(centrality_measures, collapse="|"), "", centrality_cols))

# Calculate metrics for each prefix and save to separate tables
for (prefix in prefixes) {
  result_list <- list()
  for (measure in centrality_measures) {
    col_name <- paste0(prefix, measure)
    if (col_name %in% names(data)) {
      # Handle missing data
      if (all(is.na(data[[col_name]]))) {
        TP <- NA
        TN <- NA
        FP <- NA
        FN <- NA
        MCC <- NA
      } else {
        # Calculate TP, TN, FP, FN
        tryCatch({
          TP <- sum(abs(data[[ddG_col]]) >= threshold & data[[col_name]] >= 2, na.rm = TRUE)
          TN <- sum(abs(data[[ddG_col]]) < threshold & data[[col_name]] < 2, na.rm = TRUE)
          FP <- sum(abs(data[[ddG_col]]) < threshold & data[[col_name]] >= 2, na.rm = TRUE)
          FN <- sum(abs(data[[ddG_col]]) >= threshold & data[[col_name]] < 2, na.rm = TRUE)
        }, error = function(e) {
          stop("Error calculating TP, TN, FP, FN for column ", col_name, ": ", e$message)
        })
        
        # Convert to numeric to avoid integer overflow
        TP <- as.numeric(TP)
        TN <- as.numeric(TN)
        FP <- as.numeric(FP)
        FN <- as.numeric(FN)
        
        # Calculate Matthews Correlation Coefficient (MCC)
        tryCatch({
          MCC <- (TP * TN - FP * FN) / sqrt((TP + FP) * (TP + FN) * (TN + FP) * (TN + FN))
        }, warning = function(w) {
          warning("Warning in MCC calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          MCC <- NA
          warning("Error calculating MCC for column ", col_name, ": ", e$message)
        })
      }
      
      # Store results in list
      result_list[[measure]] <- c(TP, TN, FP, FN, MCC)
    }
  }
  
  # Convert result list to data frame
  result_df <- as.data.frame(result_list)
  rownames(result_df) <- c("TP", "TN", "FP", "FN", "MCC")
  
  # Define output file path
  output_file <- paste0("/Users/makarbetlei/Documents/ProteinCentral/output/analysis_data/", prefix, ".tsv")
  
  # Save results to file
  tryCatch({
    write.table(result_df, file = output_file, sep = "\t", col.names = NA, quote = FALSE)
  }, error = function(e) {
    stop("Error saving results to file ", output_file, ": ", e$message)
  })
}

print("Script completed successfully.")