# CMake generated Testfile for 
# Source directory: /home/romerito/workspace/Doutorado/TAtoST
# Build directory: /home/romerito/workspace/Doutorado/TAtoST
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(RUN "/home/romerito/workspace/Doutorado/TAtoST/build/chentest" "../MPP_instances/n30/b30_1.brite" "../MPP_instances/preprocessing/n30/b30_1.pre")
SUBDIRS(algorithm)
SUBDIRS(network)
SUBDIRS(sttree)
SUBDIRS(statistics)
SUBDIRS(mpp_algorithms)
SUBDIRS(util)
