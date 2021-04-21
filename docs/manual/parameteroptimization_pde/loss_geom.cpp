#include<iostream>
#include "/home/devanshr/gcsc/ug4/plugins/ConstrainedOptimization/core/parameter_estimation.h"
#include "/home/devanshr/gcsc/ug4/plugins/ConstrainedOptimization/core/parameters.h"
#include <string>


int main(){
	//Define evaluation type
	std::string dir ="/home/devanshr/gcsc/ug4/apps/epi_geometry_sampler/";
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	
	//Set parameter names
	std::vector<std::string> param_names={"alpha","kappa","theta","qq","pp" };
	
	//Set parameter bounds (sequentially, low, high, low, high etc.)
	std::vector<double> bounds={0.11,0.12,0.07,0.08,0.11,0.17,9.5,10.5,5.7,6.5 };
	
	/*v_alpha=0.118812645864508 v_theta=0.114667564712574 v_kappa= 0.0779868083048391 v_qq=10.004856445012 */
	
	int n=100; //number of samples
	
	//Call sampler (which writes results to file in the evaluations folder)
	co::sample_loss_geometry(evaluator,param_names,bounds, n);
	
	system("echo \"Geometry Sampler   al5 para has completed\" | mail -s Server1News rastogi@gcsc.uni-frankfurt.de");
}
