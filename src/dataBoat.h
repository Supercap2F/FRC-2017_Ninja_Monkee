/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dataBoat.h
 * Author: majorChaos
 *
 * Created on March 17, 2017, 5:00 PM
 */
 
#ifndef DATABOAT_H
#define DATABOAT_H
#include <string>
#include <vector>

class dataBoat {
public:
    std::string className;
    double time;
    
    virtual bool toVars(std::vector< std::pair<std::string,std::string> >* vars);
    virtual bool fromVars(std::vector< std::pair<std::string,std::string> >* vars, int pos=0);
    virtual bool copy(dataBoat** to);
    dataBoat();
    dataBoat(const dataBoat& orig);
    virtual ~dataBoat();
private:

};

#endif /* DATABOAT_H */

