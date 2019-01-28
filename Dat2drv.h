#ifndef DATDRV_H
#define DATDRV_H

#include "Normalize.h"

#define MAX_DRV_COUNT                      2
#define MAX_DIRECTORY_SIZE                 1024

#define COLOR                              "Color"
#define COLOR_LASERJET                     "color laserjet"
#define LASERJET                           "laserjet"
#define LASERJET_CAP                       "LaserJet"
#define PHOTOSMART                         "photosmart"
#define PHOTOSMART_CAP                     "Photosmart"
#define DESKJET                            "deskjet"
#define DESKJET_CAP                        "Deskjet"
#define COLOR_INKJET_PRINTER               "color inkjet printer"
#define OFFICEJET                          "officejet"
#define OFFICEJET_CA                       "Officejet"
#define BUSINESS_INKJET                    "business inkjet"
#define DESIGNJET                          "designjet"
#define PRINTER_SCANNER_COPIER             "printer scanner copier"
#define COLOR_LJ                           "color lj"
#define PROFESSIONAL                       "professional"
#define PROFESSIONAL_CAP                   "Professional"
#define INKJET                             "Inkjet"
#define WIRELESS                           "Wireless"
#define TANK                               "Tank"


#define COLOR_REPLACEMENT                  "CL"
#define COLOR_LASERJET_REPLACEMENT         "CJL"
#define LASERJET_REPLACEMENT               "LJ"
#define LASERJET_CAP_REPLACEMENT           "LJ"
#define PHOTOSMART_REPLACEMENT             "PS"
#define PHOTOSMART_CAP_REPLACEMENT         "PS"
#define DESKJET_REPLACEMENT                "DJ"
#define DESKJET_CAP_REPLACEMENT            "DJ"
#define COLOR_INKJET_PRINTER_REPLACEMENT   ""
#define OFFICEJET_REPLACEMENT              "OJ"
#define OFFICEJET_CAP_REPLACEMENT          "OJ"
#define BUSINESS_INKJET_REPLACEMENT        "BIJ"
#define DESIGNJET_REPLACEMENT              "DESIGNJ"
#define PRINTER_SCANNER_COPIER_REPLACEMENT "PSC"
#define COLOR_LJ_REPLACEMENT               "CLJ"
#define PROFESSIONAL_REPLACEMENT           "Pro"
#define PROFESSIONAL_CAP_REPLACEMENT       "Pro"
#define INKJET_REPLACEMENT                 "IJ"
#define WIRELESS_REPLACEMENT               "WL"
#define TANK_REPLACEMENT                   "TK"

#define POST_SCRIPT                         "Postscript"
#define PDF                                 "PDF"


#define MAX_SHORTENING_REPLACEMENTS        20 

/** extern string FAMILY_CLASSES from Models.cpp                     */
extern string FAMILY_CLASSES[MAX_FAMILY_CLASS];

/** extern string TECH_CLASSES from Models.cpp                        */
extern string TECH_CLASSES[MAX_TECH_CLASS];

/** extern string TECH_SUBCLASSES from Models.cpp                     */
extern string TECH_SUBCLASSES[MAX_SUB_CLASS];

/** extern string SHORTENING_REPLACEMENTS from Models.cpp             */
extern string SHORTENING_REPLACEMENTS[MAX_SHORTENING_REPLACEMENTS];

/** extern string SHORTENING_REPLACEMENTS_VALUE from Models.cpp       */
extern string SHORTENING_REPLACEMENTS_VALUE[MAX_SHORTENING_REPLACEMENTS];

/** extern string TECH_CLASS_KEYS from Models.cpp                     */
extern string TECH_CLASS_KEYS[MAX_TECH_CLASS_PDL];

/** extern string TECH_CLASS_VALUES from Models.cpp                   */
extern string TECH_CLASS_VALUES[MAX_TECH_CLASS_PDL];

