#include"FindPPD.h"


/** @brief create map of model.dat 
 *
 *  @param model_dict object of FindPPD.h
 *  @return 1 and 0
 *
 */
unsigned char  CreateModelDict(MODEL_DICT_MAP &model_dict)
{
    unsigned int count = 0; 
    string model_name  = "",
           line        = "";

    unsigned char family_ppd = 0;
    fstream file_pointer;
    file_pointer.open("/usr/share/hplip/data/models/models.dat", fstream::in);

    if(!file_pointer)
    {
       fprintf(stderr, "DAT2DRV : FAILED IN CreateModelDict() -> file_pointer = NULL\n");   
       return 0;
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

	  if((line.find("family-ppd")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;
            
             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
             {
                model_dict.insert(std::make_pair(model_name, value));
                count = 0;
                family_ppd = 1;
             }
             continue;
          } 

	  if((line.find("family-class")) != std::string::npos)
          {     
             stringstream file_stream(line);
             string value;

             while(getline(file_stream, value, '='))
                   count ++;
             if(count == 2)
             {
                if (family_ppd == 1) 
                    family_ppd = 0;
                else
                    model_dict.insert(std::make_pair(model_name, value));
                count = 0;
             }
             continue;
          } 

    }
    if(file_pointer)
       file_pointer.close();    
    return 1;

}

/** @brief checks whether a model name exsist or not
 *
 *  @param model_name model name of the device
 *  @param model_dict map of models.dat
 * @return unsigned char 1 and 0
 *
 */
unsigned char CheckModelNameExsist(string model_name, MODEL_DICT_MAP &model_dict)
{

    for(MODEL_DICT_MAP::iterator it=model_dict.begin(); it!=model_dict.end(); ++it)
    {
             if(it->first == model_name)
             {
                cout << "Class PPD Name : "<< it->second<<endl;
                return 1;
             }
    }
    return 0;
}

int main (int argc, char **argv)
{

    MODEL_DICT_MAP model_dict;
    unsigned char count = 0;
   /** Creating Model.dat Map       */
    CreateModelDict(model_dict);
    for(count = 0; count < argc; count ++)
    {
        string argument = argv[count];
        /** Checking for particular Model name */ 
        if((argument.find("--m=")) != std::string::npos)
        {
            argument.assign(argument, 4, argument.length());
            if((CheckModelNameExsist(argument, model_dict)) != 1)
            {
                cout<< "ERROR : hp-locatedriver Invalid Model Name Passed"<<endl;
                return -1;
            }
            model_dict.clear();
            return 1;   
        }
        if((argument.find("-h")) != std::string::npos || (argument.find("--help")) != std::string::npos)
        {
            cout <<"hp-locatedriver [--m=model_name]"<< endl;
            cout <<"	--m=<model name> 			: model name for the device"<< endl;
            cout <<"EXAMPLE :"<< endl;
            cout <<"	hp-locatedriver --m=<model name>		: Find class ppd of the model name passed"<< endl;
            model_dict.clear();
            return 1; 
        }

    }
   cout<< "ERROR : No Input Passed"<<endl;
   model_dict.clear();
   return 1;
}







