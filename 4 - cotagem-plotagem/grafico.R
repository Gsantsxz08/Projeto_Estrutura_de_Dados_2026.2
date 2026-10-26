d <- read.csv("comparacoes.csv")

png("grafico.png", width = 900, height = 600)
plot(d$busca, d$lista, type = "l", col = "red", log = "y",
     ylim = c(1, max(d$lista)),
     xlab = "Numero da busca", ylab = "Comparacoes (escala log)",
     main = "Lista encadeada vs ABB")
lines(d$busca, d$abb, col = "blue")
legend("right", legend = c("Lista encadeada", "ABB"),
       col = c("red", "blue"), lty = 1)
dev.off()
