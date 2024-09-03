library(PRROC)

data <- read.csv("/Users/makarbetlei/Documents/ProteinCentral/output/filtered_concatenated_final_table.tsv", sep="\t")

args <- commandArgs(trailingOnly = TRUE)
if (length(args) < 1) {
  stop("You must provide a prefix as command line arguments")
}

prefix <- args[1]

thresholds <- c(2, 1.184, 0.592)

# Key words that must be present in the column name
key_words <- c("Degree", "Closeness", "Betweenness", "PageRank", "Eigenvector")

# Search for relevant columns containing both keywords and prefixes
column_names <- grep(paste0("^", prefix, ".*(", paste(key_words, collapse = "|"), ")"), names(data), value = TRUE)

if (length(column_names) < 1) {
  stop("Columns with the specified keywords and prefix were not found")
}

# Obtaining z-score values for each column
z_scores_list <- lapply(column_names, function(col) {
  scores <- data[[col]]
  scores[is.na(scores)] <- 0
  return(scores)
})

# Opening a PDF of the device to save the graph
pdf_file_name <- paste0("/Users/makarbetlei/Documents/ProteinCentral/output/", prefix, "_PR_curves.pdf")
pdf(pdf_file_name, width = 15, height = 5)

par(mfrow = c(1, length(thresholds)))

colors <- c("blue","red", "green", "yellow", "purple", "orange")

# Plotting for each value of threshold
for (threshold in thresholds) {
  labels <- ifelse(abs(data$ddG) >= threshold, 1, 0)
  
  # Calculation of PR curves
  pr_curves <- lapply(z_scores_list, function(scores) pr.curve(scores.class0 = scores, weights.class0 = labels, curve = TRUE))
  auc_values <- sapply(pr_curves, function(pr) pr$auc.integral)
  
  mean_auc <- mean(auc_values)

  plot(pr_curves[[1]]$curve[,1], pr_curves[[1]]$curve[,2], type = "l", 
       main = paste("PR curve, ddG threshold =", threshold, "\nMean AUC =", round(mean_auc, 6)), 
       col = colors[1], lwd = 1, xlab = "Recall", ylab = "Precision", xlim = c(0, 1), ylim = c(0, 1)) 
  
  min_y_at_max_x <- min(sapply(pr_curves, function(pr) pr$curve[which.max(pr$curve[,1]), 2]))
  abline(h = min_y_at_max_x, col = "grey", lty = 2)
       
  for (i in 2:length(pr_curves)) {
    lines(pr_curves[[i]]$curve[,1], pr_curves[[i]]$curve[,2], col = colors[i], lwd = 1)
  }
  
  grid()
  
  legend_labels <- sub(paste0("^", prefix), "", column_names)
  legend("bottomright", legend = legend_labels, col = colors[1:length(pr_curves)], lwd = 1.5)
}

# Closing the PDF device
dev.off()