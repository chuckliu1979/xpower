/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** progress bar routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include "../xpower-config.h"

struct _xoption {
	int verbose;
	int quiet;
	
	char *lfilename;
	char *dot_style;
	
	long dot_bytes;
	int dots_in_line;
	int dot_spacing;
	
} opt;

#define strdup _xstrdup
static char *_xstrdupdelim(const char *beg, const char *end){
	char *res = (char *)_xmalloc(end-beg+1);
	memcpy(res,beg,end-beg);
	res[end-beg] = '\0';
	return res;
}

/* implement support for "logging" */

#define _XLOG_ALWAYS     1
#define _XLOG_NOTQUIET   2
#define _XLOG_NONVERBOSE 3
#define _XLOG_VERBOSE    4
    
static FILE *_xlog_fp;
static int _xlog_inhibit_logging;
static int _xlog_save_context;
static int _xlog_flush_log;
static int _xlog_needs_flushing;

#define _XLOG_SAVE_LINES    24
#define _XLOG_STATIC_LENGTH 128
static struct _xlog_line {
	char static_line[_XLOG_STATIC_LENGTH + 1];
	char *malloced_line;
	char *content;
	
} _xlog_lines[_XLOG_SAVE_LINES];

static int _xlog_line_current = -1;
static int _xlog_trailing_line;

static void _xlog_check_redirect_output(void);

static void _xlog_free_log_line(int num){
	struct _xlog_line *ln = _xlog_lines + num;
	if (ln->malloced_line){
		_xfree(ln->malloced_line);
		ln->malloced_line = NULL;
	}
	ln->content = NULL;
}

static void _xlog_saved_append_1(const char *start, const char *end){
	int len = end - start;
	if (!len)
		return;
		
	if (!_xlog_trailing_line){
		struct _xlog_line *ln;
		
		if (_xlog_line_current == -1)
			_xlog_line_current = 0;
		else
			_xlog_free_log_line(_xlog_line_current);
		
		ln = _xlog_lines + _xlog_line_current;
		if (len > _XLOG_STATIC_LENGTH){
			ln->malloced_line = _xstrdupdelim(start,end);
			ln->content = ln->malloced_line;
		}
		else {
			memcpy(ln->static_line,start,len);
			ln->static_line[len] = '\0';
			ln->content = ln->static_line;
		}	
	}
	else {
		struct _xlog_line *ln = _xlog_lines + _xlog_line_current;
		if (ln->malloced_line){
			int old_len = strlen(ln->malloced_line);
			ln->malloced_line = _xrealloc(ln->malloced_line, old_len+len+1);
			memcpy(ln->malloced_line+old_len,start,len);
			ln->malloced_line[old_len+len] = '\0';
			ln->content = ln->malloced_line;
		}
		else {
			int old_len = strlen(ln->static_line);
			if (old_len+len > _XLOG_STATIC_LENGTH){
				ln->malloced_line = _xmalloc(old_len+len+1);
				memcpy(ln->malloced_line,ln->static_line,old_len);
				memcpy(ln->malloced_line+old_len,start,len);
				ln->malloced_line[old_len+len] = '\0';
				ln->content = ln->malloced_line;
			}
			else {
				memcpy(ln->static_line+old_len,start,len);
				ln->static_line[old_len+len] = '\0';
				ln->content = ln->static_line;
			}
		}
	}
	
	_xlog_trailing_line = !(end[-1] == '\n');
	if (!_xlog_trailing_line)
		do { 
			if (++_xlog_line_current >= _XLOG_SAVE_LINES)
				_xlog_line_current = 0;
			     
	    } while (0);
}

static void _xlog_saved_append(const char *s){
	while (*s){
		const char *end = strchr(s,'\n');
		if (!end)
			end = s + strlen(s);
		else
			++end;
		_xlog_saved_append_1(s,end);
		s = end;
	}
}

#define _XLOG_CHECK_VERBOSE(x)                \
    switch (x){                               \
    case _XLOG_ALWAYS:                        \
    	break;                                \
    case _XLOG_NOTQUIET:                      \
    	if (opt.quiet) return;                \
    	break;                                \
    case _XLOG_NONVERBOSE:                    \
    	if (opt.verbose || opt.quiet) return; \
    	break;                                \
    case _XLOG_VERBOSE:                       \
    	if (!opt.verbose) return;             \
    }

