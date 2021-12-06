#include<iostream>
#include "../../core/parameter_estimation.h"
#include "../../core/parameters.h"
#include "../../../Epidemics/models/seird_variable_alpha.h"
#include <string>
#include<filesystem>

//This is the model function to be evaluated
void evaluate_model(double t_start, double t_end, const co::EVarManager<co::EFloat64>& v, std::vector<co::EFloat64>& timepoints, std::vector<co::EFloat64>& data,co::ErrorCode& err){

	//Set up parameters
	std::vector<double> alpha={(double)v.get_param(0).val,5.00169e-07 };
	std::vector<double> alpha_limits={0.5*(t_end-t_start)};
	double kappa=0.903178;
	double theta=4.14932000304998e-07;
	double qq=3.01401;
	double pp=6.0785;
	
	//Set up initial values for the S E I R D classes
	std::vector<double> u0={764000,2*28,28,0,0}; // Inital Values: Susceptibles, Exposed, Infected, Recoverd, Deceased	
	
	//Create model class and run the simulation
	std::vector<double> cumulated_infected;
	std::vector<double> cumulated_exposed;	
	ug::epi::SEIRD_VARA<std::vector<double>> seird_model(alpha, alpha_limits,kappa,theta,qq,pp);
	seird_model.change_step_size_time(0.005);
	auto result=seird_model.run_linear_implicit(t_start,u0,t_end,&cumulated_exposed,&cumulated_infected);
	
	//The output of the SEIRD Model must be filtered to the relevant classes with respect to the real life data set
	//Filter accordingly
	
	std::vector<double> timepoints_temp=std::get<0>(result);
	std::vector<co::EFloat64> data_filtered;
	timepoints=std::vector<co::EFloat64>();	
	for (int i=0;i<timepoints_temp.size();i++){
		timepoints.push_back(co::EFloat64(timepoints_temp[i]));
		data_filtered.push_back(cumulated_infected[i]);    //Filter cumulative Infected			
	}
	data=data_filtered;

	err= co::ErrorCode::NoError;
}

//This is the model function to be evaluated
void print_run(std::string path, double t_start, double t_end, const co::EVarManager<co::EFloat64>& v){

	//Set up parameters
	std::vector<double> alpha={(double)v.get_param(0).val,5.00169e-07 };
	std::vector<double> alpha_limits={50,100};
	double kappa=0.903178;
	double theta=4.14932000304998e-07;
	double qq=3.01401;
	double pp=6.0785;
	
	//Set up initial values for the S E I R D classes
	std::vector<double> u0={764000,2*28,28,0,0}; // Inital Values: Susceptibles, Exposed, Infected, Recoverd, Deceased	
	
	//Create model class and run the simulation
	std::vector<double> cumulated_infected;
	std::vector<double> cumulated_exposed;	
	ug::epi::SEIRD_VARA<std::vector<double>> seird_model(alpha, alpha_limits,kappa,theta,qq,pp);
	seird_model.change_step_size_time(0.01);
	seird_model.check=true;	
	auto result=seird_model.run_linear_implicit(t_start,u0,t_end,&cumulated_exposed,&cumulated_infected);
	
	//The output of the SEIRD model are 5 classes. But you might not need all of them
	//Filter accordingly
	std::ofstream output;	
	output.precision(15);
	//Write outputs
	output.open(path+"output.txt");
	output<<"#time\tCumulative Infected\tDeaths\n";
	std::vector<double> timepoints_temp=std::get<0>(result);
	std::vector<double> data_unfiltered=std::get<1>(result);

	ug::epi::write_data("", "_original_sim.txt", timepoints_temp, data_unfiltered,seird_model.names);
	for (int i=0;(i*5)<data_unfiltered.size();i++){
		output<<timepoints_temp[i];	
		output<<"\t"<<cumulated_infected[i]<<"\n";    //Filter cumulative Infected		
	}	
	output.close();
}

int main(){

	std::vector<std::string> names={"alpha1"};
	
	//562054533
	//509426533
	
	std::string dir= std::filesystem::current_path().string();

//	std::vector<co::EFloat64> bounds={co::EFloat64(0),co::EFloat64(1e-1),co::EFloat64(0),co::EFloat64(100),co::EFloat64(0.1),co::EFloat64(0.9),co::EFloat64(1),co::EFloat64(10),co::EFloat64(1),co::EFloat64(10)};
//	std::vector<co::EFloat64> bounds={co::EFloat64(5e-7),co::EFloat64(1e-3),co::EFloat64(1e-10),co::EFloat64(1),co::EFloat64(0.4),co::EFloat64(0.9),co::EFloat64(1),co::EFloat64(10),co::EFloat64(1),co::EFloat64(10)};

	std::vector<co::EFloat64> bounds={co::EFloat64(0),co::EFloat64(1e-5)};


	co::PSOOptions options;
	options.set_max_iterations(10);
	options.set_n_particles(200);
	options.set_n_groups(7);
	co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct> evaluator(dir,"subset_target.lua", evaluate_model);
	co::ParticleSwarmOptimizer<co::EpidemicsEvaluation<co::EFloat64,co::ConfigComputation::Local, co::ConfigOutput::Direct>> pso(options,evaluator);
	co::EVarManager<co::EFloat64> estimated_parameters;
	pso.run(estimated_parameters,names,bounds);
	
	std::cout<<"The estimated parameters are:\n";
	for (int i=0;i<names.size();i++){
		std::cout<<names[i]<<":"<<(double)estimated_parameters.get_param(i).val<<"\n";
	}
	
	print_run("", 0, 130, estimated_parameters);
}
