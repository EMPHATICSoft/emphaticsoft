
#include <fstream>
#include <iostream>
#include "MagneticField.h"


#include "TFile.h"
#include "TTree.h"
#include "math.h"
//#define ddebug
//#define debug
using namespace std;
//Constructor and destructor:

MagneticField::MagneticField(string &filename) :
  step(0), start{-50, -50, -50}
{


	TFile mfFile(filename.c_str(), "READ");
  	cout << " ==> Opening file " << filename << " to read magnetic field..."
	 << endl;

    if (mfFile.IsZombie()) {
       cerr << "Error opening file" << endl;
       exit(-1);
    }	
    TTree *tree = (TTree*) mfFile.Get("magField");
    
    double x;
    double y;
    double z;
    double Bx;
    double By;
    double Bz;
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
    tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("Bx", &Bx);
    tree->SetBranchAddress("By", &By);
    tree->SetBranchAddress("Bz", &Bz);
    
    int nEntries = tree->GetEntries();
    
    tree->GetEntry(0);
    double xVal = x;
    double yVal = y;
    double zVal = z;
    
    //step = 0;
    tree->GetEntry(1);
    if(abs(xVal - x) > step) step = abs(xVal - x);
    else if(abs(yVal - y) > step) step = abs(yVal - y);
    else step = abs(zVal - z);
    
    start[0] = xVal;
    start[1] = yVal;
    start[2] = zVal;
    //start = {-60, -60, -60};
    
    for(int i = 0; i < nEntries; i++){
    	tree->GetEntry(i);
    	

    	int indX = (x-start[0])/step;
    	int indY = (y-start[1])/step;
    	int indZ = (z-start[2])/step;

    	std::vector<double> temp;
    	temp.push_back(Bx);
    	temp.push_back(By);
    	temp.push_back(Bz);
    	
    	//if(By > 10) cout << x << " " << y << " " <<z << " " <<By << endl;
    	
    	field[indX][indY][indZ] = temp;
    	/*if(By > 10) {cout << x << " " << y << " " <<z << " " <<By << endl;
    			cout << indX << " " << indY << " " << indZ << " " << field[indX][indY][indZ].at(0) << endl;}*/
    	if(indX == 50 && indY == 50 && indZ == 65) cout << By << endl;
    }

    ///////////////////////////////////////////////////////////////
    // Close the file
    cout << " ==> Closing file " << filename << endl;
    mfFile.Close();

}


MagneticField::~MagneticField() {
  
}


// Member functions

void MagneticField::GetField(double x[3], double &Bx, double &By, double &Bz) 
{

	
	double indX = (x[0]-start[0])/step;
	double indY = (x[1]-start[1])/step;
	double indZ = (x[2]-start[2])/step;
  
  	int ix[2] = {int(floor(indX)), int(ceil(indX))};
  	int iy[2] = {int(floor(indY)), int(ceil(indY))};
  	int iz[2] = {int(floor(indZ)), int(ceil(indZ))};

	bool skip = false;
	
	if(field.find(ix[0]) == field.end()) skip = true;
	else if(field.find(ix[1]) == field.end()) skip = true;
	else{
		if(field.at(ix[0]).find(iy[0]) == field.at(ix[0]).end()) skip = true;
		else if(field.at(ix[0]).find(iy[1]) == field.at(ix[0]).end()) skip = true;
		else if(field.at(ix[1]).find(iy[0]) == field.at(ix[1]).end()) skip = true;
		else if(field.at(ix[1]).find(iy[1]) == field.at(ix[1]).end()) skip = true;
		else{
			if(field.at(ix[0]).at(iy[0]).find(iz[0]) ==field.at(ix[0]).at(iy[0]).end()) skip = true;
			else if(field.at(ix[0]).at(iy[0]).find(iz[1]) ==field.at(ix[0]).at(iy[0]).end()) skip = true;
			else if(field.at(ix[0]).at(iy[1]).find(iz[0]) ==field.at(ix[0]).at(iy[1]).end()) skip = true;
			else if(field.at(ix[0]).at(iy[1]).find(iz[1]) ==field.at(ix[0]).at(iy[1]).end()) skip = true;
			else if(field.at(ix[1]).at(iy[0]).find(iz[0]) ==field.at(ix[1]).at(iy[0]).end()) skip = true;
			else if(field.at(ix[1]).at(iy[0]).find(iz[1]) ==field.at(ix[1]).at(iy[0]).end()) skip = true;
			else if(field.at(ix[1]).at(iy[1]).find(iz[0]) ==field.at(ix[1]).at(iy[1]).end()) skip = true;
			else if(field.at(ix[1]).at(iy[1]).find(iz[1]) ==field.at(ix[1]).at(iy[1]).end()) skip = true;
		}
	}
	

	if(skip){

		Bx = 0;
		By = 0;
		Bz = 0;
		return;
	}

	
	double sumx = 0;
	double sumy = 0;
	double sumz = 0;
	double norm = 0;
	
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 2; j++){
			for(int k = 0; k < 2; k++){
				double dist = sqrt((indX-ix[i])*(indX-ix[i]) + (indY-iy[j])*(indY-iy[j]) + (indZ-iz[k])*(indZ-iz[k]));
				sumx += field.at(ix[i]).at(iy[j]).at(iz[k]).at(0)*dist;
				sumy += field.at(ix[i]).at(iy[j]).at(iz[k]).at(1)*dist;
				sumz += field.at(ix[i]).at(iy[j]).at(iz[k]).at(2)*dist;
				norm += dist;
				
				/*sumx = field.at(ix[i]).at(iy[j]).at(iz[k]).at(0)*1;
				sumy = field.at(ix[i]).at(iy[j]).at(iz[k]).at(1)*1;
				sumz = field.at(ix[i]).at(iy[j]).at(iz[k]).at(2)*1;
				norm = 1;*/

			}		
		}	
	}
	//if(ix[50] == 0 && ix[50] == 0 && iz[0] == 65) cout << By << endl;
  	 //cout << field.at(50).at(50).at(80).at(1) << endl;
	Bx = (sumx/norm);
	By = (sumy/norm);
	Bz = (sumz/norm);
	/*cout << start[0] << " " << start[1] << start[2] << endl;
	cout << x[0] << " " << x[1] << " " << x[2] << endl;
	cout << ix[0] << " " << ix[0] << " " << iz[0] << " " << By << endl;
	cout << Bx << " " << By << " " << Bz << " " << endl;
	cout << "*************************************" << endl;*/
}

