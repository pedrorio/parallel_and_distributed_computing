library("ggplot2")

# Resolve paths relative to this script (compare/scripts/graphs.R), so it runs
# from anywhere without the TECNICO environment variable.
args <- commandArgs(trailingOnly = FALSE)
fileArg <- sub("^--file=", "", args[grep("^--file=", args)])
scriptDir <- if (length(fileArg) > 0) dirname(normalizePath(fileArg)) else getwd()
dataDir  <- normalizePath(file.path(scriptDir, "..", "data"))
plotsDir <- file.path(scriptDir, "..", "plots")
dir.create(plotsDir, showWarnings = FALSE, recursive = TRUE)

programs <- c("serial", "omp", "mpi")

readProgram <- function(name) {
  d <- read.csv(file.path(dataDir, sprintf("comparison.%s.csv", name)),
                header = TRUE, strip.white = TRUE)
  d$program <- name
  d
}

df <- do.call(rbind, lapply(programs, readProgram))
df$program <- factor(df$program, ordered = TRUE, levels = programs)

# Keep just the instance name ("../instances/NAME.in" -> "NAME").
df$fileName <- sub("\\.in$", "", basename(as.character(df$fileName)))

# Total wall-clock time from the recorded phases.
df$total <- df$readInput + df$initialLR + df$loop + df$finalFiltering

# Speedup = serial total / this run's total, per instance.
df$speedup <- NA_real_
for (inst in unique(df$fileName)) {
  serialTotal <- df$total[df$program == "serial" & df$fileName == inst]
  df$speedup[df$fileName == inst] <- serialTotal / df$total[df$fileName == inst]
}
df$speedup[df$speedup == 0] <- NaN

savePlot <- function(p, file) {
  ggsave(file.path(plotsDir, file), p, width = 11, height = 4, dpi = 120)
}

xlab <- "Number of threads / processes"

savePlot(
  ggplot(df, aes(numberOfThreads, total, color = program, group = program, fill = program)) +
    geom_bar(stat = "identity", position = position_dodge()) +
    labs(x = xlab, y = "Time (s)", title = "Total execution time") +
    facet_grid(. ~ fileName),
  "time.bar.png")

savePlot(
  ggplot(df, aes(numberOfThreads, total, color = program, group = program)) +
    geom_line() + geom_point() +
    labs(x = xlab, y = "Time (s)", title = "Total execution time") +
    facet_grid(. ~ fileName),
  "time.line.png")

savePlot(
  ggplot(df, aes(numberOfThreads, speedup, color = program, group = program, fill = program)) +
    geom_bar(stat = "identity", position = position_dodge()) +
    labs(x = xlab, y = "Speedup", title = "Speedup vs serial") +
    facet_grid(. ~ fileName),
  "speedup.bar.png")

savePlot(
  ggplot(df, aes(numberOfThreads, speedup, color = program, group = program)) +
    geom_line() + geom_point() +
    geom_hline(yintercept = 1, linetype = "dashed", color = "grey50") +
    labs(x = xlab, y = "Speedup", title = "Speedup vs serial") +
    facet_grid(. ~ fileName),
  "speedup.line.png")

cat("Wrote plots to", normalizePath(plotsDir), "\n")

# Optional LaTeX summary tables for the report (only if those packages exist).
tablePkgs <- c("dplyr", "reshape2", "knitr", "kableExtra")
if (all(vapply(tablePkgs, requireNamespace, logical(1), quietly = TRUE))) {
  suppressMessages({
    library(dplyr); library(reshape2); library(knitr); library(kableExtra)
  })
  print(kable(dcast(df, fileName ~ program + numberOfThreads, value.var = "total"),
              format = "latex", caption = "Total execution time", booktabs = TRUE))
  print(kable(dcast(df, fileName ~ program + numberOfThreads, value.var = "speedup"),
              format = "latex", caption = "Speedup", booktabs = TRUE))
} else {
  cat("(Skipping LaTeX tables: install", paste(tablePkgs, collapse = ", "), "to enable.)\n")
}
