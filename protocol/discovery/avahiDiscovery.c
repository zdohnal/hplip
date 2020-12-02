/*
 *  This file is derived from avahi_client_browse.c, part of avahi.
 *
 *  avahi is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 2.1 of the
 *  License, or (at your option) any later version.
 *
 *  avahi is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with avahi; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA.
 */

#include "avahiDiscovery.h"
#include <errno.h>
#include <dbus/dbus.h>

DBusConnection *conn;

char* aUriBuf=NULL;
char ipAddressBuff[MAX_IP_ADDR_LEN]={'\0'};
static int aBytesRead = 0;
static AvahiSimplePoll *aSimplePoll = NULL;
static int aMemAllocated = 0;
static int aAllForNow = 0, aResolving = 0;

/*
This function will check all the services are resolved or not, if resolved
it will call avahi_simple_poll_quit
*/
static void check_terminate()
{
    assert(aAllForNow >= 0);
    assert(aResolving >= 0);
    if (aAllForNow <= 0 && aResolving <= 0)
    {
        avahi_simple_poll_quit(aSimplePoll);
    }
}

/*
This function will fill the dictionary arguments for the dbus function call
*/
static void addDictWithStringValue(DBusMessageIter *iter,
                    const char *key, const char *str)
{
    DBusMessageIter dict, entry, value;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
            DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
            DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_VARIANT_AS_STRING
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);
    dbus_message_iter_open_container(&dict, DBUS_TYPE_DICT_ENTRY,
                                NULL, &entry);

    dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);

    dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT,
                    DBUS_TYPE_STRING_AS_STRING, &value);
    dbus_message_iter_append_basic(&value, DBUS_TYPE_STRING, &str);
    dbus_message_iter_close_container(&entry, &value);

    dbus_message_iter_close_container(&dict, &entry);
    dbus_message_iter_close_container(iter, &dict);
}
/*
This function will form the empty dictionary for the dbus function call
*/
static void addEmptyStringDict(DBusMessageIter *iter)
{
    DBusMessageIter dict;

    dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY,
            DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING
            DBUS_TYPE_STRING_AS_STRING DBUS_TYPE_STRING_AS_STRING
            DBUS_DICT_ENTRY_END_CHAR_AS_STRING, &dict);

    dbus_message_iter_close_container(iter, &dict);
}
/*
This function will fill the arguments for the dbus function call
*/
static void addArgumentsForAuthentication(DBusConnection *conn, DBusMessageIter *iter)
{
    const char *busname = dbus_bus_get_unique_name(conn);
    const char *kind = SYSTEM_BUS_NAME;
    const char *action = ACTION_ID;
    
    const char *cancel = "";
    dbus_uint32_t flags = 1;
    DBusMessageIter subject;

    dbus_message_iter_open_container(iter, DBUS_TYPE_STRUCT,
                            NULL, &subject);
    dbus_message_iter_append_basic(&subject, DBUS_TYPE_STRING, &kind);
    addDictWithStringValue(&subject, "name", busname);
    dbus_message_iter_close_container(iter, &subject);

    dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &action);
    addEmptyStringDict(iter);
    dbus_message_iter_append_basic(iter, DBUS_TYPE_UINT32, &flags);
    dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &cancel);
}
/*
This function will start the avahi service if the process authorized by polkit
*/
static bool systemdStartAvahiService()
{
    DBusMessage *msg, *response;
    DBusError err;

    dbus_error_init(&err);

    const char *aAvahiUnitName = AVAHI_SERVICE_NAME;
    const char *aAvahiUnitMode = AVHHI_SERVICE_MODE_REPLACE;

    msg = dbus_message_new_method_call(SYSTEMD_DBUS_NAME, SYSTEMD_DBUS_PATH, SYSTEMD_DBUS_INTF, SYSTEMD_START_SERVICE_METHOD);
    if(!msg) 
    {
        BUG("failed to create dbus message\n");
        return false;
    }
    dbus_message_append_args(msg, DBUS_TYPE_STRING, &aAvahiUnitName, DBUS_TYPE_STRING, &aAvahiUnitMode, DBUS_TYPE_INVALID);

    response = dbus_connection_send_with_reply_and_block(conn, msg,SYSTEMD_SERVICE_TIMEOUT, &err);
    
    if(dbus_error_is_set(&err)) 
    {
        dbus_message_unref(msg);
        BUG("failed to start service : %s\n", err.message);
        return false;
    }

    dbus_message_unref(msg);
    dbus_message_unref(response);
    return true;
}
/*
This function will involk polkit based authorization using dbus
*/
static bool checkAuthorizationForAvahiService()
{
    DBusMessage *msg, *reply;
    DBusMessageIter iter;
    DBusError err;
    bool authorized = false;
    conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
    if (!conn) 
    {
        BUG("Can't get on system bus");
        return authorized;
    }
    msg = dbus_message_new_method_call(POLKIT_AUTH_DBUS, POLKIT_AUTH_PATH,
                POLKIT_AUTH_INTF, POLKIT_AUTH_METHOD_CALL);
    if (!msg) 
    {
        BUG("Can't allocate new method call\n");
        return authorized;
    }
    dbus_message_iter_init_append(msg, &iter);
    addArgumentsForAuthentication(conn, &iter);
    dbus_error_init(&err);
    reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_TIMEOUT_INFINITE, &err);
    dbus_message_unref(msg);
    if (!reply) 
    {
        if (dbus_error_is_set(&err)) 
        {
            BUG("%s\n", err.message);
            dbus_error_free(&err);
        } 
        else
        {
            BUG("Can't check authorization\n");
        }
        return authorized;
    }
    //verify the response with dbus response signature
    if (dbus_message_has_signature(reply, "(bba{ss})")) 
    {
        DBusMessageIter result; 
        dbus_bool_t respAuthorized;
        //iterate the dbus response
        dbus_message_iter_init(reply, &iter);               
        dbus_message_iter_recurse(&iter, &result);
        //get the authorization status        
        dbus_message_iter_get_basic(&result, &respAuthorized);
        DBG("Authorized %d \n", respAuthorized);
        if(respAuthorized)
        {
            authorized = true;
        }
    }
    else
    {
        BUG("dbus response signature mismatch\n");    
    }
    dbus_message_unref(reply);
    return authorized;
}

