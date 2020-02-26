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
	ParseType: Type to be parsed*/
	template<class T>
	bool parse_csv(std::string filepath, std::vector<T>& out,std::string delimiter){
			std::ifstream file(filepath);
			std::string line;
			//std::array<char,64> numbuf;
			char numbuf[64];
			bool in_digit=0;
			
			while (std::getline(file,line)){
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
			return true;	
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
	bool parse_csv_specific(std::string filepath, std::vector<T>& out,std::string delimiter, std::vector<int> cols){
			std::ifstream file(filepath);
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
			

			return true;	
	}
	
		/*Parses contents specified a table and saves them in joined_tables*/
		template<class T>
		bool parse_csv_table(std::string table_dir, std::string outfile_name, std::vector<T>& joined_tables){
			std::cout<<"In Parse!\n";
			std::string outfile_path=table_dir+'/'+outfile_name; //use std filesystem later
			std::cout<<"Parse outfilepath: "<<outfile_path<<"\n";
			std::ifstream file(outfile_path);
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
						co::parse_csv_specific(filepath,v," ",selected_cols);
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
			std::cout<<"Size JT:"<<joined_tables.size()<<"\n";
			return true;
		}
	
}

#endif
