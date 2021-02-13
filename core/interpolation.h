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
	}
}
