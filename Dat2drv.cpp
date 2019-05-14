#include"Dat2drv.h"

/** @brief Intialize the variable
 *
 *  @param drv_data object of DRV_DATA passed as a reference variable
 *  @return void
 * 
 */
void InitDrvData(DRV_DATA &drv_data)
{
  drv_data.current_path                   = "";
  drv_data.dat_path                       = "";
  drv_data.drv_dir                        = "";
  drv_data.drv_in_template                = "";
  drv_data.drv_in                         = "";
  return;
}

/** @brief create map of model.dat 
 *
 *  @param drv_value object of DRV_DATA
 *  @return MODEL_DICT_MAP
 *
 */
MODEL_DICT_MAP  CreateModelDict(DRV_DATA drv_value)
{
    MODEL_DICT model_dict_values;
    string model_name, line;
    MODEL_DICT_MAP model_dict;
    STRING_VECTOR model_variants;

    
    fstream file_pointer;
    file_pointer.open(drv_value.dat_path.c_str(), fstream::in);

    if(!file_pointer)
    {
       fprintf(stderr, "DAT2DRV : FAILED IN CreateModelDict(DRV_DATA drv_value) -> file_pointer = NULL\n");   
       return model_dict;
    }
   
    while((getline(file_pointer, line)))
    {
          if(line[0] == '#')
             continue; 
          if(line[0] == '[')
          {
             line.assign(line, 1, line.length() -2);
             model_name = line;
             continue;
          }

          if((line.find("model")) != std::string::npos)
          {

             size_t found = line.find("=");
             if(found != std::string::npos)
             {
                line.assign(line, found + 1, line.length());
                model_dict_values.model_variants.push_back(line);
             }
             else
               fprintf(stderr, "DAT2DRV : FAILED IN CreateModelDict(DRV_DATA drv_value) -> = missing in models.dat\n");   
          }  
 
	  if((line.find("plugin-reason")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
             unsigned int count = 0;
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
                 model_dict_values.s_plugin_reason = atoi(value.c_str());
             continue;
          } 

	  if((line.find("plugin")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
             unsigned int count = 0;
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
                 model_dict_values.s_plugin =  atoi(value.c_str());
             continue;
          } 

	  if((line.find("family-class")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
             unsigned int count = 0; 
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
                 model_dict_values.s_family_class = value;
             continue;
          } 

	  if((line.find("tech-class")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
             unsigned int count = 0;
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
                 model_dict_values.s_tech_class = value;
             continue;
          } 

	  if((line.find("tech-subclass")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
             unsigned int count = 0;
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
                 model_dict_values.s_sub_class = value;
             model_dict_values.s_normal_model_name = "";
             model_dict.insert(std::make_pair(model_name, model_dict_values));
             model_dict_values.model_variants.clear();
             model_dict_values.s_tech_class    ="";
             model_dict_values.s_sub_class     ="";
             model_dict_values.s_family_class  ="";
             model_dict_values.s_plugin        = -1;
             model_dict_values.s_plugin_reason = -1;
             continue;
          } 
    }
    if(file_pointer)
       file_pointer.close();    
 return model_dict;

}

/** @brief replace substring
 *
 *  @param model_name model name of the device
 *  @return string
 *
 */
string FixFileName(string model_name)
{

  size_t index       = 0;
  string rep_string  = "",
         temp_string = "";
  unsigned int len   = 0;

  if((index = model_name.find("hp_")) != string::npos)
  {
      model_name.replace(index, 3, "hp-");
      temp_string = model_name;
  }
  else if((index = model_name.find("apollo_")) != string::npos)
  {
      model_name.replace(index, 7, "apollo-");
      temp_string = model_name;
  }
  else 
  {
    if((index = model_name.find("hp-")) == string::npos)
    {
       temp_string = "hp-";
       temp_string += model_name;
    } 
  }
  for(unsigned int count = 0; count < temp_string.length(); count ++)
  {
     if(temp_string[count] == '~')
       continue;
     rep_string += temp_string[count];
     len ++;
  }  
  return rep_string;
} 

/** @brief short the length of model name
 *
 *  @param model_name model name of the device passed as reference variable
 *  @return void
 *
 */
void ShortModelLength(string &model_name)
{
  size_t index = 0;
  
  for(unsigned char count = 0; count < MAX_SHORTENING_REPLACEMENTS; count ++)
  {
        index = model_name.find(SHORTENING_REPLACEMENTS[count]); 
        if(index != string::npos)
           model_name.replace(index, SHORTENING_REPLACEMENTS[count].length(), SHORTENING_REPLACEMENTS_VALUE[count]);
        else 
           continue;
  }
  return;
}

/** @brief convert the vector into the string
 *
 *  @param sub_class vector of sub classed
 *  @return string
 *
 */
string SetSubClass(STRING_VECTOR sub_class)
{
   string subclass = "";
   for(unsigned int count = 0; count< sub_class.size(); count ++)
      subclass += sub_class[count] + ',';
   subclass.assign(subclass, 0, subclass.length() - 1);
   return subclass;
}

/** @brief setting the path of the model.dat, drv & set the name of template files
 *
 *  @return DRV_DATA
 *
 */
DRV_DATA SetFileInfo(bool bHpijs)
{

  string current_path, dat_path, drv_dir, temp_dir = ""; 
  DRV_DATA drv_value;
  char working_dir[MAX_DIRECTORY_SIZE] = {0};

  current_path = getcwd(working_dir, sizeof(working_dir)); 

  dat_path = current_path;
  drv_dir  = current_path;

  drv_dir.append("/prnt/drv");
  dat_path.append("/data/models/models.dat");
 
  drv_value.current_path           = current_path;
  drv_value.dat_path               = dat_path;
  drv_value.drv_dir                = drv_dir;
  temp_dir                         = drv_dir;
  if (bHpijs)
    drv_value.drv_in_template        = temp_dir.append("/hpijs.drv.in.template");
  else
    drv_value.drv_in_template        = temp_dir.append("/hpcups.drv.in.template");
  temp_dir                         = drv_dir;
  if (bHpijs)
    drv_value.drv_in                 = temp_dir.append("/hpijs.drv.in");
  else
    drv_value.drv_in                 = temp_dir.append("/hpcups.drv.in");
  return drv_value;

}

/** @brief check the tech_class passed exsist or not
 *
 *  @param tech_class name of the tech_class
 *  @return int
 *
 */
int TechClassExsist(string tech_class)
{
  for(unsigned char count = 0; count < MAX_TECH_CLASS; count ++)
  {
     if (tech_class == TECH_CLASSES[count])
         return 1;
  }  
  //fprintf(stderr, "DAT2DRV : FAILED IN TechClassExsist(string tech_class) -> = NO TECH CLASS MATCH FOUND\n");   
  return -1;
}

/** @brief check the sub_class passed exsist or not
 *
 *  @param sub_class vector of sub classes
 *  @return unsigned char
 *
 */
unsigned char SubClassExsist(STRING_VECTOR sub_class)
{
   if(sub_class.size() == 0)
         fprintf(stderr, "DAT2DRV : FAILED IN SubClassExsist(STRING_VECTOR sub_class) -> = NO SUB CLASS MATCH FOUND\n");   
   for(unsigned int len = 0 ; len < sub_class.size(); len ++)
   {
       for(unsigned int count = 0; count < MAX_SUB_CLASS; count ++)
       {
            if(TECH_SUBCLASSES[count] == sub_class.at(len))
               return 1; 
       }
   }
   fprintf(stderr, "DAT2DRV : FAILED IN SubClassExsist(STRING_VECTOR sub_class) -> = NO SUB CLASS MATCH FOUND\n");   
   return 0;
}

/** @brief matches device with same tech class and subclass Name
 *
 *  @param model_dict map of models.dat
 *  @param sub_class vector of subclasses
 *  @param subclass  subclass name
 *  @param tech_class tech_class name
 *  @return STRING_VECTOR
 */
STRING_VECTOR CreateTechClassMatch(MODEL_DICT_MAP model_dict, STRING_VECTOR sub_class, string subclass, string tech_class)
{
   unsigned char include = 0, is_same = 0;
   STRING_VECTOR matches;
   string sub_class_1 = "", sub_class_2 = "";
   
   for(MODEL_DICT_MAP::iterator it=model_dict.begin(); it!=model_dict.end(); ++it)
   {
      if((it->second.s_tech_class.find(",")) != std::string::npos)
      {
         for(unsigned int count = 0; count < tech_class.length(); count++)
         { 
         if(it->second.s_tech_class[count] == ',')
            break;
          sub_class_1 += it->second.s_tech_class[count];
         }
         it->second.s_tech_class = sub_class_1; 
         sub_class_1 = "";
      }
      if((it->second.s_tech_class == tech_class)  && it->second.s_sub_class.length() == subclass.length())
      {
         if((sub_class.size()) < 2)
         {
            if(it->second.s_sub_class != subclass)
               continue;
            is_same = 1; 
         }
         else
         {
             for(unsigned int len = 0; len < sub_class.size(); len++)
             { 
                is_same = 1;  
                if((it->second.s_sub_class.find(sub_class[len])) == std::string::npos)
                {
                     is_same = 0;  
                    break;
                }
             } 
         }
         if(is_same == 0)
            continue;
         if(sub_class.size() < 2)
         {
            include = 0;
            for(unsigned int count = 0; count< MAX_SUB_CLASS; count ++)
            {
               if(sub_class[0] == TECH_SUBCLASSES[count]);  
                  include = 1;
            }
         }
         else
         {
            include = 0;
            for(unsigned int len = 0; len < sub_class.size(); len ++)
            {
               for(unsigned int count = 0; count< MAX_SUB_CLASS; count ++)
               {                 
                  if(sub_class[0] == TECH_SUBCLASSES[count])
                     include = 1;
               } 
            } 
          }
          if(include)
            matches.push_back(it->first);
          else
             fprintf(stderr, "DAT2DRV : FAILED IN CreateMatch(MODEL_DICT_MAP model_dict, STRING_VECTOR sub_class, string subclass, string tech_class) ->  NO MATCH FOUND\n");    
      }
   }
   return matches;
}

/** @brief checks whether a family_class exsist
 *
 *  @param family_class family class name
 *  @return unsigned char
 *
 */
unsigned char FamilyClassExsist(string family_class)
{
  for(unsigned char count = 0; count < MAX_FAMILY_CLASS; count ++)
  {
     if (family_class == FAMILY_CLASSES[count])
         return 1;
  }  
  fprintf(stderr, "DAT2DRV : FAILED IN FamilyClassExsist(string family_class) ->  NO FAMILY CLASS MATCH FOUND\n");    
  return 0;
}

/** @brief checks whether a model name exsist or not
 *
 *  @param model_name model name of the device
 *  @param model_dict map of models.dat
 *  @return unsigned char
 *
 */
unsigned char CheckModelNameExsist(string model_name, MODEL_DICT_MAP &model_dict)
{
    MODEL_DICT_MAP model_input_map;
    MODEL_DICT model_dict_values;
    for(MODEL_DICT_MAP::iterator it=model_dict.begin(); it!=model_dict.end(); ++it)
    {
             if(it->first == model_name)
             {
                model_dict_values = it->second;
                model_input_map.insert(std::make_pair(model_name, model_dict_values));
                model_dict = model_input_map;
                return 1;
             }
    }
    return 0;
}

/** @brief checks whether a family match exsist or not
 *
 *  @param model_dict map of model_dict
 *  @param family_class family class name
 *  @return STRING_VECTOR
 *
 */
STRING_VECTOR CreateFamilyClassMatch(MODEL_DICT_MAP model_dict, string family_class)
{
   unsigned char include = 0;
   STRING_VECTOR matches;
     
   for(MODEL_DICT_MAP::iterator it=model_dict.begin(); it!=model_dict.end(); ++it)
   {     
      if((it->second.s_family_class.find(family_class)) == std::string::npos)
         continue;
      include = 1;
      if(include)
         matches.push_back(it->first);
      include = 0;
   }
   return matches;
}

/** @brief create the drv files for family classes
 *
 *  @param drv_data DRV_DATA object
 *  @param model_dict models.dat map
 *  @param model_input model_name passed or not
 *  @param model_input_tech_class tech class of model_input
 *  @param model_input_sub_class sub class of model_input
 *  @return char
 *
 */
char CreateFamilyClassDrv(DRV_DATA drv_data, MODEL_DICT_MAP model_dict, string model_input, string model_input_tech_class)
{
    string family_class = "",
           line         = "";
    STRING_VECTOR matches;
    STRING_VECTOR sub_class;

    fstream file_in_pointer, file_out_pointer;

    file_in_pointer.open(drv_data.drv_in_template.c_str(), fstream::in);
    if(!file_in_pointer)  
    {
       fprintf(stderr, "DAT2DRV : FAILED IN main (int argc, char **argv) ->  UNABLE TO OPEN A FILE FOR READ\n");    
       return -1;
    }
  
    file_out_pointer.open(drv_data.drv_in.c_str(), fstream::out);
    if(!file_out_pointer)
    {
       fprintf(stderr, "DAT2DRV : FAILED IN main (int argc, char **argv) ->  UNABLE TO OPEN A FILE FOR READ\n");    
       return -1;
    } 

    while((getline(file_in_pointer, line)))
    {
            string search_family_class;
            stringstream f(line);
            size_t found = line.find("%");
            file_out_pointer << line + "\n"; 
            if(found != std::string::npos)
            {
               found = line.find("//"); 
               if(found != std::string::npos)
               {
                  while(getline(f, search_family_class, '%'))
                  {   
                     if(search_family_class[0] == ' ' || search_family_class[0] == '>' || search_family_class[3] == '/')  
                         continue;

                     stringstream f1(search_family_class);
                     unsigned char len = 0;
                     sub_class.clear();
                     while(getline(f1, search_family_class, ':'))
		     {
                        if(len == 0)
                        {
                           family_class = search_family_class;
                           len ++;
                        }
			else 
                           sub_class.push_back(search_family_class);
	             }

                     for(unsigned char count = 0 ; count < MAX_FAMILY_CLASS; count ++)
                     {
                         found = 0;
                         if(family_class == FAMILY_CLASSES[count])
                         { 
                            found = 1;  
                            break;  
                         } 
                     }
                     if((found == 0) || (FamilyClassExsist(family_class)) != 1 )
                         continue;
                      
                     size_t index = 0;
                     /*** For single model_name i.e. --m=model_name ***/

                     if(model_input.length() > 0) 
                     {  
                       if(model_input_tech_class != family_class)
                          continue; 
                     }   

                     matches =  CreateFamilyClassMatch(model_dict, family_class);
                     if(matches.size() > 0)
                     {
                        string model_name = family_class;
                        string orignal_model_name = model_name;

                        indent1 = "";
                        indent2 = "";
                        string data = " ";
                        for(unsigned int var= 0; var <line.length(); var ++)
                        {
                            if(line[var] == '/')
                               break;
                            indent1 += data;
                        }                        
                        indent2 = indent1 + "  ";


                          file_out_pointer << indent1 + "{\n";

                          if(model_name.length() > 31)
                             ShortModelLength(model_name);

                          if (model_name.length() > 31)
                          {
                             index = model_name.find("series"); 
                             if(index != string::npos)
                             { 
                                model_name.replace(index, 6, "Ser.");
                                index = model_name.find("  ");
                                if(index != string::npos)
                                  model_name.replace(index, 2, " ");
                              }
                          }
                          
                          if (model_name.length() > 31)
                          {
                             index = model_name.find("ser."); 
                             if(index != string::npos)
                             { 
                                model_name.replace(index, 4, "");
                                index = model_name.find("  ");
                                if(index != string::npos)
                                  model_name.replace(index, 2, " ");
                             }
                          }
                          if (model_name.length() > 31)
                              model_name.assign(model_name, 0, 31);

                          file_out_pointer << indent2 + "ModelName \"";
                          file_out_pointer << orignal_model_name; 
                          file_out_pointer << "\"\n";
                          write_data = "";
                          write_data += indent2 + "Attribute "+ "\"NickName\" \"\" \"" + orignal_model_name  + ", " + "hpcups "+ "$Version\""+ "\n";
                          file_out_pointer << write_data;
                          /*if(model_dict.at(matches[len]).s_plugin == 1 || model_dict.at(matches[len]).s_plugin == 2)
                           {
                             if((model_dict.at(matches[len]).s_plugin_reason & 15) == 1 
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 2 
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 3
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 4
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 5
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 6
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 8
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 9
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 10
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 12 )
                                    file_out_pointer << ", requires proprietary plugin";
                           }
                            file_out_pointer <<"\"\n";*/
                            write_data = "";
                            
                            write_data += indent2 + "Attribute \"ShortNickName\" \"\" " + "\"" + model_name + "\"" + "\n";
                            file_out_pointer << write_data;
                             
                            string famil_str = family_class;
                            string famil_str_2 = family_class;   
                            while((index = famil_str.find("_")) != string::npos) // replacing '_' to ' '
                               famil_str.replace(index, 1, " ");

                            transform(famil_str_2.begin(), famil_str_2.end(), famil_str_2.begin(),::tolower);
                            string devid = ""; 
                            
                            if ((index = famil_str_2.find("laserjet")) != string::npos || (index = famil_str_2.find("designjet")) != string::npos)
                                 devid += "MFG:Hewlett-Packard;MDL:" + famil_str + ";DES:" + famil_str + ";" ;
                            else
                                devid += "MFG:HP;MDL:" + famil_str +";DES:" + famil_str +";";
 
                            write_data = "";
                            write_data += indent2 + "Attribute \"1284DeviceID\" \"\" \"" + devid + "\"\n";
                            file_out_pointer << write_data;
 
                          if(family_class != POST_SCRIPT)
                          {
                                  write_data = "";
                                  write_data += indent2+"PCFileName \"" + FixFileName(family_class) +".ppd\"\n";
                                  file_out_pointer << write_data;
                          
                          }
                          else if (family_class == PDF)  
                          {
                                write_data = "";
                                write_data += indent2+"PCFileName \""+ FixFileName(family_class) + "-pdf.ppd\"\n";
                                file_out_pointer << write_data;

                          }
                          else
                          {
                                write_data = "";
                                write_data +=indent2+"PCFileName \""+ FixFileName(family_class) + "-ps.ppd\"\n";
                                file_out_pointer << write_data;
                          } 
                          write_data = "";
                          write_data +=indent2+"Attribute \"Product\" \"\" \"("+ family_class + ")\"\n";
                          file_out_pointer << write_data;

                          file_out_pointer << indent1 + "}\n";
     
                     }
                     else
                         continue;
                     matches.clear();
                  }

               }

            }
    }
    file_out_pointer.close();
    file_in_pointer.close();  

    if(matches.size() >= 1) ;
       matches.clear();
    return 1;
}

/** @brief create the drv files for tech classes
 *
 *  @param drv_data DRV_DATA object
 *  @param model_dict models.dat map
 *  @param tech_class_model pair of tech class
 *  @param model_input model_name passed or not
 *  @param model_input_tech_class tech class of model_input
 *  @param model_input_sub_class sub class of model_input
 *  @return char
 *
 */
char CreateTechClassDrv(DRV_DATA drv_data, MODEL_DICT_MAP model_dict, STRING_PAIR tech_class_model, string model_input, string model_input_tech_class, string model_input_sub_class, bool bHpijs)
{

    string tech_class = "",
           line       = "",
           subclass   = "";

    STRING_VECTOR sub_class, matches;
    fstream file_in_pointer, file_out_pointer;

    file_in_pointer.open(drv_data.drv_in_template.c_str(), fstream::in);
    if(!file_in_pointer)  
    {
       cout<< "DAT2DRV : FAILED IN main (int argc, char **argv) ->  UNABLE TO OPEN A FILE FOR READ"<<endl;    
       return -1;
    }  
    file_out_pointer.open(drv_data.drv_in.c_str(), fstream::out);
    if(!file_out_pointer)
    {
            cout<< "DAT2DRV : FAILED IN main (int argc, char **argv) ->  UNABLE TO OPEN A FILE FOR READ"<<endl;        
            return -1;
    } 
    while((getline(file_in_pointer, line)))
    {
            size_t found = line.find("%");
            size_t index = 0;
            string search_tech_class;
            stringstream f(line);
            file_out_pointer << line + "\n"; 
            if(found != std::string::npos)
            {
               
               found = line.find("//"); 
               if(found != std::string::npos)
               {
                  while(getline(f, search_tech_class, '%'))
                  {
                     if(search_tech_class[0] == ' ' || search_tech_class[0] == '>' || search_tech_class[3] == '/')
                         continue;
                     stringstream f1(search_tech_class);
                     unsigned char len = 0;
                     sub_class.clear();
                     while(getline(f1, search_tech_class, ':'))
		     {
                        if(len == 0)
                        {
                           tech_class = search_tech_class;
                           len ++;
                        }
			else 
                           sub_class.push_back(search_tech_class);
	             }
                     if((TechClassExsist(tech_class)) != 1)
                         continue;
                     if((SubClassExsist(sub_class)) != 1)
                         continue;
                     if(sub_class.size() > 1)
                        subclass = SetSubClass(sub_class);
                     else
                     {
                         subclass = sub_class[0];
                     } 
                     if(model_input.length() > 0)
                     {  
                       if(model_input_tech_class != tech_class)
                       {
                          sub_class.clear();
                          subclass    = "";
                          continue; 
                       }
                      if(model_input_sub_class != subclass)
                      {
                         sub_class.clear();
                         continue;
                      }  

                      STRING_VECTOR subclasses;
                      string sub_clas  = "";
                      unsigned int len = 0;
                      subclass = model_input_sub_class;
                      while((found = model_input_sub_class.find(",")) != string::npos)
                      {
                        sub_clas.assign(model_input_sub_class, len, found);
                        len = found; 
                        model_input_sub_class.assign(model_input_sub_class, found + 1, model_input_sub_class.length());
                        subclasses.push_back(sub_clas);
                       }
                       subclasses.push_back(model_input_sub_class);
                       sub_class.clear(); 
                       sub_class = subclasses;
                       model_input_sub_class = subclass  ;
                       if(subclasses.size() < 1)
                           continue;
                       subclasses.clear();
                     }
   
                     matches =  CreateTechClassMatch(model_dict, sub_class, subclass, tech_class);
                     if(matches.size() > 0)
                     {
                        unsigned int counter      = 0;
                        string model_name         = "";
                        string orignal_model_name = "";
                        indent1                   = "";
                        indent2                   = "";
                        string data               = " ";
                        for(unsigned int var= 0; var <line.length(); var ++)
                        {
                            if(line[var] == '/')
                               break;
                            indent1 += data;
                        }                        
                        indent2 = indent1 + "  ";
                        for(unsigned char len = 0; len < matches.size(); len ++)
                        {
                          file_out_pointer << indent1 + "{\n";
                          model_name = model_dict.at(matches[len]).s_normal_model_name;
                          if(bHpijs)
                          {
                            model_name.append(" hpijs");
                          }
                          orignal_model_name = model_name;
                          if(model_name.length() > 31)
                             ShortModelLength(model_name);

                          if (model_name.length() > 31)
                          {
                             index = model_name.find("series"); 
                             if(index != string::npos)
                             { 
                                model_name.replace(index, 6, "Ser.");
                                index = model_name.find("  ");
                                if(index != string::npos)
                                  model_name.replace(index, 2, " ");
                              }
                          }
                          
                          if (model_name.length() > 31)
                          {
                             index = model_name.find("ser."); 
                             if(index != string::npos)
                             { 
                                model_name.replace(index, 4, "");
                                index = model_name.find("  ");
                                if(index != string::npos)
                                  model_name.replace(index, 2, " ");
                             }
                          }
                          if (model_name.length() > 31)
                              model_name.assign(matches[len], 0, 31);

                          file_out_pointer << indent2 + "ModelName \"";
                          file_out_pointer << orignal_model_name; 
                          file_out_pointer << "\"\n";
                          index = model_dict.at(matches[len]).s_tech_class.find(","); 

                          if(index != string::npos) //len of tech-class > 1
                          {   
                             write_data = "";
                             if(!bHpijs) 
                               write_data += indent2+"Attribute "+ "\"NickName\" \"\" \"" + orignal_model_name + " " + tech_class_model.at(tech_class) + ", " + "hpcups "+ "$Version";
                             else
                               write_data += indent2+"Attribute "+ "\"NickName\" \"\" \"" + orignal_model_name + " " + tech_class_model.at(tech_class) + ", " + "$Version";
                             file_out_pointer << write_data;

                          }
                          else
                          {
                             write_data = "";
                             if(!bHpijs)
                               write_data +=indent2+"Attribute "+ "\"NickName\" \"\" \"" + orignal_model_name  + ", " + "hpcups "+ "$Version";
                             else
                               write_data +=indent2+"Attribute "+ "\"NickName\" \"\" \"" + orignal_model_name  + ", " + "$Version";
                             file_out_pointer << write_data;
                          }  
                          if(model_dict.at(matches[len]).s_plugin == 1 || model_dict.at(matches[len]).s_plugin == 2)
                           {
                             if((model_dict.at(matches[len]).s_plugin_reason & 15) == 1 
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 2 
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 3
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 4
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 5
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 6
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 8
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 9
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 10
                                || (model_dict.at(matches[len]).s_plugin_reason & 15) == 12 )
                                    file_out_pointer << ", requires proprietary plugin";
                           }
                            file_out_pointer <<"\"\n";
                            write_data = "";
                            if(bHpijs)
                              write_data += indent2 + "Attribute \"ShortNickName\" \"\" " + "\"" + model_name + "\"" + "\n";
                            else
                            {
                             string write_data1= "";
                              write_data1 = model_name + " hpijs";
                              if(write_data1.length() > 31)
                              {                              
                              ShortModelLength(write_data1);                              
                              }
                              write_data += indent2 + "Attribute \"ShortNickName\" \"\" " + "\"" + write_data1 + "\"" + "\n";                              
			     }
                            file_out_pointer << write_data;
                             
                            string pp = matches[len];
                            string p = matches[len];   
                            while((index = pp.find("_")) != string::npos) // replacing '_' to ' '
                               pp.replace(index, 1, " ");

                            transform(p.begin(), p.end(), p.begin(),::tolower);
                            string devid = ""; 
                            if((index = p.find("apollo")) != string::npos) 
                               devid += "MFG:Apollo;MDL:" + pp +";DES:" + pp + ";";

                            else if ((index = p.find("laserjet")) != string::npos || (index = p.find("designjet")) != string::npos)
                                 devid += "MFG:Hewlett-Packard;MDL:" + pp + ";DES:" + pp + ";" ;
                            else
                                devid += "MFG:HP;MDL:" + pp +";DES:" + pp +";";
 
                            write_data = "";
                            write_data += indent2 + "Attribute \"1284DeviceID\" \"\" \"" + devid + "\"\n";
                            file_out_pointer << write_data;
 
                          index = model_dict.at(matches[len]).s_tech_class.find(","); 
                          if(index != string::npos) //len of tech-class > 1
                          {   
                             write_data = "";
                             if (!bHpijs){
                               write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-" + tech_class_model.at(tech_class) + ".ppd\"\n";
                             }
                             else {
                               write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-" + tech_class_model.at(tech_class) + "-hpijs"+".ppd\"\n";
                             }
                             file_out_pointer << write_data;
                          }
                          else if(tech_class != POST_SCRIPT)
                          {
                              write_data = ""; 
                              if (!bHpijs)
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) +".ppd\"\n";
                              else
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-hpijs" + ".ppd\"\n";
                              file_out_pointer << write_data;

                          }
                          else if (tech_class == PDF)  
                          {
                              write_data = "";
                              if (!bHpijs)
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-pdf.ppd\"\n";
                              else
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-hpijs" + "-pdf.ppd\"\n";
                              file_out_pointer << write_data;
                          }
                          else
                          {
                              write_data = "";
                              if (!bHpijs)
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-ps.ppd\"\n";
                              else
                                write_data +=indent2+"PCFileName \""+ FixFileName(matches[len]) + "-hpijs" + "-ps.ppd\"\n";
                              file_out_pointer << write_data;
                          } 
                          for(unsigned int count = 0; count < model_dict.at(matches[len]).model_variants.size(); count ++)
                          {
                              write_data = "";
                              NoramlizeModeVariantlName(model_dict.at(matches[len]).model_variants[count]);
                              write_data +=indent2+"Attribute \"Product\" \"\" \"("+ model_dict.at(matches[len]).model_variants[count] + ")\"\n";
                              file_out_pointer << write_data;
                          }
                          file_out_pointer << indent1 + "}\n";
                        }
                     }
                     else
                     {  
                         continue;
                     }
                     matches.clear();
                     sub_class.clear();
                  }
               }
            }
    }
    file_out_pointer.close();
    file_in_pointer.close();  
    if(sub_class.size() >= 1)
       sub_class.clear();
    if(matches.size() >= 1) ;
       matches.clear();
    return 1;
}

/** @brief main function of the program
 *
 *  @param argc argument count
 *  @param argv agrument value
 *  @return int
 *
 */

int main (int argc, char **argv)
{
    unsigned char family_type     = 0,
                  include         = 0;  
    DRV_DATA drv_data;
    DRV_DATA drv_data_ijs;
    string model_input            = "",
           model_input_tech_class = "",
           model_input_sub_class  = "";

    string model_input_ijs            = "",
           model_input_tech_class_ijs = "",
           model_input_sub_class_ijs  = "";
     

    /** Checking number of arguments */
    if (argc < 1 || argc > 3)
    {
        cout<< "dat2drv number of argument passed are invalid" <<endl;
        return -1;
    }

    /** Initializing drv_data        */
    InitDrvData(drv_data);
    InitDrvData(drv_data_ijs);

    /** filling drv_data  structure  */
    drv_data = SetFileInfo(0);
    drv_data_ijs = SetFileInfo(1);

    /** Creating Model.dat Map       */
    MODEL_DICT_MAP model_dict = CreateModelDict(drv_data);
    MODEL_DICT_MAP model_dict_ijs = CreateModelDict(drv_data_ijs);


    /** Noramlizing Model Name       */
    NoramlizeModelName(model_dict);
    NoramlizeModelName(model_dict_ijs);
 
    for(unsigned char count = 0; count < argc; count ++)
    {
        size_t position = 0;
        string argument = argv[count];

        /** Checking Family class ppd */ 
        if((argument.find("-f")) != std::string::npos)
            family_type = 1;

        /** Checking for particular Model name */ 
        if((argument.find("--m=")) != std::string::npos)
        {
            argument.assign(argument, 4, argument.length());

            if((CheckModelNameExsist(argument, model_dict)) != 1)
            {
                cout<< "ERROR : dat2drv Invalid Model Name Passed"<<endl;
                return -1;
            }

            MODEL_DICT_MAP::iterator it=model_dict.begin();
            if(family_type == 1)
               model_input_tech_class = it->second.s_family_class;
            else
            {   
            model_input_tech_class = it->second.s_tech_class;
            model_input_sub_class  = it->second.s_sub_class;
            }
            model_input = argument;
        }

        if((argument.find("-h")) != std::string::npos || (argument.find("--help")) != std::string::npos)
        {
            cout <<"./datdrv [-f] [--m=model_name]"<< endl;
            cout <<"	-f					: create family class ppd's"<< endl;
            cout <<"	--m=<model name> 			: model name for the device"<< endl;
            cout <<"EXAMPLE :"<< endl;
            cout <<"	./datdrv				: create discreet ppd's for all devices"<< endl;
            cout <<"	./datdrv --m=<model name>		: create discreet ppd for specified model"<< endl;
            cout <<"	./datdrv -f				: create family based ppd's for all devices"<< endl;
            cout <<"	./datdrv -f --m=<model name>		: create family based ppd for specified model"<< endl;
            return 1; 
        }
    }
   
    if(family_type == 0 && model_input.length() == 0 && argc != 1)
    {
        cout<< "dat2drv number of argument passed are invalid" <<endl;
        return -1;
    }
            
    /** if -f not passed */
    if(family_type == 0)
    {
      /** Making Pair */    
      STRING_PAIR tech_class_model =  TechClassPdls();
      CreateTechClassDrv(drv_data, 
				model_dict, 
				tech_class_model, 
				model_input, 
				model_input_tech_class, 
				model_input_sub_class, 0);
//      cout<< "HPCUPS is Done" <<endl;
      /** Making Pair */    
      STRING_PAIR tech_class_model_ijs =  TechClassPdls();
      CreateTechClassDrv(drv_data_ijs, 
				model_dict_ijs, 
				tech_class_model_ijs, 
				model_input_ijs, 
				model_input_tech_class_ijs, 
				model_input_sub_class_ijs, 1);
    }
    else
      CreateFamilyClassDrv(drv_data, model_dict, model_input, model_input_tech_class);
    
    model_dict.clear();
    return 1;
}







