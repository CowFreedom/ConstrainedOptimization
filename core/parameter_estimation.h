/*! \mainpage Constrained Optimization Plugin for UG4
 *
 * \section intro_sec Overview
 *
 *The Constrained Optimization Plugin is a package consisting of various algorithms capable of optimizing functions with bounded domain. The following
 *algorithms are currently implemented:
 *File | Description
 *------------- | -------------
 *newton.h  | The Gauss Newton procedure minimizes functions that can be represented as a sum of squares.
 *
 * This package is under active development. This manual was created in March 2020 for version number 0.31.
 
 *\subsection System Requirements
 *Due to the usage of OS specific functions for process generation, the plugin is not necessarily multiplatform.
 *In its current form, the package has been shown to work on Windows 10.  Two compilers were tested, GCC 9.2.0 and Visual Studio v16.3.10. It is assumed that the package also works for older compiler versions.
 *Small experiments in Linux Fedora have also been conducted. The results were encouraging but Linux is officially not yet supported.
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
#include "efloat.h"
#include "../evaluation_classes/biogas_evaluation.h"
//#include <filesystem>

extern const float version_number=0.31; /*!< Version number of the project. Higher numbers indicated more up-to-date versions. Contrary to the version counting procedure in e.g. Rust, the version number's digits do not
give an indication about the severity of the updates. This means, that version 1.0 does not necessarily amount to significant changes compared to version 0.1.*/

namespace co{
	
	/*dir: directory where configuration files are
	shell_command: command that shall be executed (e.g. ugshell -ex biogas_app/biogas.lua -p Test.lua)
	E: Evaluation type
	*/
	bool run_parameter_estimator(std::string dir, std::string shell_command, const EVar64Manager& parameters){
		
		NewtonOptions options;
		options.set_config_method(co::ConfigMethod::Newton);
		options.set_config_mode(co::ConfigMode::Debug);
		options.set_config_output(co::ConfigOutput::File);
		options.set_config_evaluation(co::ConfigEvaluation::BiogasEvaluation);
		options.set_config_computation(co::ConfigComputation::Local);	
		
		BiogasEvaluation<EFloat64,ConfigComputation::Local> evaluator(dir,"subset_target.lua", "subset_sim.lua");
		NewtonOptimizer<BiogasEvaluation<EFloat64,ConfigComputation::Local>> optimizer(options,evaluator);
		
		std::vector<EVar64Manager> params;
		params.push_back(parameters);
		
		ErrorCode res=optimizer.run(params);
		
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
				std::cout<<"Newton procedure finished  unsuccessfully\n";
				return false;
				break;
			}
			
		}
		
		
		return true;
		
	}
	
}