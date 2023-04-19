//
//   If the ROOT interactive environment has NOT already been established,
//   then the ROOT global variable gApplication is null.  Once the
//   environment has been established, gApplication is non-null.
//
//   To create the environment, create an instance of TApplication.
//   Doing this will set gApplication to a non-null value.
//   It is ROOT's responsibility to delete the instance of TApplication
//   at the end of the job. This code must not delete it.
//

#include "EventDisplay/EnsureTApplication.h"

#include "TApplication.h"

emph::EnsureTApplication::EnsureTApplication(int argc, char** argv)
{

  if (!gApplication) {
    new TApplication("noapplication", &argc, argv);
  }
}