/*
This function will get the value for the given key if found in the string list.
Returns true if found the value ,otherwise returns false
Parameters:
          iStrList - INPUT - Avahi device (scanner) string list.
          ikey - INPUT - Key to find in the stringlist.
          oKeyValue - OUTPUT - Value of the given key.
          aMdlStrLen - OUTPUT - size of oKeyValue.
Note:
     Possible value in iStrList (avahi response for a device)
     "duplex=F" "is=platen" "cs=binary,color,grayscale" "rs=eSCL" "representation=images/printer.png" 
     "UUID=6fcbd42d-b117-5352-ab4c-35acfabefc34" "note=" "adminurl=http://HPE4E749F6A0F7.local." 
     "ty=HP DeskJet 2700 series" "pdl=application/octet-stream,application/pdf,image/jpeg" 
     "mopria-certified-scan=1.2" "vers=2.63" "txtvers=1"
*/

static bool getHPScannerModel(AvahiStringList *iStrList, const char *ikey,char **oKeyValue,size_t *aMdlStrLen, size_t *oOffset)
{
    AvahiStringList *aStrList = NULL, *bStrList = NULL;
    bool aValueFound = false;
    char *aKey = NULL,*aMfgValue = NULL;
    aStrList = avahi_string_list_find(iStrList, ikey); 
    /*
    	aStrList will be Null ,if given key is not found,
        avahi_string_list_get_pair will return zero if key found,
        also need to process the response form HP scanner only.
    */
     
    if ( ( aStrList != NULL ) && ( avahi_string_list_get_pair(aStrList, &aKey, oKeyValue, aMdlStrLen) == 0 ) && ( *oKeyValue != NULL ) ) 
    {
      //check ty field of DNS record has HP
      //otherwise look at mfg field of DNS record
      if ( strncmp(MFG_HP, *oKeyValue, MFG_HP_LEN) == 0 ) 
      {
          aValueFound = true;
          //skip the "HP " from the ty field of the DNS record
          //"ty=HP DeskJet 2700 series"
          *oOffset = HP_SKIP_MFG_NAME_SIZE;
      }
      else
      {
          //ty doesn't have the HP in the ty field
          //check the mfg field of the DNS record
          bStrList = avahi_string_list_find(iStrList, MFG_NAME);
          if (bStrList)
	  {
              aMfgValue = avahi_string_list_get_text(bStrList);
              DBG("aMfgValue : %s\n",aMfgValue);
          }
          // aMfgValue (DNS record) would be like "mfg=HP"
          // to locate HP in aMfgValue , need to increment the string position to +4
          if( (aMfgValue != NULL) && (strncmp(MFG_HP, aMfgValue + 4, MFG_HP_LEN) == 0) )
          {
             aValueFound = true;
             //Since ty field doesn't have the HP ,we can take the field as model name
             // and no need to skip the HP from the ty value.
             *oOffset = 0;
          } 
      }
      if( aValueFound )
      {
          size_t aIndex = 0;
          for(aIndex = 0; aIndex<(*aMdlStrLen); aIndex++)
          {
             if(isspace((*oKeyValue)[aIndex]))
             {
                 (*oKeyValue)[aIndex] = '_';
             }
             (*oKeyValue)[aIndex] = tolower((*oKeyValue)[aIndex]);
          }
          DBG("oKeyValue is %s\n", *oKeyValue);
      }     
    }    
    if( aKey != NULL )
        avahi_free( (void *)aKey );
    return aValueFound;  
}

