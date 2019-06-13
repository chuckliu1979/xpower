/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** util routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <ctype.h>
#include <errno.h>
#include "xpower-util.h"

#ifdef _XMATH

float _xutil_sqrt(float val){
    union {int i; float f;} c1,c2;
    
    c1.f = val;
    c2.f = val;
    c1.i = 0x1FBCF800 + (c1.i >> 1);
    c2.i = 0x5f3759df - (c2.i >> 1);
    
    return 0.5f*(c1.f + (val * c2.f));
}

float _xutil_fabs(float val){
	int tmp = *(int *)&val;
	tmp &= 0x7FFFFFFF;
	return *(float *)&tmp;
}

/*!
 * return 1/sqrt(val), a fast John Carmack implementation
 */
float _xutil_rsqrt(float val){
	long i;
	float x,y;
	
	x = val * 0.5f;
	y = val;
	i = *(long*)&y; /* evil floating point bit level hacking */
	i = 0x5f3759df - (i >> 1);
	y = *(float*)&i;
	y = y*(1.5f - (x*y*y));
	
	return y;
}

#endif

char *_xutil_basename(char *path){
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #define _xhas_dos_drive_prefix(path) (isalpha(*(path)) && (path)[1] == ':')
  #define _xis_dir_sep(c)              ((c) == '/' || (c) == '\\')
#else
  #define _xhas_dos_drive_prefix(path) 0
  #define _xis_dir_sep(c)              ((c) == '/')
#endif

	const char *base;
	/* skip over the disk name in MSDOS pathnames */
	if (_xhas_dos_drive_prefix(path))
		path += 2;
	for (base=path; *path; path++){
		if (_xis_dir_sep(*path))
			base = path + 1;
	}
	return (char *)base;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <windows.h>
static void __xclear(HANDLE hConsole){
	COORD coordScreen = {0,0}; /*here's where we'll home the cursor*/

	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;/*to get buffer info*/
	DWORD dwConSize;                /*number of character cells in the current buffer*/

	/*get the number of character cells in the current buffer*/
	bSuccess = GetConsoleScreenBufferInfo(hConsole,&csbi);
	dwConSize = csbi.dwSize.X*csbi.dwSize.Y;

	/*fill the entire screen with blanks*/
	bSuccess = FillConsoleOutputCharacter(hConsole,(TCHAR)' ',dwConSize,coordScreen,&cCharsWritten);

	/*get the current text attribute*/
	bSuccess = GetConsoleScreenBufferInfo(hConsole,&csbi);

	/*now set the buffer's attributes accordingly*/
	bSuccess = FillConsoleOutputAttribute(hConsole,csbi.wAttributes,dwConSize,coordScreen,&cCharsWritten);

	/*put the cursor at (0,0)*/
	bSuccess = SetConsoleCursorPosition(hConsole,coordScreen);
}
void _xutil_clear(FILE *fd){
	HANDLE hConsole;

	if (fd == stdout){
		if (!isatty(_fileno(stdout))) return;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		__xclear(hConsole);
		return;
	}

	if (fd == stderr){
		if (!isatty(_fileno(stderr))) return;
		hConsole = GetStdHandle(STD_ERROR_HANDLE);
		__xclear(hConsole);
		return;
	}
}
#else
void _xutil_clear(FILE *fd){
	if (isatty(_fileno(fd))){
		fprintf(fd, "%c%c%c%c%c%c",27,'[','H',27,'[','J');
	}
}
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable:4996)
#include <windows.h>

int _xutil_call_command(char **nargv){
	if (nargv){
		int i=0,ret;
		FILE *fp=NULL;
	    char *argv,cmd[1024]={0};

	    STARTUPINFO si;
	    PROCESS_INFORMATION pi;
	    memset(&pi,0,sizeof(pi));
	    memset(&si,0,sizeof(si));
	    si.cb = sizeof(si);
		/*XXX redirect stdout/stderr to nul ???
		 * fp = fopen("nul","wb");
		 */
		while (argv=nargv[i++]){
			strcat(cmd,argv);
			cmd[strlen(cmd)] = '\x20';
		}
		ret = CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, FALSE, 
		                    NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
		if (ret){
			DWORD exitcode;
			WaitForSingleObject(pi.hProcess,INFINITE);
			ret = GetExitCodeProcess(pi.hProcess,&exitcode);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			if (ret)
				return (int)(exitcode);
		}
	}

	return -1;
}

void _xutil_sleep(int milliseconds){
	Sleep((DWORD)milliseconds);
}

#else
#include <unistd.h>

