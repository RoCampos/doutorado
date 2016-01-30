#include <iostream>

#include <boost/regex.hpp>
#include "gurobi_c++.h"

#include "models.h"
#include "network.h"
#include "group.h"
#include "reader.h"

using std::cout;
using std::endl;

struct result_t {

	int id;
	std::vector<rca::Link> tree;
	int cost = 0;
	int size = 0;
	double runtime = 0.0;

};

std::string G_INSTANCE = "";

result_t get_result (GRBModel&, rca::Network&, rca::Group&);
void write_result (std::vector<result_t>& results);

int main(int argc, char const *argv[])
{
	std::string file = argv[1];
	G_INSTANCE = argv[2];

	rca::Network net;
	std::vector<rca::Group> multicast_group;
	std::vector<shared_ptr<rca::Group>> temp;	
	
	MultipleMulticastReader r (file);		
	r.configure_real_values (&net,temp);
	for (auto g : temp) {
		multicast_group.push_back (*g);
	}

	std::vector<result_t> results;

	size_t GROUPS = multicast_group.size ();
	for (int k = 0; k < GROUPS; ++k)
	{

		GRBEnv env = GRBEnv ();
		GRBModel model = GRBModel (env);

		SteinerTreeModel st (model, net, multicast_group[k], 
			SteinerMode::OPTIMIZE_BY_COST);	

		std::stringstream ss;
		ss << G_INSTANCE << "_" << k << ".lp";
		model.write (ss.str () );
		model.optimize ();

		result_t result = 
			get_result (model, net, multicast_group[k]);

		result.id = k;
		results.push_back (result);

	}

	write_result (results);

	return 0;
}

result_t get_result (GRBModel& modelo, 

	rca::Network& net, rca::Group& group) {

	GRBVar * array = modelo.getVars ();
	int NUMVAR = modelo.get (GRB_IntAttr_NumVars);

	boost::regex regex_expr ("(\\d+)");
	
	result_t result;	

	for (int i = 0; i < NUMVAR; ++i)
	{
		GRBVar var = array[i];
		std::string varname = var.get(GRB_StringAttr_VarName);
		if (varname.find ("y") == std::string::npos) continue;

		if (var.get (GRB_DoubleAttr_X) == 1) {			

			boost::sregex_iterator rit (varname.begin(), 
				varname.end(), regex_expr);

			int x = atoi (rit->str ().c_str ());
			rit++;
			int y = atoi (rit->str ().c_str ());

			result.cost += net.getCost(x-1,y-1);
			result.tree.push_back (rca::Link(x-1,y-1,0));
			result.size++;

		}
	}

	result.runtime = modelo.get(GRB_DoubleAttr_Runtime);

	return result;
}

void write_result (std::vector<result_t>& results) {

	std::string fname = G_INSTANCE;
	fname += ".detail";

	std::ofstream file (fname.c_str(), std::ofstream::out);

	fname = G_INSTANCE;
	fname += ".opt";
	std::ofstream opt_file (fname.c_str (), std::ofstream::out);

	for(auto&& res : results) {
		
		cerr << "Tree: " <<res.id << endl;
		cerr << "\tCost: " << res.cost << endl;
		cerr << "\tSize: " << res.size << endl;
		cerr << "\tTime: " << res.runtime << endl;

		file << "Tree: " <<res.id << endl;
		file << "\tCost: " << res.cost << endl;
		file << "\tSize: " << res.size << endl;
		file << "\tTime: " << res.runtime << endl;

		cerr << "\tTree Representation:" << endl;
		for(auto&& l : res.tree) {			
			cerr << "\t\t" << l << endl;
		}

		opt_file << res.cost << " " << res.size << endl;

		cerr << endl;

	}

	opt_file.close ();
	file.close ();

}