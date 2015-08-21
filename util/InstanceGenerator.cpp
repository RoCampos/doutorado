#include "InstanceGenerator.h"

void InstanceGenerator::generator ()
{
	std::cout << "Construction started!\n";
	std::ofstream file;
	
	file.open ( this->m_file_name.c_str(), std::ios::app );
	
	if (!file.good()) {
		std::cerr << "Problem in reading file: " << m_file_name << "" << std::endl;
	}
	
	
	std::string line(62, '-');	
	file << "\n" << line << "\n";
	file << line << "\n";
	
	file << "\nGroups: ( " << this->m_num_groups << " )\n\n";
	
// 	std::vector<int> list = this->create_list();
	
	for (int i = 0; i < this->m_num_groups; i++) {
		
		
		//generating the groups
		file << "Group Member: ( " << i << " )\t\t ";
		
		generate_traffic (file);
		
		generate_group_info (file);
		
	}
	
	//end of groups construction
	file.close ();
	std::cout << "Construction finished!\n";
}

void InstanceGenerator::set_mode (bool g_mode, bool tk_mode, bool cap_mode)
{
	this->m_group_mode = g_mode;
	this->m_traffic_mode = tk_mode;
	this->m_capacity_mode = cap_mode;
	
	std::cout << m_group_mode << std::endl;
	std::cout << m_capacity_mode << std::endl;
	std::cout << m_traffic_mode << std::endl;
}

void InstanceGenerator::generate_traffic ( std::ostream & out )
{
	float value = 1.0;
	
	if (this->m_traffic_mode == PERCENTUAL) {
		m_randomInt.reset_interval (this->m_tk_min,m_tk_max);
		value = m_randomInt.rand ();
		out << "Traffic: ( " << value << " )\n";
	} else {
		out << "Traffic: ( " << value << " )\n";	
	}
	
}

void InstanceGenerator::generate_group_info ( std::ostream & out)
{
	int source = 0;
	int size = 0;
	
	std::vector<int> list = create_list ();
	
	m_randomInt.reset_interval (0, this->m_num_nodes-1);		
	if (this->m_group_mode == PERCENTUAL) {
		
		source = m_randomInt.rand ();
		out << "source: ( " << source << " )\t\t";
		
		int p_min = ((this->m_num_nodes * this->m_g_min )/100);
		int p_max = ((this->m_num_nodes * this->m_g_max )/100);
		
		//when the size of group is a percentual of number of nodes.
		m_randomInt.reset_interval (p_min, p_max);
		size = m_randomInt.rand ();
		out << "size: ( " << size << " )\n";
		
	} else {
	
		
		source = m_randomInt.rand ();
		out << "source: ( " << source << " )\t\t";
		
		//with size of group as fixed parameter
		size = m_g_max;
		out << "size: ( " << size << " )\n";
	}
	
	list.erase (list.begin()+source);
	
	this->generate_members (out, list, size);
}

void InstanceGenerator::generate_members ( std::ostream & out, 
										   std::vector<int> & nodes, 
										   int group_size)
{
	
	for (int i=0; i < group_size; i++) {
		m_randomInt.reset_interval (0, nodes.size ()-1);	
		int node = m_randomInt.rand ();
		
		out << nodes [node] << " ";
		nodes.erase (nodes.begin () + node);
		
	}
	out << "\n\n";
}



// Este arquivo é utilizado para criar instâncias do 
// tipo brite.

// O binário recebe como entrada 
// - usage ./script <isntance dir> file.dsc binary

// Group mode: 0	--> fixed or percentual
// Traffic mode: 1	--> fized or percentual
// Cap mode: 0		--> fixed or percentual
// NumGroups: 10 | NumNodes: 200	--> fixed
// tk_max: 5 | tk_min: 1	
// g_max: 10 | g_min: 10
// c_max: 10 | c_min: 1

int main (int argc, char**argv) {

 	std::string file (argv[1]);
	
	std::string descriptor (argv[2]);
	
	std::fstream archive( descriptor.c_str() );

	//fixed 0 and Percentual 1
	int g_mode, tk_mode, cap_mode;
	
	int num_groups, num_nodes;
	float tk_max, tk_min;
	float g_max, g_min;
	float c_max, c_min;
	
	if (!archive.good()) exit (1);
	
	std::string line;
	getline (archive, line);
	sscanf (line.c_str (), "Group mode: %d", &g_mode);
	getline (archive, line);
	sscanf (line.c_str (), "Traffic mode: %d", &tk_mode);
	getline (archive, line);
	sscanf (line.c_str (), "Cap mode: %d", &cap_mode);
	getline (archive, line);
	sscanf (line.c_str (), "NumGroups: %d | NumNodes: %d", &num_groups, &num_nodes);
	getline (archive, line);
	sscanf (line.c_str (), "tk_max: %f | tk_min: %f", &tk_max, &tk_min);
	getline (archive, line);
	sscanf (line.c_str (), "g_max: %f | g_min: %f", &g_max, &g_min);
	getline (archive, line);
	sscanf (line.c_str (), "c_max: %f | c_min: %f", &c_max, &c_min);
		
 	InstanceGenerator gen (file);
 	
	gen.set_mode (g_mode, tk_mode, cap_mode);
	
	gen.set_num_groups (num_groups);
	gen.set_num_nodes (num_nodes);
	gen.set_group_size (g_min, g_max);	
	gen.set_capacity (c_min, c_max);	
	gen.set_traffic (tk_min, tk_max);
	
 	gen.generator ();
	

	archive.close ();	
	
	return 0;
}