int _xutil_call_command(char **nargv){
	int ret,cpid,wpid;
	switch (cpid=fork()){
	case 0 : /*child*/
	    /*
         * command should run in same locale as caller,
         * for locale-sensitive functions like getdate()
         */
		(void) putenv("LC_ALL=C");
	    if(freopen("/dev/null", "w+", stdout) == NULL ||
           freopen("/dev/null", "w+", stderr) == NULL ||
           execvp(nargv[0], nargv) == -1)
           exit(EX_FAILURE);
		break;
	case -1 : /*error*/
		return (EX_FAILURE);
	default : /*parent*/
		while ((wpid=wait(&ret)) != cpid){
			if (wpid == -1) return (EX_FAILURE);
		}
		ret = (ret >> 8) & 0xFF;
	}

	return ret;
}

void _xutil_sleep(int milliseconds){
	int rval;
    struct timespec timeout;
    
    timeout.tv_sec  = milliseconds/1000; milliseconds %= 1000;
    time0ut.tv_nsec = milliseconds*1000000;
    
    while (1){
    	/* sleep for the time specified, if interrupted by
    	 * a signal, place the remaining time left to sleep
    	 * back
    	 */
        rval = nanosleep(&timeout,&timeout);
        if (rval && errno == EINTR)
        	continue;
        else
        	break;
    }
}

#endif

/*!
 * classical Runge-Kutta method (4th order Runge-Kutta method)
 * let an initial value problem be specified as following :
 *
 * y' = f(t,y) y(t0) = y0
 *
 * the rk4 method for this problem is given by the following equations:
 *
 * yn+1 = yn + 1/6(k1 + 2k2 + 2k3 + k4)
 * tn+1 = tn + h
 *
 * where yn+1 is the rk4 approximation of y(tn+1),and
 * k1 = hf(tn, yn)
 * k2 = hf(tn + 1/2h, yn + 1/2k1)
 * k3 = hf(tn + 1/2h, yn + 1/2k2)
 * k4 = hf(tn + h, yn + k3)
 *
 */
double _xutil_rk4(_xvec *(*f)(double,_xvec *,_xvec *),double t,double h,_xvec *x){
    _XSTATIC _xvec *v1=(_xvec *)NULL, *v2=(_xvec *)NULL;
    _XSTATIC _xvec *v3=(_xvec *)NULL, *v4=(_xvec *)NULL;
    _XSTATIC _xvec *temp=(_xvec *)NULL;
    
    /*do not work with NULL initial vector*/
    if (!x) _xerror(_XE_NULL,"_xutil_rk4");
    
    /*ensure that v1,...v4,temp are of the correct size*/
    v1 = _xvec_resize(v1, x->dim);
    v2 = _xvec_resize(v2, x->dim);
    v3 = _xvec_resize(v3, x->dim);
    v4 = _xvec_resize(v4, x->dim);
    temp = _xvec_resize(temp, x->dim);
    
    /*register workspace variables*/
    _XMEM_STAT_REG(v1,_XTYPE_VEC);
    _XMEM_STAT_REG(v2,_XTYPE_VEC);
    _XMEM_STAT_REG(v3,_XTYPE_VEC);
    _XMEM_STAT_REG(v4,_XTYPE_VEC);
    _XMEM_STAT_REG(temp,_XTYPE_VEC);
    
    f(t,x,v1);_xvec_mltadd(x,v1,0.5*h,temp);         /* temp=x+.5*h*v1 */
    f(t+0.5*h,temp,v2);_xvec_mltadd(x,v2,0.5*h,temp);/* temp=x+.5*h*v2 */
    f(t+0.5*h,temp,v3);_xvec_mltadd(x,v3,h,temp);    /* temp=x+h*v3 */
    f(t+h,temp,v4);
    
    /*now add v1+2*v2+2*v3+v4*/
    _xvec_vcopy(v1,temp);          /* temp=v1 */
    _xvec_mltadd(temp,v2,2.0,temp);/* temp=v1+2*v2 */
    _xvec_mltadd(temp,v3,2.0,temp);/* temp=v1+2*v2+2*v3 */
    _xvec_add(temp,v4,temp);       /* temp=v1+2*v2+2*v3+v4 */
    
    /*adjust x*/
    _xvec_mltadd(x,temp,h/6.0,x);  /* x=x+(h/6)*temp */
    
#ifdef _XTHREAD_SAFE
	_xvec_vfree(v1);_xvec_vfree(v2);
	_xvec_vfree(v3);_xvec_vfree(v4);_xvec_vfree(temp);
#endif

    /*new time*/
    return t+h;
}

