/*****************************************************************************\
    hppsfilter.c : HP PS filter for PostScript printers

    Copyright (c) 2011, HP Co.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the Hewlett-Packard nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PATENT INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

   Author: Yashwant Kumar Sahu
\*****************************************************************************/
#include "hppsfilter.h"


/*  save final output ps file: in cupsd.conf file  value #hpLogLevel 15  */
static int g_savepsfile = 0;

/*  final ps outfile file handle */
static FILE *g_fp_outdbgps = NULL;

/*  temp booklet ps outfile file handle */
static FILE *ptempbooklet_file = NULL;
static char temp_filename[FILE_NAME_SIZE] = {0};
static char booklet_filename[FILE_NAME_SIZE] = {0};
static char Nup_filename[FILE_NAME_SIZE] = {0};
extern void PS_Booklet(char *tempfile, char *bookletfile, char *nupfile,int order, int nup, char* pagesize, int bookletMaker);

/* get log level from the cups config file */
static void get_LogLevel ()
{
    FILE    *fp;
    char    str[258];
    char    *p;
    fp = fopen ("/etc/cups/cupsd.conf", "r");
    if (fp == NULL)
        return;
    while (!feof (fp))
    {
        if (!fgets (str, 256, fp))
        {
            break;
        }
        if ((p = strstr (str, "hpLogLevel")))
        {
            p += strlen ("hpLogLevel") + 1;
            g_savepsfile = atoi (p);
            break;
        }
    }
    fclose (fp);
}


