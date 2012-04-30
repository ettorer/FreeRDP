#ifndef __RDPFile__
#define __RDPFile__

#include <freerdp/settings.h>
#include <freerdp/utils/args.h>

typedef char * rdp_config_t[3];
#ifdef WIN32
#define RAIL_PLUGIN  "rail.dll"
#define DYNVC_PLUGIN "drdynvc.dll"
#else
#define RAIL_PLUGIN  "rail.so"
#define DYNVC_PLUGIN "drdynvc.so"
#define stricmp strcasecmp
#endif
#ifdef __cplusplus
extern "C" {
#endif

rdp_config_t * rdp_decode_config(char * string);
rdp_config_t * rdp_create_config_str(char*szAddress,char*szUser,char*szDomain,char*szPassword);
char *  rdp_config_get(rdp_config_t * config, const char * name);

//char ** rdp_compose_rdesktop_options(char*title,int x, int y, rdp_config_t * config, int bPraimRDP);
//char *  rdp_convert_to_ce_format(char*title, rdp_config_t * config, int *piLen, int bCe6);
char * rdp_char_substitution(char* sz, char chSrc, char chDst);
int    is_rdp_config_file(const char*szFileName);
int    rdp_config_file_parse(rdpSettings* settings,
           ProcessPluginArgs plugin_callback, void* plugin_user_data,
	       ProcessUIArgs ui_callback, void* ui_user_data);

#ifdef __cplusplus
}
#endif

#if defined(WIN32) || defined(UNDER_CE)
#define stricmp  _stricmp
#define strdup   _strdup
#ifdef __cplusplus
extern "C" 
#else
extern
#endif
char * strsep(char **stringp, const char *delim);
#endif

#endif
