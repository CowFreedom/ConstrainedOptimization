#pragma once
#include "options.h"
#include "efloat.h"

namespace co{

	namespace utility{
		/*Linearly interpolates source into target  and saves it into storage
		nt: length of target
		ns: length of source
		cols: number of columns in source!*/
		template<class T>
		ErrorCode tailor_array_linearly(std::vector<T>& targettimes, std::vector<T>& sourcetimes, const std::vector<T>& source, std::vector<T>& storage, int cols){
	
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
				substract=1; //war vorher: substract=cols
			}
			if ((double)sourcetimes[0]>(double)targettimes[0]){
				sourcetimes[0]=targettimes[0];
			}
			else{
				targettimes[0]=sourcetimes[0];
			}
			if((double)targettimes[nt-1]>(double)sourcetimes[ns-1]){
				std::cerr<<"Error: The target vector's last time value is higher than the source vectors last time value. This error is often caused by UG4 not finishing the computation. Check the most recent console_output.log in the respective evaluation folders.";
				//std::cin.get();
				return ErrorCode::ParseError;
			}
			//sourcetimes[0]=targettimes[0]; //remove later. dont do it
			//std::cout<<"Bis hier!\n";
			while((saved_rows*cols)<(nt-substract)*cols){
				//	std::cout<<"sourcetime[j]: "<< sourcetimes[j]<<"sourcetime[j+1]"<<sourcetimes[j+1]<<"  targettimes[i]"<<targettimes[i]<<"\n";
				//	std::cout<<"nt-substract:"<<nt-substract<<"\n";
				if (j<(sourcetimes.size()-1)){
					if (sourcetimes[j]>sourcetimes[j+1]){
						std::cout<<"Error: The sourcetimes vector is not monotonically increasing.\n";
						return ErrorCode::ParseError;
					}
				}
			
					if (((double)sourcetimes[j]<=(double)targettimes[i])&&((double)sourcetimes[j+1]>=(double)targettimes[i])){
						T t1=sourcetimes[j];
						T t2=sourcetimes[j+1];
						T tc=targettimes[i];
						T t=(tc-t1)/(-t1+t2);
						//std::cout<<"-t1:"<<-t1<<"  t2:"<<t2<<"    -t1+t2:"<<-t1+t2<<"    tc"<<tc<<"tc-t1:"<<tc-t1<<"\n";
						for (int k=0;k<cols;k++){
				//		std::cout<<"Bis hier: j: "<<j<<" saved_rows: "<<saved_rows<<" k:"<<k<<"t: "<<t<<"\n";
				//		std::cout<<"n:"<<saved_rows*cols+k<<" value: "<<source[j*cols+k]*(T(1.0)-t)+source[(j+1)*cols+k]*t<<"\n";
						storage[saved_rows*cols+k]=source[j*cols+k]*(T(1.0)-t)+source[(j+1)*cols+k]*t;
					//	std::cout<<targettimes[i]<<"   "<<cols<<"   "<<saved_rows*cols+k<<"\n";
					//	std::cin.get();
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

		/*Interpolates the simulation grid world coordinates to the target world coordinates (i.e. experimental data world coordinates).
		Interpolation is very simple (and not even triangular) so use only if grid is relatively dense compared to real world data*/
		template<class T>
		ErrorCode planar_grid_to_world(std::vector<T> world_position_target, std::vector<T> world_position_grid, std::vector<T> griddata, std::vector<T>& storage, int dim_data, int dim_time){
			int nDatapoints=0.5*world_position_target.size();
			int points_per_dimension=griddata.size()/(dim_time*dim_data);
			std::vector<int> min_indices;
			for (int i=0;i<world_position_target.size();i+=2){
				T wx=world_position_target[i];
				T wy=world_position_target[i+1];
				T gx=world_position_grid[0];
				T gy=world_position_grid[1];
				size_t min_index=0;
				size_t current_index=1;
				T current_distance=(wx-gx)*(wx-gx)+(wy-gy)*(wy-gy);
				T min_distance=current_distance;
				for (int j=2;j<world_position_grid.size();j+=2){
					gx=world_position_grid[j];
					gy=world_position_grid[j+1];
					current_distance=(wx-gx)*(wx-gx)+(wy-gy)*(wy-gy);
					if ((double)current_distance<(double)min_distance){
						min_index=current_index;
						min_distance=current_distance;
					}
					current_index++;
				}
				min_indices.push_back(min_index);
					
			}
				for(int i=0;i<dim_time;i++){
					for (int k= 0;k<min_indices.size();k++){
						for (int j=0;j<dim_data;j++){
							//std::cout<<min_indices[k]+j*points_per_dimension+i*(griddata.size()/dim_time)<<"\t";
							storage.push_back(griddata[min_indices[k]+j*points_per_dimension+i*(griddata.size()/dim_time)]);
						}
					}
					
				}
			return ErrorCode::NoError;
		}
	}
}
