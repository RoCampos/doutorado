options(echo=FALSE) # if you want see commands in output file
args <- commandArgs(trailingOnly = TRUE)

ls <- read.table (args[1], header = T);
rls <- read.table (args[2], header = T);

pvalue = wilcox.test (ls$CAP_R, rls$CAP_R)$p.value
lsmean <- mean (ls$CAP_R)
rlsmean <- mean (rls$CAP_R)
lsmedian <- median (ls$CAP_R)
rlsmedian <- median (rls$CAP_R)

H1 <- NULL

if (lsmean == rlsmean) {
  H1 = 0
} else if (pvalue < 0.05) {
  H1 = 1
} else {
  H1 = 0
}

tuple <- c(pvalue, lsmean, rlsmean, lsmedian, rlsmedian, H1)

lsbool = FALSE
clsbool = FALSE
out = ""

if (H1 == 1) {
	if (lsmean > rlsmean) {
		lsbool = TRUE
		out <- "1 > 2"
	}


	if (lsmean < rlsmean) {
		Clsbool = TRUE
		out <- "1 < 2"
	}
}

output <- c(lsmean, rlsmean, lsbool, clsbool, H1, out)
print (output)


