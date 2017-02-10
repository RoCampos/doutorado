# -*- coding: utf-8 -*-

import networkx as nx
import sys
from sets import Set

def readfile(file):

	Global = nx.Graph ()
	Cost = 0

	#looping over the output file
	with open (file) as f:
		#getting the num of groups
		numgroups = int(f.readline ().split()[1])
		print ("Testing Trees: ")
		# loop over the groups
		for g in xrange (0, numgroups):
			end = f.readline ()
			if end == 'Resultado':
				break
			f.readline ()
			sources = map(int,f.readline ().split ()[1:])
			sources = {sources[x]:x for x in xrange (0,len(sources))}
			members = map(int,f.readline ().split ()[1:])
			tk = f.readline ().split ()[1]			
			graphs = {x:nx.Graph() for x in xrange (0, len(sources))}
			#for each node a path is created
			for x in xrange (0, len (members)):
				path = f.readline ().split ()							
				path = path[1:len(path)-1] #path format ['1','2','3']
				path = map (int, path)				
				#getting the sources and updating the graph
				graphs[sources[path[0]]].add_path (path)
			f.readline ();

			#testing if all demand nodes were found by source
			test_tree (graphs, g, members)
			Cost = Cost + test_usage (Global, graphs)

		resultado = f.readline ().split ()
		z = int(resultado[1])
		cost = int (resultado[2])
		time = float (resultado[3])

		max_usage = max (Global.edges (data='uso'), key=lambda x: x[2])
		print ("Is usage correct? %d" % ((numgroups-max_usage[2]) == z))
		print ("Is cost correct? %d" % (Cost == cost))

		print (Cost, cost)
		print ((numgroups-max_usage[2]),z)

def test_tree (graphs, group_id, members):
	print ("\t# of group %d" % group_id)

	sink = set()
	for id, g in graphs.items ():
		if len (g.nodes ()) > 0:
			#check if g is a tree (show source)
			if (nx.is_tree (g)):
				#getting the nodes found by source of g
				clients = set(members).intersection (set(g.nodes()));
				#add the nodes to all nodes found so far
				sink = sink | clients
	
	print ("\tAre group equals: %d" % (sink.issubset(members)))

def test_usage (Global, graphs):
	cost = 0
	for id, g in graphs.items ():
		edges = g.edges ()
		#updating the cost of solution
		cost = cost + len (edges)
		#for all links of the trees of the current group
		for e in edges:	
			x, y = e		
			#adding or updating the usage of each edge
			if Global.has_edge (x,y):				
				Global[x][y]['uso'] = Global[x][y]['uso']+1
			else:
				Global.add_edge (*e)
				Global[x][y]['uso'] = 1
	return cost


if __name__ == '__main__':
	file = sys.argv[1]
	readfile(file)