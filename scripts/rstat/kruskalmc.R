require (pgirmess)

options(echo=FALSE) # if you want see commands in output file
args <- commandArgs(trailingOnly = TRUE)


if ( length (args) <= 1 ) {
	
	print ("Getting out1!");

} else {
	
	# para cada algoritmo
	print ( paste("Samples:", length(args)) )	
	algorithms <- NULL
	for ( i in 1:length(args) ) {

		#constroi uma tabela onde cada coluna é uma amostra de uma instância.		
		table_i <- NULL
		for ( inst in dir(args[i],pattern="*.result" ) ) {
			
			path <- paste ( args[i], inst, sep="/" );			
			t <- read.table ( path, header = T )	
			table_i <- cbind ( table_i, t$CAP_R )

		}

		#Adiciona a tabela a abaixo da talela i-1
		algorithms <- rbind (algorithms, table_i)
	}

	#criando as categorias	
	categ <- as.factor ( rep( c(1:length(args)), times=1, each=50  )  )	

	#pega o número de instâncias
	for ( i  in 1:ncol(algorithms) ) {

		#pega a coluna inteira que é a soma das amostras do
		#algoritmos
		resp <- algorithms[,i]
		
		#faz o teste para a instância i
		print (kruskalmc (resp~categ, probs=0.05), cont="two-tailed")
	}

}