static FILE *_xlog_get_fp(void){
	if (_xlog_inhibit_logging) return NULL;
	if (_xlog_fp) return _xlog_fp;
	return stderr;
}

static void _xlog_flush(void){
	FILE *fp = _xlog_get_fp();
	if (fp)
		fflush(fp);
	_xlog_needs_flushing = 0;
}

static void _xlog_set_flush(int flush){
	if (flush == _xlog_flush_log)
		return;
	if (flush == 0)
		_xlog_flush_log = 0;
	else {
		if (_xlog_needs_flushing)
			_xlog_flush();
		_xlog_flush_log = 1;
	}
}

static void _xlog_puts(int option, const char *s){
	FILE *fp;
	
	_xlog_check_redirect_output();
	if (!(fp=_xlog_get_fp()))
		return;
	_XLOG_CHECK_VERBOSE(option);
	
	fputs(s,fp);
	if (_xlog_save_context)
		_xlog_saved_append(s);
	if (_xlog_flush_log)
		_xlog_flush();
	else
		_xlog_needs_flushing = 1;
}

struct _xlog_vprintf_state {
	char *bigmsg;
	int expected_size;
	int allocated;
};

static int _xlog_vprintf(_xlog_vprintf_state *state, const char *fmt, va_list args){
	char smallmsg[128];
	char *write_ptr = smallmsg;
	int available_size = sizeof(smallmsg);
	int numwritten;
	FILE *fp = _xlog_get_fp();
	
	if (!_xlog_save_context){
		vfprintf(fp,fmt,args);
		goto flush;
	}
	
	if (state->allocated != 0){
		write_ptr = state->bigmsg;
		available_size = state->allocated;
	}
	
	numwritten = vsnprintf(write_ptr,available_size,fmt,args);
	if (numwritten == -1){
		int newsize = available_size << 1;
		state->bigmsg = _xrealloc(state->bigmsg,newsize);
		state->allocated = newsize;
		return 0;
	}
    else if(numwritten >= available_size){
    	int newsize = numwritten + 1;
    	state->bigmsg = _xrealloc(state->bigmsg,newsize);
    	state->allocated = newsize;
    	return 0;
	}
	
	_xlog_saved_append(write_ptr);
	fputs(write_ptr,fp);
	if (state->bigmsg)
		_xfree(state->bigmsg);
		
flush:
	if (_xlog_flush_log)
		_xlog_flush();
	else
		_xlog_needs_flushing = 1;
		
	return 1;
}

static int _xlog_set_save_context(int savep){
	int old = _xlog_save_context;
	_xlog_save_context = savep;
	return old;
}

static void _xlog_printf(int option, const char *fmt, ...){
	va_list args;
	struct _xlog_vprintf_state lpstate;
	int done;
	
	_xlog_check_redirect_output();
	if (_xlog_inhibit_logging)
		return;
	_XLOG_CHECK_VERBOSE(option);
	
	memset(&lpstate,'\0',sizeof(lpstate));
	do {
		va_start(args,fmt);
		done = _xlog_vprintf(&lpstate,fmt,args);
		va_end(args);
		
	} while (!done);
}

static void _xlog_init(const char *file, int appendp){
	if (file){
		_xlog_fp = fopen(file,appendp ? "a" : "w");
		if (!_xlog_fp){
			perror(opt.lfilename);
			exit(1);
		}
	}
	else {
		_xlog_fp = stderr;
		if (isatty(fileno(_xlog_fp)))
			_xlog_save_context = 1;
	}
}

static void _xlog_close(void){
    int i;
    
    if (_xlog_fp)
    	fclose(_xlog_fp);
    _xlog_fp = NULL;
    _xlog_inhibit_logging = 1;
    _xlog_save_context = 0;
    
    for (i=0; i<_XLOG_SAVE_LINES; i++)
        _xlog_free_log_line(i);
        
    _xlog_line_current = -1;
    _xlog_trailing_line = 0;
}

static void _xlog_dump_context(void){
	int num = _xlog_line_current;
	FILE *fp = _xlog_get_fp();
	
	if (!fp)
		return;
	if (num == -1)
		return;
		
	if (_xlog_trailing_line)
	    do { 
			if (++num >= _XLOG_SAVE_LINES)
				num = 0;
			     
	    } while (0);
	    
    do {
    	struct _xlog_line *ln = _xlog_lines + num;
    	if (ln->content)
    		fputs(ln->content,fp);
    	do { 
			if (++num >= _XLOG_SAVE_LINES)
				num = 0;
			     
	    } while (0);
    } while (num != _xlog_line_current);
    
    if (_xlog_trailing_line)
    	if (_xlog_lines[_xlog_line_current].content)
    		fputs(_xlog_lines[_xlog_line_current].content,fp);
    fflush(fp);
}

