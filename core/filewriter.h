#pragma once

#include "efloat.h"
#include "parameters.h"
#include <fstream>
//#include <limits>
#include <iomanip>

namespace co{
	
	template<class T>
	class Writer{
		
	};
	
	template<>
	class Writer<EFloat64>{
		public:
		//Reason: Reason must be a single line
		void write(std::string& path, const EVarManager<EFloat64>& m,std::string reason=""){
			std::string output;
			if (reason.length()!=0){
				output.append("--");
				output.append(reason);
				output+="\n";				
			}
			std::ofstream lua_output;
			std::ofstream general_output;
			lua_output.open (path+"parameters.lua");
			general_output.open (path+"parameters.txt");
			general_output<<output;
			lua_output<<"parameters={";
			auto& v=m.get_params();
			auto& names=m.get_names();
			for (int i=0;i<v.size();i++){
				lua_output<<names[i];
				lua_output<<"=";
				lua_output<<(v[i]).to_lua_string();	
				lua_output<<",\n";
				general_output<<names[i]<<"="<<v[i]<<"\n";
			}
			//Now lua table version:

			
			lua_output<<"}";
		
			lua_output.close();
			general_output.close();
		}
		
		void write_matrix(std::string path,std::vector<EFloat64>& mat, size_t n,size_t m,std::string name,std::string reason=""){
			std::string output;
			if (reason.length()!=0){
				output.append("--");
				output.append(reason);
				output+="\n";				
			}
			
			std::ofstream general_output;
			general_output.precision(12);
			//general_output<<std::fixed<<std::setprecision(12);
		//	general_output.precision(std::numeric_limits<double>::max_digits10);
			general_output.open (path+name);
			general_output<<output;
			for (int i=0;i<n;i++){
				for (int j=0;j<m;j++){
					general_output<<mat[i*m+j].get_v()<<"   ";
				}
				general_output<<"\n";
				
			}
			general_output.close();
		}
		
		
	};
	
}