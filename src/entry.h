/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   entry.h
 * Author: majorChaos
 *
 * Created on March 17, 2017, 4:54 PM
 */
 
#ifndef ENTRY_H
#define ENTRY_H

#include <stdlib.h>

#include "dataBoat.h"

class entry {
private:
public:
    
    dataBoat* data;
    u_int32_t id;
    void clear();
    void init();
    entry();
    entry(const entry& orig);
    virtual ~entry();
private:
};

#endif /* ENTRY_H */

