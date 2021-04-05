#ifndef UG_DATAPARSE
#define UG_DATAPARSE
#pragma once

#include <fstream>
#include <cstdlib>
#include <vector>
#include <array>
#include<string>
/*
Usage of std:: path: https://www.bfilipek.com/2017/08/cpp17-details-filesystem.html#working-with-the-path-object
*/

namespace co{

	namespace utility{
	
	/*Options that can be given to a parse instance*/
	class ParseOptions{
		public:
		const int omit=0;
		int* line_numbers=nullptr;
		ParseOptions(const int omit, int*		line_numbers):omit(omit),line_numbers(line_numbers){}
		ParseOptions(){}
	};
	
	//Signature shall be changed to std::expected to function
	/*Merges files in a given path. Merging is done sequentially in order of the input files.
	in: path of the folder with files to be merged
	
	fix: fixed columnnames
	out: Container object that implements push_back
	delimiter: delimiting string
	cols: Number of columns in the dataset*/
	template<class T>
	ErrorCode parse_csv(std::string filepath, std::vector<T>& out,std::string delimiter, int* _cols=nullptr){
			std::ifstream file(filepath);
			if (file.fail()){
				std::cerr<<"Couldn't open  "<<filepath<<"\n";
				return ErrorCode::ParseError;
			}
			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			int cols=0;
			while (std::getline(file,line)){
				cols=0;
				//Skip if line starts with a comment #
				if (line.front()=='#'){
					continue;
				}	
				size_t bufsize=0;
				line.push_back('\n'); //add last delimiter so that number parsing is correct
				for (auto& x: line){	
					if (in_digit){
						if (isdigit(x) || x == '.' || x == 'e' || x == '-' || x == '+'){
							numbuf[bufsize]=x;
							bufsize++;
						}
						else{
							in_digit=false;
							numbuf[bufsize]='\0';
							//out.push_back(std::atof(numbuf.begin()));
							out.push_back(std::atof(numbuf));
							bufsize=0;
							cols++;
						}
					}
					else{
						if (isdigit(x) || x == '.' || x == '-' || x == '+'){
							in_digit=true;
							numbuf[bufsize]=x;
							bufsize++;
						}
					}		
				}	
			}
			if (_cols!=nullptr){
				*_cols=cols;
			}
			return ErrorCode::NoError;	
	}

/*	fix: fixed columnnames
	out: Container object that implements push_back
	delimiter: delimiting string
	cols: Number of columns in the dataset*/
	template<class T, class C1, class C2>
	ErrorCode parse_csv_names(std::string filepath, C1 out,C2 names, std::string delimiter, int* _cols=nullptr){
			std::ifstream file(filepath);
			if (file.fail()){
				std::cerr<<"Couldn't open  "<<filepath<<"\n";
				return ErrorCode::ParseError;
			}
			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			int cols=0;
			bool names_parsed = false;
			while (std::getline(file,line)){
				if (names_parsed==false && line.front() != '#') {
					return ErrorCode::ParseError;
				}
				//Parse name if comment starts with a #
				if (line.front()=='#' && names_parsed==false){
					names_parsed = true;
					bool in_name = false;
					std::string curr_name = "";
					int i = 0;
					int counter = 0;
					for (auto& x:line){
						counter++;
						if (in_name) {
							if (x == '\t' || x == ',' || counter == line.size()) {
								in_name = false;
								curr_name += x;
								(*names).push_back(curr_name);
								curr_name = "";
								cols++;
								i++;
							}
							else {
								curr_name += x;
							}
						}
						else if (x == '\n') {
							break;
						}
						else if (x != '#' && x!=' ' && x!='\t') {
							curr_name += x;
							in_name = true;
						}
					}
					continue;
				}	

				size_t bufsize=0;
				line.push_back('\n'); //add last delimiter so that number parsing is correct
				
				for (auto& x: line){	
					if (in_digit){
						if (isdigit(x) || x == '.' || x == 'e' || x == '-' || x == '+'){
							numbuf[bufsize]=x;
							bufsize++;
						}
						else{
							in_digit=false;
							numbuf[bufsize]='\0';
							//out.push_back(std::atof(numbuf.begin()));
							(*out).push_back(std::atof(numbuf));
							bufsize=0;
						}
					}
					else{
						if (isdigit(x) || x == '.' || x == '-' || x == '+'){
							in_digit=true;
							numbuf[bufsize]=x;
							bufsize++;
						}
					}		
				}	
				
			}
			if (_cols!=nullptr){
				*_cols=cols;
			}

			
			return ErrorCode::NoError;	
	}



