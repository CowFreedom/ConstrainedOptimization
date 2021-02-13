/** \file biogas_evaluation.h
*This file defines a child class of the abstract base class Evaluation, which 
*can be used by optimizers like newton.h. BiogasEvaluation defines how data is loaded and the target function to be optimized is evaluated
*for the UG4 biogas_app problem.
*This class can be reused for other problems that behave "similar" (in the sense of data loading, interpolation and evaluation) to the 
*biogas_app problem (which are many).
*As evaluation subclasses are not only dependent on the numeric type but also the computation mode ConfigComputation, different definitions of
*BiogasEvaluation might be needed.
*/

#pragma once
#include "evaluation.h"
#include "../core/options.h"
#include "../core/efloat.h"
#include <fstream>
#include <string>
#include <sstream>
#include "../core/parse.h"
#include "../core/computation_modes.h"
#include "../core/filewriter.h"
#include "../core/interpolation.h"


namespace co{

	/*! This class represents how problems of the UG4 biogas_app kind are evaluated. This specific example servers as an interface. For an implementation of this
	* interface see BiogasEvaluation<T,ConfigComputation::Local,ConfigOutput::File>.
	*/
	template<class T,ConfigComputation M, ConfigOutput O>
	class BiogasEvaluation: public Evaluation<T,M,O>{
		public:
		const ConfigComputation computation_mode=M;
		
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v, const std::vector<T>& target, ErrorCode& e,std::string message="") =0;
		
		ErrorCode load_target(std::vector<T>& t,std::vector<T>& d)=0;
		
