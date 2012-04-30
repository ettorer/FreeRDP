/*
 * FreeRDP: A Remote Desktop Protocol client.
 * Rdp file Parsing
 *
 * Copyright 2012 Praim Srl, Ettore Roberto Rizza
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>
#include <freerdp/utils/rdpfile.h>
#include <freerdp/utils/memory.h>

rdp_config_t void_rdp_config[] = {
     { "administrative session",   "i", NULL },   //Connect to the administrative session. 0 = non administrative, 1 = administrative (like /admin)
     { "alternate full address",   "s", NULL },   //Like "full address" but with higher priority
     { "audiocapturemode",         "i", NULL },   //0= do not capture audio from the local computer (Do not record). 1= capture audio from the local computer and send to the remote computer (Record from this computer).
     { "audiomode",                "i", NULL },   //0= play to this computer, 1= play at remote computer, 2= do not play
     { "audioqualitymode",         "i", NULL },   //0= dynamic adjust, 1= medium, 2= uncompressed
     { "authentication level",     "i", NULL },   //server certificate check failure handling. 0=Always connect, 1=Do not connect, 2=Warn, 3=No check
     { "autoreconnection enabled", "i", NULL },
     { "autoreconnect max retries","i", NULL },   //default 20 retries
     { "bitmapcachepersistenable", "i", NULL },
     { "compression",              "i", NULL },   //bulk compression
     { "connection type",          "i", NULL },   //1=modem, 2=low-speed (256Kbps /2Mbps), 3=satellite (2/16 Mbps high latency), 4=high-speed (2/10 Mbps), 5=WAN (10+ Mbps, high latency), 6=LAN
     { "desktopheight",            "i", NULL },   //200-2048
     { "desktopwidth",             "i", NULL },   //200-4096
     { "devicestoredirect",        "s", NULL },   //"*"=redirect all supported Plug and Play devices, "DynamicDevices", "<hardware id>"
     { "disable ctrl+alt+del",     "i", NULL },   //
     { "disableprinterredirection","i", NULL },   //0=Easy print enabled, 1=Easy print disabled
     { "disableclipboardredirection","i",NULL},   //0=clipboard redirection enabled, 1=clipboard redirection disabled
     { "displayconnectionbar",     "i", NULL },   //
     { "domain",                   "s", NULL },
     { "enablecredsspsupport",     "i", NULL },   //0=do not use CredSSP (no NLA), 1=use CredSSP (enable NLA)
     { "full address",             "s", NULL },
     { "keyboardhook",             "i", NULL },   //handling of key combinations (alt-tab etc...) 0=Local, 1=Remote, 2=Remote if fullscreen
     { "loadbalanceinfo",          "s", NULL },   // ex. tsv://someserver.1.somefarm
     { "negotiate security layer", "i", NULL },   //0=SSL(TLS), 1=RDP security (x.224)
     { "pinconnectionbar",         "i", NULL },   //0=topbar not pinned, 1=topbar pinned
     { "prompt for credentials on client","i",NULL},//0=force NLA to authenticate, 1=ask credentials if server does not support server authentication (NLA?)
     { "redirectclipboard",        "i", NULL },
     { "redirectcomports",         "i", NULL },
     { "redirectdrives",           "i", NULL },
     { "redirectprinters",         "i", NULL },
     { "redirectsmartcards",       "i", NULL },
     { "screen mode id",           "i", NULL },   //1 - windowed, 2 - fullscreen
     { "server port",              "i", NULL },   //server port decimal value server port:i:3389, or exadecimal ex: server port:i:0xF3D
     { "session bpp",              "i", NULL },   //8, 15, 16, 24, 32 bpp
     { "smart sizing",             "i", NULL },
     { "span monitors",            "i", NULL },
     { "usbdevicestoredirect",     "s", NULL },   //(RemoteFX) "*"=redirect all possible devices, "{Device Setup Class GUID}", "USB\InstanceID"=Export this device, "-USB\InstanceID"=Do not export this device
     { "username",                 "s", NULL },
     { "use multimon",             "i", NULL },   //0 Do not enable multiple monitor support. 1 Enable multiple monitor support.
     { "videoplaybackmode",        "i", NULL },
     { "winposstr",                "s", NULL },   //6 values of WINDOWSPOS structure, ex: 0,3,0,0,1920,1080
     { "workspaceid",              "s", NULL },   //RemoteApp and Desktop ID (es. contoso1)
     { "Workspace Id",             "s", NULL },   //RemoteApp and Desktop ID (es. contoso1)

     { "auto connect",             "i", NULL },
     { "redirectposdevices",       "i", NULL },
     { "password",                 "s", NULL },
     { "password 51",              "b", NULL },
     { "alternate shell",          "s", NULL },
     { "shell working directory",  "s", NULL },
     { "disable wallpaper",        "i", NULL },
     { "disable full window drag", "i", NULL },
     { "disable menu anims",       "i", NULL },
     { "disable themes",           "i", NULL },
     { "disable cursor blink",     "i", NULL }, 
     { "disable cursor shadow",    "i", NULL }, 
     { "connect to console",       "i", NULL },
     { "allow desktop composition","i", NULL },
     { "allow font smoothing",     "i", NULL },
     { "disable cursor setting",   "i", NULL },
     { "autoreconnection enabled", "i", NULL },
     { "prompt for credentials",   "i", NULL },
     { "remoteapplicationmode",    "i", NULL },
     { "remoteapplicationprogram", "s", NULL },
     { "remoteapplicationname",    "s", NULL },
     { "remoteapplicationcmdline", "s", NULL },
     { "gatewayhostname",          "s", NULL },
     { "gatewayusagemethod",       "i", NULL },
     { "gatewaycredentialssource", "i", NULL },
     { "gatewayprofileusagemethod","i", NULL },
     { "promptcredentialonce",     "i", NULL },   //credential sharing in RD Gateway connections (pasthroug from RD gateway and RDP sessions)
     { "drivestoredirect",         "s", NULL },
     { "redirectdirectx",          "i", NULL },
     { "use redirection server name","i",NULL},

     { NULL },
};

#ifdef WIN32
char *strsep(char **stringp, const char *delim)
{
	register char *s;
	register const char *spanp;
	register int c, sc;
	char *tok;

	if ((s = *stringp) == NULL)
		return (NULL);
	for (tok = s; ; ) {
		c = *s++;
		spanp = delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*stringp = s;
				return (tok);
			}
		} while (sc != 0);
	}
}
#endif

rdp_config_t * rdp_config_new()
{
     rdp_config_t * config = (rdp_config_t *)malloc(sizeof(void_rdp_config));
     memcpy(config, void_rdp_config, sizeof(void_rdp_config));

     return config;
}

char* rdp_char_substitution(char* sz, char chSrc, char chDst)
{
    char*p = sz;
    while(*p) {
        if(*p==chSrc)
            *p = chDst;
        p++;
    }
    return sz;
}

static int rdp_config_set(rdp_config_t * config, char * name, char * value)
{
     int j;
     char*p;
     /* lookup option name into configuration table */
     for (j = 0; config[j][0]; j++) {
	  if (strcmp(config[j][0], name) == 0) {
          p = &value[0];
          while(*p >= ' ')
              p++;
          *p = 0;
	       /* set value */
	       config[j][2] = value;
	       return 1;
	  }
     }
     /* error if the option name is not in the table */
     #ifdef DEBUG
     printf("Warning: unknown configuration option '%s'.\n", name);
     #endif
     return 0;
}
 
