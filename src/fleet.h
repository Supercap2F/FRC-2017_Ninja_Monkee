/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fleet.h
 * Author: majorChaos
 *
 * Created on March 17, 2017, 5:18 PM
 */
 
#ifndef FLEET_H
#define FLEET_H

#include <vector>

#include "entry.h"

class fleet {
public:
    std::vector<entry> templates;
    u_int32_t templateNextId;
    std::vector<entry> entries;
    u_int32_t entriesNextId;
    
    bool addEntry(dataBoat* data, u_int32_t* id);
    bool addTemplate(dataBoat* data, u_int32_t* id);
    bool getTemplateWithClassName(std::string* name, dataBoat** out);
    bool removeEntryAt(unsigned int pos);
    bool toString(std::string* data);
    bool fromString(std::string* data, bool clearFirst=false);
    
    fleet();
    fleet(const fleet& orig);
    virtual ~fleet();
private:

};

#endif /* FLEET_H */