/*
This function will gets called whenever a service has been resolved successfully or timed out
*/
static void resolve_callback(
    AvahiServiceResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const char *type,
    const char *domain,
    const char *host_name,
    const AvahiAddress *address,
    uint16_t port,
    AvahiStringList *txt,
    AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void* userdata) {
    assert(r);
    switch (event) {
        case AVAHI_RESOLVER_FAILURE:
            BUG( "(Resolver) Failed to resolve service '%s' of type '%s' in domain '%s': %s\n", name, type, domain, avahi_strerror(avahi_client_errno(avahi_service_resolver_get_client(r))));
            break;

        case AVAHI_RESOLVER_FOUND: {
            char aIPAddress[AVAHI_ADDRESS_STR_MAX]={'\0'};
            char *aMdlStr = NULL;
            size_t aMdlStrLen = 0 ;
            size_t aOffset = 0;
            DBG( "Service '%s' of type '%s' in domain '%s':\n", name, type, domain);
            avahi_address_snprint(aIPAddress, AVAHI_ADDRESS_STR_MAX, address);
            DBG("avahi_address_snprint : \n %s \n",aIPAddress);
            if( getHPScannerModel(txt,TYPE_NAME,&aMdlStr,&aMdlStrLen,&aOffset) == true ) 
            {
                DBG("aMdlStr name is %s \n",aMdlStr); 
                DBG("aMdlStrLen is %zu \n",aMdlStrLen);
                char aTempUri[MAX_URI_LEN] = {'\0'};
                snprintf( aTempUri, MAX_URI_LEN, "hp:/net/%s?ip=%s&queue=false", aMdlStr+aOffset, aIPAddress);
                if(aUriBuf == NULL)
                {
                    aUriBuf = (char *)calloc(HP_MAX_SCAN_BUFF,sizeof(char));
                    if(aUriBuf == NULL)
                    {
               	        BUG("Unable to alloacate the memeory\n");
                        exit(0);
                    }
                    aMemAllocated = HP_MAX_SCAN_BUFF ;                        
                }
                //Check whether buffer has enough space to add new URI and check for duplicate URIs.
                if( !strstr(aUriBuf, aTempUri) )
                {   
                    if ( (aBytesRead + MAX_URI_LEN) > aMemAllocated )
                    {
                	aUriBuf = realloc(aUriBuf, ( MAX_URI_LEN * sizeof(char) ) );
                        aMemAllocated = aMemAllocated + HP_EXT_SCAN_BUFF;
                    }
                    aBytesRead += snprintf(aUriBuf + aBytesRead, MAX_URI_LEN,"%s;", aTempUri);
                }
            }
            if( aMdlStr != NULL )
            	avahi_free( (void *)aMdlStr );               
            break;
        }
    }
    //avahi_service_resolver_free(r);
    assert(aResolving > 0);
    aResolving--;
    check_terminate();
}
/* Called whenever a new services becomes available on the LAN or is removed from the LAN */
static void browse_callback(
    AvahiServiceBrowser *b,
    AvahiIfIndex interface,
    AvahiProtocol protocol,
    AvahiBrowserEvent event,
    const char *name,
    const char *type,
    const char *domain,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    void* userdata) {

    AvahiClient *c = (AvahiClient *)userdata;
    assert(b);
    switch (event) {

        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        case AVAHI_BROWSER_REMOVE:
             break;

        case AVAHI_BROWSER_FAILURE:
            BUG( "(Browser) %s\n", avahi_strerror(avahi_client_errno(avahi_service_browser_get_client(b))));
            avahi_simple_poll_quit(aSimplePoll);
            return;

        case AVAHI_BROWSER_NEW:
            DBG( "(Browser) NEW: service '%s' of type '%s' in domain '%s'\n", name, type, domain);

            /* We ignore the returned resolver object. In the callback
               function we free it. If the server is terminated before
               the callback function is called the server will free
               the resolver for us. */

            if (!(avahi_service_resolver_new(c, interface, protocol, name, type, domain, AVAHI_PROTO_INET, (AvahiLookupFlags)0, resolve_callback, c)))
                BUG( "Failed to resolve service '%s': %s\n", name, avahi_strerror(avahi_client_errno(c)));
            else
+               aResolving++;

            break;

        case AVAHI_BROWSER_ALL_FOR_NOW:             
             aAllForNow--;
             check_terminate();
             break;
    }
}
/* Called whenever the client or server state changes */
static void client_callback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata) {
    assert(c);
    if (state == AVAHI_CLIENT_FAILURE) {
        BUG( "Server connection failure: %s\n", avahi_strerror(avahi_client_errno(c)));
        avahi_simple_poll_quit(aSimplePoll);
    }
}
/*
This function will gets called whenever a host name has been resolved successfully or timed out
*/
static void host_name_resolver_callback(
    AvahiHostNameResolver *r,
    AVAHI_GCC_UNUSED AvahiIfIndex interface,
    AVAHI_GCC_UNUSED AvahiProtocol protocol,
    AvahiResolverEvent event,
    const char *name,
    const AvahiAddress *a,
    AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
    AVAHI_GCC_UNUSED void *userdata) {

    assert(r);

    switch (event) {
        case AVAHI_RESOLVER_FOUND: {
            avahi_address_snprint(ipAddressBuff, sizeof(ipAddressBuff), a);
            DBG("%s\t%s\n", name, ipAddressBuff);
            avahi_simple_poll_quit(aSimplePoll);
            break;
        }
        case AVAHI_RESOLVER_FAILURE:
            BUG( "Failed to resolve host name '%s'\n", name);
            avahi_simple_poll_quit(aSimplePoll);
            break;
    }
    avahi_host_name_resolver_free(r);
}
/*
This function will register the callbacks for the avahi calls.
Parameters:
          iCommandType - INPUT - Lookup / Probe scanner.
          iHostName - INPUT - Host name of the scanner.
Note:
     iHostName will be empty for command type AVAHI_NET_DISCOVERY
*/
static void avahi_setup(const int iCommandType, const char* iHostName)
{

    AvahiClient *client = NULL;
    AvahiServiceBrowser *sb = NULL;
    int error = 0;
    if (!(aSimplePoll = avahi_simple_poll_new()))
    {
        BUG( "Failed to create simple poll object.\n");
        goto fail;
    }

    if (!(client = avahi_client_new(avahi_simple_poll_get(aSimplePoll), AVAHI_CLIENT_IGNORE_USER_CONFIG, client_callback, NULL, &error)))
    {
        //if Daemon connection failed or daemon not running, 
        //call systemctl for authorization once it authorized start the service
        if ( ( ( error == AVAHI_ERR_NO_DAEMON ) || (error == AVAHI_ERR_DISCONNECTED) ) 
              && checkAuthorizationForAvahiService() && systemdStartAvahiService() )
        { 
            if (!(client = avahi_client_new(avahi_simple_poll_get(aSimplePoll), AVAHI_CLIENT_IGNORE_USER_CONFIG, client_callback, NULL, &error)))
            {
                BUG( "Failed to create client object: %s\n", avahi_strerror(error));
                goto fail;
            }  
        }
        else
        {
            BUG( "Failed to create client object: %s\n", avahi_strerror(error));
            goto fail;            
        }
    }

   if ( iCommandType == AVAHI_NET_DISCOVERY ) /* Probe network scanner */
   {
       /* Create the service browser */
       if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, "_uscan._tcp", NULL, (AvahiLookupFlags)0, browse_callback, client))) 
       {
        //if Daemon connection failed or daemon not running, 
        //call systemctl for authorization once it authorized start the service
        if ( ( ( error == AVAHI_ERR_NO_DAEMON ) || (error == AVAHI_ERR_DISCONNECTED) ) 
              && checkAuthorizationForAvahiService() && systemdStartAvahiService() )
        { 
            if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, "_uscan._tcp", NULL, (AvahiLookupFlags)0, browse_callback, client))) 
            {
                BUG( "Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(client)));
                goto fail;
            }  
        }
        else
        {
            BUG( "Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(client)));
            goto fail;            
        }
       }
       aAllForNow++;
       /* Create the service browser */
       
       if (!(sb = avahi_service_browser_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET, "_scanner._tcp", NULL, (AvahiLookupFlags)0, browse_callback, client))) 
       {
           BUG( "Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(client)));
           goto fail;
       }
       aAllForNow++;
   }
   else if ( iCommandType == AVAHI_HOST_LOOKUP ) /* Find the IP (IPV4) address of given host name */
   {
       if ( !(avahi_host_name_resolver_new(client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, iHostName, AVAHI_PROTO_INET, (AvahiLookupFlags)0, 
            host_name_resolver_callback, NULL) ) )
       {
           BUG( "Failed to create host name resolver: %s\n", avahi_strerror(avahi_client_errno(client)));
           goto fail;
       }
   }
   avahi_simple_poll_loop(aSimplePoll);
