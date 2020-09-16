library("ggplot2")
library("stringr")
library("dplyr")
library("reshape2")
library("knitr")
library("kableExtra")
library("reshape")

basisName <- 'serial'
path <- Sys.getenv("TECNICO")
repoPath <- path + "/distributed_and_parallel_computing/"

df <- read.csv(str_interp("comparison.${basisName}.csv"), header = T)

df$program <- basisName
df$program <- factor(df$program, ordered = TRUE, levels = c("serial", "omp", "mpi"))

otherNames <- list('omp', 'mpi')

buildDf <- function(df, basisName, otherNames) {
  for (i in 1:length(otherNames)) {
    iDf <- read.csv(str_interp("comparison.${otherNames[i]}.csv"), header = T)
    iDf$program <- otherNames[i]
    df <- rbind(df, iDf)
  }

  df$fileName <- sapply(str_split(df$fileName, "/", 3), function(x) (x[3]))
  df$total <- df$readInput +
    df$initialLR +
    df$loop +
    df$finalFiltering

  df
}

df <- buildDf(df, "serial", list("omp", "mpi"))

for (i in unique(df$fileName)) {
  df$speedup[df$fileName == i] <- df[df$program == basisName & df$fileName == i,]$total / df$total[df$fileName == i]
}
df$speedup[df$speedup == 0] <- NaN

ggplot(df, aes(x = numberOfThreads, y = total, color = program, group = program, fill = program)) +
  geom_bar(stat="identity", position=position_dodge()) +
  labs(x = "Number of threads", y = "Time (s)", title = "Total Execution Time") + facet_grid(. ~ fileName)
ggsave(repoPath + "plots/time.bar.png")


ggplot(df, aes(x = numberOfThreads, y = total, color = program, group = program, fill = program)) +
  geom_line(stat="identity", position=position_dodge()) +
  labs(x = "Number of threads", y = "Time (s)", title = "Total Execution Time") + facet_grid(. ~ fileName)
ggsave(repoPath + "plots/time.line.png")

ggplot(df, aes(x = numberOfThreads, y = speedup, color = program, group = program, fill = program)) +
  geom_bar(stat="identity", position=position_dodge()) +
  labs(x = "Number of threads", y = "Speedup", title = "Speedup") + facet_grid(. ~ fileName)
ggsave(repoPath + "plots/speedup.bar.png")

ggplot(df, aes(x = numberOfThreads, y = speedup, color = program, group = program, fill = program)) +
  geom_line(stat="identity", position=position_dodge()) +
  labs(x = "Number of threads", y = "Speedup", title = "Speedup") + facet_grid(. ~ fileName)
ggsave(repoPath + "plots/speedup.line.png")

kable(dcast(group_by(df, total), fileName ~ program + numberOfThreads, value.var = "total"), format = "latex", caption = "Total Execution Time", booktabs = TRUE) %>%
  kable_styling(latex_options = c("scale_down", "hold_position"), position = "center")

kable(dcast(group_by(df, total), fileName ~ program + numberOfThreads, value.var = "speedup"), format = "latex", caption = "Speedup", booktabs = TRUE) %>%
  kable_styling(latex_options = c("scale_down", "hold_position"), position = "center")
