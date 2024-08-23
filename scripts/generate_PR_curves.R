library(PRROC)
data <- read.csv("/Users/makarbetlei/Documents/ProteinCentral/output/filtered_concatenated_final_table.tsv", sep="\t")

args <- commandArgs(trailingOnly = TRUE)
if (length(args) == 0) {
 stop("Threshold value not provided. Please provide the threshold as a command line argument.")
}
threshold <- as.numeric(args[1])

# Создаем вектор чисел
degree_z_scores <- data$sep1_dist_unweighted_Degree
closeness_z_scores <- data$sep1_dist_unweighted_Closeness
betweenness_z_scores <- data$sep1_dist_unweighted_Betweenness
pagerank_z_scores <- data$sep1_dist_unweighted_PageRank
eigenvector_z_scores <- data$sep1_dist_unweighted_Eigenvector

# Определяем условие: значения больше 2
labels <- ifelse(abs(data$ddG) >= threshold, 1, 0)

pr_degree <- pr.curve(scores.class0 = degree_z_scores, weights.class0 = labels, curve = TRUE)
pr_closeness <- pr.curve(scores.class0 = closeness_z_scores, weights.class0 = labels, curve = TRUE)
pr_betweenness <- pr.curve(scores.class0 = betweenness_z_scores, weights.class0 = labels, curve = TRUE)
pr_pagerank <- pr.curve(scores.class0 = pagerank_z_scores, weights.class0 = labels, curve = TRUE)
pr_eigenvector <- pr.curve(scores.class0 = eigenvector_z_scores, weights.class0 = labels, curve = TRUE)

# Выводим результат
plot(pr_degree, main = "PR-кривая на основе z-оценок, ddG threshold = 2", col = "blue", lwd = 1)
lines(pr_closeness$curve[,1], pr_closeness$curve[,2], col = "red", lwd = 1)
lines(pr_betweenness$curve[,1], pr_betweenness$curve[,2], col = "green", lwd = 1)
lines(pr_pagerank$curve[,1], pr_pagerank$curve[,2], col = "yellow", lwd = 1)
lines(pr_eigenvector$curve[,1], pr_eigenvector$curve[,2], col = "purple", lwd = 1)

legend("bottomright", legend = c("Degree", "Closeness", "Betweenness", "PageRank", "Eigenvector"), col = c("blue", "red", "green", "yellow", "purple"), lwd = 1.5)
