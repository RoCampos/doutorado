options(echo=FALSE) # if you want see commands in output file
args <- commandArgs(trailingOnly = TRUE)

#must be the directory

if ( length (args) <= 1 ) {
	
	print ("Getting out1!");

} else {

	#para cada algoritmo 
	algorithms <- NULL
	for (i in 1:length(args) ) {
		
		#para cada grupo de instâncias (n30, n60...)
		i_column <- NULL #guarda todas as amostra do algoritmo I	
		for ( d in dir(args[i]) ) {			


			#pegar lista de instacias do grupo d
			path <- paste (args[i], d, sep = "")
			inst_i <- dir (path, pattern = ".result")
			
			#para instância obter o valor de média de N execuções
			d_column <- NULL #guarda todas as amostras do grupo de instância D
			for ( inst in inst_i) {		
		
				fullpath <- paste (path, inst, sep = "/")			
				
				#READING THE SAMPLE
				table <- read.table (fullpath, header = T)
				param <- table$CAP_R	
				d_column <- rbind (d_column, mean (param))
				
			}			
			i_column <- rbind ( i_column, d_column )
		}
		algorithms <- cbind (algorithms, i_column)
	}

	dnames <- NULL
	for (i in length (args)) {	
		for (d in dir(args[i]) ) {			
			instances <- dir( paste(args[i], d, sep="/"), pattern = ".result" )
			dnames <- cbind (dnames,instances)
		}
	}

	dnames <- c(dnames)
	
	#print ( list(dnames, c("a1", "a2")) )	

	m <- matrix (algorithms, nrow = 120, dimnames = list( dnames, c("a1", "a2", "a3", "a4")) )
	friedman.test (m)

}