/* create ps file for debugging purpose using job id */
static void open_dbg_outfile(char* szjob_id)
{
    g_fp_outdbgps = NULL;
    if (g_savepsfile & SAVE_PS_FILE)
    {
        char    sfile_name[FILE_NAME_SIZE] = {0};
        sprintf(sfile_name, "%s/%s_%s.out", DBG_TMP_FOLDER,DBG_PSFILE, szjob_id);
        g_fp_outdbgps= fopen(sfile_name, "w");
        chmod(sfile_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }
}

/* Writting into out file and debug file if debug level is set to 15 */
static int hpwrite (void *pBuffer, size_t size)
{
    int ndata_written = 0;
    if(g_fp_outdbgps)
    {
        ndata_written = fwrite (pBuffer, 1, size, g_fp_outdbgps);
    }

    ndata_written = write (STDOUT_FILENO, pBuffer, size);
    return ndata_written;
}

static void open_tempbookletfile(char *mode)
{
    ptempbooklet_file= fopen(temp_filename, mode);
    if(ptempbooklet_file == NULL)
    {
            fprintf(stderr, "ERROR: Unable to open temp file %s\n", temp_filename);
            return 1;
    }  
    chmod(temp_filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

}

static int Dump_tempbookletfile (void *pBuffer, size_t size)
{
    int  ndata_written = 0;

    ndata_written = fwrite (pBuffer, 1, size, ptempbooklet_file);

    return ndata_written;
}

/*
	This function would check what are the PJL attributes available in PPD.
 *      @return update ppd_values array values as 1 if attribute found in ppd and -1 if attribute not found in ppd

*/

static char * GetPPDValues()
{
    static char ppd_values[13] = {0},
                *bytes         = NULL;
    unsigned int count         = 0;
    
    /* Getting the PPD file                  */
    char *pppd_file = getenv("PPD");

    fprintf(stderr, "HP PS filter func = GetPPDValues : PPD NAME %s\n", pppd_file);    
    if(pppd_file != NULL)
    {
      /* Opening PPD File                      */
      FILE * file_pointer = fopen(pppd_file, "r");
      if(!file_pointer)
      {
         fprintf(stderr, "HP PS filter func = GetPPDValues : GET PPD VALUES FAILED - 1\n");
         return NULL;
      }
      else
      {
        /* Intializing variables                 */
        for(count = 0; count < 12; count ++)
           ppd_values[count] = -1;
        count = 0;
        fseek(file_pointer, 0L, SEEK_END );
        count = ftell(file_pointer);
        fseek(file_pointer, 0L, SEEK_SET );
        /* Alloacting memory for PPD content    */
        bytes = (char *)malloc(count);
        if(!bytes)
           fprintf(stderr, "HP PS filter func = GetPPDValues : GET PPD VALUES FAILED - 2\n");
        else
        {
          memset(bytes, 0, count);
          fread(bytes, count, 1, file_pointer);
          /* Checking HPDigit in PPD           */
          if((strstr(bytes, "HPDigit")) != NULL)
             ppd_values[0] = 1;
          /* Checking HPACCOUNTINGINFO in PPD  */
          if((strstr(bytes, HPACCOUNTINGINFO)) != NULL)
            ppd_values[1] = 1;
          /* Checking HPBOD in PPD             */
          if((strstr(bytes, HPBOD)) != NULL)
            ppd_values[2] = 1;
          /* Checking HPPJLECONOMODE in PPD    */
          if((strstr(bytes, HPPJLECONOMODE)) != NULL)
            ppd_values[3] = 1;
          /* Checking HPPJLECONOMODE2 in PPD   */
          if((strstr(bytes, HPPJLECONOMODE2)) != NULL)
            ppd_values[4] = 1;
          /* Checking HPPJLPRINTQUALITY in PPD */
          if((strstr(bytes, HPPJLPRINTQUALITY)) != NULL)
            ppd_values[5] = 1;
          /* Checking HPPJLOUTPUTMODE in PPD   */
          if((strstr(bytes, HPPJLOUTPUTMODE)) != NULL)
            ppd_values[6] = 1;
          /* Checking HPPJLDRYTIME in PPD      */
          if((strstr(bytes, HPPJLDRYTIME)) != NULL)
            ppd_values[7] = 1;
          /* Checking HPPJLSATURATION in PPD   */
          if((strstr(bytes, HPPJLSATURATION)) != NULL)
            ppd_values[8] = 1;
          /* Checking HPPJLINKBLEED in PPD     */
          if((strstr(bytes, HPPJLINKBLEED)) != NULL)
            ppd_values[9] = 1;
          /* Checking HPPJLCOLORASGRAY in PPD  */
          if((strstr(bytes, HPPJLCOLORASGRAY)) != NULL)
            ppd_values[10] = 1;
          /* Checking HPPJLTRUEBLACK in PPD    */
          if((strstr(bytes, HPPJLTRUEBLACK)) != NULL)
            ppd_values[11] = 1;
          ppd_values[12] = '\0';
          free(bytes);
          /* Closing the PPD file                */
          fclose(file_pointer);
          return ppd_values;
        }

      }
    }
     fprintf(stderr, "HP PS filter func = GetPPDValues : GET PPD VALUES FAILED -3\n");
     return NULL;
}

/*
	This function would check if '-' or ' ' is in string if it is than remove the such characters
 *      @return 1 for success -1 for Failure
 *      char input_slot[] = array which will be written

*/

static signed char RemoveCharacters(char input_slot[])
{
    unsigned int len          =  0,
             temp_len         =  0;
    char temp_input_slot[128] = {0};

    if((strlen(input_slot)) > 0)
    {
        while(input_slot[len] != '\0')
        {
            if (input_slot[len] != '-' && input_slot[len] != ' ')
            {
                temp_input_slot[temp_len] = input_slot[len];
                temp_len ++;
                len ++;
            }
            else
                len ++;
        }
        temp_input_slot[temp_len] = '\0';
        strcpy(input_slot, temp_input_slot);
        return 1;
    }
    fprintf(stderr, "HP PS filter func = RemoveCharacters : REMOVE CHARACTER FAILED\n");
    return -1;
}


/*
	This function would write PLJheader
 *      char **argument = argv
*/

static void WriteHeader(char **argument)
{
    char buffer[MAX_BUFFER] = {0};
    
    /*		Writing Header Information
    argument[1] = JOB ID , argument[2]= USERNAME,  argument[3] = TITLE		*/
    hpwrite("\x1b%-12345X@PJL JOBNAME=", strlen("\x1b%-12345X@PJL JOBNAME="));
    sprintf(buffer, "hplip_%s_%s\x0a", argument[2], argument[1]);
    hpwrite(buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "@PJL SET USERNAME=\"%s\"\x0a", argument[2]);
    hpwrite(buffer, strlen(buffer));
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "@PJL SET JOBNAME=\"%s\"\x0a", argument[3]);
    hpwrite(buffer, strlen(buffer));
    fprintf(stderr, "HP PS filter func = WriteHeader           : WRITING PJL HEADER INFO\n");
    return;
}

/*
	This function write secure printing command
 *      @return 1 as success 0 failure
 *      char is_secure_printing_old = 1 or 0 , int num_options = No. of options, cups_option_t *options= options

*/

static unsigned char WriteSecurePrinting(char is_secure_printing_old, int num_options, cups_option_t *options) 
{
  char sec_pin[MAX_BUFFER]   = {0},
      input_slot[MAX_BUFFER] = {0}; 
  const char *val            = NULL;

  if(is_secure_printing_old == -1)
  {
           /* This is for Old ppd secure printing support */
            if((val = cupsGetOption("HPFIDigit", num_options, options)) != NULL)
                sec_pin[0] = val[0]; 
            else 
                sec_pin[0] = '0'; 

            if((val = cupsGetOption("HPSEDigit", num_options, options)) != NULL)
                sec_pin[1] = val[0]; 
            else 
                sec_pin[1] = '0'; 

            if((val = cupsGetOption("HPTHDigit", num_options, options)) != NULL)
                sec_pin[2] = val[0]; 
            else 
                sec_pin[2] = '0'; 

            if((val = cupsGetOption("HPFTDigit", num_options, options)) != NULL)
                sec_pin[3] = val[0]; 
            else 
                sec_pin[3] = '0';  
            sec_pin[4] = '\0';
  }
  else 
  {
     if(is_secure_printing_old == 1)
       {
         /* This is for new ppd secure printing support */
         if((val = cupsGetOption("HPDigit", num_options, options)) != NULL)
         {
           strncpy(input_slot, val, strlen(val));
           if((strstr(input_slot, "Custom.")) != NULL)
           {
              char *sec_pin_value = strtok(input_slot, "Custom.");
              if(sec_pin_value == NULL)
                 return 0;
              strcpy(sec_pin, sec_pin_value);
              if((strlen(sec_pin)) != 4)
                 strcpy(sec_pin, "0000");
           }
           else
               strcpy(sec_pin, input_slot);  
         }
         else
           strcpy(sec_pin, "0000");
       }
     else
     {
      fprintf(stderr, "HP PS filter func = WriteSecurePrinting : SECURE PRINTING FAILED 1\n");
      return 0;
     }
  }
  if((strlen(sec_pin)) == 4)
  {

       /* Sending Secure Printing PJL Command */
       char sec_pin_command[MAX_BUFFER] = {0};
       sprintf(sec_pin_command, "%s%s%s", "@PJL SET HOLDKEY=", sec_pin, "\x0a");
       hpwrite("@PJL SET HOLD=ON\x0a", strlen("@PJL SET HOLD=ON\x0a"));
       hpwrite("@PJL SET HOLDTYPE=PRIVATE\x0a", strlen("@PJL SET HOLDTYPE=PRIVATE\x0a"));
       hpwrite(sec_pin_command, strlen(sec_pin_command));
       fprintf(stderr, "HP PS filter func = WriteSecurePrinting   : WRITING SECURE PRINTING INFO\n");
       return 1;
  }
  fprintf(stderr, "HP PS filter func = WriteSecurePrinting : SECURE PRINTING FAILED 2\n");
  return 0; 
}

/*
	This function write Job accounting information
 *      @return 1 as success 0 failure
 *      char **argument = argv , int num_options = No. of options, cups_option_t *options= options

*/

static unsigned char WriteJobAccounting(char **argument, int num_options, cups_option_t *options)
{
       char buffer[MAX_BUFFER]     = {0},
            name[100]              = {0},
            outstr[200]            = {0},
            input_slot[MAX_BUFFER] = {0};
       const char *val             = NULL;
       time_t t;
       struct tm *tmp;


       sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct1=%s\"\x0a", argument[2]);
       hpwrite(buffer, strlen(buffer));
       memset(buffer, 0, sizeof(buffer));
       gethostname(name, sizeof(name));
       if((strlen(name)) < 1)
       { 
         sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct2=%s\"\x0a", "unknown_system_name");
         hpwrite(buffer, strlen(buffer));
         memset(buffer, 0, sizeof(buffer));
       }
       else
       {
         sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct2=%s\"\x0a", name);
         hpwrite(buffer, strlen(buffer));
         memset(buffer, 0, sizeof(buffer));
       }
       memset(name, 0, sizeof(name));
       getdomainname(name,sizeof(name));
       if(strstr(name, "none") != NULL)
       { 
           sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct3=%s\"\x0a", "unknown_domain_name");
           hpwrite(buffer, strlen(buffer));
           memset(buffer, 0, sizeof(buffer));
       }
       else
       {
           sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct3=%s\"\x0a", name);
           hpwrite(buffer, strlen(buffer));
           memset(buffer, 0, sizeof(buffer));
       }
         t = time(NULL);
         tmp = localtime(&t);
         if(tmp == NULL)
         {
            fprintf(stderr, "HP PS filter func = WriteJobAccounting :  JOB ACCOUNTING INFO FAILED -1\n");
            return 0; 
         }   
          if (strftime(outstr, sizeof(outstr), "%Y%m%d%I%M%S", tmp) == 0)
          {  
            fprintf(stderr, "HP PS filter func = WriteJobAccounting : JOB ACCOUNTING INFO FAILED -2\n");
             return 0;
          }

           sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct4=%s\"\x0a", outstr);
           hpwrite(buffer, strlen(buffer));
           memset(buffer, 0, sizeof(buffer));
           if((val = cupsGetOption("job-uuid", num_options, options)) != NULL)
            {
                strncpy(input_slot, val, strlen(val));
                if(input_slot)
                {
                 sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct5=%s\"\x0a", input_slot);
                 hpwrite(buffer, strlen(buffer));
                 memset(buffer, 0, sizeof(buffer));
                 
                }       
            }
            else
            { 
              fprintf(stderr, "HP PS filter func = WriteJobAccounting : JOB ACCOUNTING INFO FAILED -3\n");
              return 0;
            }
            sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct6=%s\"\x0a", "HP Linux Printing");
            hpwrite(buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));

            sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct7=%s\"\x0a", "HP Linux Printing");
            hpwrite(buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));

            sprintf(buffer, "@PJL SET JOBATTR=\"JobAcct8=%s\"\x0a", argument[2]);
            hpwrite(buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
            fprintf(stderr, "HP PS filter func = WriteJobAccounting    : WRITING JOB ACCOUNTING INFO\n");
            return 1;
}