enum {RR_NONE, RR_REQUESTED, RR_DONE} _xredirect_request = RR_NONE;
static const char *_xredirect_request_signal_name;

static int _xfile_exists(const char *filename){
#ifdef _XHAVE_ACCESS
	return access(filename,F_OK) >= 0;
#else
    struct stat buf;
    return stat(filename,&buf) >= 0;
#endif
}

static char *_xunique_name(const char *prefix){
	int len,count = 1;
	int plen = strlen(prefix);
	char *temp = (char *)_xmalloc(plen+1+24);
	char *temp_tail = temp + plen;
	
	memcpy(temp,prefix,plen);
	*temp_tail++ = '.';
	
	do {
		len = sprintf(temp_tail,"%ld",count++);
		temp_tail[len] = '\0';
	} while(_xfile_exists(temp));
	
	return temp;
}

static char *_xunique_name(const char *file, int allow_passthrough){
	if (!_xfile_exists(file))
		return allow_passthrough ? (char *)file : _xstrdup(file);
			
	return _xunique_name_1(file);
}

#define _XDEFAULT_LOGFILE "_xxx_log"
static void _xlog_redirect_output(void){
	char *logfile = _xunique_name(_XDEFAULT_LOGFILE,0);
	fprintf(stderr,"\n%s received, redirecting output to '%s'.\n",
	        _xredirect_request_signal_name, logfile);
	        
	_xlog_fp = fopen(logfile,"w");
	if (!_xlog_fp){
		fprintf(stderr,"%s: %s; disabling logging.\n",logfile,strerror(errno));
		_xlog_inhibit_logging = 1;
	}
	else {
		_xlog_dump_context();
	}
	
	_xfree(logfile);
	_xlog_save_context = 0;
}

static void _xlog_check_redirect_output(void){
	if (_xredirect_request == RR_REQUESTED){
		_xredirect_request = RR_NONE;
		_xlog_redirect_output();
	}
}

static void _xlog_request_redirect_output(const char *signal_name){
	if (_xredirect_request == RR_NONE && _xlog_save_context)
		_xredirect_request = RR_REQUESTED;
	_xredirect_request_signal_name = signal_name;
}

/* implement progressbar */
static int _xnumdigit(long number){
	int cnt = 1;
	if (number < 0){
		number = -number;
		++cnt;
	}
	while ((number /= 10) > 0)
	    ++cnt;
	    
	return cnt;
}

struct _xprogressbar_implementation {
	char *name;
	void *(*create)(long,long);
	void (*update)(void*,long,double);
	void (*finish)(void*,double);
	void (*set_params)(const char *);
};

/* necessary forward declarations */
static void *_xdot_create(long,long);
static void _xdot_update(void*,long,double);
static void _xdot_finish(void*,double);
static void _xdot_set_params(const char *);

static void *_xbar_create(long,long);
static void _xbar_update(void*,long,double);
static void _xbar_finish(void*,double);
static void _xbar_set_params(const char *);

static struct _xprogressbar_implementation _ximplementations[] = {
	{"dot",_xdot_create,_xdot_update,_xdot_finish,_xdot_set_params},
	{"bar",_xbar_create,_xbar_update,_xbar_finish,_xbar_set_params}
};
static struct _xprogressbar_implementation *_xcurrent;
static int _xcurrent_locked;

int _xprogressbar_valid_implementation(const char *name){
	size_t i;
	struct _xprogressbar_implementation *pi = _ximplementations;
	char *colon = strchr(name,':');
	size_t namelen = colon ? (size_t)(colon - name) : strlen(name);
		
	for (i=0; i<sizeof(_ximplementations)/sizeof(_ximplementations[0]); i++,pi++)
	    if (!strncmp(pi->name,name,namelen))
	    	return _XTRUE;
	
	return _XFALSE;
}

