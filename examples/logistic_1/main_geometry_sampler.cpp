#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include <string>

int main(){
	//Define evaluation type
	std::string dir ="/home/server1/Programs/ug4/ug4/plugins/ConstrainedOptimization/examples/logistic_1/";
	co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local,co::ConfigOutput::File> evaluator(dir, "subset_target.lua","subset_sim.lua");
	
	//Set parameter names
	std::vector<std::string> param_names={"theta1","theta2","theta3"};
	
	//Set parameter bounds (sequentially, low, high, low, high etc.)
	std::vector<double> bounds={0,10,0,5,0,20};
	
	int n=20; //number of samples
	
	//Call sampler (which writes results to file in the evaluations folder)
	co::sample_loss_geometry(evaluator,param_names,bounds, n);
}
