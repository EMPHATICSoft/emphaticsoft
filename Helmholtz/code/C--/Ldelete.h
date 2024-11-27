/*  L++ Correctly delete or free memory
 *  Created by Leo Bellantoni on 27 Jun 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  In C/C++, neither delete nor free reset the pointer to the freed
 *  memory to NULL/nullptr.  The code patterns in L++ maintain that
 *  that a pointer will either be null or valid.  Hence this.
 *
 *  Warning! delete (and maybe free?) call the destructor of the
 *  class; so if you call Ldelete from the destructor, you recurse
 *  infinitely.  So don't do that!
 *
 *  The arguments are pointers to T, but called by reference so the
 *  assignment here to nullptr takes effect in the calling routine.
 *
 */
#pragma once



#include <cstdlib>



template <typename T> void Ldelete(T*& pointer) {
    delete pointer;
    pointer = nullptr;
    return;
}

template <typename T> void Lfree(T*& pointer) {
    free(pointer);
    pointer = nullptr;
    return;
}