	/*Merges files in a given path. Merging is done sequentially in order of the input files.
	in: path of the folder with files to be merged
	
	fix: fixed columnnames
	out: Container object that implements push_back
	delimiter: delimiting string
	cols: Set containing columns. Only values inside these columns will be saved
	rows: returns counted numbers of rows that do not start with "#" comments. Note: Empty rows will still be counted!
*/	
	template<class T>
	ErrorCode parse_csv_specific(std::string filepath, std::vector<T>& out,std::string delimiter, std::vector<int> cols){
			std::ifstream file(filepath);
			if (file.fail()){
				std::cerr<<"Couldn't open  "<<filepath<<"\n";
				return ErrorCode::ParseError;
			}

			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			size_t line_number=0;
			
			while (std::getline(file,line)){
				//Skip if line starts with a comment #
				if (line.front()=='#'){
					continue;
				}	
				size_t bufsize=0;
				line.push_back('\n'); //add last delimiter so that number parsing is correct
				size_t current_col=0;
				int i=0;
				for (auto& x: line){
					if (in_digit){
						if (isdigit(x) || x == '.' || x == 'e' || x == '-' || x == '+'){
							numbuf[bufsize]=x;
							bufsize++;
						}
						else{
							in_digit=false;
							if (i<cols.size()){
								if (cols[i]==current_col){
								numbuf[bufsize]='\0';
								//out.push_back(std::atof(numbuf.begin()));
								out.push_back(T(std::atof(numbuf)));
								bufsize=0;
								i++;
							}
								
							}
							
							bufsize=0;
							current_col++;
							
						}
					}
					else{
						if (isdigit(x) || x == '.' || x == '-' || x == '+'){
							in_digit=true;
							numbuf[bufsize]=x;
							bufsize++;
						}
					}		
				}	
				
				line_number++;
			}
			
			return ErrorCode::NoError;	
	}
	
