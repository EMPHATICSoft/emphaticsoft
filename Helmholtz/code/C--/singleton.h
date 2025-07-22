/*  L++ singleton
 *  Created by Leo Bellantoni on 15 Aug 2018.
 *  Copyright 2018 FRA. All rights reserved.
 *
 *  Example of canonical singleton coding pattern.  This one encapsulates a
 *  (void*) data, so the data would be recast upon retrieval.  This can't be
 *  used as a base class, but you could make a singleton contain an instance.
 *  of a class.  You can NOT make two singletons in the same program though!
 *
 *  Repeated occurances of the definition and initialization of handle will all
 *  return the same instance of the encapsulated data.  This is why the singleton
 *  is a near-global in main rather than a global; if you are in some method
 *  somewhere and you do not define & initialize handle, you do not see the singleton.
 *
 *  Not thread-safe!
 *
 */
#pragma once





class singleton {
public:
    // Here is the Instance method declaration
    static singleton* Instance();



    // Copy, assignment constructors, destructor don't happen in singletons!
    singleton(const singleton& arg) = delete; // Copy constructor
    singleton(const singleton&& arg) = delete;  // Move constructor
    singleton& operator=(const singleton& arg) = delete; // Assignment operator
    singleton& operator=(const singleton&& arg) = delete; // Move operator
    ~singleton() = delete;



    // Encapsulate some data
    inline void  setSingletonData(void* val) {data = val;}
    inline void* getSingletonData() const {return data; }

private:
    singleton();                   // Constructor is private!
    static singleton* fInstance;

    // Here's the pointer to the data that the singleton holds
    void* data;
};
