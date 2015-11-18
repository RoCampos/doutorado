sttre = read.table(file = "n60.sttre")
steiner = read.table(file = "n60.steiner")
min <- min(sttre$V1, steiner$V1)
max <- max(sttre$V1, steiner$V1)
plot (y = sttre$V1, x=c(1:30), ylab = 'Time', xlab = 'Instance(N=60)', type='l', lwd=2 ,col=1, ylim = c(min,max), main="Comparação entre Representações de Soluções")
lines (y = steiner$V1, x=c(1:30), type='l', lwd=2, col=2)
legend("bottomright", c('STTre', 'steiner'), title = "bottomright", lwd=2, col=c(1,2))

