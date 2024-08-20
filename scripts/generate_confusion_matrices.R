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

# Get the threshold value from the command line arguments
args <- commandArgs(trailingOnly = TRUE)
if (length(args) == 0) {
  stop("Threshold value not provided. Please provide the threshold as a command line argument.")
}
threshold <- as.numeric(args[1])

# Create a directory named after the threshold value
output_dir <- paste0("/Users/makarbetlei/Documents/ProteinCentral/output/analysis_data/threshold_", threshold)
dir.create(output_dir, showWarnings = FALSE)

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
        PPV <- NA
        NPV <- NA
        Sensitivity <- NA
        Specificity <- NA
        F1_score <- NA
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
        
        tryCatch({
          PPV <- TP / (TP + FP)
        }, warning = function(w) {
          warning("Warning in PPV calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          PPV <- NA
          warning("Error calculating PPV for column ", col_name, ": ", e$message)
        })
        
        tryCatch({
          NPV <- TN / (TN + FN)
        }, warning = function(w) {
          warning("Warning in NPV calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          NPV <- NA
          warning("Error calculating NPV for column ", col_name, ": ", e$message)
        })
        
        tryCatch({
          Sensitivity <- TP / (TP + FN)
        }, warning = function(w) {
          warning("Warning in Sensitivity calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          Sensitivity <- NA
          warning("Error calculating Sensitivity for column ", col_name, ": ", e$message)
        })
        
        tryCatch({
          Specificity <- TN / (TN + FP)
        }, warning = function(w) {
          warning("Warning in Specificity calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          Specificity <- NA
          warning("Error calculating Specificity for column ", col_name, ": ", e$message)
        })
        
        tryCatch({
          F1_score <- (2 * TP) / ((2 * TP) + FP + FN)
        }, warning = function(w) {
          warning("Warning in F1_score calculation for column ", col_name, ": ", w$message)
        }, error = function(e) {
          F1_score <- NA
          warning("Error calculating F1_score for column ", col_name, ": ", e$message)
        })
        
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
      result_list[[measure]] <- c(TP, TN, FP, FN, PPV, NPV, Sensitivity, Specificity, F1_score, MCC)
    }
  }
  
  # Convert result list to data frame
  result_df <- as.data.frame(result_list)
  rownames(result_df) <- c("TP", "TN", "FP", "FN", "PPV", "NPV", "Sensitivity", "Specificity", "F1_score", "MCC")
  
# Define output file path within the threshold directory
  output_file <- paste0(output_dir, "/", prefix, ".tsv")
  
  # Save results to file
  tryCatch({
    write.table(result_df, file = output_file, sep = "\t", col.names = NA, quote = FALSE)
  }, error = function(e) {
    stop("Error saving results to file ", output_file, ": ", e$message)
  })
}

print("Script completed successfully.")