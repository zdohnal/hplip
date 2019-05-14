#ifndef _NORMALIZE_H
#define _NORMALIZE_H

#include"Common.h"

#define MAX_TECH_CLASS_PDL                 67
#define MAX_TECH_CLASS                     76
#define MAX_FAMILY_CLASS                   53
#define MAX_SUB_CLASS                      30


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
#define INKJET	                           "Inkjet"
#define WIRELESS	                   "Wireless"
#define TANK		                   "Tank"

#define COLOR_REPLACEMENT		   "CL"
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
#define INKJET_REPLACEMENT        	   "IJ"
#define WIRELESS_REPLACEMENT        	   "WL"
#define TANK_REPLACEMENT                   "TK"

#define MAX_SHORTENING_REPLACEMENTS        20

/** @brief create pair for tech classes
 *  @return STRING_PAIR
 *
 */
STRING_PAIR TechClassPdls();

/** @brief create pair for tech classes
 *
 *  @return string
 *
 */
string ReplaceStrChar(string model_name, const string &replace, char value);

/** @brief Capitalize the model name character
 *  @param model_dict  map of models.dat
 *  @return void
 *
 */
void Capitalize(string &model_name);

/** @brief Normal the model name
 *  @param model_dict  map of models.dat
 *  @return void
 *
 */
void  NoramlizeModelName(MODEL_DICT_MAP &model_dict );


void NoramlizeModeVariantlName(string &model_name);

#endif
