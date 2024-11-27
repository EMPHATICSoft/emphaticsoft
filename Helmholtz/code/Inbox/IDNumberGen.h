//  IDNumberGen.h
//
//  Created by Leo Bellantoni on 2 Aug 2019.
//  With lots of help from Paul Russo :)
//
//  For implementing operator==() in Track, Vertex, Cluster classes -
//  maybe in other ReconstructionDataProducts classes later
//
//  Implemented as a thread-safe singleton using C++11 features esp std::atomic
//
//  Put
//      #include "IDNumberGen.h"
//  in the header for your ReconstructionDataProducts class.  Then put
//      IDNumberGen::IDNumber fIDnumero;
//  in the private part of the definition of your class; then put
//      IDNumberGen::create(firstNumber);
//      fIDnumero = IDNumberGen::create()->getNewOne();
//  in the constructor.  Be sure to do this also in the default constructor, 
//  which can be invoked by art.  IDNumber is just a typedef to size_t.
//  The firstNumber is just that; maybe you want to count tracks numbering
//  from 100000, clusters from 200000 and vertices from 300000.  Maybe you want
//  to define the firstNumber as a static IDNumberGen::IDNumber const in the 
//  private section of the class definition.  Probably you want firstNumber
//  for lower level objects like a TPC cluster or a CaloHit to start with a higher
//  value such as 100100000 or 100200000.  If there is already an instance of a
//  ReconstructionDataProducts class with the same fIDnumero as firstNumber,
//  then you will have two objects out there with the same ID.  They are probably
//  of different classes, so the rest of your analysis code might work.  Maybe.
//
//  Then implement an operator==(const ...& rhs), an operator!=(const ...& rhs)
//  and a IDNumber <T>::getIDNumber() as public in the reconstruction data
//  products class <T>.
//
//  Finally, we need to reset the counter at the start of every event.
//  That requires an art::EDProducer module (e.g. EventInit_module.cc which is
//  sequenced to be at the start of the trigger_path in the fcl file and which, 
//  in its produce method, has the line
//     IDNumberGen::create()->newEventReset();



#pragma once



#include <cstdlib>
#include <atomic>
#include <limits>
#include <set>
#include <iostream>



typedef size_t IDNumber;



class IDNumberGen {
public:

    static IDNumberGen* create(IDNumber iniValue = std::numeric_limits<IDNumber>::max());
    IDNumber getNewOne();
    void newEventReset();



    // Copy constructors, assignment constructors don't happen in singletons!
    IDNumberGen(const IDNumberGen& arg) = delete;   // Copy constructor
    IDNumberGen(const IDNumberGen&& arg) = delete;  // Move constructor
    IDNumberGen& operator=(const IDNumberGen& arg) = delete;  // Assignment operator
    IDNumberGen& operator=(const IDNumberGen&& arg) = delete; // Move operator



private:
    IDNumberGen();
    ~IDNumberGen();

    static std::atomic<IDNumber> nextOneToMake;
    static std::set<IDNumber> previousInitializers;
};
