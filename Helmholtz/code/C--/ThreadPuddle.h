/*  L++ ThreadPuddle.h
 *  Created by Leo Bellantoni on 10 Jan 2024.
 *  Copyright 2024 FRA. All rights reserved.
 *
 *  Not a real thread pool.  Used by ThreadHandler.h, which needs an old
 *  C-style vector of threads & their associated info; I just couldn't get
 *  C++ style containers to work with that template class.
 *
 */
#pragma once



class ThreadPuddle {
public:
    enum ThreadStatus {working, done, available};

    // This is the largest number of threads that this handler will handle.
    // you probably want to save at least one for your main thread!
    int maxThreads;



    void allocate(int nThreadsRequested) {
        cVectorThreads = (ThreadInfo*) malloc(nThreadsRequested * sizeof(ThreadInfo));
        for (int iThr=0; iThr<nThreadsRequested; ++iThr) {
            cVectorThreads[iThr].status = available;
            // Dubious initialization.  Seems to help in startThread though.
            memset( &cVectorThreads[iThr].yarn, 0, sizeof(future<R>) );
        }
    }



    int size() {    // Number of active threads, not max number of threads
        int retval = 0;     // Updates status of all threads via getStatus
        for (int iThread=0; iThread<maxThreads; ++iThread) {
            if (getStatus(iThread)!=available) ++retval;
        }
        return retval;
    };



    bool startThread(ChoreInfo newChore) {
        for (int iThread=0; iThread<maxThreads; ++iThread) {
            if (getStatus(iThread)==available) {
                // This bombed once when yarn was not null.  No idea why.
                cVectorThreads[iThread].yarn =
                        std::async(launch::async, newChore.f,newChore.argue);
                cVectorThreads[iThread].status = working;
                return true;
            }
        }
        return false;
    };



    ThreadStatus getStatus(int iThread) {
        // Actually updates the status of this thread along with getting it
        // No bounds check on iThread; all calls have 0<= iThread <maxThread
        if (cVectorThreads[iThread].status == available) return available;
        if (cVectorThreads[iThread].status == done)      return done;

        // But if the thread has working status, it might have finished by now
        std::future_status rightNow =
            (cVectorThreads[iThread].yarn).wait_for(std::chrono::milliseconds(1));
        if (rightNow != std::future_status::ready) {
            cVectorThreads[iThread].status = working;
            return working;
        } else {
            cVectorThreads[iThread].status = done;
            return done;
        }
    }



    R pullThread(int iThread) {
        R retval;
        retval = (cVectorThreads[iThread].yarn).get();
        cVectorThreads[iThread].status = available;
        // How to call the destructor of cVectorThreads[iThread].yarn?  Can one?
        // However, the memory leak here, if there is one, is small.
        //Ldelete(cVectorThreads[iThread].yarn);
        return retval;
    };





    ThreadPuddle() : cVectorThreads(nullptr) {};
    ~ThreadPuddle() {
        Lfree(cVectorThreads);
    };


private:
    // You might ask what R is.  R is the template parameter to
    // ThreadHandler; and this file is #include -ed into ThreadHandler
    // in such a way so as to have R here still in scope.  Tricky, eh?
    struct ThreadInfo {future<R> yarn; ThreadStatus status;};
    ThreadInfo* cVectorThreads;
};



