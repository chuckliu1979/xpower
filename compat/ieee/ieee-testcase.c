/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <math.h>
#include <time.h>
#include <ctype.h> 
#include <stdlib.h>
#include <stdarg.h>
#include "ieee-testcase.h"
#include "../../xpower-def.h"
#include "../../xpower-config.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable:4996 4018)
#endif

/* global variables */
#undef _XIEEE_VAR
#define _XIEEE_VAR(name) size_t _xieee_cdf_##name##_size = 0;   \
                         _xieee_##name *_xieee_cdf_##name = NULL;                       
_XIEEE_VAR(bus)
_XIEEE_VAR(zone)
_XIEEE_VAR(ichg)
_XIEEE_VAR(line)
_XIEEE_VAR(branch)

_xieee_title _xieee_cdf_title;

#define _xieee_len 132
static const char *_xieee_token[] = {
    "END OF DATA",
    "BUS DATA FOLLOWS",
    "BRANCH DATA FOLLOWS",
    "LOSS ZONES FOLLOWS",
    "INTERCHANGE DATA FOLLOWS",
    "TIE LINES FOLLOW",
    "-9",
    "-99",
    "-999"
};
#define _xieee_cdf_end       _xieee_token[0]

#define _xieee_header_bus    _xieee_token[1]
#define _xieee_header_branch _xieee_token[2]
#define _xieee_header_zone   _xieee_token[3]
#define _xieee_header_ichg   _xieee_token[4]
#define _xieee_header_line   _xieee_token[5]

#define _xieee_delimiter_bus    _xieee_token[8]
#define _xieee_delimiter_branch _xieee_token[8]
#define _xieee_delimiter_zone   _xieee_token[7]
#define _xieee_delimiter_ichg   _xieee_token[6]
#define _xieee_delimiter_line   _xieee_token[8]

/* convert string line[beg-1:end-1] to float */
static inline float _xieee_str2float(const char *line, int beg, int end){
    char buf[32] = {0},*p=(char *)line+beg-1;
    
    while (!isdigit((int)*p) && *p != '-') p++;
    _xmem_copy(p, buf, (line+end-p)%sizeof(buf));
    
    return (float)atof(buf);
}

/* copy string and trim space */
static inline void _xieee_copy(const char *from, char *to, int len){
    _xmem_copy(from, to, len);
    if (1){
        char *p = to + len;
        while (--p != to && isspace((int)*p)) *p = '\0';
    }
}

/*!
 * parse and read data from IEEE Common Data Format file 
 */
