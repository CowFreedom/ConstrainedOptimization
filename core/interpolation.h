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
		
		/*Interpolates the simulation grid world coordinates to the target world coordinates (i.e. experimental data world coordinates).
		Interpolation is very simple (and not even triangular) so use only if grid is relatively dense compared to real world data*/
		template<class T>
		ErrorCode planar_grid_to_world(std::vector<T> world_position_target, std::vector<T> world_position_grid, std::vector<T> griddata, std::vector<T>& storage, int dim_data){
			int nDatapoints=0.5*world_positions_target.size();
			int points_per_dimension=griddata.size()/dim_data;
			for (int i=0;i<world_positions_target.size();i+=2){
				F wx=world_position_target[i];
				F wy=world_position_target[i+1];
				F gx=world_position_grid[0];
				F gy=world_position_grid[1];
				size_t min_index=0;
				size_t current_index=1;
				T current_distance=(wx-gx)*(wx-gx)+(wy-gy)*(wy-gy);
				T min_distance=current_distance;
				for (int j=2;j<0.5*world_position_grid.size();j+=2){
					gx=world_position_grid[j];
					gy=world_position_grid[j+1];
					current_distance=(wx-gx)*(wx-gx)+(wy-gy)*(wy-gy);
					if (current_distance<min_distance){
						min_index=current_index;
					}
				}
				for (int j=0;j<dim_data;j++){
					storage.push_back(griddata[min_index+j*dim_data]);
				}
			}
		}
	}
}