void _xprogressbar_set_implementation(const char *name){
	size_t i,namelen;
	struct _xprogressbar_implementation *pi = _ximplementations;
	char *colon;
	
	if (!name)
		name = "bar";
	colon = strchr(name,':');
	namelen = colon ? (size_t)(colon - name) : strlen(name);
		
	for (i=0; i<sizeof(_ximplementations)/sizeof(_ximplementations[0]); i++,pi++)
	    if (!strncmp(pi->name,name,namelen)){
	    	_xcurrent = pi;
	    	_xcurrent_locked = 0;
	    	
	    	if (colon)
	    		++colon;	
	    	if (pi->set_params)
	    		pi->set_params(colon);
	    	return;
	    }
	    
	abort();
}

static int _xoutput_redirected;
void _xprogressbar_output_redirected(void){
	_xoutput_redirected = 1;
}

void *_xprogressbar_create(long initial, long total){
	if (_xoutput_redirected){
		if (!_xcurrent_locked)
			_xprogressbar_set_implementation("dot");
		_xoutput_redirected = 0;
	}
	
	return _xcurrent->create(initial,total);
}

void _xprogressbar_update(void *progress, long howmuch, double dltime){
	return _xcurrent->update(progress,howmuch,dltime);
}

void _xprogressbar_finish(void *progress, double dltime){
	return _xcurrent->finish(progress,dltime);
}

struct _xdot_progress {
	long initial_length;
	long total_length;
	int accumulated;
	int rows;
	int dots;
	double last_timer_value;
};

static void *_xdot_create(long initial,long total){
    struct _xdot_progress *dp = _xcalloc(1,sizeof(struct _xdot_progress));
    dp->initial_length = initial;
    dp->total_length   = total;
    
    if (dp->initial_length){
    	int dot_bytes = opt.dot_bytes;
    	long row_bytes = opt.dot_bytes * opt.dots_in_line;
    	
    	int remainder = (int)(dp->initial_length % row_bytes);
    	long skipped  = dp->initial_length - remainder;
    	
    	if (skipped){
    		int skipped_k = (int)(skipped / 1024);
    		int skipped_k_len = _xnumdigit(skipped_k);
    		if (skipped_k_len < 5)
    			skipped_k_len = 5;
    		_xlog_printf(_XLOG_VERBOSE, "\n%*s[ skipping %dK ]",
    		             2 + skipped_k_len, "", skipped_k);
    	}
    	
    	_xlog_printf(_XLOG_VERBOSE, "\n%5ldK", skipped/1024);
    	for (; remainder >= dot_bytes; remainder -= dot_bytes){
    		if (dp->dots % opt.dot_spacing == 0)
    			_xlog_puts(_XLOG_VERBOSE, " ");
    		_xlog_puts(_XLOG_VERBOSE, ",");
    		++dp->dots;
    	}
    	assert(dp->dots < opt.dots_in_line);
    	dp->accumulated = remainder;
    	dp->rows = skipped / row_bytes;
    }
    
    return dp;
}

static void _xprint_percentage(long bytes, long expected){
	int percentage = (int)(100.0 * bytes / expected);
	_xlog_printf(_XLOG_VERBOSE, "%3d%%", percentage);
}
static void _xprint_download_speed(struct _xdot_progress *dp, long bytes, double dltime){
	_xlog_printf(_XLOG_VERBOSE, " %s", retr_rate(bytes,dltime-dp->last_timer_value,1));
	dp->last_timer_value = dltime;
}

static void _xdot_update(void *progress, long howmuch, double dltime){
	struct _xdot_progress *dp = progress;
	int dot_bytes = opt.dot_bytes;
	long row_bytes = opt.dot_bytes * opt.dots_in_line;
	
	_xlog_set_flush(0);
	dp->accumulated += howmuch;
	for (; dp->accumulated >= dot_bytes; dp->accumulated -= dot_bytes){
		if (dp->dots == 0)
			_xlog_printf(_XLOG_VERBOSE, "\n%5ldK", dp->rows * row_bytes / 1024);
		if (dp->dots % opt.dot_spacing == 0)
			_xlog_puts(_XLOG_VERBOSE, " ");
		_xlog_puts(_XLOG_VERBOSE, ".");
		
		++dp->dots;
		if (dp->dots >= opt.dots_in_line){
			long row_qty = row_bytes;
		    if (dp->rows == dp->initial_length / row_bytes)
			    row_qty -= dp->initial_length % row_bytes;
			    
			++dp->rows;
			dp->dots = 0;
			if (dp->total_length)
		        _xprint_percentage(dp->rows * row_bytes,dp->total_length);
	
		    _xprint_download_speed(dp,row_qty,dltime);
		}
	}

	_xlog_set_flush(1);
}

