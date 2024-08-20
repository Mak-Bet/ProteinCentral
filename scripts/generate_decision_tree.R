# Load data
data <- read.csv("/Users/makarbetlei/Documents/ProteinCentral/output/concatenated_final_table.tsv", sep="\t")

#args <- commandArgs(trailingOnly = TRUE)
#if (length(args) == 0) {
#  stop("Threshold value not provided. Please provide the threshold as a command line argument.")
#}
#threshold <- as.numeric(args[1])
threshold <- 0.592

# Creating binary labels
data$binary_ddG <- as.integer(abs(data$ddG) > threshold)
#data$binary_zscore <- ifelse(apply(data[, grep("sep1_dist_unweighted", names(data))], 1, max, na.rm=TRUE) > 2, 1, 0)
data$binary_zscore <- ifelse(data$sep1_dist_unweighted_Degree > 2, 1, 0)

# Combining labels into one binary variable
data$binary_target <- ifelse(data$binary_ddG == 1 | data$binary_zscore == 1, 1, 0)
table(data$binary_target)
# Building a decision tree
library(rpart)
tree_model <- rpart(binary_target ~ . - binary_ddG - binary_zscore, data=data, method="class")

# Building a ROC curve
library(pROC)
roc_curve <- roc(data$binary_target, predict(tree_model, data, type="prob")[,1])
plot(roc_curve)

# Building a PR curve
library(PRROC)
pr_curve <- pr.curve(scores.class0 = predict(tree_model, data, type="prob")[,1], weights.class0=data$binary_target, curve=T)
plot(pr_curve)