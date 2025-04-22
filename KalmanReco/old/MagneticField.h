#ifndef MagneticField_H
#define MagneticField_H

#include <map>
#include <vector>
#include <string>

class G4FieldManager;

class MagneticField{
public:
  MagneticField(std::string &name);
  ~MagneticField();  
      
  void GetField(double x[3], double &Bx, double &By, double &Bz) ;

private:
	std::map<int, std::map<int, std::map<int, std::vector<double> > > > field;
	double step;
	double start[3];

};

#endif
