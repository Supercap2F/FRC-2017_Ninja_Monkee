/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fleet.cpp
 * Author: majorChaos
 * 
 * Created on March 17, 2017, 5:18 PM
 */

#include "fleet.h"
#include "toolbox.h"

bool fleet::addEntry(dataBoat* data, u_int32_t* id) {
    if(data==NULL || id==NULL){return false;}
    int pos = -1;
    for(int x=0;x<this->entries.size();x++){
        if(this->entries[x].data==NULL){
            pos=x;
            break;
        }
    }
    if(pos<0){
        pos=this->entries.size();
        this->entries.push_back(entry());
    }
    data->copy(&this->entries[pos].data);
    *id = this->entriesNextId++;
    this->entries[pos].id = *id;
    return true;
}

bool fleet::addTemplate(dataBoat* data, u_int32_t* id) {
    if(data==NULL || id==NULL){return false;}
    int pos = -1;
    for(int x=0;x<this->templates.size();x++){
        if(this->templates[x].data==NULL){
            pos=x;
            break;
        }
    }
    if(pos<0){
        pos=this->templates.size();
        this->templates.push_back(entry());
    }
    data->copy(&this->templates[pos].data);
    if(this->templates[pos].data==NULL){return false;}
    this->templates[pos].id = this->templateNextId++;
    return true;
}

bool fleet::getTemplateWithClassName(std::string* name, dataBoat** out){
    if(name==NULL || out==NULL || name->size() < 1){return false;}
    for(int x=0;x<this->templates.size();x++){
        if(this->templates[x].data!=NULL && this->templates[x].data->className==(*name)){
            (*out) = this->templates[x].data;
            return true;
        }
    }
    return false;
}


bool fleet::toString(std::string* data) {
    if(data==NULL){return false;}
    std::vector< std::pair<std::string,std::string> > vars;
    std::string str = "";
    std::string tmp;
    int count = 0;
    for(int x=0;x<this->entries.size();x++){
        if(this->entries[x].data!=NULL){
            vars.clear();
            if(!this->entries[x].data->toVars(&vars)){return false;}
            if(vars.size()<1){continue;}
            count++;
            
            //put obj className to string
            toolbox::toString(this->entries[x].data->className.size(),&tmp);
            tmp+=',';
            str.append(tmp);
            str.append(this->entries[x].data->className);
            
            //amount of variables in obj
            toolbox::toString(vars.size(),&tmp);
            tmp+=',';
            str.append(tmp);
            for(int y=0;y<vars.size();y++){
                
                //variable name, size
                toolbox::toString(vars[y].first.size(),&tmp);
                tmp+=',';
                str.append(tmp);
                
                //variable name, value
                str.append(vars[y].first);
                
                //variable value, size
                toolbox::toString(vars[y].second.size(),&tmp);
                tmp+=',';
                str.append(tmp);
                
                //variable value, value
                str.append(vars[y].second);
            }
            tmp='\n';
            str.append(tmp);
        }
    }
    toolbox::toString(count,&tmp);
    tmp+=',';
    data->append(tmp);
    data->append("\n");
    data->append(str);
    return true;
}


bool fleet::fromString(std::string* data, bool clearFirst) {
    if(data==NULL || data->size()<1){return false;}
    std::vector< std::pair<std::string,std::string> > vars;
    std::string tmp;
    dataBoat* t_boat = NULL;
    u_int32_t t_32;
    std::pair<std::string,std::string> t_entry;
    int t_int;
    int pos = 0;
    int amountOfVars = 0;
    int count =0;
    if(clearFirst){
        this->entries.clear();
    }
    
    // parse amount of entries
    pos+=toolbox::splitString(data,&tmp,pos,',');
    toolbox::toInt(&tmp,&t_int);
    
    while(pos<data->size()){
        //Clear vars
        vars.clear();
        
        //parse obj name size
        pos+=toolbox::splitString(data,&tmp,pos,',');
        toolbox::toInt(&tmp,&t_int);
        
        //parse obj name
        pos+=toolbox::splitString(data,&tmp,pos,pos+t_int);
        if(!this->getTemplateWithClassName(&tmp,&t_boat)){return false;}
        
        //parse obj amount of vars
        pos+=toolbox::splitString(data,&tmp,pos,',');   
        toolbox::toInt(&tmp,&amountOfVars);
        
        //parse obj vars
        count = 0;
        while(pos<data->size() && count<amountOfVars){
            
            //get var name size
            pos+=toolbox::splitString(data,&tmp,pos,',');
            toolbox::toInt(&tmp,&t_int);
            
            //get var name
            pos+=toolbox::splitString(data,&tmp,pos,pos+t_int);
            t_entry.first = tmp;
                    
            //get var value size
            pos+=toolbox::splitString(data,&tmp,pos,',');
            toolbox::toInt(&tmp,&t_int);
            
            //get var value
            pos+=toolbox::splitString(data,&tmp,pos,pos+t_int);
            t_entry.second = tmp;
            
            //Add entry to vars
            vars.push_back(t_entry);
            
            //Increment vars found
            count++;
        }
        
        //Send vars to be parsed
        if(!t_boat->fromVars(&vars)){return false;}
        
        //Add entry
        if(!this->addEntry(t_boat,&t_32)){return false;}
    }
    return true;
}




fleet::fleet() {
    this->templateNextId=0;
    this->entriesNextId=0;
}

fleet::fleet(const fleet& orig) {
}

fleet::~fleet() {
}