fail:
    if (client)
       avahi_client_free(client);
    if (aSimplePoll)
       avahi_simple_poll_free(aSimplePoll);
}
/*
This function will return URIs of the scanner found in 
the network. 
Parameters:
          oUrisBuf - OUTPUT - Buffer to hold URIs of Scanner
          iBufsize - INPUT - Size of output buffer.
Note:
     URIs will be splited useing ';' delimiter
*/
int avahi_probe_nw_scanners()
{
    int aRet = AVAHI_STATUS_ERROR;
    avahi_setup(AVAHI_NET_DISCOVERY,"");
    //size_t aStrLen = strlen(aUriBuf);
    if(aBytesRead > 0)
    {
        aRet = AVAHI_STATUS_OK;     
    }
    return aRet;
}
/*
This function will return the IP address of given hostname
Parameters:
          iHostName - INPUT - Hostname of the scanner
          oIP - OUTPUT - IP Address of given host name
Note:
     Output IP address will be in IPV4 format           
*/
int avahi_lookup(const char *iHostName)
{
    int aRet = AVAHI_STATUS_ERROR;
    char aUpdatedHostname[MAX_NAME_LENGTH] = {0};
    int aHostNameLen = strlen(iHostName);
    size_t aStrLen = 0;
    //DNS host name size is 256 inclusive of '.local'
    //So the incoming host name length should be less than or equal to 250
    if ( aHostNameLen <= ( MAX_NAME_LENGTH - APPEND_LOCAL_LEN ) )
    {
	    //Host name should be appended with .local 
	    snprintf(aUpdatedHostname,MAX_NAME_LENGTH,"%s.local", iHostName);
	    avahi_setup(AVAHI_HOST_LOOKUP,aUpdatedHostname);
	    aStrLen = strlen(ipAddressBuff);
	    if ( (aStrLen <= MAX_IP_ADDR_LEN) && (aStrLen > 0) )
	    {
	        //strncpy(oIP,ipAddressBuff,aStrLen);
	        DBG("IP address is %s \n",ipAddressBuff);
	        aRet = AVAHI_STATUS_OK;
	    }
    }
    return aRet;
}