/*
	This function write Born-on-date information
 *      @return 1 as success 0 failure

*/

static unsigned char WriteBornOnDate()
{
      char * bod = "0400040101020D10100115";
      char name[100]          = {0},
           outstr[200]        = {0},
           final[200]         = {0},
           buffer[MAX_BUFFER] = {0};
      unsigned int count      = 0,
                   len        = 0;
      time_t t;
      struct tm *tmp;
      
      t = time(NULL);
      tmp = localtime(&t);
      if(tmp == NULL)
      {
         fprintf(stderr, "HP PS filter func = WriteBornOnDate : BORN-ON-DATE FAILED -1\n");
         return 0;   
      }
      if (strftime(outstr, sizeof(outstr), "%Y%m%d%I%M%S", tmp) == 0)
      {
            fprintf(stderr, "HP PS filter func = WriteBornOnDate : BORN-ON-DATE FAILED -2\n");
          return 0;
      }
   
      for(count = 0; count < strlen(outstr) ; count++, len += 2)
          sprintf(&final[len], "%x", outstr[count]);
          
      sprintf(buffer, "@PJL DMINFO ASCIIHEX=\"%s%s\"\x0A", bod, final);
      hpwrite(buffer, strlen(buffer));
    fprintf(stderr, "HP PS filter func = WriteBornOnDate       : WRITING BORN-ON-DATE\n");
    return 1;
} 

