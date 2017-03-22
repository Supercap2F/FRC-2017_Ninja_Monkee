/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   entry.cpp
 * Author: majorChaos
 * 
 * Created on March 17, 2017, 4:54 PM
 */

#include "entry.h"
 
void entry::clear() {
    if(this->data!=NULL){
        delete this->data;
    }
    this->data=NULL;
}

void entry::init(){
    this->data=NULL;
}

entry::entry() {
    this->init();
}


entry::entry(const entry& orig) {
    this->init();
    if(orig.data!=NULL){
        orig.data->copy(&this->data);
    }
    this->id = orig.id;
}

entry::~entry() {
    this->clear();
}



