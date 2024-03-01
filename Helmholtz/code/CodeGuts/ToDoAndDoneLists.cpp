//
//  ToDoAndDoneLists.cpp
//  L++
//
//  Created by Leo Bellantoni on 5 Jan 2024.
//
#include "ToDoAndDoneLists.h"





ToDoAndDoneLists::ToDoAndDoneLists(string toDoList_in, string doneList_in)
    : toDoListName(toDoList_in), doneListName(doneList_in) {
    // Bring the to-do list into this object; write it out later, when
    // when it changes.  File is csv format, a kind of text file
    FieldMapPoint temp = zeroPoint;

    inputtextfile getToDo(toDoListName);
    string makeWhite = ",";
    try {
        while (getToDo.fetchline(makeWhite) != -1) {
            temp.x = getToDo.tokenasd(0);
            temp.y = getToDo.tokenasd(1);
            temp.z = getToDo.tokenasd(2);
            toDoList.push_back(temp);
        }
    } catch (LppExcept& anLppException) {
        cout<< anLppException.what() <<endl;
        cout<< "Maybe there is a blank line at the start or end of the file." <<endl;
        exit(1);
    }
    // Push getToDo out of scope right now!
    return;
}
ToDoAndDoneLists::~ToDoAndDoneLists() {};





bool ToDoAndDoneLists::createFreshList(double R, double Zlo, double Zhi,
                     double stepX, double stepY, double stepZ,
                     std::string freshListName) {

    if (toDoList.size()!=0) {
        LppUrk.UsrIssue(-1, Fatal, "ToDoAndDoneLists::createFreshList",
            "This instance of ToDoAndDoneLists already has a toDoList in it.");
        return false;
    }

    FieldMapPoint temp = zeroPoint;

    double z = sign(Zlo) * stepZ * fint(abs(Zlo)/stepZ);
    while (z<Zhi) {
        double y = -stepY * (fint(R/stepY) +1);
        double yMax = -y;
        while (y-yMax<stepY) {
            double x = -stepX * (fint(R/stepX) +1);
            double xMax = -x;
            while (x-xMax<stepX) {

                if (Qadd(x, y)<R) {
                    temp.x = x;     temp.y = y;     temp.z = z;
                    toDoList.push_back(temp);
                }

                x += stepX;
            }
            y += stepY;
        }
    z += stepZ;
    }

    return outputToDoList(freshListName);
}





bool ToDoAndDoneLists::fetchNewPoint(FieldMapPoint& retval) {
    if (toDoList.size()==0) return false;
    retval = toDoList.front();
    toDoList.pop_front();
    return true;
};





bool ToDoAndDoneLists::addToDoneList(FieldMapPoint doinkDone) {
    fstream* fileOUT = new fstream(doneListName.c_str(),ios_base::app);
    if (fileOUT->fail()) {
        cout << doneListName << endl;
        LppUrk.UsrIssue(-2, Fatal, "ToDoAndDoneLists::addToDoneList","File could not opened for appendage.");
        fileOUT->close();       // Not sure what state fileOUT is in at this point
        Ldelete(fileOUT);
        return false;
    }

    sprintf(outLine,"%7.4f,   %7.4f,   %7.4f,     %9.6f,   %9.6f,   %9.6f,     %9.6f,   %9.6f,   %9.6f",
        doinkDone.x,   doinkDone.y,   doinkDone.z,
        doinkDone.BHx, doinkDone.BHy, doinkDone.BHz,
        doinkDone.BCx, doinkDone.BCy, doinkDone.BCz);
    *fileOUT << outLine << endl;
    fileOUT->close();
    return true;
}





bool ToDoAndDoneLists::outputToDoList(string outputFileName) {
    fstream* fileOUT = new fstream(outputFileName.c_str(),ios_base::out);
    if (fileOUT->fail()) {
        cout << outputFileName << endl;
        LppUrk.UsrIssue(-3, Fatal, "ToDoAndDoneLists::outputToDoList","File could not be created.");
        fileOUT->close();       // Not sure what state fileOUT is in at this point
        Ldelete(fileOUT);
        return false;
    }

    for (FieldMapPoint i : toDoList) {
        sprintf(outLine,"%7.4f,   %7.4f,   %7.4f", i.x, i.y, i.z);
        *fileOUT << outLine << endl;
    }

    fileOUT->close();
    return true;
}