/*
	This function write Economode information
   *    char **argument = argv

*/

static void WriteECONOMODE(char **argument)
{

   if((strstr(argument[5], HPPJLECONOMODE)) != NULL && (strstr(argument[5], "noHPPJLEconoMode")) == NULL)
     {
        hpwrite("@PJL SET ECONOMODE=ON\x0a", strlen("@PJL SET ECONOMODE=ON\x0a"));
        hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
        hpwrite("@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET BITSPERPIXEL=2\x0a"));
     }
    else
     {
        hpwrite("@PJL SET ECONOMODE=OFF\x0a", strlen("@PJL SET ECONOMODE=OFF\x0a"));
        hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
        hpwrite("@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET BITSPERPIXEL=2\x0a"));
     }
     fprintf(stderr, "HP PS filter func = WriteECONOMODE        : WRITNING ECONOMODE\n");
     return;
}

/*
	This function write Economode2 information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteECONOMODE2(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLECONOMODE2, num_options, options)) != NULL)
  {
    strncpy(input_slot, val, strlen(val));
    if(input_slot)
    {
       if((RemoveCharacters(input_slot)) == -1)
       {
           fprintf(stderr, "HP PS filter func = WriteECONOMODE2 : ECONOMODE FAILED\n");
           return;
       }
       if((strcmp(input_slot, "yes")) == 0 || (strcmp(input_slot, "on")) == 0 )
       {
          hpwrite("@PJL SET ECONOMODE=ON\x0a", strlen("@PJL SET ECONOMODE=ON\x0a"));
          hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
          hpwrite("@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET BITSPERPIXEL=2\x0a"));
       }
       else
       {
          hpwrite("@PJL SET ECONOMODE=OFF\x0a", strlen("@PJL SET ECONOMODE=OFF\x0a"));
          hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
          hpwrite("@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET BITSPERPIXEL=1\x0a"));
       }
    }
   }
   else
   {
      hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
      hpwrite("@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET BITSPERPIXEL=1\x0a"));
   }
   fprintf(stderr, "HP PS filter func = WriteECONOMODE2       : WRITING ECONOMODE\n"); 
   return;

} 

/*
	This function write PrintQuality information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLPRINTQUALITY(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLPRINTQUALITY, num_options, options)) != NULL)
  {
   strncpy(input_slot, val, strlen(val));
   if(input_slot)
   {
       if((RemoveCharacters(input_slot)) == -1)
       {    
           fprintf(stderr, "HP PS filter func = WriteHPPJLPRINTQUALITY : PRINT QUALITY FAILED\n");
           return;
       }
      if((strcmp(input_slot, "ProRes1200"))  == 0)
          hpwrite("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=1\x0a", strlen("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=1\x0a"));
      else if((strcmp(input_slot, "600dpi")) == 0)
          hpwrite("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=1\x0a", strlen("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=1\x0a"));
      else
          hpwrite("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=2\x0a"));
    }
  }
  else
     hpwrite("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=2\x0a", strlen("@PJL SET RESOLUTION=600\x0a@PJL SET BITSPERPIXEL=2\x0a"));
  fprintf(stderr, "HP PS filter func = WriteHPPJLPRINTQUALITY: WRITING PRINT QUALITY INFO\n");
  return;
}

/*
	This function write Output Mode information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLOUTPUTMODE(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLOUTPUTMODE, num_options, options)) != NULL)
  {
     strncpy(input_slot, val, strlen(val));
     if(input_slot)
     {
       if((RemoveCharacters(input_slot)) == -1)
       {    
           fprintf(stderr, "HP PS filter func = WriteHPPJLOUTPUTMODE : OUTPUTMODE FAILED\n");
           return;
       }
       if((strcmp(input_slot, "GeneralOffice"))     == 0)
           hpwrite("@PJL SET PRINTQUALITY=DRAFT\x0a", strlen("@PJL SET PRINTQUALITY=DRAFT\x0a"));
       else if((strcmp(input_slot, "Professional")) == 0)
                hpwrite("@PJL SET PRINTQUALITY=NORMAL\x0a", strlen("@PJL SET PRINTQUALITY=NORMAL\x0a"));
       else if((strcmp(input_slot, "Presentation")) == 0)
                hpwrite("@PJL SET PRINTQUALITY=BEST\x0a", strlen("@PJL SET PRINTQUALITY=BEST\x0a"));
       else if((strcmp(input_slot, "MaximumDPI"))   == 0)
                hpwrite("@PJL SET PRINTQUALITY=MAX\x0a", strlen("@PJL SET PRINTQUALITY=MAX\x0a"));
       else
                hpwrite("@PJL SET PRINTQUALITY=NORMAL\x0a", strlen("@PJL SET PRINTQUALITY=NORMAL\x0a"));
     }
   }
   else
      hpwrite("@PJL SET PRINTQUALITY=NORMAL\x0a", strlen("@PJL SET PRINTQUALITY=NORMAL\x0a"));
   fprintf(stderr, "HP PS filter func = WriteHPPJLOUTPUTMODE  : WRITING OUTPUTMODE INFO\n");

   return;
}

/*
	This function write DryTime information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLDRYTIME(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLDRYTIME, num_options, options)) != NULL)
  {
     strncpy(input_slot, val, strlen(val));
     if(input_slot)
     {
       if((RemoveCharacters(input_slot)) == -1)
       {
           fprintf(stderr, "HP PS filter func = WriteHPPJLDRYTIME : DRYTIME FAILED\n");
          return;
       }
       if((strcmp(input_slot, "Medium"))    == 0)
           hpwrite("@PJL SET DRYTIME=4\x0a", strlen("@PJL SET DRYTIME=4\x0a"));
       else if((strcmp(input_slot, "Long")) == 0)
           hpwrite("@PJL SET DRYTIME=7\x0a", strlen("@PJL SET DRYTIME=7\x0a"));
       else
           hpwrite("@PJL SET DRYTIME=0\x0a", strlen("@PJL SET DRYTIME=0\x0a"));
     }
   }
   else
      hpwrite("@PJL SET DRYTIME=0\x0a", strlen("@PJL SET DRYTIME=0\x0a")); 
   fprintf(stderr, "HP PS filter func = WriteHPPJLDRYTIME     : WRITING DRYTIME INFO\n");
   return;
}

/*
	This function write Saturation information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLSATURATION(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLSATURATION, num_options, options)) != NULL)
  {
      strncpy(input_slot, val, strlen(val));
      if(input_slot)
      {
         if((RemoveCharacters(input_slot)) == -1)
         {
           fprintf(stderr, "HP PS filter func = WriteHPPJLSATURATION : SATURATION FAILED\n");
            return;
         }
         if((strcmp(input_slot, "-2"))      == 0)
            hpwrite("@PJL SET SATURATION=0\x0a", strlen("@PJL SET SATURATION=0\x0a"));
         else if((strcmp(input_slot, "-1")) == 0)
            hpwrite("@PJL SET SATURATION=2\x0a", strlen("@PJL SET SATURATION=2\x0a"));
         else if((strcmp(input_slot, "0"))  == 0)
            hpwrite("@PJL SET SATURATION=4\x0a", strlen("@PJL SET SATURATION=4\x0a"));
         else if((strcmp(input_slot, "1"))  == 0)
            hpwrite("@PJL SET SATURATION=6\x0a", strlen("@PJL SET SATURATION=6\x0a"));
         else if((strcmp(input_slot, "2"))  == 0)
            hpwrite("@PJL SET SATURATION=8\x0a", strlen("@PJL SET SATURATION=8\x0a"));
         else
            hpwrite("@PJL SET SATURATION=4\x0a", strlen("@PJL SET SATURATION=4\x0a"));
        }
   }
   else
      hpwrite("@PJL SET SATURATION=4\x0a", strlen("@PJL SET SATURATION=4\x0a")); 
   fprintf(stderr, "HP PS filter func = WriteHPPJLSATURATION  : WRITING SATURATION INFO\n");
   return;
}

/*
	This function write InkBleed information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLINKBLEED(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLINKBLEED, num_options, options)) != NULL)
  {
     strncpy(input_slot, val, strlen(val));
     if(input_slot)
     {
       if((RemoveCharacters(input_slot)) == -1)
       {
           fprintf(stderr, "HP PS filter func = WriteHPPJLINKBLEED : INKBLEED FAILED\n");
          return;
       }
       if((strcmp(input_slot, "-2")) == 0 || (strcmp(input_slot, "Default")) == 0)
           hpwrite("@PJL SET INKBLEED=0\x0a", strlen("@PJL SET INKBLEED=0\x0a"));
       else if((strcmp(input_slot, "-1")) == 0)
           hpwrite("@PJL SET INKBLEED=2\x0a", strlen("@PJL SET INKBLEED=2\x0a"));
       else if((strcmp(input_slot, "1")) == 0)
           hpwrite("@PJL SET INKBLEED=6\x0a", strlen("@PJL SET INKBLEED=6\x0a"));
       else if((strcmp(input_slot, "2")) == 0)
           hpwrite("@PJL SET INKBLEED=8\x0a", strlen("@PJL SET INKBLEED=8\x0a"));
       else if((strcmp(input_slot, "Least")) == 0)
           hpwrite("@PJL SET INKBLEED=7\x0a", strlen("@PJL SET INKBLEED=7\x0a"));
       else
           hpwrite("@PJL SET INKBLEED=4\x0a", strlen("@PJL SET INKBLEED=4\x0a"));
      }  
   }
   else
      hpwrite("@PJL SET INKBLEED=4\x0a", strlen("@PJL SET INKBLEED=4\x0a"));
   fprintf(stderr, "HP PS filter func = WriteHPPJLINKBLEED    : WRITING INKBLEED INFO\n");
   return;
}

/*
	This function write ColorAsGray information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLCOLORASGRAY(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLCOLORASGRAY, num_options, options)) != NULL)
  {
     strncpy(input_slot, val, strlen(val));
     if(input_slot)
     {
        if((RemoveCharacters(input_slot)) == -1)
        {
           fprintf(stderr, "HP PS filter func = WriteHPPJLCOLORASGRAY : COLORASGRAY FAILED\n");
           return;
        }
        if((strcmp(input_slot, "HighQuality")) == 0 || (strcmp(input_slot, "Default")) == 0)
        {
            hpwrite("@PJL SET GRAYSCALE=COMPOSITE\x0a", strlen("@PJL SET GRAYSCALE=COMPOSITE\x0a"));
            hpwrite("@PJL SET PLANESINUSE=1\x0a", strlen("@PJL SET PLANESINUSE=1\x0a"));
        }
        else if((strcmp(input_slot, "BlackInkOnly")) == 0)
        {
            hpwrite("@PJL SET GRAYSCALE=BLACKONLY\x0a", strlen("@PJL SET GRAYSCALE=BLACKONLY\x0a"));
            hpwrite("@PJL SET PLANESINUSE=1\x0a", strlen("@PJL SET PLANESINUSE=1\x0a"));
        }
        else if((strcmp(input_slot, "yes")) == 0 || (strcmp(input_slot, "on")) == 0)
        {
            hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
            hpwrite("@PJL SET BITSPERPIXEL=8\x0a", strlen("@PJL SET BITSPERPIXEL=8\x0a"));
            hpwrite("@PJL SET GRAYSCALE=COMPOSITE\x0a", strlen("@PJL SET GRAYSCALE=COMPOSITE\x0a"));
        }
        else if((strcmp(input_slot, "no")) == 0)
        {
            hpwrite("@PJL SET RESOLUTION=600\x0a", strlen("@PJL SET RESOLUTION=600\x0a"));
            hpwrite("@PJL SET BITSPERPIXEL=8\x0a", strlen("@PJL SET BITSPERPIXEL=8\x0a"));
        }
        else if((strcmp(input_slot, "Plain")) == 0 || (strcmp(input_slot, "True")) == 0)
            hpwrite("@PJL SET PLANESINUSE=1\x0a", strlen("@PJL SET PLANESINUSE=1\x0a"));
        else
            hpwrite("@PJL SET GRAYSCALE=OFF\x0a", strlen("@PJL SET GRAYSCALE=OFF\x0a"));
      }
   }
   else 
      hpwrite("@PJL SET GRAYSCALE=OFF\x0a", strlen("@PJL SET GRAYSCALE=OFF\x0a"));
   fprintf(stderr, "HP PS filter func = WriteHPPJLCOLORASGRAY : WRITING COLORASGRAY INFO\n");
   return;
}

/*
	This function write TrueBlack information
 *      int num_options = No. of options, cups_option_t *options= options

*/

