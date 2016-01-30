
# Script que plota o pareto resultado da otimização de 
# modelo bi-objetivo considerando custo e capacidade residual sujeito
# a limite de hops e capacidade das arestas.

# O script recebe como argumento 4 parâmetros
#  	1 - arquivo contendo o pareto obtido que consiste de 
#  um arquivo de texto com três colunas: Z, Optimal e Bound
#  	2 - nome de arquivo de saída com extensão em pdf.
#  	3 - Tempo de processamento em ponto flutuante
#  	4 - nome da instância referente ao pareto



  
args <- commandArgs(trailingOnly = TRUE)

setEPS ();
postscript (args[2], paper = "a4");

table <- read.table (args[1]); #reading the table
  
aux <- table[!duplicated(table$V2),];  #removing dominated solutions

col_names <- c("Res", "Obj", "Bound");
colnames (aux) <- c(col_names); #rename columns
  
print (aux);
  
plot (aux$Obj~aux$Res, type = "o", ylab = "Custo", xlab = "Z");

#putting the name of instance
legend ("topleft", paste("Pareto(s)", args[3], sep = ":") ,title = args[4])
dev.off ()