int _xieee_cdf_input(FILE *fd){
    int idx,seq,option;
    _xieee_bus *bus;
    _xieee_zone *zone;
    _xieee_ichg *ichg;
    _xieee_line *line;
    _xieee_branch *branch;
    char buf[_xieee_len + sizeof(int)];/*132+sizeof(int)*/
    
    /* parse title section and only get the base MVA */    
  do {
        _xmem_zero(&_xieee_cdf_title, sizeof(_xieee_title));
        if (fgets(buf, _xieee_len+1, fd) == NULL) return _XFALSE;
        if (buf[3] == '/' && buf[6] == '/'){ /* make sure it's a real cdf file title */
            _xieee_cdf_title.base_mva = _xieee_str2float(buf, 32, 37);
            break;
        }
    } while (!feof(fd));
    if (feof(fd)) return _XFALSE;
    if (fabs(_xieee_cdf_title.base_mva) <= _XMACHEPS)
        return _XFALSE;

/* begin a mandatory section and parse the section header */
#define _xieee_cdf_mandatory_section(name)                              \
        do {                                                            \
            idx = seq = 0;                                              \
            if (fgets(buf,_xieee_len+1,fd) == NULL) return _XFALSE;     \
            if (!strncmp(buf,_xieee_header_##name,                      \
                 strlen(_xieee_header_##name))){                        \
                char *p,*q = buf + strlen(_xieee_header_##name);        \
                while (!isdigit((int)*q))                               \
                    q++;                                                \
                if (q >= buf + _xieee_len)                              \
                    return _XFALSE;                                     \
                p=q;                                                    \
                while (isdigit((int)*q))                                \
                    q++;                                                \
                if (q >= (buf + _xieee_len))                            \
                    return _XFALSE;                                     \
                seq = (size_t)_xieee_str2float(p,1,q-p);                \
                break;                                                  \
            }                                                           \
        } while(!feof(fd));                                             \
        if (feof(fd)) return _XFALSE;                                   \
        if (seq > _xieee_cdf_##name##_size){                            \
            _xfree(_xieee_cdf_##name);                                  \
            _xieee_cdf_##name##_size = seq;                             \
            _xieee_cdf_##name = (_xieee_##name *)                       \
                                 _xcalloc(_xieee_cdf_##name##_size,     \
                                          sizeof(_xieee_##name));       \
        }                                                               \
        name=_xieee_cdf_##name;                                         \
        while (fgets(buf, _xieee_len+1, fd))
/* begin an optional section and parse the section header */   
#define _xieee_cdf_optional_section(name)                               \
        idx = seq = 0;                                                  \
        if (!strncmp(buf,_xieee_header_##name,                          \
             strlen(_xieee_header_##name))){                            \
            char *p,*q = buf + strlen(_xieee_header_##name);            \
            while (!isdigit((int)*q))                                   \
                q++;                                                    \
            if (q >= buf + sizeof(buf))                                 \
                return _XFALSE;                                         \
            p=q;                                                        \
            while (isdigit((int)*q))                                    \
                q++;                                                    \
            if (q >= (buf + sizeof(buf)))                               \
                return _XFALSE;                                         \
            seq = (size_t)_xieee_str2float(p,1,q-p);                    \
        }                                                               \
        if (seq > _xieee_cdf_##name##_size){                            \
            _xfree(_xieee_cdf_##name);                                  \
            _xieee_cdf_##name##_size = seq;                             \
            _xieee_cdf_##name = (_xieee_##name *)                       \
                                 _xcalloc(_xieee_cdf_##name##_size,     \
                                          sizeof(_xieee_##name));       \
        }                                                               \
        name=_xieee_cdf_##name;                                         \
        while (fgets(buf, _xieee_len+1, fd))
/* check whether reach section end line or not */
#define _xieee_check_cdf_section_end(name)                              \
        buf[strlen(buf)-1] = '\0';                                      \
        if (!strlen(buf)) continue;                                     \
        if (!strncmp(buf,_xieee_delimiter_##name,                       \
             strlen(_xieee_delimiter_##name))) break;                   \
        if (++idx > _xieee_cdf_##name##_size) return _XFALSE;

    _xieee_cdf_mandatory_section(bus){
        _xieee_check_cdf_section_end(bus)
          
        bus->number = (int)_xieee_str2float(buf,1,4);
        _xieee_copy(buf+5,bus->name,sizeof(bus->name));
        bus->area         = (int)_xieee_str2float(buf,19,20);
        bus->zone         = (int)_xieee_str2float(buf,21,23);
        bus->type         = (int)_xieee_str2float(buf,26,26);
        bus->voltage      = _xieee_str2float(buf,28,33);
        bus->angle        = _xieee_str2float(buf,34,40);
        bus->load_mw      = _xieee_str2float(buf,41,49);
        bus->load_mvar    = _xieee_str2float(buf,50,58);
        bus->gen_mw       = _xieee_str2float(buf,59,67);
        bus->gen_mvar     = _xieee_str2float(buf,68,75);
        bus->base_kv      = _xieee_str2float(buf,77,83);
        bus->desired_volt = _xieee_str2float(buf,85,90);
        bus->max_mvar     = _xieee_str2float(buf,91,98);
        bus->min_mvar     = _xieee_str2float(buf,99,106);
        bus->g            = _xieee_str2float(buf,107,114);
        bus->b            = _xieee_str2float(buf,115,122);
        bus->cntrl_bus    = (int)_xieee_str2float(buf,124,127);
        
        _xmem_zero(buf,sizeof(buf));
        bus++;
    }
    
    _xieee_cdf_mandatory_section(branch){
        _xieee_check_cdf_section_end(branch)
          
        branch->tapbus      = (int)_xieee_str2float(buf,1,4);
        branch->zbus        = (int)_xieee_str2float(buf,6,9);
        branch->area        = (int)_xieee_str2float(buf,11,12);
        branch->zone        = (int)_xieee_str2float(buf,13,15);
        branch->circuit     = (int)_xieee_str2float(buf,17,17);
        branch->type        = (int)_xieee_str2float(buf,19,19);
        branch->r           = _xieee_str2float(buf,20,29);
        branch->x           = _xieee_str2float(buf,30,39);
        branch->b           = _xieee_str2float(buf,41,49);
        branch->mva_rating1 = (int)_xieee_str2float(buf,51,55);
        branch->mva_rating2 = (int)_xieee_str2float(buf,57,61);
        branch->mva_rating3 = (int)_xieee_str2float(buf,63,67);
        branch->cntrl_bus   = (int)_xieee_str2float(buf,69,72);
        branch->side        = (int)_xieee_str2float(buf,74,74);
        branch->ratio       = _xieee_str2float(buf,77,82);
        branch->angle       = _xieee_str2float(buf,84,90);
        branch->min_tap     = _xieee_str2float(buf,91,97);
        branch->max_tap     = _xieee_str2float(buf,98,104);
        branch->step        = _xieee_str2float(buf,106,111);
        branch->min_volt    = _xieee_str2float(buf,113,119);
        branch->max_volt    = _xieee_str2float(buf,120,126);
        
        _xmem_zero(buf,sizeof(buf));
        branch++;
    }

OPTION:
    if (fgets(buf,_xieee_len+1,fd) == NULL) return _XFALSE;
    if (!strncmp(buf,_xieee_header_zone,strlen(_xieee_header_zone)))
        option = 1;
    else if (!strncmp(buf,_xieee_header_ichg,strlen(_xieee_header_ichg)))
        option = 2;
    else if (!strncmp(buf,_xieee_header_line,strlen(_xieee_header_line)))
        option = 3;
    else
        option = 0;
            
    switch (option){
    case 1 :
        _xieee_cdf_optional_section(zone){
            _xieee_check_cdf_section_end(zone)
             
            zone->number = (int)_xieee_str2float(buf,1,3);
            _xieee_copy(buf+4,zone->name,sizeof(zone->name));
            
            _xmem_zero(buf,sizeof(buf));
            zone++;
        }
        break;
    case 2 :
        _xieee_cdf_optional_section(ichg){
            _xieee_check_cdf_section_end(ichg)
            
            ichg->area         = (int)_xieee_str2float(buf,1,2);
            ichg->swing_number = (int)_xieee_str2float(buf,4,7);
            _xieee_copy(buf+8,ichg->swing_name,sizeof(ichg->swing_name));
            ichg->mw_export    = _xieee_str2float(buf,21,28);
            ichg->mw_tolerance = _xieee_str2float(buf,29,35);
            _xieee_copy(buf+37,ichg->area_code,sizeof(ichg->area_code));
            _xieee_copy(buf+45,ichg->area_name,sizeof(ichg->area_name)-2);
            
            _xmem_zero(buf,sizeof(buf));
            ichg++;
        }
        break;
    case 3 :     
        _xieee_cdf_optional_section(line){
            _xieee_check_cdf_section_end(line)
              
            line->metered_bus  = (int)_xieee_str2float(buf,1,4);
            line->metered_area = (int)_xieee_str2float(buf,7,8);
            line->bus          = (int)_xieee_str2float(buf,11,14);
            line->area         = (int)_xieee_str2float(buf,17,18);
            line->circuit      = (int)_xieee_str2float(buf,21,21);
            
            _xmem_zero(buf,sizeof(buf));
            line++;
        }   
        break;
    }
    if (option) goto OPTION;
    if (strncmp(buf,_xieee_cdf_end,strlen(_xieee_cdf_end))) return _XFALSE;
    
    return _XTRUE;
} 

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define snprintf _snprintf
#endif

#define _xieee_snprintf snprintf

/*!
 * dump the data to IEEE Common Data Format file. Actually, 
 * this is not conform to IEEE Common Data Format, we add
 * newline at comlumn 133 for a file rather than a tape, so 
 * the fixed length of one record is 133, not 132. 
 */
int _xieee_cdf_dump(FILE *fd){
    int i;
    char buf[_xieee_len+sizeof(int)];

#define _XIEEE_RECORD_BEG _xmem_zero(buf,sizeof(buf)),fflush(fd);
#define _XIEEE_RECORD_END buf[_xieee_len]='\n';                                        \
                          if (fwrite(buf,sizeof(char),_xieee_len+1,fd) != _xieee_len+1)\
                              return _XFALSE;
    /* title data, one record only */
    _XIEEE_RECORD_BEG
    _xieee_snprintf(buf+1, sizeof(_xieee_cdf_title.date),"%s",_xieee_cdf_title.date);
    _xieee_snprintf(buf+10,sizeof(_xieee_cdf_title.sender),"%s",_xieee_cdf_title.sender);
    _xieee_snprintf(buf+31,6,"%6.1f",_xieee_cdf_title.base_mva); 
    _xieee_snprintf(buf+38,4,"%4d",_xieee_cdf_title.year%10000);
    _xieee_snprintf(buf+43,1,"%c",_xieee_cdf_title.season%128);
    _xieee_snprintf(buf+45,sizeof(_xieee_cdf_title.caseid),"%s",_xieee_cdf_title.caseid);
    _XIEEE_RECORD_END
                                              
#define _XIEEE_SECTION_BEG(name)                                         \
        _XIEEE_RECORD_BEG                                                \
        _xieee_snprintf(buf,strlen(_xieee_header_##name),"%s",           \
                                   _xieee_header_##name);                \
        _xieee_snprintf(buf+44,20,"%d ITEMS", _xieee_cdf_##name##_size); \
        _XIEEE_RECORD_END                                                \
        for (i=0; i<_xieee_cdf_##name##_size; i++){                      \
            _xieee_##name *name = _xieee_cdf_##name + i;                 \
            _XIEEE_RECORD_BEG
            
#define _XIEEE_SECTION_END(name)                                         \
            _XIEEE_RECORD_END                                            \
        }                                                                \
        _XIEEE_RECORD_BEG                                                \
        _xieee_snprintf(buf,strlen(_xieee_delimiter_##name),"%s",        \
                                   _xieee_delimiter_##name);             \
        _XIEEE_RECORD_END
    
    /* bus data loop */
    _XIEEE_SECTION_BEG(bus)
    _xieee_snprintf(buf,4,"%4d",bus->number%10000);
    _xieee_snprintf(buf+5,sizeof(bus->name),"%s",bus->name);
    _xieee_snprintf(buf+18,2,"%2d",bus->area%100);
    _xieee_snprintf(buf+20,3,"%3d",bus->zone%1000);
    _xieee_snprintf(buf+25,1,"%1d",bus->type%10);
    _xieee_snprintf(buf+27,6,"%-6.3f",bus->voltage);
    _xieee_snprintf(buf+33,7,"%6.2f",bus->angle);
    _xieee_snprintf(buf+40,9,"%8.1f",bus->load_mw);
    _xieee_snprintf(buf+49,9,"%8.1f",bus->load_mvar);
    _xieee_snprintf(buf+58,9,"%8.1f",bus->gen_mw);
    _xieee_snprintf(buf+67,8,"%8.1f",bus->gen_mvar);
    _xieee_snprintf(buf+76,7,"%6.1f",bus->base_kv);
    _xieee_snprintf(buf+84,6,"%5.3f",bus->desired_volt);
    _xieee_snprintf(buf+90,8,"%7.1f",bus->max_mvar);
    _xieee_snprintf(buf+98,8,"%7.1f",bus->min_mvar);
    _xieee_snprintf(buf+106,8,"%7.3f",bus->g);
    _xieee_snprintf(buf+115,8,"%5.3f",bus->b);
    _xieee_snprintf(buf+123,4,"%4d",bus->cntrl_bus%10000);
    _XIEEE_SECTION_END(bus)
     
    /* branch data loop */
    _XIEEE_SECTION_BEG(branch)   
    _xieee_snprintf(buf,4,"%4d",branch->tapbus%10000);
    _xieee_snprintf(buf+5,4,"%4d",branch->zbus%10000);
    _xieee_snprintf(buf+10,2,"%2d",branch->area%100);
    _xieee_snprintf(buf+12,3,"%3d",branch->zone%1000);
    _xieee_snprintf(buf+16,1,"%1d",branch->circuit%10);
    _xieee_snprintf(buf+18,1,"%1d",branch->type%10);
    _xieee_snprintf(buf+19,10,"%9.5f",branch->r);
    _xieee_snprintf(buf+29,10,"%9.5f",branch->x);
    _xieee_snprintf(buf+40,9,"%9.5f",branch->b);
    _xieee_snprintf(buf+50,5,"%5d",branch->mva_rating1%10000);
    _xieee_snprintf(buf+56,5,"%5d",branch->mva_rating2%10000);
    _xieee_snprintf(buf+62,5,"%5d",branch->mva_rating3%10000);
    _xieee_snprintf(buf+68,4,"%4d",branch->cntrl_bus%10000);
    _xieee_snprintf(buf+73,1,"%1d",branch->side%10);
    _xieee_snprintf(buf+76,6,"%5.3f",branch->ratio);
    _xieee_snprintf(buf+83,7,"%7.2f",branch->angle);
    _xieee_snprintf(buf+90,7,"%6.1f",branch->min_tap);
    _xieee_snprintf(buf+97,7,"%6.1f",branch->max_tap);
    _xieee_snprintf(buf+105,6,"%5.1f",branch->step);
    _xieee_snprintf(buf+112,7,"%6.3f",branch->min_volt);
    _xieee_snprintf(buf+119,7,"%6.3f",branch->max_volt);
    _XIEEE_SECTION_END(branch)
    
    /* zone data loop */
    _XIEEE_SECTION_BEG(zone)  
    _xieee_snprintf(buf,3,"%3d",zone->number%1000);
    _xieee_snprintf(buf+4,sizeof(zone->name),"%s",zone->name); 
    _XIEEE_SECTION_END(zone)
    
    /* ichg data loop */
    _XIEEE_SECTION_BEG(ichg)
    _xieee_snprintf(buf,2,"%2d",ichg->area%100);
    _xieee_snprintf(buf+3,4,"%4d",ichg->swing_number%10000);
    _xieee_snprintf(buf+8,sizeof(ichg->swing_name),"%s",ichg->swing_name);
    _xieee_snprintf(buf+20,8,"%6.2f",ichg->mw_export);
    _xieee_snprintf(buf+28,7,"%6.2f",ichg->mw_tolerance);
    _xieee_snprintf(buf+37,sizeof(ichg->area_code),"%s",ichg->area_code);
    _xieee_snprintf(buf+45,sizeof(ichg->area_name),"%s",ichg->area_name); 
    _XIEEE_SECTION_END(ichg)
    
    /* line data loop */
    _XIEEE_SECTION_BEG(line)    
    _xieee_snprintf(buf,4,"%4d",line->metered_bus%10000);
    _xieee_snprintf(buf+6,1,"%4d",line->metered_area%10000);
    _xieee_snprintf(buf+10,4,"%4d",line->bus%10000);
    _xieee_snprintf(buf+16,1,"%4d",line->area%10000);
    _xieee_snprintf(buf+20,1,"%1d",line->area%10);
    _XIEEE_SECTION_END(line)
    
    /* last record */
    _XIEEE_RECORD_BEG
    _xieee_snprintf(buf,_xieee_len,"%s",_xieee_cdf_end);
    _XIEEE_RECORD_END
    
    return _XTRUE;
}

/*!
 * output the memory as a CDF file, if no title valid,
 * generate a title with current time
 */
int _xieee_cdf_output(FILE *fd){
    time_t t;
    struct tm *gmt;
    
    tzset(); t = time(NULL);gmt = gmtime(&t);
    /*
     * make sure the title is valid and/or readable
     */
    if (_xieee_cdf_title.date[2] != '/' &&
        _xieee_cdf_title.date[5] != '/'){/* non-initialized */
          
        _xieee_cdf_title.date[0] = '0' + gmt->tm_mday/10;
        _xieee_cdf_title.date[1] = '0' + gmt->tm_mday%10;
        _xieee_cdf_title.date[2] = '/';
        _xieee_cdf_title.date[3] = '0' + (gmt->tm_mon+1)/10;
        _xieee_cdf_title.date[4] = '0' + (gmt->tm_mon+1)%10;
        _xieee_cdf_title.date[5] = '/';
        _xieee_cdf_title.date[6] = '0' + (gmt->tm_year%100)/10;
        _xieee_cdf_title.date[7] = '0' + (gmt->tm_year%100)%10;
    }
    if (_xieee_cdf_title.sender[0] == '\0'){
        _xmem_zero(_xieee_cdf_title.sender,sizeof(_xieee_cdf_title.sender));
        _xieee_snprintf(_xieee_cdf_title.sender, \
                 sizeof(_xieee_cdf_title.sender),"XPOWER ARCHIVE");
    }
    if (fabs(_xieee_cdf_title.base_mva) <= _XMACHEPS)
        _xieee_cdf_title.base_mva = (float)_XBASE_MVA;
    if (_xieee_cdf_title.year <= 0)
        _xieee_cdf_title.year = gmt->tm_year + 1900;
    if (_xieee_cdf_title.season <= 0)
        _xieee_cdf_title.season = gmt->tm_mon >= 10 || \
                                  gmt->tm_mon <= 4 ? 'W' : 'S';
    if (_xieee_cdf_title.caseid[0] == '\0'){
        _xmem_zero(_xieee_cdf_title.caseid,sizeof(_xieee_cdf_title.caseid));
        _xieee_snprintf(_xieee_cdf_title.caseid, \
                 sizeof(_xieee_cdf_title.caseid),"XPOWER CASE %d",(int)t); 
    }
    /*
     * dump without any modification
     */
    return _xieee_cdf_dump(fd); 
}

/*!
 * deallocate the memory allocated
 */
void _xieee_cdf_free(void){

    _xmem_zero(&_xieee_cdf_title, sizeof(_xieee_title));
    
#undef _XIEEE_VAR
#define _XIEEE_VAR(name) _xfree(_xieee_cdf_##name);_xieee_cdf_##name = NULL;_xieee_cdf_##name##_size = 0;
                       
_XIEEE_VAR(bus)
_XIEEE_VAR(zone)
_XIEEE_VAR(ichg)
_XIEEE_VAR(line)
_XIEEE_VAR(branch)

}

static int _xieee_bus_index(int number){
    int i,idx = -1;
    _xieee_bus *ieeefbus;
    
    for (i=0; i<(int)(_xieee_cdf_bus_size); i++){
    	ieeefbus = _xieee_cdf_bus + i;
    	if (ieeefbus->number == number){
    		idx = i;
    		break;
    	}
    }
    
    return idx;
}

/*!
 * create the ybus matrix with elements (g+jb)
 */
#define _XPI 3.1415926535897932
int _xieee_cdf_ybus(_xperm **pivot, _xspmat **g, _xspmat **b, int imag){
	unsigned int i,j,k;
	_xperm *px = *pivot;
	_xspmat *gmat, *bmat;
	_xieee_bus *ieeefbus;
	_xieee_branch *ieeebranch;
	double ratio,angle;
	double vij,gij,bij,Gii,Gjj,Bii,Bjj,Bij,Gij,Bji,Gji;

	if (!px || px->size != _xieee_cdf_bus_size){
		px = _xperm_resize(px, _xieee_cdf_bus_size);
		if (px == NULL) return _XFALSE;
		*pivot = px;
	}

	if ((bmat = _xspmat_resize(*b, px->size, px->size)) == NULL) 
		return _XFALSE;
	*b = bmat;
	_xspmat_zero(bmat);
	_xspmat_compact(bmat,_XMACHEPS);

	if (!imag){
		if ((gmat = _xspmat_resize(*g, px->size, px->size)) == NULL) 
			return _XFALSE;
		*g = gmat;
		_xspmat_zero(gmat);
	    _xspmat_compact(gmat,_XMACHEPS);
	}
		
	for (i=0; i<px->size; i++){
		ieeefbus = _xieee_cdf_bus + i;
		
		j = px->pe[i];
		_xspmat_set_val(bmat,j,j,ieeefbus->b);
		if (!imag) _xspmat_set_val(gmat,j,j,ieeefbus->g);  
	}
	
	for (k=0; k<_xieee_cdf_branch_size; k++){
		ieeebranch = _xieee_cdf_branch + k;
		i = _xieee_bus_index(ieeebranch->tapbus);
		j = _xieee_bus_index(ieeebranch->zbus);
		
		if (i<0 || j<0) return _XFALSE;
		i = px->pe[i];
		j = px->pe[j];
		
		vij = 1.0/(ieeebranch->r*ieeebranch->r + ieeebranch->x*ieeebranch->x);
		gij = ieeebranch->r*vij;
		bij =-ieeebranch->x*vij;
			
		ratio = ieeebranch->ratio == 0.0f ? 1.0 : ieeebranch->ratio;
		angle = -_XPI/180*ieeebranch->angle;
		/* ieee300cdf.txt branch (196 2040) is phase shifter, 
		 * but force to angle 0.0 currently
		 */
		if (ieeebranch->type == 4)
			angle = 0.0;
		
		vij = 1.0/(ratio*ratio);
		Gjj = gij, Gii = Gjj*vij;
		Bjj = bij + 0.5*ieeebranch->b, Bii = Bjj*vij;
		
		ratio = ratio*cos(angle);
		angle = ratio*tan(angle); 
			
		Gij = -1.0*vij*(gij*ratio-bij*angle);
		Bij = -1.0*vij*(gij*angle+bij*ratio);
		
		Gji = -1.0*vij*(gij*ratio+bij*angle);
		Bji = -1.0*vij*(bij*ratio-gij*angle);
		
		_xspmat_set_val(bmat,i,j,_xspmat_get_val(bmat,i,j)+Bij);
		_xspmat_set_val(bmat,i,i,_xspmat_get_val(bmat,i,i)+Bii);
		_xspmat_set_val(bmat,j,i,_xspmat_get_val(bmat,j,i)+Bji);
		_xspmat_set_val(bmat,j,j,_xspmat_get_val(bmat,j,j)+Bjj);

		if (!imag){
			_xspmat_set_val(gmat,i,j,_xspmat_get_val(gmat,i,j)+Gij);
			_xspmat_set_val(gmat,i,i,_xspmat_get_val(gmat,i,i)+Gii);
			_xspmat_set_val(gmat,j,i,_xspmat_get_val(gmat,j,i)+Gji);
			_xspmat_set_val(gmat,j,j,_xspmat_get_val(gmat,j,j)+Gjj);
		}
	}
	
	return _XTRUE;
}

/*!
 * create the fastpq matrices, B prime and B double prime
 */
int _xieee_cdf_bbus(_xperm **pivot, _xspmat **bp, _xspmat **bpp, int xb){
	unsigned int i,ret = _XFALSE;
    _xieee_bus *tmpbus = NULL;
    _xieee_branch *tmpbranch = NULL;
    
    /* make a copy */
    if ((tmpbus = (_xieee_bus *)_xcalloc(_xieee_cdf_bus_size,sizeof(_xieee_bus))) == NULL)
    	goto DONE;
    _xmem_copy(_xieee_cdf_bus,tmpbus,_xieee_cdf_bus_size*sizeof(_xieee_bus));
    if ((tmpbranch = (_xieee_branch *)_xcalloc(_xieee_cdf_branch_size,sizeof(_xieee_branch))) == NULL)
    	goto DONE;
    _xmem_copy(_xieee_cdf_branch,tmpbranch,_xieee_cdf_branch_size*sizeof(_xieee_branch));
    
    /* form B prime */
    for (i=0; i<_xieee_cdf_bus_size; i++){
    	_xieee_bus *ieeebus = _xieee_cdf_bus + i;
    	ieeebus->b = 0.0f;  /* zero out shunts at buses */
    }
    for (i=0; i<_xieee_cdf_branch_size; i++){
    	_xieee_branch *ieeebranch = _xieee_cdf_branch + i;
    	ieeebranch->b     = 0.0f;     /* zero out line charging shunts */
    	ieeebranch->ratio = 1.0f;     /* cancel out taps */
    	if (xb) ieeebranch->r = 0.0f; /* if XB, zero out line resistance */
    }
    if (!_xieee_cdf_ybus(pivot, NULL, bp, _XTRUE))
    	goto DONE;
    _xspmat_smlt(*bp,-1.0,*bp);
    
    /* form B double prime */
    for (i=0; i<_xieee_cdf_branch_size; i++){
    	_xieee_branch *ieeebranch = _xieee_cdf_branch + i;
    	ieeebranch->angle = 0.0f;      /* zero out phase shifters */
    	if (!xb) ieeebranch->r = 0.0f; /* if BX, zero out line resistance */
    }
    if (!_xieee_cdf_ybus(pivot, NULL, bpp, _XTRUE))
    	goto DONE;
    _xspmat_smlt(*bpp,-1.0,*bpp);
    
    ret = _XTRUE;
    	
DONE:
	if (tmpbus){
	    _xmem_copy(tmpbus, _xieee_cdf_bus, _xieee_cdf_bus_size*sizeof(_xieee_bus));
	    _xfree(tmpbus);
	}
	if (tmpbranch){
	    _xmem_copy(tmpbranch, _xieee_cdf_branch, _xieee_cdf_branch_size*sizeof(_xieee_branch));
	    _xfree(tmpbranch);
    }
	
	return ret;
}