		/*Parses contents specified a table and saves them in joined_tables*/
		template<class T>
		ErrorCode parse_csv_table(std::string table_dir, std::string outfile_name, std::vector<T>& joined_tables){
			std::cout<<"In Parse!\n";
			std::string outfile_path=table_dir+'/'+outfile_name; //use std filesystem later
			std::cout<<"Parse outfilepath: "<<outfile_path<<"\n";
			std::ifstream file(outfile_path);
			if (file.fail()){
				std::cerr<<"Couldn't open parse input table at "<<outfile_path<<"\n";
				return ErrorCode::ParseError;
			}
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string s=buffer.str();
			
			bool inFileName=false;
			bool inSelectedColumns=false;
			std::string filename;
			std::vector<std::vector<double>> files; //content of loaded .csv files
			std::vector<int> cols; //columns per file
			std::vector<int> selected_cols;
			int rows;
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
				        std::vector<double> v;
						std::string filepath= table_dir+'/'+filename; //path to file
						std::cout<<"File path:"<<filepath<<"\n";
						ErrorCode file_error=co::utility::parse_csv_specific(filepath,v," ",selected_cols);
						if (file_error!=ErrorCode::NoError){
							return file_error;
						}
						files.push_back(v);
						cols.push_back(selected_cols.size());
						rows=v.size()/selected_cols.size();
						selected_cols.clear();
						filename.clear();
						
						
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
						joined_tables.push_back(files[j][i*cols[j]+k]);
					}
				}
			}
			//std::cout<<"Size JT:"<<joined_tables.size()<<"\n";
			return ErrorCode::NoError;
		}
		
				/*Parses contents specified a table and saves them in data and times. As opposed to 
		parse_csv_table, this time the first entry of the first datafile will be considered as the time vector.
		Data will accordingly only consist of datapoints (e.g. Methane production) and the time vector only of the times.
		The reason this is not in the parse.h file, is that other differential problems might have multiple variables (
		as usual for PDE's). Therefore, this is implemented on a per-problem basis.
		Datapath is the path of the data, if it differs from the paths in tabledir*/
		template<class T>
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
					char numbuf[3];
					if (isdigit(s[i])){
						numbuf[0]=s[i];
					
						if (isdigit(s[i+1])){
							numbuf[1]=s[i+1];
							numbuf[2]='\0';
							i++;
						}
						else{
							numbuf[1]='\0';
						}
						selected_cols.push_back(static_cast<int>(std::atof(numbuf)));
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
							co::utility::parse_csv_specific(filepath,times," ",time_col);
							co::utility::parse_csv_specific(filepath,v," ",selected_cols);
						}
						else{
							co::utility::parse_csv_specific(filepath,v," ",selected_cols);
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

	template<class T>
	ErrorCode parse_csv_specific_pde(std::string filepath, std::vector<T>& out, std::vector<T>& positions, std::vector<T>& times, std::string delimiter, std::vector<int> cols){
			std::ifstream file(filepath);
			if (file.fail()){
				std::cerr<<"Couldn't open  "<<filepath<<"\n";
				return ErrorCode::ParseError;
			}

			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			size_t line_number=0;
			bool first_row_finished=false;
				
			while (std::getline(file,line)){
				//Skip if line starts with a comment #
				if (line.front()=='#'){
					continue;
				}	
				size_t bufsize=0;
				line.push_back('\n'); //add last delimiter so that number parsing is correct
				size_t current_col=0;
				int i=0;
				for (auto& x: line){
					int iter=0;
					if (in_digit){
						if (isdigit(x) || x == '.' || x == 'e' || x == '-' || x == '+'){
							numbuf[bufsize]=x;
							bufsize++;
						}
						else{
							in_digit=false;
							if (i<cols.size()){
								if (cols[i]==current_col){
								numbuf[bufsize]='\0';
								if (i==0){
									if ((first_row_finished==false)){
									times.push_back(std::atof(numbuf));
									first_row_finished=true;
									}
									else{
										;
									}
								}
								else if(i==1 || i==2){
									positions.push_back(std::atof(numbuf));
								}
								else{
									out.push_back(T(std::atof(numbuf)));									
								}
								bufsize=0;
								iter++;
								i++;
							}
								
							}
							
							bufsize=0;
							current_col++;
							
						}
					}
					else{
						if (isdigit(x) || x == '.' || x == '-' || x == '+'){
							in_digit=true;
							numbuf[bufsize]=x;
							bufsize++;
						}
					}		
				}	
				
				line_number++;
			}
			
			return ErrorCode::NoError;	
	}
	
	template<class T>
	ErrorCode parse_csv_specific_pde(std::string filepath, std::vector<T>& out, std::string delimiter, std::vector<int> cols){
			std::ifstream file(filepath);
			if (file.fail()){
				std::cerr<<"Couldn't open  "<<filepath<<"\n";
				return ErrorCode::ParseError;
			}

			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			size_t line_number=0;
			
			while (std::getline(file,line)){
				//Skip if line starts with a comment #
				if (line.front()=='#'){
					continue;
				}	
				size_t bufsize=0;
				line.push_back('\n'); //add last delimiter so that number parsing is correct
				size_t current_col=0;
				int i=0;
				for (auto& x: line){
					int iter=0;
					if (in_digit){
						if (isdigit(x) || x == '.' || x == 'e' || x == '-' || x == '+'){
							numbuf[bufsize]=x;
							bufsize++;
						}
						else{
							in_digit=false;
							if (i<cols.size()){
								if (cols[i]==current_col){
								numbuf[bufsize]='\0';
								out.push_back(T(std::atof(numbuf)));																
								bufsize=0;
								iter++;
								i++;
							}
								
							}
							
							bufsize=0;
							current_col++;
							
						}
					}
					else{
						if (isdigit(x) || x == '.' || x == '-' || x == '+'){
							in_digit=true;
							numbuf[bufsize]=x;
							bufsize++;
						}
					}		
				}	
				
				line_number++;
			}
			
			return ErrorCode::NoError;	
	}	
	
		template<class T>
		ErrorCode parse_csv_table_times_pde(std::string table_dir, std::string _outfile_name, std::vector<T>& data, std::vector<T>& positions,std::vector<T>& times, std::vector<int>& _selected_columns, std::string data_path="", int* _rows=0){
			//std::cout<<"In Parse!\n";
			std::string outfile_path=table_dir+'/'+_outfile_name; //use std filesystem later

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
			bool already_parsed_selected_columns=false;
			bool got_filename=false;
			for (int i=0;i<s.size();i++){
				if (inFileName){
					if(s[i]!='\"'){
						filename+=s[i];
					}
					else{
						inFileName=false;
						got_filename=true;
					}
				}
				else if (s[i]=='\"'){
					inFileName=true;
				}
				else if (inSelectedColumns || got_filename){
					if (isdigit(s[i]) &&(already_parsed_selected_columns == false)){
						char* numbuf=new char[1000000];						
						int counter=0;
						
						while(isdigit(s[i])){
							numbuf[counter]=s[i];
							i++;
						}	
						i--;
						numbuf[i]='\0';
						selected_cols.push_back(static_cast<int>(std::atof(numbuf)));
						delete[] numbuf;
					
					}
					else if (s[i]==']' || already_parsed_selected_columns){
						inSelectedColumns=false;		
				        std::vector<T> v1;
						std::string filepath= file_dir+'/'+filename; //path to file
						//std::cout<<"File path:"<<filepath<<"\n";
						if(num_files==0){
							already_parsed_selected_columns=true;
							co::utility::parse_csv_specific_pde(filepath,v1,positions,times," ",selected_cols);
							//selected_cols.erase(selected_cols.begin()+1);//erase first three elements
							//selected_cols.erase(selected_cols.begin()+1);

						}
						else{
							std::vector<T> temp;
							co::utility::parse_csv_specific_pde(filepath,v1,temp,times," ",selected_cols);
						}
						files.push_back(v1);
						cols.push_back(selected_cols.size()-3);
						rows=v1.size()/(selected_cols.size()-3);

						//selected_cols.clear();
						filename.clear();
						num_files++;
						got_filename=false;								
					}
				}
				else if (s[i]=='['){
					inSelectedColumns=true;
				}
			
			}
			
			/*Merge all loaded .csv files*/		
			for(int j=0;j<files.size();j++){
				for (int i=0;i<rows;i++){
					for (int k=0;k<cols[j];k++){
						data.push_back(files[j][i*cols[j]+k]);
					}
				}
			}
			if (_rows!=0){
				*_rows=rows; //assign outside rows to rows
			}
			_selected_columns=selected_cols;
			return ErrorCode::NoError;
		}
		



	}
}

#endif
