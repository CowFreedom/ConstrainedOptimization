/** \file geometry_sampler.h
 * Functions defined here sample points from other functions. Useful to visualize loss functions for example.
 */



#pragma once

#include "options.h"
#include "parameters.h"
#include "efloat.h"
#include "../evaluation_classes/evaluation.h"
#include <random>
#include <sstream>
#include <string>

namespace co{

	/*! Samples points from the loss function of an evaluation class and saves these results to file. Loss function
	must only satisfy continuity in the specified sample interval.
	@param[in] evaluator Evaluation type used for calculating the function evaluations
	@param[in] param_names sets parameters names
	@param[in] bounds configures parameter bounds
	@param[in] n number of samples thrown
	\return Code indicating success or failure of sampling from the loss function
	*/
	template<class F, class E>
	ErrorCode sample_loss_geometry(E& evaluator,const std::vector<std::string>& param_names,const std::vector<F>& bounds, size_t n){
		int dim=bounds.size()/2;
		std::vector<EVarManager<EFloat64>> evaluations(n);
		std::vector<std::vector<EFloat64>> position(n);
				
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()	
		std::vector<EFloat64> target_data;
		std::vector<EFloat64> target_times;
		ErrorCode load_code=evaluator.load_target(target_times,target_data); //load target vector
		
		if (load_code!=ErrorCode::NoError){
			std::cerr<<"Error loading target data!\n";
			return load_code;
		}			
		for (int i=0;i<n;i++){
			for (int j=0;j<dim;j++){
				F low=bounds[2*j];
				F high=bounds[2*j+1];
				std::uniform_real_distribution<> dis((double) low, (double) high);
				EFloat64 val(dis(gen));
				position[i].push_back(val);
				EVar<EFloat64> var=EVar<EFloat64>(val, low, high);
				evaluations[i].add(param_names[j],var);
			}					
		}
		ErrorCode eval_error;
		std::vector<std::vector<EFloat64>> evals=evaluator.eval(evaluations, target_times, eval_error,"Evaluating function");
		if(eval_error!=ErrorCode::NoError){
			std::cout<<"Error evaluating the target function\n";
			return eval_error;
		}
		
		std::ostringstream res;
		res.precision(12);
		for (int j=0;j<dim;j++){
			res<<param_names[j]<<"\t";			
		}
		res<<"Loss function value\n";

		
		for (int i=0;i<n;i++){
			for (int j=0;j<dim;j++){
				res<<F(position[i][j])<<"\t";			
			}
			res<<F(evaluator.s(evals[i], target_data))<<"\n";
		}
		evaluator.send_info(res.str(), "samples");
		
		return ErrorCode::NoError;
		
	}
	/** \example sample_loss_geometry_example.cpp
 * This is an example on how to use this function in conjunction
 * with an Evaluation instance.
 */
}
