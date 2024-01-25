//
//  ToDoAndDoneLists.h
//  L++
//
//  Created by Leo Bellantoni on 5 Jan 2024.
//
#pragma once



#include <fstream>
#include <string>
#include <deque>
using namespace std;

#include "../C--/Ldelete.h"
#include "../general/LppGlobals.h"
#include "../problems/LppExcept.h"
#include "../io/filehamna.h"
#include "../io/inputtextfile.h"
#include "../functions/intsNabs.h"
#include "../functions/quadd.h"





struct FieldMapPoint {double x; double y; double z;             // Position in the map, usual coordinate system
                      double BHx; double BHy; double BHz;       // Magnetic field from Helmholtz decomposition
                      double BCx; double BCy; double BCz;};     // Ad hoc correction added to BH.



class ToDoAndDoneLists {
public:
    // User interface methods
    bool createFreshList(double R, double Zlo, double Zhi,
                         double stepX, double stepY, double stepZ,
                         string freshListName = "FreshList.csv");
    
    bool fetchNewPoint(FieldMapPoint& retval);

    bool outputToDoList(string outputFileName);             // Put a copy of *toDoList onto disk

    bool addToDoneList(FieldMapPoint doinkDone);



    // Constructors etc.
    ToDoAndDoneLists() :
        toDoListName(""), doneListName("") {};
    ToDoAndDoneLists(string toDoList_in, string doneList_in);
    ~ToDoAndDoneLists();



private:
    string toDoListName;
    string doneListName;
    std::deque<FieldMapPoint> toDoList;                     // The list of remaining points

    FieldMapPoint zeroPoint = {0,0,0, 0,0,0, 0,0,0};        // To initialize FieldMapPoints

    char outLine[200];                                      // ANSI C string to format output into
    
    // We are not doing the complete singleton pattern here, but there should be only
    // one instance of this class, so the copy and move constructors and assignments
    // are made private.
    ToDoAndDoneLists(ToDoAndDoneLists const&);
    ToDoAndDoneLists& operator=(ToDoAndDoneLists const&);
    ToDoAndDoneLists(ToDoAndDoneLists const&&);
    ToDoAndDoneLists& operator=(ToDoAndDoneLists const&&);

    
};
