/*  L++ xmlSteppers
 *  Created by Leo Bellantoni on 8 Apr 2019.
 *  Copyright 2019 URA. All rights reserved.
 *
 *  For some reason, certain xml files - in particular, the ones produced by
 *  GENIE's gmkspl - have a lot of "text" nodes.  These little routines let you
 *  step to the next, or to the child, nodes while skipping over these.
 *
 */
#pragma once



#include <libxml/parser.h>



xmlNodePtr kidNoText(xmlNodePtr inNode);

xmlNodePtr nextNoText(xmlNodePtr inNode);