		std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times,std::string folder_name, ErrorCode& e,std::string message="")=0;
		
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride)=0;
		
	
		virtual T s(const std::vector<T>& x, const std::vector<T>& y)=0;

		
		};

	/*! This class represents how problems of the UG4 biogas_app kind are evaluated. This class can be reused for other problems that behave "similar" (in the sense of data loading, interpolation and evaluation) to the 
	* biogas_app problem (which are many).
	* Because the ConfigComputation enum is set to Local, all function evaluations in computation_modes.h are local. The template parameter
	* ConfigOutput is set to File, which means it assumes that function evaluations f(t) are written to disk and must be parsed in order to be used.*/
	template<class T>
	class BiogasEvaluation<T,ConfigComputation::Local, ConfigOutput::File>: public Evaluation<T,ConfigComputation::Local, ConfigOutput::File>{
		
		private:
		//std::vector<T> timepoints; //This vector is only used to tailor the sim vector accordingly.
		std::string table_dir;
		std::string infile_name;
		ComputationMode<ConfigComputation::Local,ConfigOutput::File,BiogasEvaluation<T,ConfigComputation::Local, ConfigOutput::File>,T> computer; //evaluates inputs according to the model formulation TODO: Change threadcount (last argument)
		
		public:
		std::string outfile_name;
		std::vector<T> target_times;
		BiogasEvaluation(){
			
		}
	/*	
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride) override{	
			for (size_t i=0;i<x.size();i++){
				if (i%2 ==0){
					result[i+stride]=(y[i]-x[i]);
				}
				else{
					result[i+stride]=T(10.0)*(y[i]-x[i]);
				}
				
			}			
		}
		*/
		
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride) override{	
			for (size_t i=0;i<x.size();i++){
				result[i+stride]=(y[i]-x[i]);
			}			
		}
		
		
		virtual T s(const std::vector<T>& x, const std::vector<T>& y) override{
			T sum=T(0.0);
			for (size_t i=0;i<x.size();i++){
				sum+=(y[i]-x[i])*(y[i]-x[i]);
			}	
			return sum;
		}		
		
		
	/*
		virtual T s(const std::vector<T>& x, const std::vector<T>& y) override{
			T sum=T(0.0);
			for (size_t i=0;i<x.size();i++){
				
								if (i%2 ==0){
					sum+=(y[i]-x[i])*(y[i]-x[i]);
				}
				else{
					sum+=T(10.0)*(y[i]-x[i])*(y[i]-x[i]);
				}
			}	
			return sum;
		}
		*/
		BiogasEvaluation(std::string _table_dir,  std::string _infile_name, std::string _outfile_name):table_dir(_table_dir),infile_name(_infile_name), outfile_name(_outfile_name),computer(ComputationMode<ConfigComputation::Local,ConfigOutput::File,BiogasEvaluation<T,ConfigComputation::Local, ConfigOutput::File>,T>(this,NTHREADS_SUPPORTED,_table_dir)){
			//computer=ComputationMode<ConfigComputation::Local,T>(ConfigOutput::File,4);
			//std::cout<<"Pointer address outside at creation:"<<this<<"\n";
		}
	
		const ConfigComputation computation_mode=ConfigComputation::Local;
		
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times,ErrorCode& e, std::string message="") override{
			//std::cout<<"Evaluiere!\n";
			
			//parse("D:/Documents/Programming/ug4/ug4/apps/parameter_estimation/estebis_downflow_pe","subset_output.lua");
			
			target_times=_target_times;
			return computer.eval(v,e,message);
		};
		
		std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times, std::string folder_name,ErrorCode& e, std::string message=""){
			
			target_times=_target_times;
			return computer.eval_specific(v,folder_name,computer.get_current_iteration()-1,e,message);
		};
		
		//d=data, t=targettimes
		ErrorCode load_target(std::vector<T>& t,std::vector<T>& d) override{
			ErrorCode ret=parse_csv_table_times(table_dir,infile_name,d,t); //TODO Remove function arguments 
			return ret;
			
		}
		
		/*Writes the Jacobi matrix to file. Jacobi is an NxM matrix*/
		bool send_matrix(std::vector<T>& jacobi, size_t n,size_t m,std::string description="") const{			
			std::string path=computer.get_current_evaluation_path();
			size_t iteration=computer.get_current_iteration()-1;
			path+=std::string("/iteration_")+std::to_string(iteration)+"/";
			Writer<T> w;
			w.write_matrix(path,jacobi,n,m,description+".txt",description);	
			return true;
		}
		
		
		bool send_parameters(EVarManager<T>& params, std::string description="") const{
			std::string path=computer.get_current_evaluation_path();
			size_t iteration=computer.get_current_iteration()-1;
			Writer<T> w;
			w.write(path,params,description);
			return true;	
		}
		

		bool send_info(std::string info, std::string description=""){
			std::string path=computer.get_current_evaluation_path();
			size_t iteration=computer.get_current_iteration()-1;
			path+=std::string("/iteration_")+std::to_string(iteration)+"/";
			Writer<T> w;
			w.write_info(path,description+".txt",info);
			return true;
		}
		
		/*just copies parse_csv_table_times. Needed if ConfigComputation::File is set, so that computation_modes.h can parse the result
		*/
		ErrorCode parse(std::string& data_path, std::vector<T>& data){
						
			std::vector<T> times;
			std::vector<T> raw_data;
			int rows;
			ErrorCode ret=parse_csv_table_times(table_dir,outfile_name,raw_data, times, data_path, &rows);
			int cols=raw_data.size()/rows;
			//If parsing was successful, linearly interpolate data to target times
			if (ret==ErrorCode::NoError){
				ret=tailor_array(target_times,times,raw_data,data,cols);
			}
			return ret;
		}
		
		/*Parses contents specified a table and saves them in data and times. As opposed to 
		parse_csv_table, this time the first entry of the first datafile will be considered as the time vector.
		Data will accordingly only consist of datapoints (e.g. Methane production) and the time vector only of the times.
		The reason this is not in the parse.h file, is that other differential problems might have multiple variables (
		as usual for PDE's). Therefore, this is implemented on a per-problem basis.
		Datapath is the path of the data, if it differs from the paths in tabledir*/
		ErrorCode parse_csv_table_times(std::string table_dir, std::string _outfile_name, std::vector<T>& data, std::vector<T>& times, std::string data_path="", int* _rows=0){
			return co::utility::parse_csv_table_times(table_dir,_outfile_name,data,times,data_path,_rows);
		}
		
		/*Linearly interpolates source into target  and saves it into storage
		nt: length of target
		ns: length of source
		cols: number of columns in source!*/
		ErrorCode tailor_array(std::vector<T>& targettimes, std::vector<T>& sourcetimes, const std::vector<T>& source, std::vector<T>& storage, int cols){
	
			return co::utility::tailor_array_linearly(targettimes, sourcetimes, source,storage,cols);
		}		

};


	/*! This class represents how problems of the UG4 biogas_app kind are evaluated. This class can be reused for other problems that behave "similar" (in the sense of data loading, interpolation and evaluation) to the 
	* biogas_app problem (which are many).
	* Because the ConfigComputation enum is set to Local, all function evaluations in computation_modes.h are local. The template parameter
	* ConfigOutput is set to Direct, which means it assumes that function evaluations f(t) of the problem formulation are invoked through C++ calls directly.*/
	template<class T>
	class BiogasEvaluation<T,ConfigComputation::Local, ConfigOutput::Direct>: public Evaluation<T,ConfigComputation::Local, ConfigOutput::Direct>{
		
		private:
		ComputationMode<ConfigComputation::Local,ConfigOutput::File,BiogasEvaluation<T,ConfigComputation::Local, ConfigOutput::Direct>,T> computer; //evaluates inputs according to the model formulation TODO: Change threadcount (last argument)
		
		public:
		std::vector<T> (&eval_function)(std::vector<T>&);
		
		std::vector<T> target_times;
		BiogasEvaluation(){
			
		}
		
		BiogasEvaluation(std::vector<T>(&_eval_function)(std::vector<T>&)):eval_function(_eval_function){
			
		}
			
		std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v, const std::vector<T>& target, ErrorCode& e,std::string message="") override{
			
			return computer.eval(v,e,message);
		}
		
		ErrorCode load_target(std::vector<T>& t,std::vector<T>& d) {
			return ErrorCode::NoError;
		}
		
		std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times,std::string folder_name, ErrorCode& e,std::string message=""){
			return computer.eval(v,e,message);
			
		}
		
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride) override{	
			for (size_t i=0;i<x.size();i++){
				result[i+stride]=(y[i]-x[i]);
			}			
		}
		
		
		virtual T s(const std::vector<T>& x, const std::vector<T>& y) override{
			T sum=T(0.0);
			for (size_t i=0;i<x.size();i++){
				sum+=(y[i]-x[i])*(y[i]-x[i]);
			}	
			return sum;
		}				
		
	};
}