static void  WriteHPPJLTRUEBLACK(int num_options, cups_option_t *options)
{
  char input_slot[MAX_BUFFER] = {0};
  const char *val             = NULL;

  if((val = cupsGetOption(HPPJLTRUEBLACK, num_options, options)) != NULL)
  {
      strncpy(input_slot, val, strlen(val));
      if(input_slot)
      {
        if((RemoveCharacters(input_slot)) == -1)
        {
           fprintf(stderr, "HP PS filter func = WriteHPPJLTRUEBLACK : TRUEBLACK FAILED\n");
           return;
        }
        if((strcmp(input_slot, "HighQuality")) == 0)
           hpwrite("@PJL SET GRAYSCALE=COMPOSITE\x0a", strlen("@PJL SET GRAYSCALE=COMPOSITE\x0a"));
        else
           hpwrite("@PJL SET GRAYSCALE=BLACKONLY\x0a", strlen("@PJL SET GRAYSCALE=BLACKONLY\x0a"));
      }
  }
  else
     hpwrite("@PJL SET GRAYSCALE=BLACKONLY\x0a", strlen("@PJL SET GRAYSCALE=BLACKONLY\x0a"));
  fprintf(stderr, "HP PS filter func = WriteHPPJLTRUEBLACK   : WRITING TRUEBLACK INFO\n");
  return;
}


