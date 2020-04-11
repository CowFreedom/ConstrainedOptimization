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

namespace co{

	/*! This class represents how problems of the UG4 biogas_app kind are evaluated. This specific example servers as an interface. For an implementation of this
	* interface see BiogasEvaluation<T,ConfigComputation::Local>.
	*/
	template<class T,ConfigComputation M>
	class BiogasEvaluation: public Evaluation<T,M>{
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
	* Because the ConfigComputation enum is set to Local, all function evaluations in computation_modes.h are local. Within the code
	* the ConfigOutput is set to File, which means it assumes that function evaluations f(t) are written to disk and must be parsed in order to be used.*/
	template<class T>
	class BiogasEvaluation<T,ConfigComputation::Local>: public Evaluation<T,ConfigComputation::Local>{
		
		private:
		//std::vector<T> timepoints; //This vector is only used to tailor the sim vector accordingly.
		std::string table_dir;
		std::string infile_name;
		
		ComputationMode<ConfigComputation::Local,BiogasEvaluation<T,ConfigComputation::Local>,T> computer; //evaluates inputs according to the model formulation TODO: Change threadcount (last argument)
		
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
		BiogasEvaluation(std::string _table_dir,  std::string _infile_name, std::string _outfile_name):table_dir(_table_dir),infile_name(_infile_name), outfile_name(_outfile_name),computer(ComputationMode<ConfigComputation::Local,BiogasEvaluation<T,ConfigComputation::Local>,T>(this, ConfigOutput::File,NTHREADS_SUPPORTED,_table_dir)){
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
		bool send_parameters(EVarManager<T>& params, std::string description=""){
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
			//std::cout<<"In Parse!\n";
			std::string outfile_path=table_dir+'/'+_outfile_name; //use std filesystem later
			//std::cout<<"Parse: Lua table path "<<outfile_path<<"\n";
			std::ifstream file(outfile_path);
			if (file.fail()){
				std::cerr<<"Couldn't open parse input table at "<<outfile_path<<"\n";
				return ErrorCode::ParseError;
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string s=buffer.str();
			
			std::string file_dir=table_dir;
			if (data_path!=""){
				file_dir=data_path;
			}
			
			bool inFileName=false;
			bool inSelectedColumns=false;
			std::string filename;
			std::vector<std::vector<T>> files; //content of loaded .csv files
			std::vector<int> cols; //columns per file
			std::vector<int> selected_cols;
			int rows;
			int num_files=0; //counts the number of files opened
			for (int i=0;i<s.size();i++){
				if (inFileName){
					if(s[i]!='\"'){
						filename+=s[i];
					}
					else{
						inFileName=false;
					}
				}
				else if (s[i]=='\"'){
					inFileName=true;
				}
				else if (inSelectedColumns){
					if (isdigit(s[i])){
						selected_cols.push_back(static_cast<int>(s[i]-'0'));
					}
					else if (s[i]==']'){
						inSelectedColumns=false;		
				        std::vector<T> v;
						std::string filepath= file_dir+'/'+filename; //path to file
						//std::cout<<"File path:"<<filepath<<"\n";
						if(num_files==0){
							std::vector<int> time_col;
							time_col.push_back(selected_cols.front());
							selected_cols.erase(selected_cols.begin());
							co::parse_csv_specific(filepath,times," ",time_col);
							co::parse_csv_specific(filepath,v," ",selected_cols);
						}
						else{
							co::parse_csv_specific(filepath,v," ",selected_cols);
						}
						
						files.push_back(v);
						cols.push_back(selected_cols.size());
						rows=v.size()/selected_cols.size();
						selected_cols.clear();
						filename.clear();
						num_files++;
						
						
					}
				}
				else if (s[i]=='['){
					inSelectedColumns=true;
				}
			
			}
			
			/*Merge all loaded .csv files*/		
			for(int i=0; i<rows;i++){
				for (int j=0;j<files.size();j++){
					for (int k=0;k<cols[j];k++){
						data.push_back(files[j][i*cols[j]+k]);
					}
				}
			}
			if (_rows!=0){
				*_rows=rows; //assign outside rows to rows
			}
			
			return ErrorCode::NoError;
		}
		
		/*Linearly interpolates source into target  and saves it into storage
		nt: length of target
		ns: length of source
		cols: number of columns in source!*/
		ErrorCode tailor_array(std::vector<T>& targettimes, std::vector<T>& sourcetimes, const std::vector<T>& source, std::vector<T>& storage, int cols){
	
			int saved_rows=0;
			int j=0;
			int nt=targettimes.size();
			int ns=sourcetimes.size();
			storage.resize(nt*cols);
			int i=0;
			int substract=0;
			if (nt==ns){
				//std::cout<<"ns==nt!";	
				//std::cin.get();
				T t1=sourcetimes[nt-2];
				T t2=sourcetimes[nt-1];
				T tc=targettimes[nt-1];
				T t=(tc-t1)/(-t1+t2);
				for (int k=0;k<cols;k++){
				//std::cout<<"Bis hier: j: "<<j<<" saved_rows: "<<saved_rows<<" k:"<<k<<"\n";
				storage[(nt-1)*cols+k]=source[(nt-2)*cols+k]*(T(1.0)-t)+source[(nt-1)*cols+k]*t;
				}
				substract=cols;
			}
			if (sourcetimes[0]>targettimes[0]){
				sourcetimes[0]=targettimes[0];
			}
			else{
				targettimes[0]=sourcetimes[0];
			}
			if(targettimes[nt-1]>sourcetimes[ns-1]){
				std::cerr<<"Error: The target vector's last time value is higher than the source vectors last time value. This error is often caused by UG4 not finishing the computation. Check the most recent console_output.log in the respective evaluation folders.";
				//std::cin.get();
				return ErrorCode::ParseError;
			}
			//sourcetimes[0]=targettimes[0]; //remove later. dont do it
			//std::cout<<"Bis hier!\n";
			while((saved_rows*cols)<(nt-substract)*cols){
				//	std::cout<<"sourcetime[j]: "<< sourcetimes[j]<<"sourcetime[j+1]"<<sourcetimes[j+1]<<"  targettimes[i]"<<targettimes[i]<<"\n";
				//	std::cout<<"nt-substract:"<<nt-substract<<"\n";
					if (sourcetimes[j]>sourcetimes[j+1]){
						std::cout<<"Error: The sourcetimes vector is not monotonically increasing.\n";
						return ErrorCode::ParseError;
					}
					
					if ((sourcetimes[j]<=targettimes[i])&&(sourcetimes[j+1]>=targettimes[i])){
						T t1=sourcetimes[j];
						T t2=sourcetimes[j+1];
						T tc=targettimes[i];
						T t=(tc-t1)/(-t1+t2);
						//std::cout<<"-t1:"<<-t1<<"  t2:"<<t2<<"    -t1+t2:"<<-t1+t2<<"    tc"<<tc<<"tc-t1:"<<tc-t1<<"\n";
						for (int k=0;k<cols;k++){
				//		std::cout<<"Bis hier: j: "<<j<<" saved_rows: "<<saved_rows<<" k:"<<k<<"t: "<<t<<"\n";
				//		std::cout<<"n:"<<saved_rows*cols+k<<" value: "<<source[j*cols+k]*(T(1.0)-t)+source[(j+1)*cols+k]*t<<"\n";
						storage[saved_rows*cols+k]=source[j*cols+k]*(T(1.0)-t)+source[(j+1)*cols+k]*t;
						}
		
					saved_rows++;
					i++;
				}		
				else{
					j++;
				}
			}
		
			return ErrorCode::NoError;
		}		

};


}