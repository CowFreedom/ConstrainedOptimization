/*! \mainpage Constrained Optimization Plugin for UG4
 *
 * \section intro_sec Overview
 *
 *The Constrained Optimization Plugin is a package consisting of various algorithms capable of optimizing functions with bounded domain. The following
 *algorithms are currently implemented:
 *File | Description
 *------------- | -------------
 *newton.h  | The Gauss Newton procedure minimizes functions that can be represented as a sum of squares.
 *pso.h  | The Particle Swarm Optimization (PSO) algorithm minimizes any continuous function on a compact domain.
 * This package is under active development. This manual was created in June 2020 for version number 0.32.
 
 *\subsection System Requirements
 *Due to the usage of OS specific functions for process generation, the plugin is not necessarily multiplatform.
 *In its current form, the package has been shown to work on Windows 10, Linux Ubuntu and macOS Mojave 10.14.4. On Windows 10, two compilers were tested, GCC 9.2.0 and Visual Studio v16.3.10. It is assumed that the package also works for older compiler versions.
 *On Linux Ubuntu only GCC was tested. If GCC is used, UG4 might have to be rebuilt with the -pthread flag. Using Clang might necessitates adding the -std=c++11 flag to the build process.
 * For a list of bugs and recommended features see the page issues.
 *

 */

/** \file parameter_estimation.h
 * This file is used if the optimization functions are indirectly called.
 * Indirectly means that the user only passes secondary information like path, options and
 * shell commands instead of directly calling the optimization functions.
 * The only work this file does is configuring the optimizers and reporting the
 * status of the end result. Useful if the optimizers can be selected and configured
 * based on user input.
 */

#pragma once
#include<string>
#include "options.h"
#include "parameters.h"
#include "../optimizers/newton.h"
#include "../optimizers/pso.h"
#include "efloat.h"
#include "../evaluation_classes/biogas_evaluation.h"
#include <typeinfo> //to dynamically check type
//#include <filesystem>

extern const float version_number=0.33; /*!< Version number of the project. Higher numbers indicated more up-to-date versions. Contrary to the version counting procedure in e.g. Rust, the version number's digits do not
give an indication about the severity of the updates. This means, that version 1.0 does not necessarily amount to significant changes compared to version 0.1.*/

namespace co{
	
	/*dir: directory where configuration files are
	shell_command: command that shall be executed (e.g. ugshell -ex biogas_app/biogas.lua -p Test.lua)
	E: Evaluation type
	*/
	
	template<class E, class T>
	bool run_newton_gauss(E& evaluator,const EVarManager<T>& parameters, EVarManager<T>& estimated_parameters,NewtonOptions options){
	//	BiogasEvaluation<EFloat64,ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua",ConfigOutput::File);
		
		NewtonOptimizer<E> optimizer(options,evaluator);
		ErrorCode res=optimizer.run(parameters, estimated_parameters);
		switch (res){
			case ErrorCode::NoError:{
				std::cout<<"Newton procedure finished successfully.\n";
				return true;
				break;
				
			}
			case ErrorCode::ParseError:{
				std::cout<<"Newton procedure finished unsuccessfully due to a error in parsing the data.\n";
				return false;
				break;
			}
			case ErrorCode::ComputationError:{
				std::cout<<"Newton procedure finished successfully due to an error in the computation.\n";
				return false;
				break;
			}
			case ErrorCode::OptimizationError:{
				std::cout<<"Newton procedure finished unsuccessfully due to an error in the optimization procedure.\n";
				return false;
				break;
			}
			case ErrorCode::PathError:{
				std::cout<<"Newton procedure finished unsuccessfully due to a path error.\n";
				return false;
				break;
			}
			default:{
				std::cout<<"Newton procedure finished unsuccessfully\n";
				return false;
				break;
			}
			
		}		
		return true;
	}

	bool RunNewtonGauss_BiogasEval(std::string dir,const EVarManager<EFloat64>& parameters, EVarManager<EFloat64>& estimated_parameters,double alpha){
		BiogasEvaluation<EFloat64,ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua",ConfigOutput::File);
		NewtonOptions options;
		options.set_stepsize_alpha(alpha);
		options.set_stepsize_decay(0.9);
		return run_newton_gauss<BiogasEvaluation<EFloat64,ConfigComputation::Local>, EFloat64>(evaluator, parameters,estimated_parameters,options) ;
	}
	
	template<class E, class T>
	bool run_pso(E& evaluator,const VarDescriptor<T>& var_descriptor, EVarManager<T>& estimated_parameters,PSOOptions options){
		co::ParticleSwarmOptimizer<E> pso(options,evaluator);
		
		ErrorCode res =pso.run(estimated_parameters,var_descriptor.get_names(),var_descriptor.get_bounds());
		switch (res){
			case ErrorCode::NoError:{
				std::cout<<"PSO procedure finished successfully.\n";
				return true;
				break;
				
			}
			case ErrorCode::ParseError:{
				std::cout<<"PSO procedure finished unsuccessfully due to a error in parsing the data.\n";
				return false;
				break;
			}
			case ErrorCode::ComputationError:{
				std::cout<<"PSO procedure finished successfully due to an error in the computation.\n";
				return false;
				break;
			}
			case ErrorCode::OptimizationError:{
				std::cout<<"PSO procedure finished unsuccessfully due to an error in the optimization procedure.\n";
				return false;
				break;
			}
			case ErrorCode::PathError:{
				std::cout<<"PSO procedure finished unsuccessfully due to a path error.\n";
				return false;
				break;
			}
			default:{
				std::cout<<"PSO procedure finished unsuccessfully\n";
				return false;
				break;
			}
			
		}			
	}

	bool RunPSO_BiogasEval(std::string dir, const VarDescriptor64& var_descriptor,EVar64Manager& estimated_parameters,int n_particles, int n_groups, int max_iterations){
		co::BiogasEvaluation<co::EFloat64,co::ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua", co::ConfigOutput::File);
		PSOOptions options;
		options.set_max_iterations(max_iterations);
		options.set_n_particles(n_particles);
		options.set_n_groups(n_groups);
		return run_pso<BiogasEvaluation<EFloat64,ConfigComputation::Local>, EFloat64>(evaluator,var_descriptor,estimated_parameters, options);

	}
	
}