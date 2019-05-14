#include"Normalize.h"

/** string array of family classes                 */
string FAMILY_CLASSES[MAX_FAMILY_CLASS] = {"PCL3-Class3A","PCL3-Class3B","PCL3-Class3","PCLM_COLOR","PCLM_MONO","PCL4-Class1","LJ-Class1","LJ-Class2","LJ-Class3","LJ-Class4","LJ-Class4A","LJ-Class5","LJ-Class6","DJGenericVIP","DJ9xxVIP","DJ55xx","Stabler","StingrayOJ","Copperhead","CopperheadXLP","Copperhead12","CopperheadIPH","CopperheadIPH15","CopperheadIPH17","CLE","CLE17","PyramidRefresh17","Saipan","Saipan15B","Kapan","ViperPlusVIP","ViperMinusVIP","Corbett","Ampere","Python","Python10","Python11","Mimas","Mimas15","Mimas17","MimasTDR","PyramidRefresh15","P15_CISS","Pyramid","Pyramid15","PyramidPlus","Gemstone","SPDOfficejetProAsize","SPDOfficejetProBsize","OJ7000","OJProKx50","PSP100","PSP470"}; 

/** string array of tech classes                 */
string TECH_CLASSES[MAX_TECH_CLASS] = {
    "Undefined", 
    "Unsupported",
    "Postscript",
    "PDF",
    "DJGenericVIP",
    "PSB9100",
    "LJMono",
    "LJColor",
    "LJFastRaster",
    "LJJetReady",
    "DJ350",
    "DJ400", 
    "DJ540",
    "DJ600",
    "DJ6xx",
    "DJ6xxPhoto",
    "DJ630",
    "DJ660",
    "DJ8xx",
    "DJ8x5",
    "DJ850",
    "DJ890",
    "DJ9xx",
    "DJ9xxVIP",
    "DJ3600",
    "DJ3320",
    "DJ4100",
    "AP2xxx",
    "AP21xx",
    "AP2560",
    "PSP100",
    "PSP470",
    "LJZjsMono",
    "LJZjsColor",
    "LJm1005",
    "QuickConnect",
    "DJ55xx",
    "OJProKx50",
    "LJP1XXX",
    "Stabler",
    "ViperPlusVIP",
    "ViperMinusVIP",
    "ViperPlusTrim",
    "ViperMinusTrim",
    "Corbett",
    "Python",
    "OJ7000",
    "Pyramid",
    "Pyramid15",
    "Python10",
    "Mimas",
    "Mimas15",
    "StingrayOJ",
    "Copperhead",
    "CopperheadXLP",
    "Copperhead12",
    "CopperheadIPH",
    "CopperheadIPH15",
    "CopperheadIPH17",
    "PyramidRefresh15",
    "PyramidRefresh17",
    "Ampere",
    "Python11",
    "Saipan",
    "PyramidPlus",
    "Hbpl1",
    "Kapan",
    "MimasTDR",
    "Saipan15B",
    "Gemstone",
    "SPDOfficejetProAsize",
    "CLE",
    "SPDOfficejetProBsize",
    "CLE17",
    "Mimas17",
    "P15_CISS"
   
};

/** string array of tech sub classes                 */
string TECH_SUBCLASSES[MAX_SUB_CLASS] = {
    "LargeFormatSuperB",
    "LargeFormatA3",
    "CoverMedia",
    "FullBleed",
    "Duplex",
    "Normal",
    "Apollo2000",
    "Apollo2200",
    "Apollo2500",
    "NoPhotoMode",
    "NoPhotoBestHiresModes",
    "No1200dpiNoSensor",
    "NoFullBleed",
    "4x6FullBleed",
    "300dpiOnly",
    "GrayscaleOnly",
    "NoAutoTray", 
    "NoEvenDuplex",
    "NoAutoDuplex",
    "NoCDDVD",
    "NoMaxDPI",
    "NoMaxDPI",
    "SmallMargins",
    "Trim",
    "4800x1200dpi",
    "Advanced",
    "Mono",
    "Color",
    "AutoDuplex",
    "K10"
};

/** string array of shortening replacements                 */
string SHORTENING_REPLACEMENTS[MAX_SHORTENING_REPLACEMENTS] = {
"Color",
"color laserjet",
"laserjet",
"LaserJet",
"photosmart",
"Photosmart",
"deskjet",
"Deskjet",
"color inkjet printer",
"officejet",
"Officejet",
"business inkjet",
"designjet",
"printer scanner copier",
"color lj",
"professional",
"Professional",
"Inkjet",
"Wireless",
"Tank",
};

/** string array of shortening replacements values                 */
string SHORTENING_REPLACEMENTS_VALUE[MAX_SHORTENING_REPLACEMENTS] = {
"CJ",
"CJL",
"LJ",
"LJ",
"PS",
"PS",
"DJ",
"DJ",
"",
"OJ",
"OJ",
"BIJ",
"DESIGNJ",
"PSC",
"CLJ",
"Pro",
"Pro",
"IJ",
"WL",
"TK",
};

/** @brief create vector of STRING STRING pair
 *
 *  @return STRING_PAIR
 *
 */

