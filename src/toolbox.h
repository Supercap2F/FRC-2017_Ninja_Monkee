/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   toolbox.h
 * Author: majorChaos
 *
 * Created on March 17, 2017, 5:46 PM
 */
 
#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <string>

namespace toolbox{
    enum toolboxErrors{ toolboxError_noError, toolboxError_fileNotFound, toolboxError_writeError, toolboxError_readError, toolboxError_paramError, toolboxError_unknownError};
    
    /*
     * some stuff
     */
    toolboxErrors toString(int value, std::string* output);
    
    toolboxErrors boolToString(bool value, std::string* output);
    
    int splitString(std::string* in, std::string* out, int start, char endChar);
    
    int splitString(std::string* in, std::string* out, int start, int endPos);
    
    bool toInt(std::string* in, int* out);
    
    bool toBool(std::string* in, bool* out);
    
    bool formattedWrite(std::string* path, std::string* data);
    
    bool readFormatted(std::string* path, std::string* data);
    
    std::pair<std::string,std::string> makePair(std::string name, std::string value);
}

#endif /* TOOLBOX_H */

