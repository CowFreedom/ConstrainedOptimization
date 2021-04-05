/** \file epidemics_evaluation.h
*This file defines a child class of the abstract base class Evaluation, which 
*can be used by optimizers like newton.h. EpidemicsPDEEvaluation defines how data is loaded and the target function to be optimized is evaluated
*for the UG4 epidemics_app problem.
*This class can be reused for other problems that behave "similar" (in the sense of data loading, interpolation and evaluation) to the 
*epidemics_app problem (which are many).
*As evaluation subclasses are not only dependent on the numeric type but also the computation mode ConfigComputation, different definitions of
*EpidemicsPDEEvaluation might be needed.
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

	/*! This class represents how problems of the UG4 epidemics_app kind are evaluated. This specific example servers as an interface. For an implementation of this
	* interface see EpidemicsPDEEvaluation<T,ConfigComputation::Local,ConfigOutput::File>.
	*/
	template<class T,ConfigComputation M, ConfigOutput O>
	class EpidemicsPDEEvaluation: public Evaluation<T,M,O>{
		public:
		const ConfigComputation computation_mode=M;
		
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v, const std::vector<T>& target, ErrorCode& e,std::string message="") =0;
		
		ErrorCode load_target(std::vector<T>& t,std::vector<T>& d)=0;
		
		std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times,std::string folder_name, ErrorCode& e,std::string message="")=0;
		
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride)=0;
		
	
		virtual T s(const std::vector<T>& x, const std::vector<T>& y)=0;

		
		};

	/*! This class represents how problems of the UG4 epidemics_app kind are evaluated. This class can be reused for other problems that behave "similar" (in the sense of data loading, interpolation and evaluation) to the 
	* epidemics_app problem (which are many).
	* Because the ConfigComputation enum is set to Local, all function evaluations in computation_modes.h are local. The template parameter
	* ConfigOutput is set to File, which means it assumes that function evaluations f(t) are written to disk and must be parsed in order to be used.*/
	template<class T>
	class EpidemicsPDEEvaluation<T,ConfigComputation::Local, ConfigOutput::File>: public Evaluation<T,ConfigComputation::Local, ConfigOutput::File>{
		
		private:
		std::string table_dir;
		std::string infile_name;
		std::vector<std::string> sim_filenames;
		std::vector<int> sim_selected_dimensions;
		size_t points_per_timestep;
		ComputationMode<ConfigComputation::Local,ConfigOutput::File,EpidemicsPDEEvaluation<T,ConfigComputation::Local, ConfigOutput::File>,T> computer; //evaluates inputs according to the model formulation TODO: Change threadcount (last argument)
		
		public:
		std::string outfile_name;
		std::vector<T> target_times;
		std::vector<T> target_positions;
		std::vector<int> target_selected_columns;
		EpidemicsPDEEvaluation(){
			
		}
		
		//This function generates the residual vector. If changed, the method "s" below has also be adjusted
		virtual void r_i(const std::vector<T>& x, const std::vector<T>& y, std::vector<T>& result,int stride) override{	
			for (size_t i=0;i<x.size();i++){
				result[i+stride]=(y[i]-x[i]);
			}			
		}
		
		//This function calculates the squared loss from the resiudals defined in the method r_i. If changed, r_i above has also be adjusted
		virtual T s(const std::vector<T>& x, const std::vector<T>& y) override{
			T sum=T(0.0);
			for (size_t i=0;i<x.size();i++){
				sum+=(y[i]-x[i])*(y[i]-x[i]);
			}	
			return sum;
		}		

		EpidemicsPDEEvaluation(std::string _table_dir,  std::string _infile_name, std::string _outfile_name):table_dir(_table_dir),infile_name(_infile_name), outfile_name(_outfile_name),computer(ComputationMode<ConfigComputation::Local,ConfigOutput::File,EpidemicsPDEEvaluation<T,ConfigComputation::Local, ConfigOutput::File>,T>(this,NTHREADS_SUPPORTED,_table_dir)){
		}
	
		//This means the computation is optimized for a local machine and no PC cluster for example.
		const ConfigComputation computation_mode=ConfigComputation::Local;
		
		virtual std::vector<std::vector<T>> eval(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times,ErrorCode& e, std::string message="") override{
			target_times=_target_times;
			return computer.eval(v,e,message);
		};
		
		std::vector<std::vector<T>> eval_specific(const std::vector<EVarManager<T>>& v,const std::vector<T>& _target_times, std::string folder_name,ErrorCode& e, std::string message=""){
			
			target_times=_target_times;
			return computer.eval_specific(v,folder_name,computer.get_current_iteration()-1,e,message);
		};
		
		//This specifies how the target data is loaded
		ErrorCode load_target(std::vector<T>& t,std::vector<T>& d) override{
			if (target_positions.size()>=1){
				target_positions=std::vector<T>();
			}
			ErrorCode ret=co::utility::parse_csv_table_times_pde(table_dir,infile_name,d,target_positions,t,target_selected_columns); //TODO Remove function arguments 
			co::utility::parse_table_dim_pde(table_dir,outfile_name, sim_selected_dimensions, sim_filenames);
			points_per_timestep=d.size()/t.size();		
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
		
		//Write the parameters to file
		bool send_parameters(EVarManager<T>& params, std::string description="") const{
			std::string path=computer.get_current_evaluation_path();
			size_t iteration=computer.get_current_iteration()-1;
			Writer<T> w;
			w.write(path,params,description);
			return true;	
		}
		
		//Writes arbitrary information to file
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
			/*			
			std::vector<T> times;
			std::vector<T> raw_data;
			std::vector<T> sim_positions;
			std::vector<int> temp;
			int rows;
			ErrorCode ret=co::utility::parse_csv_table_times_pde(table_dir,outfile_name,raw_data,target_positions,times,temp,data_path,&rows);
			int cols=raw_data.size()/rows;
			std::vector<T> interpolated_data;
			std::string path=computer.get_current_evaluation_path();
			size_t iteration=computer.get_current_iteration()-1;
			co::utility::parse_csv(path+=std::string("/iteration_")+std::to_string(iteration)+"/"+"gridmapping_"+"outfile_name", sim_positions,"\t");
*/
			//ErrorCode ret=co::utility::parse_csv(path+=std::string("/iteration_")+std::to_string(iteration)+"/"+"output0.txt", std::vector<T>& tmp,std::string delimiter, int* _cols=nullptr)

			//Parsing of simulated_data
			double current_time=0;
			double successor_time=0;
			int i=0;
			int index=0;

			//collected sim data 
			std::vector<T> sim_data;
			//tmp vector
			std::vector<T> parsed_sim_times;
			std::vector<T> grid_world_coordinates;
			co::utility::parse_csv(data_path+"gridmapping_"+sim_filenames[0]+".txt", grid_world_coordinates,"\t");	


			while (true){
				std::string path = data_path+sim_filenames[0]+std::to_string(i)+".txt";
				std::string path_successor=data_path+sim_filenames[0]+std::to_string(i+1)+".txt";
				std::ifstream file(path_successor);
				std::vector<double> tmp;
						
				//If file does not exist, exist loop
				if (file.fail()){
					break;
				}
	
				co::utility::parse_pde_time(path, current_time, "\t");
				co::utility::parse_pde_time(path_successor, successor_time, "\t");
				i++;
				//std::cout<<"\n\nCurrent time of simulated data: "<<current_time<< "  The successor time is:"<<successor_time<<"\n";	
				
				if ((current_time <= (double)target_times[index]) && (successor_time > (double)target_times[index])){
					parsed_sim_times.push_back(current_time);
										
					co::utility::parse_csv(path,tmp,"\t");
					int offset=grid_world_coordinates.size() / 2;
		
					for (int i = 0; i<target_selected_columns.size()-3;i++){
						for(int j=0;j<offset;j++){
							//std::cout<<j+offset*(selected_columns[i+3]-3)<<"\n";
							sim_data.push_back(tmp[j+offset*(sim_selected_dimensions[i])]);
							
						}
					}
						
					index++;
				}
			}
			

				std::vector<T> filtered_data;
				co::utility::planar_grid_to_world(target_positions, grid_world_coordinates , sim_data, filtered_data, target_selected_columns.size()-3,parsed_sim_times.size());
/*
			for (auto& x: filtered_data){
				std::cout<<(double)x<<"\t";
			}
			std::cout<<"\n\n";
*/
			//co::utility::planar_grid_to_world(target_positions, sim_positions, raw_data, interpolated_data, cols);
			//If parsing was successful, linearly interpolate data to target times
			ErrorCode ret=tailor_array(target_times,parsed_sim_times,filtered_data,data,points_per_timestep);
/*			for (auto& x: data){
				std::cout<<(double)x<<"\t";
			}
			std::cout<<"\n\n";
			std::cin.get();
*/			
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

}