PAIR_VECTOR TechClassPdlKeySvalues()
{

    PAIR_VECTOR  TECH_CLASS_KEY_VALUE;

    TECH_CLASS_KEY_VALUE.push_back(make_pair("Postscript", "ps"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("PDF", "pdf"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJGenericVIP", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJMono", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJColor", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJFastRaster", "pclxl"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJJetReady", "pclxl"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ350", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ540", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ600", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ6xx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ6xxPhoto", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ630", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ8xx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ8x5", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ850", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ890", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ9xx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ9xxVIP", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ3600", "lidil"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ3320", "lidil"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ4100", "lidil"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("AP2xxx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("AP21xx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("AP2560", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("PSP100", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("PSP470", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJZjsMono", "zjs"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJZjsColor", "zjs"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJm1005", "zxs"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("QuickConnect", "jpeg"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("DJ55xx", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("OJProKx50", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("LJP1XXX", "zxs"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Stabler", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("ViperPlusVIP", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("ViperMinusVIP", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("ViperPlusTrim", "lidil"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("ViperMinusTrim", "lidil"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Corbett", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Python", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("OJ7000", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Python10", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Mimas", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Mimas15", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("StingrayOJ", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Pyramid15", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Copperhead", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CopperheadXLP", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Copperhead12", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CopperheadIPH", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CopperheadIPH15", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CopperheadIPH17", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("PyramidRefresh15", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("PyramidRefresh17", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Ampere", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Hbpl1", "hbpl1"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Kapan", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("MimasTDR", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Saipan15B", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Gemstone", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("SPDOfficejetProAsize", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CLE", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("SPDOfficejetProBsize", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("CLE17", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("Mimas17", "pcl3"));
    TECH_CLASS_KEY_VALUE.push_back(make_pair("P15_CISS", "pcl3"));
    return TECH_CLASS_KEY_VALUE;
}


/** @brief create pair for tech classes
 *
 *  @return STRING_PAIR
 *
 */

STRING_PAIR TechClassPdls()
{
      STRING_PAIR model_tech_class;
      PAIR_VECTOR  TECH_CLASS_KEY_VALUE = TechClassPdlKeySvalues();

      for(unsigned char count = 0; count < MAX_TECH_CLASS_PDL; count ++)
          model_tech_class.insert(std::make_pair(TECH_CLASS_KEY_VALUE[count].first, TECH_CLASS_KEY_VALUE[count].second));
     return model_tech_class;

}

/** @brief create pair for tech classes
 *
 *  @return string
 *
 */
string ReplaceStrChar(string model_name, const string &replace, char value)
{

  size_t pos = model_name.find_first_of(replace);

  while (pos != string::npos)
  { 
    model_name[pos] = value;
    pos = model_name.find_first_of(replace, pos+1);
  }
  return model_name; 
}

/** @brief Capitalize the model name character
 *  @param model_dict  map of models.dat
 *  @return string
 *
 */
void Capitalize(string &model_name)
{
  bool cap_check = true;
 
  for (int index = 0; index < model_name.length(); index++)
  {
    if (isalpha(model_name.at(index))&& cap_check == true)
    {
        model_name.at(index) = toupper(model_name.at(index));
        cap_check = false;
    }
    else if (isdigit (model_name.at(index)))
    {
	cap_check = false;
        if(isupper(model_name.at(index-1))!=1)
        {
           model_name.at(index-1)=tolower(model_name.at(index-1));
	}
        if (isupper(model_name.at(index-2))!=1)
        {
   	    model_name.at(index-2)=tolower(model_name.at(index-2));
	} 
    }
    else if (isspace (model_name.at(index)))
         cap_check = true;
    else if(model_name.at(index) == '~')
         model_name.at(index)=' ';
  }


  if(model_name.find("Psc")!= std::string::npos){
  	size_t pos=model_name.find("Psc");
	model_name.replace(pos,3,"PSC");
   }
   if(model_name.find("Mfp")!= std::string::npos){
   	size_t pos=model_name.find("Mfp");
	model_name.replace(pos,3,"MFP");
   }
   if(model_name.find("Laserjet")!= std::string::npos){
       size_t pos=model_name.find("Laserjet");	
       model_name.replace(pos,8,"LaserJet");
   }
   

}

/** @brief Normal the model name
 *  @param model_dict  map of models.dat
 *  @return void
 *
 */
void  NoramlizeModelName(MODEL_DICT_MAP &model_dict )
{
	
    for(MODEL_DICT_MAP::iterator it= model_dict.begin(); it!=model_dict.end(); ++it)
    {
       string model_name=it->first;
       if (model_name.find("apollo")!= std::string::npos)
       {
           model_name = ReplaceStrChar(model_name, "_ ",' ');
       }
       else if(model_name.find("hp")!= std::string::npos)
       {
		
           model_name = ReplaceStrChar(model_name, "_ ",' ');
           size_t pos = model_name.find("hp");
           model_name.replace(pos, 2, "HP");	
       }
       else
       { 	   
	  model_name = ReplaceStrChar(model_name, "_ ",' ');	 
          model_name.insert(0,"HP ");
       }
       Capitalize(model_name);
       unsigned int len = 0;
       len = model_name.length(); 
       if (model_name[len-1] == ' ')
          model_name.assign(model_name, 0, model_name.length()-1);
       if(model_name[0] == 'H' && model_name[1] == 'p')
          model_name[1] = 'P';
       it->second.s_normal_model_name = model_name;
    }
}


void NoramlizeModeVariantlName(string &model_name){			  

  if (model_name.find("Apollo")!= std::string::npos)
      return;

   for (int index=0; index < model_name.length(); index++){
   	if(isalpha(model_name.at(index))&& isupper(model_name.at(index))){
	   model_name.at(index)=tolower( model_name.at(index));
     	}
   }

   Capitalize(model_name);
    if(model_name.find("Hp")!= std::string::npos)
    {
       size_t pos = model_name.find("Hp");
       model_name.replace(pos, 2, "HP");	
    } else{       
       model_name.insert(0,"HP ");       
    }
    	
}

