/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dataBoat.cpp
 * Author: majorChaos
 * 
 * Created on March 17, 2017, 5:00 PM
 */

#include "dataBoat.h"
 
dataBoat::dataBoat() {
    this->className = "dataBoat";
    this->time=0;
}

dataBoat::dataBoat(const dataBoat& orig) {
}

dataBoat::~dataBoat() {
}

bool dataBoat::toVars(std::vector<std::pair<std::string, std::string> >* vars) {
    //vars->push_back(std::pair<std::string, std::string>("*","*"));
    return true;
}

bool dataBoat::fromVars(std::vector<std::pair<std::string, std::string> >* vars, int pos) {
    //pos+=1;
    return true;
}

bool dataBoat::copy(dataBoat** to) {
    if(to==NULL){return false;}
    if(*to==NULL){
        *to = new dataBoat();
    }
    return true;
}