static void _xdot_finish(void *progress, double dltime){
	struct _xdot_progress *dp = progress;
	int dot_bytes = opt.dot_bytes;
	long row_bytes = opt.dot_bytes * opt.dots_in_line;
	int i;
	
	_xlog_set_flush(0);
	if (dp->dots == 0)
		_xlog_printf(_XLOG_VERBOSE,"\n%5ldK",dp->rows * row_bytes / 1024);
	for (i=dp->dots; i<opt.dots_in_line; i++){
		if (i % opt.dot_spacing == 0)
			_xlog_puts(_XLOG_VERBOSE, " ");
		_xlog_puts(_XLOG_VERBOSE, " ");
	}
	if (dp->total_length){
		_xprint_percentage(dp->rows * row_bytes
		                   + dp->dots * dot_bytes
		                   + dp->accuulated,
		                   dp->total_length);
	}
	if (1){
		long row_qty = dp->dots * dot_bytes + dp->accumulated;
		if (dp->rows == dp->initial_length / row_bytes)
			row_qty -= dp->initial_length % row_bytes;
		_xprint_download_speed(dp,row_qty,dltime);
	}
	_xlog_puts(_XLOG_VERBOSE, "\n\n");
	_xlog_set_flush(0);
	_xfree(dp);
}

static void _xdot_set_params(const char *params){
	if (!params || !*params)
		params = opt.dot_style;
	if (!params)
		return;
	if (!strcasecmp(params,"default")){
		opt.dot_bytes = 1024;
		opt.dot_spacing = 10;
		opt.dots_in_line = 50;
	}
    else if (!strcasecmp(params,"binary")){
    	opt.dot_bytes = 8192;
    	opt.dot_spacing = 16;
    	opt.dots_in_line = 48;
	}
    else if (!strcasecmp(params,"mega")){
    	opt.dot_bytes = 65536L;
    	opt.dot_spacing = 8;
    	opt.dots_in_line = 48;
	}
    else if (!strcasecmp(params,"giga")){
    	opt.dot_bytes = (1L << 20);
    	opt.dot_spacing = 8;
    	opt.dots_in_line = 32;
	}
	else
		fprintf(stderr,"Invalid dot style specification\n");
}

#define _XDEFAULT_SCREEN_WIDTH 80
#define _XMINIMUM_SCREEN_WIDTH 45
#define _XDLSPEED_HISTORY_SIZE 20
#define _XDLSPEED_SAMPLE_MIN   150

static int _xscreen_width = _XDEFAULT_SCREEN_WIDTH;

struct _xbar_progress {
	long initial_length;
	long total_length;
	long count;
	
	double last_screen_update;
	int width;
	char *buffer;
	int tick;
	
	struct _xbar_progress_hist {
		int pos;
		long times[_XDLSPEED_HISTORY_SIZE];
		long bytes[_XDLSPEED_HISTORY_SIZE];
		
		long total_time;
		long total_bytes;
	} hist;
	
	double recent_start;
	long recent_bytes;
	double last_eta_time;
	long last_eta_value;
};

static void _xcreate_image(struct _xbar_progress *, double);
static void _xdisplay_image(char *);

static void *_xbar_create(long initial, long total){
	struct _xbar_progress *bp = _xcalloc(1,sizeof(struct _xbar_progress));
	if (initial > total)
		total = initial;
		
	bp->initial_length = initial;
	bp->total_length   = total;
	bp->width          = _xscreen_width - 1;
	
	_xlog_puts(_XLOG_VERBOSE,"\n");
	
	_xcreate_image(bp,0);
	_xdisplay_image(bp->buffer);
	
	return bp;
}

static void _xupdate_speed_ring(struct _xbar_progress *,long,double);

static void _xbar_update(void *progress, long howmuch, double dltime){
	struct _xbar_progress *bp = progress;
	int force_screen_update   = 0;
	
	bp->count += howmuch;
	if (bp->total_length > 0 &&
		bp->count + bp->initial_length > bp->total_length)
		bp->total_length = bp->initial_length + bp->count;
	
	_xupdate_speed_ring(bp,howmuch,dltime);
	if (_xscreen_width - 1 != bp->width){	
        bp->width = _xscreen_width - 1;
        bp->buffer = _xrealloc(bp->buffer,bp->width+1);
        force_screen_update = 1;
    }
    if (dltime-bp->last_screen_update < 200 && !force_screen_update)
    	return;
	
	_xcreate_image(bp,dltime);
	_xdisplay_image(bp->buffer);
	
	bp->last_screen_update = dltime;
}

