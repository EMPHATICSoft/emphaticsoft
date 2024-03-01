/*  L++ ThreadHandler
 *  Created by Leo Bellantoni on 8 Jan 2024.
 *  Copyright 2024 FRA. All rights reserved.
 *
 *  A simple and really kinda slow "thread" handler.  A real thread pool will
 *  construct a bunch of threads and set each of them to run on a worker
 *  function that halts until it recieves a signal, runs some specified
 *  function, sends out a signal and then halts itself.  In this way, the
 *  overhead of creating threads is eliminated.  This template class is
 *  not that smart.  Also it isn't really threads in the sense of a
 *  pthread or a TBB thread - it's C++ std::async futures here.  Which
 *  has some minor improvement in portability, in principle.
 *
 *  The template parameter R is the type of the return result from the function
 *  being sent off to one of the thread.  The parameter P is a structure which
 *  contains all the arguments to be passed to the function.
 *
 */
#pragma once



#include <future>
#include <deque>

#include "../ra/ra.h"
#include "../problems/LppExcept.h"




template <typename R, typename P> class ThreadHandler {

    // Structure defining chores to do; needed for the ThreadPuddle definition
    public: struct ChoreInfo {R (*f)(P); P argue;};

    #include "ThreadPuddle.h"
    ThreadPuddle threadPool;



    int addChore(ChoreInfo bother) {
        hunnyDo.push_back(bother);
        return startChores();
    }



    pair<int,int> threadCount() {
        pair<int,int> retval;
        retval.first  = threadPool.size();
        retval.second = hunnyDo.size();
        return retval;
    }



    // To poll the existing threads and return the results of the 1st one which is
    // done; and then to give it a new chore.  The 2nd return value will be invalid
    // if the 1st return value is true, which happens when there are no more chores
    // at all to do.
    pair<bool,R> getResult() {
        R retval;
        while (threadPool.size()>0) {   //N.B. size() updates status of all threads
            int iThread = 0;    bool foundOne = false;
            for (; iThread<threadPool.maxThreads; ++iThread) {
                if (threadPool.getStatus(iThread)==ThreadPuddle::done) {
                    retval = threadPool.pullThread(iThread);
                    foundOne = true;
                    break;
                }
            }
            if (foundOne) break;    // double break, yah
        }

        // Having gotten a result and freed up a thread, maybe start another chore
        int numberStarted = startChores();
        if (threadPool.size()==0 && hunnyDo.size()==0) {
            return pair(false,retval);
        } else {
            return pair(true,retval);
        }
        
    }



private:
    // Here's the chores to be put into threads when you have one available.
    // Use std::deque so you can have FIFO execution should you want to keep
    // track of how the job is progressing.
    std::deque<ChoreInfo>  hunnyDo;



    // Internally used routine to move chores from to-do to doing status.  Return
    // value is number of chores successfully moved
    int startChores() {
        int retval = 0;
        while (threadPool.size()<threadPool.maxThreads && hunnyDo.size()!=0) {
            // Put new chores into play
            ChoreInfo requisition = hunnyDo.front();
            if (threadPool.startThread(requisition)) {
                hunnyDo.pop_front();
                ++retval;
            }
        }
        return retval;
    };



    // Constructors, destructors.  There should only be one instance of this class.  We
    // aren't doing the whole singleton encapsulated data thing though.
    public: ThreadHandler(int nThreadsRequested=1) {
        threadPool.maxThreads = nThreadsRequested;
        threadPool.allocate(nThreadsRequested);
    };
    public: ~ThreadHandler() {};    // ~ThreadPuddle called automatically

    private: ThreadHandler(ThreadHandler const&);
    private: ThreadHandler& operator=(ThreadHandler const&);
    private: ThreadHandler(ThreadHandler const&&);
    private: ThreadHandler& operator=(ThreadHandler const&&);

};
