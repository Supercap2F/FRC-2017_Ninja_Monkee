/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   toolbox.cpp
 * Author: majorChaos
 * 
 * Created on March 17, 2017, 5:46 PM
 */

#include "toolbox.h"
  
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <fstream>

toolbox::toolboxErrors toolbox::toString(int value, std::string* output){
    std::stringstream s;
    s << value;
    *output = s.str();
    return toolbox::toolboxError_noError;
}


toolbox::toolboxErrors toolbox::doubleToString(double value, std::string* output){
    std::stringstream s;
    s << (value*1000);
    *output = s.str();
    return toolbox::toolboxError_noError;
}

toolbox::toolboxErrors toolbox::boolToString(bool value, std::string* output){
    if(value){
        (*output) = "true";
    }else{
        (*output) = "false"; 
    }
    return toolbox::toolboxError_noError;
}

int toolbox::splitString(std::string* in, std::string* out, int start, char endChar){
    char c;
    int count=0;
    out->clear();
    for(unsigned int x=start;x<in->size();x++,count++){
        c=(*in)[x];
        if(c==endChar){
            count++;
            break;
        }
        out->push_back(c);
    }
    return count;
}

int toolbox::splitString(std::string* in, std::string* out, int start, int endPos){
    int count=0;
    out->clear();
    for(unsigned int x=(unsigned)start;x<in->size() && x<(unsigned)endPos;x++,count++){
        out->push_back((*in)[x]);
    }
    return count;
}

bool toolbox::toInt(std::string* in, int* out){
    (*out) = atoi(in->c_str());
    return true;
}


bool toolbox::toDouble(std::string* in, double* out){
    (*out) = atoi(in->c_str())/(double)1000;
    return true;
}

bool toolbox::toBool(std::string* in, bool* out){
    if((*in) == "true"){
        (*out) = true;
    }else if((*in) == "false"){
        (*out) = false;
    }else if(in->size()==1){
        if((*in)[0] == '0'){
            (*out) = false;
        } else if((*in)[0] == '1'){
            (*out) = true;
        }else{
            return false;
        }
    }else{
        return false;
    }
    return true;
}

bool toolbox::formattedWrite(std::string* path, std::string* data){
    if(path==NULL || data==NULL){return false;}
    std::ofstream file(path->c_str(), std::ios::binary);
    if(!file.good()){file.close();return false;}
    for(unsigned int x=0;x<data->size();x++){
        file.put((*data)[x]);
        if(!file.good()){file.close();return false;}
    }
    file.close();
    return true;
}

bool toolbox::readFormatted(std::string* path, std::string* data){
    if(path==NULL || data==NULL){return false;}
    std::ifstream file(path->c_str(), std::ios::binary);
    if(!file.good()){file.close();return false;}
    std::string tmp = "";
    while(!file.eof()){
        file >> tmp;
        data->append(tmp);
    }
    file.close();
    return true;
}


std::pair<std::string,std::string> toolbox::makePair(std::string name, std::string value){
    return std::pair<std::string,std::string>(name,value);
}