static void _xbar_finish(void *progress, double dltime){
    struct _xbar_progress *bp = progress;
    if (bp->total_length > 0 &&
		bp->count + bp->initial_length > bp->total_length)
		bp->total_length = bp->initial_length + bp->count;
		
	_xcreate_image(bp,dltime);
	_xdisplay_image(bp->buffer);
	
	_xlog_puts(_XLOG_VERBOSE,"\n\n");
	
	_xfree(bp->buffer);
	_xfree(bp);
}

static void _xupdate_speed_ring(struct _xbar_progress *bp, long howmuch, double dltime){
	struct _xbar_progress_hist *hist = &bp->hist;
	double recent_age = dltime - bp->recent_start;
	
	bp->recent_bytes += howmuch;
	if (recent_age < _XDLSPEED_SAMPLE_MIN)
		return;
		
	hist->total_time  -= hist->times[hist->pos];
	hist->total_bytes -= hist->bytes[hist->pos];
	
	hist->times[hist->pos] = recent_age;
	hist->bytes[hist->pos] = bp->recent_bytes;
	hist->total_time  += recent_age;
	hist->total_bytes += bp->recent_bytes;
	
	bp->recent_start = dltime;
	bp->recent_bytes = 0;
	
	if (++hist->pos == _XDLSPEED_HISTORY_SIZE)
		hist->pos = 0;
}

#define _XAPPEND_LITERAL(s) do {        \
	    memcpy(p,s,sizeof(s)-1);        \
	    p += sizeof(s) - 1;             \
    } while (0)

static char *_xlegible(long l){
	static char outbuf[48];
	char inbuf[24]={0};
	int i,i1,mod;
	char *outptr,*inptr;
	
	sprintf(inbuf,"%ld",l);
	outptr = outbuf, inptr = inbuf;
	
	if (*inptr == '-'){
		*outptr++ = '-';
		++inptr;
	}
	
	mod = strlen(inptr) % 3;
	for (i=0; i<mod; i++)
	    *outptr++ = inptr[i];
	for (i1=i,i=0; inptr[i1]; i++,i1++){
		if (i % 3 == 0 && i1 != 0)
			*outptr++ = ',';
		*outptr++ = inptr[i1];
	}
	*outptr = '\0';
	
	return outbuf;
}

static double _xcalc_rate(long bytes, double msecs, int *units){
	double dlrate;
	
	assert(msecs >= 0);
	assert(bytes >= 0);
	
	dlrate = (double)1000 * bytes / msecs;
	if (dlrate < 1024.0)
		*units = 0;
	else if (dlrate < 1024.0 * 1024.0)
		*units = 1, dlrate /= 1024.0;
	else if (dlrate < 1024.0 * 1024.0 * 1024.0)
		*units = 2, dlrate /= (1024.0 * 1024.0);
	else
		*units = 3, dlrate /= (1024.0 * 1024.0 * 1024.0);
		
	return dlrate;
}
  
