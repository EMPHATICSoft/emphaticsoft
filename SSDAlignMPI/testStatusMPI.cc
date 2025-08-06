#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <complex.h>
#include <mpi.h>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include <float.h>
#include <sys/time.h>

int main(int argc, char **argv) {

      MPI_Init(NULL, NULL);

    // Get the number of processes
    int np;
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    std::cerr << " testStatusMPI, world size " << np << std::endl;
    
        // Get the rank of the process
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    
    double aValue=std::sqrt(45. + 2.0*myRank); //some calculation.. 
    
    const bool checkStatus = true; // if true, crash on perfectly travial transfer.  If fals, I got what I want.
    
    if (myRank != 0) {
      std::cerr << " From rank " << myRank << " value " << aValue << std::endl;
      int tagTally = 1000*myRank + 1; // arbitrary.. 
      MPI_Send((void*) &aValue, 1, MPI_DOUBLE, 0, tagTally, MPI_COMM_WORLD );
    } else {
      for (int kp=1; kp != np; kp++ ) { 
        MPI_Status statusRec;
        double anOtherValue = 0.;
        MPI_Recv((void*) &anOtherValue, 1, MPI_DOUBLE, kp, MPI_ANY_TAG, MPI_COMM_WORLD, &statusRec );
        if (checkStatus && (statusRec.MPI_ERROR != MPI_SUCCESS)) {
	      std::cerr << "  .... collect data,  failed received buffer, !!!!!! " << std::endl; 
	      char errNow[1024]; int il=1024; MPI_Error_string(statusRec.MPI_ERROR, errNow, &il);
	      std::string errStr(errNow);
	      std::cerr << "  ...., collect data,  failed received buffer, error " << errStr << " tag is " 
		<< statusRec.MPI_TAG << " from rank " << kp  << " ---" << std::endl;
	   }
        std::cerr << " ..... Rank 0, from kp " << kp << " the value is  " << anOtherValue << std::endl;
      }
    }
    // end game... 
    if (myRank == 0) std::cerr << " And call it quit " << std::endl;
    MPI_Finalize(); 
}
