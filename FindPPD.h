#ifndef FIND_PPD_H
#define FIND_PPD_H

#include<iostream>
#include<string>
#include<fstream>
#include<unistd.h>
#include<sstream>
#include<map>

using namespace std;


/** typedef  map< string,  string> */
typedef map<string,  string> MODEL_DICT_MAP;


/** @brief create map of model.dat 
 *
 *  @param drv_value object of DRV_DATA
 *  @return MODEL_DICT_MAP
 */
unsigned char  CreateModelDict(MODEL_DICT_MAP &model_dict);

#endif
