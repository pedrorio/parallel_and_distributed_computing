library("ggplot2")
library("stringr")
library("dplyr")
library("reshape2")
library("knitr")
library("kableExtra")

df <- read.csv("comparison.csv", header = T)
df$fileName <- sapply(str_split(df$fileName, "/", 3), function(x) (x[3]))
df$total <- df$readInput +
  df$initialLR +
  df$loop +
  df$finalFiltering
df$number_of_processors <- df$OMP_NUM_THREADS

df$program <- factor(df$number_of_processors, ordered = TRUE, levels = c("Serial", "1 Thread", "2 Threads", "4 Threads", "8 Threads"))
df$program[df$number_of_processors == 0] <- "Serial"
df$program[df$number_of_processors == 1] <- "1 Thread"
df$program[df$number_of_processors == 2] <- "2 Threads"
df$program[df$number_of_processors == 4] <- "4 Threads"
df$program[df$number_of_processors == 8] <- "8 Threads"

for (i in unique(df$fileName)) {
  df$speedup[df$fileName == i] <- df[df$program == "Serial" & df$fileName == i,]$total / df$total[df$fileName == i]
}
df$speedup[df$speedup == 0] <- NaN

ggplot(df, aes(x = program, y = total, color = fileName, group = fileName)) +
  geom_line() +
  labs(x = "Program", y = "Time (s)", title = "Total Execution Time")
ggsave("time.png")

df <- df[df$program != "Serial",]
droplevels(df$program)

ggplot(df, aes(x = program, y = speedup, color = fileName, group = fileName)) +
  geom_line() +
  labs(x = "Program", y = "Speedup", title = "Speedup")
ggsave("speedup.png")


kable(dcast(group_by(df, total), fileName ~ program, value.var = "total"), format = "latex", caption = "Total Execution Time", booktabs = TRUE) %>%
  kable_styling(latex_options = c("scale_down", "hold_position"), position = "center")

kable(dcast(group_by(df, total), fileName ~ program, value.var = "speedup"), format = "latex", caption = "Speedup", booktabs = TRUE) %>%
  kable_styling(latex_options = c("scale_down", "hold_position"), position = "center")
      