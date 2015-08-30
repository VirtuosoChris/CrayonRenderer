//  Copyright (c) 2013 Christopher Pugh. All rights reserved.

#ifndef DEFAULT_MATERIALS_H_INCLUDED
#define DEFAULT_MATERIALS_H_INCLUDED

#include <string>
#include "CrayonRenderer.h"

extern std::string materialPaths[];

extern std::string userNames[]; //string names of the materials that the user sees

extern std::vector< std::vector<Crayon> > crayonsForMaterial;

void loadCrayons();

extern float materialScales[];


#endif