char * rdp_config_get(rdp_config_t * config, const char * name)
{
     int j;

     /* lookup option name into configuration table */
     for (j = 0; config[j][0]; j++) {
	  if (stricmp(config[j][0], name) == 0) {
	       /* return value */
	       return config[j][2];
	  }
     }
     /* error if the option name is not in the table */
     #ifdef DEBUG
     printf("Warning: unknown configuration option '%s'.\n", name);
     #endif
     return NULL;
}

rdp_config_t * rdp_decode_config(char * string)
{
     char * lines[128];
     char * p = string;
     int i;

     /* create a new configuration table */
     rdp_config_t * config = rdp_config_new();

     /* brerak string into lines */
     for (i = 0; i < sizeof(lines); i++) {
	  lines[i] = strsep(&p, "\n");
	  if (lines[i] == NULL)
	       break;
     }

     /* for each line */
     for (i = 0; lines[i]; i++) {
	  char * name;
	  char * type;
	  char * value;

	  p = lines[i];

      /* skip empty lines */
	  if (!p || p[0] == '\0')
	       continue;

	  /* break line into three parts delimited by a colon */
	  name  = strsep(&p, ":");
	  type  = strsep(&p, ":");
	  value = strsep(&p, ":");

	  /* set the right value in the configuration table */
      if(name && type && value)
	    rdp_config_set(config, name, rdp_char_substitution(value, '§', ':')); //this allow the ':' character inside a config
     }

     return config;
}

int is_rdp_config_file(const char*szFileName)
{
    struct stat st;
    if(!szFileName || !strlen(szFileName))
        return 0;
    if(strstr(szFileName,".rdp") && (0==stat(szFileName,&st)))
        return 1;
    return 0;
}

