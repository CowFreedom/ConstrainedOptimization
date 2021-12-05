#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include "../../../Epidemics/models/seird_variable_alpha.h"
#include <string>
#include<filesystem>

//This is the model function to be evaluated
void evaluate_model(double t_start, double t_end, const co::EVarManager<co::EFloat64>& v, std::vector<co::EFloat64>& timepoints, std::vector<co::EFloat64>& data,co::ErrorCode& err){

	//Set up parameters
	std::vector<double> alpha={(double)v.get_param(0).val,(double)v.get_param(1).val };
	std::vector<double> alpha_limits={0.5*(t_end-t_start)};
	double kappa=(double)v.get_param(2).val;
	double theta=4.14932000304998e-07;
	double qq=(double)v.get_param(3).val;
	double pp=(double)v.get_param(4).val;
	
	//Set up initial values for the S E I R D classes
	std::vector<double> u0={1000,3,0,0,0}; // Inital Values: Susceptibles, Exposed, Infected, Recoverd, Deceased	
	
	//Create model class and run the simulation
	ug::epi::SEIRD_VARA<std::vector<double>> seird_model(alpha, alpha_limits,kappa,theta,qq,pp);
	auto result=seird_model.run_linear_implicit(t_start,u0,t_end);
	
	//The output of the SEIRD model are 5 classes. But you might not need all of them
	//Filter accordingly
	
	std::vector<double> timepoints_temp=std::get<0>(result);
	std::vector<double> data_unfiltered=std::get<1>(result);
	std::vector<co::EFloat64> data_filtered;
	timepoints=std::vector<co::EFloat64>();
	for (int i=0;(i+4)<data_unfiltered.size();i+=5){
		data_filtered.push_back(co::EFloat64(data_unfiltered[i+1]));    //Filter Infected
		data_filtered.push_back(co::EFloat64(data_unfiltered[i+4]));	//Filter Deceased			
	}	
	for (int i=0;i<timepoints_temp.size();i++){
		timepoints.push_back(co::EFloat64(timepoints_temp[i]));
	}
	data=data_filtered;

	err= co::ErrorCode::NoError;
}


int main(){

	//std::vector<std::string> names={"alpha","kappa","theta","qq","pp"};
	//std::vector<std::string> names={"alpha","theta", "qq"};
	std::vector<std::string> names={"alpha1","alpha2","kappa","qq","pp"};
	
	
	std::string dir= std::filesystem::current_path().string();

	std::vector<co::EFloat64> bounds={co::EFloat64(1e-8),co::EFloat64(0.5),co::EFloat64(1e-13),co::EFloat64(0.5),co::EFloat64(1e-13),co::EFloat64(0.5),co::EFloat64(1),co::EFloat64(10),co::EFloat64(1),co::EFloat64(10)};


	co::PSOOptions options;
	options.set_max_iterations(10);
	options.set_n_particles(30);
	options.set_n_groups(5);
	co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct> evaluator(dir,"subset_target.lua", evaluate_model);
	co::ParticleSwarmOptimizer<co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);
	
	std::cout<<"The estimated parameters are:\n";
	for (int i=0;i<names.size();i++){
		std::cout<<names[i]<<":"<<(double)estimated_parameters.get_param(i).val<<"\n";
	}
	
		//ug::epi::write_data(argv[0], "_test.txt", timepoints, data,seird_model.names);
}