int main (int argc, char **argv)
{
   cups_file_t   *fp_input     =  NULL;			/* input file: stdin or physical file */
   cups_file_t   *fp_bookletinput     =  NULL;			/* input file: booklet file */
   cups_option_t *options      =  NULL;
   char line[LINE_SIZE]        = {0},
        job_id[MAX_BUFFER]     = {0},
        user[MAX_BUFFER]       = {0},
        title[MAX_BUFFER]      = {0},
        copies[MAX_BUFFER]     = {0},    
        input_slot[MAX_BUFFER] = {0};
   unsigned int count          = 0;  
   char *ppd_values            = NULL;
   int order = 1;// default value
   int nup = 2;// default value
   char newline[64] = {0};  // where we will put a copy of the input options string
   char *subString = NULL; // pagesize value from input
   int booklet_enabled=0;// default for testing
   int bookletMaker=0;

    get_LogLevel();
    setbuf (stderr, NULL);

    fprintf (stderr, "HP PS filter starting : %s \n", *argv);

    /* Logging cups filter arguments */
    for(count = 0; count < argc; count++)
        fprintf (stderr, "DEBUG: hppsfilter: argv[%d] = %s\n", count, argv[count]);

    /* Logging debug information and creating outfile for debug */
    if (g_savepsfile & SAVE_PS_FILE)
        open_dbg_outfile(argv[1]); /* opening ps debug file */

    /* Check command-line...  */
    if (argc < 6 || argc > 7)
    {
        fputs("ERROR: hppsfilter job-id user title copies options [file]\n", stderr);
        return (1);
    }
    /* If we have 7 arguments, print the file named on the command-line.
    * Otherwise, send stdin instead...  */
    if (argc == 6)
        fp_input = cupsFileStdin();
    else
    {
        /* Try to open the print file...    */
        if ((fp_input = cupsFileOpen(argv[6], "r")) == NULL)
        {
            fprintf(stderr, "ERROR: Unable to open print file \"%s\"", argv[6]);
            return 1;
        }
    }

    int num_options = cupsParseOptions(argv[5], 0, &options);

    if(options)
    {
         WriteHeader(argv);
         ppd_values = GetPPDValues();
         if((strstr(argv[5], "HPPinPrnt")) != NULL && (strstr(argv[5], "noHPPinPrnt"))== NULL)
             WriteSecurePrinting(ppd_values[0], num_options, options);
         if( ((strstr(argv[5], "HPBookletFilter")) != NULL) && ((strstr(argv[5], "fitplot")) != NULL) && ((strstr(argv[5], "Duplex=DuplexTumble")) != NULL) && ((strstr(argv[5], "number-up=1")) != NULL) )
         {
             booklet_enabled = 1;
             order = 1;
             if((subString = strstr(argv[5], "HPBookletPageSize")) != NULL)
             {
                 strncpy(newline, subString,64);
                 subString = strtok(newline,"="); // find the first double quote
                 subString = strtok(NULL," ");   // find the second double quote
             }
             else
                 subString = "letter";   
         }
         else
             booklet_enabled = 0;

         if( ((strstr(argv[5], "FoldStitch")) != NULL))
         {
             bookletMaker=1;
         }

         if(ppd_values[1] == 1)
             WriteJobAccounting(argv, num_options, options);
         if(ppd_values[2] == 1)
             WriteBornOnDate();
         if(ppd_values[3] == 1)
             WriteECONOMODE(argv);
         if(ppd_values[4] == 1)
             WriteECONOMODE2(num_options, options);
         if(ppd_values[5] == 1)
             WriteHPPJLPRINTQUALITY(num_options, options);
         if(ppd_values[6] == 1)
             WriteHPPJLOUTPUTMODE(num_options, options);
         if(ppd_values[7] == 1)
             WriteHPPJLDRYTIME(num_options, options);
         if(ppd_values[8] == 1)
             WriteHPPJLSATURATION(num_options, options);
         if(ppd_values[9] == 1)
             WriteHPPJLINKBLEED(num_options, options);
         if(ppd_values[10] == 1)
             WriteHPPJLCOLORASGRAY(num_options, options);
         if(ppd_values[11] == 1)
             WriteHPPJLTRUEBLACK(num_options, options);
         hpwrite("@PJL ENTER LANGUAGE=POSTSCRIPT\x0a", strlen("@PJL ENTER LANGUAGE=POSTSCRIPT\x0a")); 
    }
    unsigned int numBytes = 0;

    /* Perform the below when booklet option is enabled */
    if(booklet_enabled)
    {
        /* 1. dump  the contents of the input file into temp file */
        sprintf(booklet_filename, "/tmp/%s.ps","booklet");
        sprintf(temp_filename, "/tmp/%s.ps","temp");
        sprintf(Nup_filename, "/tmp/%s.ps","NUP");
        open_tempbookletfile("w");
	while( (numBytes = cupsFileGetLine(fp_input, line, sizeof(line))) > 0)
            Dump_tempbookletfile (line, numBytes);
        fclose(ptempbooklet_file);

        /* 2. Perform the booklet operation on the PS file */
        PS_Booklet(temp_filename,booklet_filename,Nup_filename,order,nup,subString,bookletMaker);

        /* 3. Dump the modified file into the output.    */
        numBytes = 0;
        if ((fp_bookletinput = cupsFileOpen(Nup_filename, "r")) == NULL)
        {
            fprintf(stderr, "ERROR: Unable to open Nup_filename print file \"%s\"", Nup_filename);
            return 1;
        }
        while ( (numBytes = cupsFileGetLine(fp_bookletinput, line, sizeof(line))) > 0)
            hpwrite (line, numBytes);
        cupsFileClose (fp_bookletinput);

        /* 4. Unlink function to remove the temp temporary files created */
        if( (unlink(booklet_filename)) == -1)
        {
            fprintf(stderr, "ERROR: Unable to remove temporary files in /tmp dir \"%s\" ",booklet_filename);
            return 1;
        }
        if( (unlink(temp_filename)) == -1)
        {
            fprintf(stderr, "ERROR: Unable to remove temporary files in /tmp dir \"%s\"  ",temp_filename);
            return 1;
        }
        if( (unlink(Nup_filename)) == -1)
        {
            fprintf(stderr, "ERROR: Unable to remove temporary files in /tmp dir \"%s\" ",Nup_filename);
            return 1;
        }
        booklet_enabled = 0;
        bookletMaker=0;
    }
    else
    {
        while ( (numBytes = cupsFileGetLine(fp_input, line, sizeof(line))) > 0)
            hpwrite (line, numBytes);
    }

    /* WRITING END OF JOB */
    hpwrite("\x1b%-12345X@PJL EOJ\x0a\x1b%-12345X", strlen("\x1b%-12345X@PJL EOJ\x0a\x1b%-12345X"));
    if ( (argc == 7) && (fp_input != NULL) )
        cupsFileClose (fp_input);

    if(g_fp_outdbgps != NULL)
    {
        fclose (g_fp_outdbgps);
    }
    fprintf (stderr, "HP PS filter ENDS\n");
    return 0;
}
