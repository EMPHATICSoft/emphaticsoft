/*  L++ xmlSteppers
*  Created by Leo Bellantoni on 03/26/22.
*  Copyright 2022 FRA. All rights reserved.
*
*  A 4-vector class, implemented as simply as possible.
*
*/
#include "xmlSteppers.h"



xmlNodePtr kidNoText(xmlNodePtr inNode) {
    xmlNodePtr retNode = inNode->children;
    // I do not know if nullptr would work instead of NULL here.
    while ( retNode!=NULL && !xmlStrcmp(retNode->name, (const xmlChar*) "text") ) {
        retNode = retNode->next;
    }
    return retNode;
}

xmlNodePtr nextNoText(xmlNodePtr inNode) {
    xmlNodePtr retNode = inNode->next;
    while ( retNode!=NULL && !xmlStrcmp(retNode->name, (const xmlChar*) "text") ) {
        retNode = retNode->next;
    }
    return retNode;
}