static void _xcreate_image(struct _xbar_progress *bp, double dl_total_time){
	char *p = bp->buffer;
	long size = bp->initial_length + bp->count;
	
	char *size_legible = _xlegible(size);
	int size_legible_len = strlen(size_legible);
	
	struct _xbar_progress_hist *hist = &bp->hist;
	
	int dlbytes_size = 1 + _xmax(size_legible_len,11);
	int progress_size = bp->width - (4 + 2 + dlbytes_size + 11 + 13);
	if (progress_size < 5)
		progress_size = 0;
		
	if (bp->total_length > 0){
		int percentage = (int)(100.0 * size / bp->total_length);
		assert(percentage <= 100);
		if (percentage < 100)
			sprintf(p,"%2d%%",percentage);
		else
			strcpy(p,"100%");
			
		p += 4;
	}
	else {
		_XAPPEND_LITERAL("    ");
	}
	
	if (progress_size && bp->total_length > 0){
		int insz = (double)bp->initial_length / bp->total_length * progress_size;
		int dlsz = (double)size / bp->total_length * progress_size;
		
		char *begin;
		int i;
		
		assert(dlsz <= progress_size);
		assert(insz <= dlsz);
		
		*p++ = '[';
		begin = p;
		for (i=0; i< insz; i++)
		    *p++ = '+';
		dlsz -= insz;
		if (dlsz > 0){
			for (i=0; i<dlsz-1; i++)
			    *p++ = '=';
			*p++ = '>';
		}
		
		while (p-begin < progress_size)
		    *p++ = ' ';
		
		*p++ = ']';
	}
    else if (progress_size){
    	int ind = bp->tick % (progress_size * 2 - 6);
    	int i,pos;
    	
    	if (ind < progress_size - 2)
    		pos = ind + 1;
    	else
    		pos = progress_size - (ind - progress_size + 5);
    		
    	*p++ = '[';
    	for (i=0; i<progress_size; i++){
    		if      (i == pos - 1) *p++ = '<';
    		else if (i == pos    ) *p++ = '=';
    		else if (i == pos + 1) *p++ = '>';
    		else
    			*p++ = ' ';
    	}
    	
    	*p++ = ']';
    	++bp->tick;
	}
	
	sprintf(p," %-11s", _xlegible(size));
	p += strlen(p);
	
	if (hist->total_time && hist->total_bytes){
		static char *short_units[] = {"B/s","K/s","M/s","G/s"};
		int units = 0;
		
		long dlquant = hist->total_bytes + bp->recent_bytes;
		double dltime = hist->total_time + (dl_total_time - bp->recent_start);
		double dlspeed = _xcalc_rate(dlquant,dltime,&units);
		sprintf(p," %7.2f%s",dlspeed,short_units[units]);
		p += strlen(p);
	}
	else {
		_XAPPEND_LITERAL("  --.--K/s");
	}
	
	if (bp->total_length > 0 && dl_total_time > 3000){
		long eta;
		int eta_hrs, eta_min, eta_sec;
		
		if (bp->total_length != size &&
			bp->last_eta_value != 0  &&
			dl_total_time - bp->last_eta_time < 900)
			eta = bp->last_eta_value;
		else {
			double time_sofar = (double)dl_total_time / 1000;
			long bytes_remaining = bp->total_length - size;
			eta = (long)(time_sofar * bytes_remaining / bp->count);
			
			bp->last_eta_value = eta;
			bp->last_eta_time  = dl_total_time;
		}
		
		eta_hrs = eta / 3600, eta %= 3600;
		eta_min = eta / 60,   eta %= 60;
		eta_sec = eta;
		
		if (eta_hrs > 99)
			goto no_eta;
		
		if (eta_hrs == 0){
			_XAPPEND_LITERAL("    ");
			sprintf(p," ETA %02d:%02d",eta_min,eta_sec);
		}
		else {
			if (eta_hrs < 10)
				*p++ = ' ';
			sprintf(p," ETA %d:%02d:%02d",eta_hrs,eta_min,eta_sec);
		}
		p += strlen(p);
	}
    else if (bp->total_length > 0){
    no_eta:
        _XAPPEND_LITERAL("            ");	
	}
	
	assert(p-bp->buffer <= bp->width);
	while (p<bp->buffer+bp->width)
	    *p++ = ' ';
	*p = '\0';
}

static int _xdetermine_screen_width(void){
#ifndef TIOCGWINSZ
    return 0;
#else
    int fd;
    struct winsize wsz;
    
    if (opt.lfilename != NULL)
    	return 0;
    	
    fd = fileno(stderr);
    if (ioctl(fd,TIOCGWINSZ,&wsz) < 0)
    	return 0;
    	
    return wsz.ws_col;
#endif
}

static void _xbar_set_params(const char *params){
	int sw;
	char *term = getenv("TERM");
	
	if (params && !strcmp(params,"force"))
		_xcurrent_locked = 1;
		
	if ((opt.lfilename || !isatty(fileno(stderr)) || (term && !strcmp(term,"emacs"))) &&
		!_xcurrent_locked){
		_xprogressbar_set_implementation("dot");
		return;
	}
	
	sw = _xdetermine_screen_width();
	if (sw && sw >= _XMINIMUM_SCREEN_WIDTH)
		_xscreen_width = sw;
}

static void _xdisplay_image(char *buf){
	int old = _xlog_set_save_context(0);
	_xlog_puts(_XLOG_VERBOSE,"\r");
	_xlog_puts(_XLOG_VERBOSE,buf);
	_xlog_set_save_context(old);
}
