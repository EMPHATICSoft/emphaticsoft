#include "GeometryConfig.h"
#include <iostream>
#include <stdlib.h>
#include "Utility.h"
#include <algorithm>
#include <fstream>
#include "jsmn.h"
#include "Utility.h"
#include <sstream>
#include "JSONError.h"
#include "Plane.h"
#include <iomanip>

using namespace std;
using namespace utl;

void ReadGeometryConfig(std::string configFile, Reconstruction &reconstruction){
	CheckFile(configFile);
	ifstream in(configFile);
	string contents((istreambuf_iterator<char>(in)), 
		istreambuf_iterator<char>());	


	contents.erase(remove(contents.begin(), contents.end(), '\t'), contents.end());
	contents.erase(remove(contents.begin(), contents.end(), '\n'), contents.end());
	contents.erase(remove(contents.begin(), contents.end(), ' '), contents.end());		
	jsmn_parser p;
	jsmntok_t t[4096];//[512];
	
	
	jsmn_init(&p);
	cout << contents << endl;
	int r = jsmn_parse(&p, contents.c_str(), contents.size(), t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
		cerr << "Failed to parse JSON file. Exiting..." << endl;
		
		exit(EXIT_FAILURE);
	}
	
	if (r < 3) {
		cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
		cerr << "Number of tokens too small. Exiting..." << endl;
		exit(EXIT_FAILURE);
	}
	CheckJSMNType(t[1], JSMN_STRING, __LINE__, __func__, __FILE__);
	CheckJSMNType(t[2], JSMN_OBJECT, __LINE__, __func__, __FILE__);
		
	
	string configName = contents.substr(t[1].start, t[1].end-t[1].start);
	if(configName != "Geometry"){
		cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
		cerr << "First object in the JSON file must be Geometry. Provided name: " << configName << " Exiting..." << endl;
		exit(EXIT_FAILURE);
	}
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	for(int i = 3; i < r; i++){
		CheckJSMNType(t[i], JSMN_STRING, __LINE__, __func__, __FILE__);
		
		string optName = contents.substr(t[i].start, t[i].end-t[i].start);
		
		cout << optName << endl;
		if(i < r-1) i++;
		else continue;
		
		if(optName == "Plane"){
			CheckJSMNType(t[i], JSMN_OBJECT, __LINE__, __func__, __FILE__);
			
			cout << "+++++++++++++++++++++++++++++++++++++++++++++++" << endl;
			Plane plane;	
			vector<double> layers;
			vector<double> radLengths;
			
			int count = 1;
			int nDaug  = 0;
		
			while(nDaug < t[i].size){

				CheckJSMNType(t[i+count], JSMN_STRING, __LINE__, __func__, __FILE__);
			
				string option = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
				count++;

				if(option == "Efficiency"){

					CheckJSMNType(t[i+count], JSMN_PRIMITIVE, __LINE__, __func__, __FILE__);
					double val = stod(contents.substr(t[i+count].start, t[i+count].end-t[i+count].start));
					plane.SetEfficiency(val);
					cout << "Efficiency: " << val << endl; 				
				}
				else if(option == "ChargeSharing"){

					CheckJSMNType(t[i+count], JSMN_PRIMITIVE, __LINE__, __func__, __FILE__);
					double val = stod(contents.substr(t[i+count].start, t[i+count].end-t[i+count].start));
					plane.SetChargeSharing(val);
					cout << "Charge sharing: " << val << endl; 				
				}
				else if(option == "Pitch"){

					CheckJSMNType(t[i+count], JSMN_PRIMITIVE, __LINE__, __func__, __FILE__);
					double val = stod(contents.substr(t[i+count].start, t[i+count].end-t[i+count].start));
					plane.SetPitch(val);
					cout << "Pitch: " << val << endl; 				
				}
				else if(option == "ADCChannels"){

					CheckJSMNType(t[i+count], JSMN_PRIMITIVE, __LINE__, __func__, __FILE__);
					int val = stoi(contents.substr(t[i+count].start, t[i+count].end-t[i+count].start));
					plane.SetNumberOfADCChannels(val);
					cout << "Number of ADC channels: " << val << endl; 				
				}
				else if(option == "ActiveLayer"){

					CheckJSMNType(t[i+count], JSMN_PRIMITIVE, __LINE__, __func__, __FILE__);
					int val = stoi(contents.substr(t[i+count].start, t[i+count].end-t[i+count].start));
					plane.SetActiveLayer(val);
					cout << "Is y plane: " << val << endl; 				
				}
				else if(option == "SizeXY"){
			
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;
					if(valSize != 2){
						cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
						cerr << "The number of coordinates is not 3! Check your geometry JSON file." << endl;
						exit(EXIT_FAILURE);
					}
					vector<double> values;
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						values.push_back(stod(val));	
					}
					
					plane.SetSize(values.at(0), values.at(1));
					cout << fixed << setprecision(5);
					cout << "(x, y) = (" << values.at(0) << ", " << values.at(1) << ") cm" << endl;
				}				
				else if(option == "Layers"){
			
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;

					cout << fixed << setprecision(5);
					cout << "layers thickness = (";
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						layers.push_back(stod(val));	
						if(k < valSize - 1) cout << val << ", ";
						else cout << val << ") cm" << endl;
					}
					
				}
				else if(option == "RadiationLengths"){
			
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;

					cout << fixed << setprecision(5);
					cout << "Radiation lengths = (";
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						radLengths.push_back(stod(val));	
						if(k < valSize - 1) cout << val << ", ";
						else cout << val << ") gcm-2" << endl;
					}
					
				}
				else if(option == "Position"){
			
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;
					if(valSize != 3){
						cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
						cerr << "The number of coordinates is not 3! Check your geometry JSON file." << endl;
						exit(EXIT_FAILURE);
					}
					vector<double> values;
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						values.push_back(stod(val));	
					}
					
					plane.SetPosition(values.at(0), values.at(1), values.at(2));
					cout << fixed << setprecision(5);
					cout << "(x, y, z) = (" << values.at(0) << ", " << values.at(1) << ", " << values.at(2) << ") cm" << endl;
				}	
				else if(option == "AlignFit"){
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;
					if(valSize != 3){
						cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
						cerr << "The number of alignment options is not 3 (use_in_fit,fit_rotation,fit_z)! Check your geometry JSON file." << endl;
						exit(EXIT_FAILURE);
					}
					vector<bool> values;
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						values.push_back(stoi(val));	
					}
					
					plane.SetAlignFit(values.at(0), values.at(1), values.at(2));
					cout << "(use_in_fit, fit_rot, fit_z) = (" 
					     << std::boolalpha << values.at(0) << ", " 
					     << std::boolalpha << values.at(1) << ", " 
					     << std::boolalpha << values.at(2) << ") " << endl;
				}	
				        
				        
				  
				else if(option == "Rotation"){
			
					CheckJSMNType(t[i+count], JSMN_ARRAY, __LINE__, __func__, __FILE__);
					int valSize = t[i+count].size;
					if(valSize != 3){
						cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
						cerr << "The number of angles is not 3! Check your geometry JSON file." << endl;
						exit(EXIT_FAILURE);
					}
					vector<double> values;
					for(int k = 0; k < valSize; k++){
						count++;
						string val = contents.substr(t[i+count].start, t[i+count].end-t[i+count].start);
						values.push_back(stod(val));	
					}
					
	
					plane.SetRotation(values.at(0), values.at(1), values.at(2));

					
					cout << fixed;
					cout << setprecision(5);
					cout << "(alpha, beta, gamma) = (" << values.at(0) << ", " << values.at(1) << ", " << values.at(2) << ") deg" << endl;

			
				}	
				else if(option == "comment"){
					//if(i < r-1) i++;
					//else break;
				}
				else{
					cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
					cerr << "Unknown option: " <<  contents.substr(t[i+count].start, t[i+count].end-t[i+count].start) << ". Exiting..." << endl;
					exit(EXIT_FAILURE);					
				}
			
				nDaug++;
				if(nDaug < t[i].size) count++;				
			}
			plane.AddLayers(layers, radLengths);
			reconstruction.AddPlane(plane);
			i += count;		
		}
		else if(optName == "comment"){
			//skip
		}
		else{
			cerr << "Error in " << __func__ << ", line " << __LINE__ << " in " << __FILE__ << endl;
			cerr << "Unknown option: " <<  contents.substr(t[i].start, t[i].end-t[i].start) << ". Exiting..." << endl;
			exit(EXIT_FAILURE);					
		}		
	}
		
}