/** @struct _DRV_DATA_

 *  @brief This structure contains files path & file name info
 *  @var _DRV_DATA_::current_path 
 *  Member 'current_path' contains source path info

 *  @var _DRV_DATA_::dat_path 
 *  Member 'dat_path' contains model.path path info

 *  @var _DRV_DATA_::drv_dir 
 *  Member 'drv_dir' contains drv path info

 *  @var _DRV_DATA_::drv_in_template[MAX_DRV_COUNT]
 *  Member 'drv_in_template[MAX_DRV_COUNT]' contains template names info

 *  @var _DRV_DATA_::drv_in[MAX_DRV_COUNT];
 *  Member 'drv_in[MAX_DRV_COUNT];' contains update template drv in names info

*/

typedef struct _DRV_DATA_
{
  string current_path;
  string dat_path;
  string drv_dir;
  string drv_in_template;
  string drv_in;

}DRV_DATA;

/** used for spacing in drv files                 */
string indent1 = "";
string indent2 = "";
/** global string used to write data in drv files */
string write_data = "";



/** @brief Intialize the variable
 *
 *  @param drv_data object of DRV_DATA passed as a reference variable
 *  @return void 
 *
 */
void InitDrvData(DRV_DATA &drv_data);


/** @brief create map of model.dat 
 *
 *  @param drv_value object of DRV_DATA
 *  @return MODEL_DICT_MAP
 */
MODEL_DICT_MAP  CreateModelDict(DRV_DATA drv_value);

/** @brief replace substring
 *
 *  @param model_name model name of the device
 *  @return string
 */
string FixFileName(string model_name);

/** @brief short the length of model name
 *
 *  @param model_name model name of the device passed as reference variable
 *  @return void
 */
void ShortModelLength(string &model_name);

/** @brief convert the vector into the string
 *
 *  @param sub_class vector of sub classes
 *  @return string
 */
string SetSubClass(STRING_VECTOR sub_class);

/** @brief setting the path of the model.dat, drv & set the name of template files
 *
 *  @return DRV_DATA
 *
 */
DRV_DATA SetFileInfo();

/** @brief check the tech_class passed exsist or not
 *
 *  @param tech_class name of the tech_class
 *  @return int
 *
 */
int TechClassExsist(string tech_class);

/** @brief check the sub_class passed exsist or not
 *
 *  @param sub_class vector of sub classes
 *  @return unsigned char
 *
 */
unsigned char SubClassExsist(STRING_VECTOR sub_class);

/** @brief matches device with same tech class and subclass Name
 *
 *  @param model_dict map of models.dat
 *  @param sub_class vector of subclasses
 *  @param subclass  subclass name
 *  @param tech_class tech_class name
 *  @return STRING_VECTOR
 */
STRING_VECTOR CreateTechClassMatch(MODEL_DICT_MAP model_dict, STRING_VECTOR sub_class, string subclass, string tech_class);

/** @brief checks whether a family_class exsist
 *
 *  @param family_class family class name
 *  @return unsigned char
 *
 */
unsigned char FamilyClassExsist(string family_class);

/** @brief checks whether a model name exsist or not
 *
 *  @param model_name model name of the device
 *  @param model_dict map of models.dat
 *  @return unsigned char
 *
 */
unsigned char CheckModelNameExsist(string model_name, MODEL_DICT_MAP &model_dict);

/** @brief checks whether a family match exsist or not
 *
 *  @param model_dict map of model_dict
 *  @param family_class family class name
 *  @return STRING_VECTOR
 *
 */
STRING_VECTOR CreateFamilyClassMatch(MODEL_DICT_MAP model_dict, string family_class);

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
char CreateFamilyClassDrv(DRV_DATA drv_data, MODEL_DICT_MAP model_dict, string model_input, string model_input_tech_class, string model_input_sub_class);

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
char CreateTechClassDrv(DRV_DATA drv_data, MODEL_DICT_MAP model_dict, STRING_PAIR tech_class_model, string model_input, string model_input_tech_class, string model_input_sub_class, bool bHpijs);

#endif