int rdp_config_file_parse(rdpSettings* settings,
    ProcessPluginArgs plugin_callback, void* plugin_user_data,
	ProcessUIArgs ui_callback, void* ui_user_data)
{
    FILE* fp;
    struct stat st;
    unsigned char* pRdp = NULL;
    char *pStr,*p;
    rdp_config_t * config;
    int xx,xy,ww,hh;
    bool bVideoRedirection;

    if(!settings)
        return 0;
    if(stat(settings->hostname, &st) || !st.st_size)
        return 0;
    pRdp = (unsigned char*)malloc(st.st_size+1);
    if(!pRdp)
        return 0;
    memset(pRdp, 0, st.st_size+1);
    fp = fopen(settings->hostname, "rb");
    if(!fp) {
        free(pRdp);
        return 0;
    }
    fread(pRdp, st.st_size, 1, fp);
    fclose(fp);
    if(pRdp[0] == 0xff && pRdp[1] == 0xfe) {//Unicode format
        pStr = freerdp_uniconv_in(settings->uniconv, &pRdp[2], st.st_size);
        free(pRdp);
        if(!pStr)
            return 0;
    } else {
        pStr = (char*)pRdp;
    }
    config = rdp_decode_config(pStr);

//------ parsing -----------------------------------------
    p = rdp_config_get(config, "alternate full address");
    if(p && strlen(p)) {
        xfree(settings->hostname);
        settings->hostname = xstrdup(p);
    } else {
        p = rdp_config_get(config, "full address");
        if(p) {
            xfree(settings->hostname);
            settings->hostname = xstrdup(p);
        }
    }

    p = rdp_config_get(config, "server port");
    if(p) {
        if(strstr(p,"0x")==p)
            sscanf(p,"%X", &settings->port);
        else
            sscanf(p,"%d", &settings->port);
    }

    p = rdp_config_get(config, "username");
    if(p) {
        xfree(settings->username);
        settings->username = xstrdup(p);
    }

    p = rdp_config_get(config, "domain");
    if(p) {
        xfree(settings->domain);
        settings->domain = xstrdup(p);
    }

    p = rdp_config_get(config, "winposstr");
    if(p) {
        if(4 == sscanf(p,"0,0,%d,%d,%d,%d",&xx,&xy,&ww,&hh)) {
			settings->pos_x  = xx;
		    settings->pos_y  = xy;
            settings->width  = ww;
            settings->height = hh;
        }
    } else {
        if(rdp_config_get(config,"desktopwidth") && rdp_config_get(config,"desktopheight")) {
            sscanf(rdp_config_get(config,"desktopwidth"),  "%d", &settings->width);
            sscanf(rdp_config_get(config,"desktopheight"), "%d", &settings->height);
        }
    }

    p = rdp_config_get(config, "session bpp");
    if(p) {
        sscanf(p,"%d", &settings->color_depth);
    }

    p = rdp_config_get(config, "screen mode id");
    if(p) {
        settings->fullscreen = (strcmp(p,"2")==0);
    }

    p = rdp_config_get(config,  "keyboardhook");  //handling of key combinations (alt-tab etc...) 0=Local, 1=Remote, 2=Remote if fullscreen
    if(p) {
        if(strcmp(p,"0")==0)
            settings->grab_keyboard = false;
        else if(strcmp(p,"1")==0)
            settings->grab_keyboard = true;
        else if(strcmp(p,"2")==0)
            settings->grab_keyboard = settings->fullscreen;
    }
    			
    p = rdp_config_get(config, "connection type");   //1=modem, 2=low-speed (256Kbps /2Mbps), 3=satellite (2/16 Mbps high latency), 4=high-speed (2/10 Mbps), 5=WAN (10+ Mbps, high latency), 6=LAN
    if(p) {
        sscanf(p,"%d", &settings->connection_type);
        switch(settings->connection_type) {
        case CONNECTION_TYPE_MODEM:
			settings->performance_flags = PERF_DISABLE_WALLPAPER | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS | PERF_DISABLE_THEMING;
            break;
        case CONNECTION_TYPE_BROADBAND_LOW:
        case CONNECTION_TYPE_SATELLITE:
			settings->performance_flags = PERF_DISABLE_WALLPAPER | PERF_DISABLE_FULLWINDOWDRAG | PERF_DISABLE_MENUANIMATIONS;
            break;
        case CONNECTION_TYPE_BROADBAND_HIGH:
            settings->performance_flags = PERF_DISABLE_WALLPAPER;
            break;
        case CONNECTION_TYPE_WAN:
        case CONNECTION_TYPE_LAN:
            settings->performance_flags = PERF_FLAG_NONE;
            break;
        default:
            settings->connection_type   = CONNECTION_TYPE_BROADBAND_HIGH;
            settings->performance_flags = PERF_DISABLE_WALLPAPER;
            break;
        }
    }

    p = rdp_config_get(config, "disable wallpaper");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_DISABLE_WALLPAPER;

    p = rdp_config_get(config, "disable full window drag");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_DISABLE_FULLWINDOWDRAG;

    p = rdp_config_get(config, "disable menu anims");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_DISABLE_MENUANIMATIONS;

    p = rdp_config_get(config, "disable themes");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_DISABLE_THEMING;

    p = rdp_config_get(config, "allow font smoothing");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_ENABLE_FONT_SMOOTHING;
   
    p = rdp_config_get(config, "allow desktop composition");
    if(p && strcmp(p, "1")==0)
        settings->performance_flags |= PERF_ENABLE_DESKTOP_COMPOSITION;
    
    p = rdp_config_get(config, "compression");
    if(p) {
        settings->compression = (strcmp(p,"1")==0);
    }

    p = rdp_config_get(config,"bitmapcachepersistenable");
    if(p) {
        settings->bitmap_cache = (strcmp(p,"1")==0);
    }

    p = rdp_config_get(config, "audiomode"); //0 - Bring to this computer, 1 - Leave at remote computer, 2 - Do not play
    if(p) {
        if(strcmp(p,"0")==0) {
            settings->audio_playback = true;
            settings->console_audio  = false;
        } else if(strcmp(p,"1")==0) {
            settings->audio_playback = false;
            settings->console_audio  = true;
        } else if(strcmp(p,"2")==0) {
            settings->audio_playback = false;
            settings->console_audio  = false;
        }
    }
    bVideoRedirection = false;
    p = rdp_config_get(config, "videoplaybackmode");
    if(p && strcmp(p,"1")==0)
        bVideoRedirection = true;
    p = rdp_config_get(config, "audiocapturemode");
    if(p) settings->audio_capture = (strcmp(p,"1")==0);
    if((bVideoRedirection || settings->audio_capture) && plugin_callback) {
        RDP_PLUGIN_DATA*plugin_data;
        plugin_data = (RDP_PLUGIN_DATA*) xmalloc(sizeof(RDP_PLUGIN_DATA)*3);
        if(plugin_data) {
            memset(plugin_data,0,sizeof(RDP_PLUGIN_DATA)*3);
            ww = 0;
            if(settings->audio_capture) {
                plugin_data[0].size = sizeof(RDP_PLUGIN_DATA);
                plugin_data[0].data[0] = xstrdup("audin");
                ww++;
            }
            if(bVideoRedirection) {
                plugin_data[ww].size = sizeof(RDP_PLUGIN_DATA);
                plugin_data[ww].data[0] = xstrdup("tsmf");
            }
            plugin_callback(settings, DYNVC_PLUGIN, plugin_data, plugin_user_data);
        }
    }
    
    p = rdp_config_get(config, "connect to console");
    if(p) settings->console_session = (strcmp(p,"1")==0);
    p = rdp_config_get(config, "administrative session");
    if(p) settings->console_session = (strcmp(p,"1")==0);

    p = rdp_config_get(config, "negotiate security layer");
    if(p) settings->tls_security = (strcmp(p,"0")==0); 

    p = rdp_config_get(config, "enablecredsspsupport");
    if(p) settings->nla_security = (strcmp(p,"1")==0);

    p = rdp_config_get(config, "alternate shell");
    if(p) {
        xfree(settings->shell);
        settings->shell = xstrdup(p);
    }

    p = rdp_config_get(config, "shell working directory");
    if(p) {
        xfree(settings->directory);
        settings->directory = xstrdup(p);
    }

    p = rdp_config_get(config, "remoteapplicationmode");
    if(p && strcmp(p,"1")==0 && rdp_config_get(config, "remoteapplicationprogram")) {
		settings->remote_app = true;
		settings->rail_langbar_supported = true;
		settings->workarea   = true;
		settings->performance_flags = PERF_DISABLE_WALLPAPER | PERF_DISABLE_FULLWINDOWDRAG;
        if(plugin_callback) {
            RDP_PLUGIN_DATA*plugin_data;
            plugin_data = (RDP_PLUGIN_DATA*) xmalloc(sizeof(RDP_PLUGIN_DATA)*2);
            if(plugin_data) {
                memset(plugin_data,0,sizeof(RDP_PLUGIN_DATA)*2);
                plugin_data[0].size = sizeof(RDP_PLUGIN_DATA);
                plugin_data[0].data[0] = xstrdup(rdp_config_get(config, "remoteapplicationprogram"));
                plugin_data[0].data[1] = xstrdup(settings->directory);//working folder
                plugin_data[0].data[2] = xstrdup(rdp_config_get(config, "remoteapplicationcmdline"));
                plugin_callback(settings, RAIL_PLUGIN, plugin_data, plugin_user_data);
            }
        }
    }

    free(config);
    free(pStr);
    return 0;
}
