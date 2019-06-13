/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** power flow solution routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <time.h>
#include <math.h>
#include <ctype.h>
#include "xpower-def.h"
#include "xpower-util.h"
#include "xpower-powerflow.h"
#include "./compat/ieee/ieee-testcase.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable:4996 4018)
#endif

#define _XPI 3.1415926535897932

void _xzspmat_foutput(_xzspmat *zspmat, FILE *fd){
	unsigned int i,j;
	_xspmat *im,*re;
	
	if (!zspmat || !fd)
		_xerror(_XE_NULL,"_xzspmat_foutput");
		
	im = zspmat->im, re = zspmat->re;
	if (!im)
		_xerror(_XE_NULL,"_xzspmat_foutput");
	if (re){
		if (re->m < im->m || re->n < im->n)
			_xerror(_XE_SIZES,"_xzspmat_foutput");
	}
	
	for (i=0; i<im->n; i++){
		for (j=0; j<im->m; j++){
			_xsprow *r = im->row + j;
			int idx = _xsprow_idx(r,i);
			if (idx >= 0){
				double vre = re ? _xspmat_get_val(re,j,i) : 0.0;
				double vim = _xspmat_get_val(im,j,i);
				fprintf(fd,"\t(%3d,%3d)\t%7.4f %s %7.4fi\n",j+1,i+1,
				        vre,vim >= 0.0 ? "+" : "-", fabs(vim));
			}
		}
	}	
	
}

/*!
 * initialize xpc struct with zero values
 */
void _xpowerflow_init_xpc(_xpowerflow_xpc *xpc){
	_xmem_zero(xpc, sizeof(_xpowerflow_xpc));
}

/*!
 * deallocate the xpc struct and reset with zero values
 */
void _xpowerflow_free_xpc(_xpowerflow_xpc *xpc){
	if (xpc){
		_xivec_ifree(xpc->order.i2e);

		_xfree(xpc->bus.bucket);
		_xfree(xpc->gen.bucket);
		_xfree(xpc->branch.bucket);
		_xfree(xpc->areas.bucket);
		_xfree(xpc->gencost.bucket);

		_xmat_mfree(xpc->A);
	    _xmat_mfree(xpc->N);

	    _xmem_zero(xpc, sizeof(_xpowerflow_xpc));
	}
}

/*!
 * dump the xpc for debug purpose.
 */
void _xpowerflow_dump_xpc(_xpowerflow_xpc *xpc, FILE *fd){
	unsigned int i,len;

	if (!xpc){
		fprintf(fd,"_xpowerflow_xpc: NULL\n");
		return;
	}
	if (xpc->order.state != 'i' && xpc->order.state != 'e'){
		fprintf(fd,"_xpowerflow_xpc: state out of [\'i\' \'e\']\n");
		return;
	}
	fprintf(fd,"_xpowerflow_xpc: state [\'%c\']\n",xpc->order.state);
	fprintf(fd,"xpc.version = \'%d\'\n",xpc->version);
	fprintf(fd,"xpc.baseMVA = %g\n",xpc->baseMVA);

	fprintf(fd,"xpc.bus     = [\n");
	len = xpc->order.state == 'i' ? xpc->bus.ilen : xpc->bus.elen;
	for (i=0; i<len; i++){
		_xpowerflow_bus *bus = xpc->bus.bucket + i;
		fprintf(fd,"%5d %3d %7.2f %7.2f %7.2f %7.2f %3d %7.4f %7.2f %7g %3d %7.2f %7.2f\n",
			    bus->bus_i,
				bus->bus_type,
				bus->Pd,
				bus->Qd,
				bus->Gs,
				bus->Bs,
				bus->bus_area,
				bus->Vm,
				bus->Va,
				bus->base_kV,
				bus->zone,
				bus->Vmax,
				bus->Vmin);
	}
	fprintf(fd,"]\n");

	fprintf(fd,"xpc.gen     = [\n");
	len = xpc->order.state == 'i' ? xpc->gen.ilen : xpc->gen.elen;
	for (i=0; i<len; i++){
		_xpowerflow_gen *gen = xpc->gen.bucket + i;
		fprintf(fd,"%4d %7.2f %7.2f %7.2f %7.2f %7.4f %g %2d %7.2f %7.2f %g %g %g %g %g %g %g %g %g %g %g\n",
			    gen->gen_bus,
				gen->Pg,
				gen->Qg,
				gen->Qmax,
				gen->Qmin,
				gen->Vg,
				gen->mBase,
				gen->gen_status,
				gen->Pmax,
				gen->Pmin,
				gen->Pc1,
				gen->Pc2,
				gen->Qc1min,
				gen->Qc1max,
				gen->Qc2min,
				gen->Qc2max,
				gen->ramp_agc,
				gen->ramp_10,
				gen->ramp_30,
				gen->ramp_q,
				gen->apf);
	}
	fprintf(fd,"]\n");

	fprintf(fd,"xpc.branch  = [\n");
	len = xpc->order.state == 'i' ? xpc->branch.ilen : xpc->branch.elen;
	for (i=0; i<len; i++){
		_xpowerflow_branch *branch = xpc->branch.bucket + i;
		fprintf(fd,"%5d %5d %8.5f %8.5f %8.5f %g %g %g %7.3f %7.2f %3d %g %g\n",
			    branch->f_bus,
				branch->t_bus,
				branch->br_r,
				branch->br_x,
				branch->br_b,
				branch->rate_a,
				branch->rate_b,
				branch->rate_c,
				branch->tap,
				branch->shift,
				branch->br_status,
				branch->ang_min,
				branch->ang_max);
	}
	fprintf(fd,"]\n");

	if (xpc->version == 1){
	    fprintf(fd,"xpc.areas   = [\n");
	    len = xpc->order.state == 'i' ? xpc->areas.ilen : xpc->areas.elen;
	    for (i=0; i<len; i++){
		    _xpowerflow_areas *areas = xpc->areas.bucket + i;
		    fprintf(fd,"%3d %5d\n",areas->area_i, areas->price_ref_bus);
	    }
	    fprintf(fd,"]\n");
	}

	fprintf(fd,"xpc.gencost = [\n");
	len = xpc->order.state == 'i' ? xpc->gencost.ilen : xpc->gencost.elen;
	for (i=0; i<len; i++){
		_xpowerflow_gencost *gencost = xpc->gencost.bucket + i;
		fprintf(fd,"%3d %3d %3d %5d %9.7f %g %g\n",
			    gencost->model,
				gencost->startup,
				gencost->shutdown,
				gencost->ncost,
				gencost->cost[0],
				gencost->cost[1],
				gencost->cost[2]);
	}
	fprintf(fd,"]\n");
}

/*!
 * convert data from IEEE Common Data Format
 *
 * The IEEE CDF does not include some data need to run an optimal power
 * flow. This script creates default values for some of this data as
 * follows:
 *
 *     Bus data:
 *         Vmin = 0.94 p.u.
 *         Vmax = 1.06 p.u.
 *     Gen data:
 *         Pmin = 0 MW
 *         Pmax = Pg + baseMVA
 *     Gen cost data:
 *         Quadratic costs with:
 *             c2 = 10 / Pg, c1 = 20, c0 = 0, if Pg is non-zero, and
 *             c2 = 0.01,    c1 = 40, c0 = 0, if Pg is zero
 */
int _xpowerflow_cdf2matp(const char *cdf_file,_xpowerflow_xpc *xpc){
    FILE *fd;
    unsigned int i,igen=0,refgen;
    double total_pd = 0.0,total_pg = 0.0;
    _xieee_bus *ieeebus;
    _xieee_branch *ieeebranch;
    _xpowerflow_bus *bus;
    _xpowerflow_gen *gen;
    _xpowerflow_branch *branch;
    _xpowerflow_gencost *gencost;
    
    if (!cdf_file || !xpc)
    	_xerror(_XE_NULL,"_xpowerflow_ieee_cdf");
    if ((fd = fopen(cdf_file,"r")) == NULL)
    	return _XFALSE;
    if (!_xieee_cdf_input(fd)){
    	fclose(fd);
    	_xieee_cdf_free();
    	return _XFALSE;
    }
	xpc->version = 2,xpc->order.state = 'e';

    /* get baseMVA */
	xpc->baseMVA = (_xreal)(_xieee_cdf_title.base_mva);
    	
    /* get bus data */
	xpc->bus.elen = _xieee_cdf_bus_size;
	xpc->bus.bucket = (_xpowerflow_bus *)_xcalloc(xpc->bus.elen,sizeof(_xpowerflow_bus));
    for (i=0; i<_xieee_cdf_bus_size; i++){
    	ieeebus = _xieee_cdf_bus + i;

    	bus = xpc->bus.bucket + i;
    	bus->bus_i    = ieeebus->number;
    	bus->bus_type = ieeebus->type;
    	
    	if (bus->bus_type == 0) bus->bus_type = 1;
    	if (bus->bus_type < 2)
    		bus->Pd = ieeebus->load_mw - ieeebus->gen_mw;
    	else if (bus->bus_type >= 2){
    		igen++;
    		bus->Pd = ieeebus->load_mw;
    	}
    	
    	bus->Qd = ieeebus->load_mvar;
    	bus->Gs = xpc->baseMVA*ieeebus->g;
    	bus->Bs = xpc->baseMVA*ieeebus->b;
    	bus->bus_area = ieeebus->area;
    	bus->Vm = ieeebus->voltage;
    	bus->Va = ieeebus->angle;
    	bus->base_kV = ieeebus->base_kv;
    	bus->zone = ieeebus->zone;
    	bus->Vmax = 1.06f;
    	bus->Vmin = 0.94f;
    	
    	total_pd += bus->Pd;	
    }

	xpc->gen.elen = xpc->gencost.elen = igen;
	xpc->gen.bucket = (_xpowerflow_gen *)_xcalloc(igen,sizeof(_xpowerflow_gen));
	xpc->gencost.bucket = (_xpowerflow_gencost *)_xcalloc(igen,sizeof(_xpowerflow_gencost));
    for (i=igen=0; i<_xieee_cdf_bus_size; i++){
    	ieeebus = _xieee_cdf_bus + i;
    	if (ieeebus->type < 2) continue;
    	if (ieeebus->type == 3) refgen = igen;
    			
    	gen = xpc->gen.bucket + igen;
    	gen->gen_bus = ieeebus->number;
    	gen->Pg = ieeebus->gen_mw;
    	
    	if (gen->Pg < 0.0f){/* negative Pg is transformed as load */
			bus = xpc->bus.bucket + i;
    		bus->Pd = bus->Pd - gen->Pg;
    		total_pd -= gen->Pg;
    		gen->Pg = 0.0f;
    	}
    	
    	gen->Qg = ieeebus->gen_mvar;
    	gen->Qmax = ieeebus->max_mvar;
    	gen->Qmin = ieeebus->min_mvar;
    	if (gen->Qmax - gen->Qmin < 0.01)/* Qmax is modified */
			gen->Qmax = gen->Qmin + 0.1*xpc->baseMVA;
    	gen->Vg = ieeebus->desired_volt;
		gen->mBase = xpc->baseMVA;
    	gen->gen_status = 1;
		gen->Pmax = gen->Pg + xpc->baseMVA;
    	gen->Pmin = 0.0f;
    	
		gencost = xpc->gencost.bucket + igen;
    	gencost->model    = _XCOST_POLYNOMIAL;
    	gencost->startup  = 0;
    	gencost->shutdown = 0;
    	gencost->ncost    = 3;
    	
    	gencost->cost = (_xreal *)_xcalloc(3,sizeof(_xreal));
    	if (gencost->cost == NULL) return _XFALSE;
    	gencost->cost[0] = 0.01f;
    	gencost->cost[1] = 0.3f;
    	gencost->cost[2] = 0.2f;
    	
    	total_pg += gen->Pg;
    	igen++;
    }
    if (total_pg < 1.04*total_pd){/* Pg at slack bus is modified */
		gen = xpc->gen.bucket + refgen;
    	gen->Pmax = gen->Pg + 1.1*total_pd - total_pg;
    }

    /*set up the cost coefficients of generators */
	for (i=0; i<xpc->gen.elen; i++){
		gencost = xpc->gencost.bucket + i;
    	
		gen = xpc->gen.bucket + i;
    	if (gen->Pg == 0.0f){
            gencost->cost[0] = 0.01f;
            gencost->cost[1] = 40.0f;
    	}
    	else {
    		gencost->cost[0] = 10.0 / gen->Pg;
    		gencost->cost[1] = 20.0f;
    	}
    	gencost->cost[2] = 0.0f;
    }
    
    /* get branch data */
	xpc->branch.elen = _xieee_cdf_branch_size;
	xpc->branch.bucket = (_xpowerflow_branch *)_xcalloc(xpc->branch.elen,sizeof(_xpowerflow_branch));
    for (i=0; i<_xieee_cdf_branch_size; i++){
    	ieeebranch = _xieee_cdf_branch + i;
    	
		branch = xpc->branch.bucket + i;
    	branch->f_bus = ieeebranch->tapbus;
    	branch->t_bus = ieeebranch->zbus;	
        branch->br_r  = ieeebranch->r;
        branch->br_x  = ieeebranch->x;
        branch->br_b  = ieeebranch->b;
        
        branch->rate_a = ieeebranch->mva_rating1;
        if (branch->rate_a < 0.000001)
			branch->rate_a = 99 * xpc->baseMVA;
        	
        branch->rate_b = ieeebranch->mva_rating2;
        branch->rate_c = ieeebranch->mva_rating3;
        branch->tap    = ieeebranch->ratio;
        branch->shift  = 0.0f;
        branch->br_status = 1;
		branch->ang_min = -360.0f;
		branch->ang_max =  360.0f;
    }
   
    /* free ieee cdf*/
    fclose(fd);
    _xieee_cdf_free();
    
    return _XTRUE;
}

/*!
 * load .m case files or data struct in MATPOWER format
 */
int _xpowerflow_loadcase(const char *casefile, _xpowerflow_xpc *xpc){
	FILE *fd;
	int i,j,ret,done,once;
	long pos;
	size_t len;
	const char *str;
	char *p,line[1024]={0};

	if (!casefile || !xpc)
		_xerror(_XE_NULL,"_xpowerflow_loadcase");

	/* check for explicit extension */
	p = _xutil_basename((char *)casefile);
	while (*p && *p != '.') p++;
	if (*p == '\0') return _XFALSE;
	if (strncmp(p,".mat",4)){
		if (*(p+1) != 'm' || *(p+2) != '\0')
			return _XFALSE;
	}
	else {
		/* in fact, we cannot support .mat binary file*/
		return _XFALSE;
	}

	if ((fd = fopen(casefile,"rb")) == NULL)
		return _XFALSE;

	ret = _XFALSE;
	xpc->order.state = 'e';

	/* function header */
    #define _xfunction_header1 "function [baseMVA, bus, gen, branch, areas, gencost] = "
    #define _xfunction_header2 "function [baseMVA, bus, gen, branch] = "
    #define _xfunction_header3 "function mpc = "
	xpc->version = 0;

	if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	line[strlen(line)-1] = '\0';/* remove new-line */
	if (strncmp(line,_xfunction_header1,strlen(_xfunction_header1))){
		if (strncmp(line,_xfunction_header2,strlen(_xfunction_header2))){
			if (strncmp(line,_xfunction_header3,strlen(_xfunction_header3)))
				goto DONE;
			else
				xpc->version = 2;
		}
	}
	if (xpc->version < 2)
		xpc->version = 1;

	/* baseMVA header */
    #define _xbaseMVA_header1 "baseMVA = "
	#define _xbaseMVA_header2 "mpc.baseMVA = "
	if (xpc->version == 1)
		str=_xbaseMVA_header1,len=strlen(_xbaseMVA_header1);
	else
		str=_xbaseMVA_header2;len=strlen(_xbaseMVA_header2);
	done = _XFALSE; 
	do {
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,str,len)){
			done = _XTRUE;
			p = line + len;
			while (*p && isspace((int)(*p))) p++;
			if (*p){
				double val;
				if (sscanf(p,"%Lf;",&val) == 1){
					xpc->baseMVA = (_xreal)val;
				    break;
				}
			}
		}
	} while (!feof(fd));
	if (!done) goto DONE;

	/* bus data */
    #define _xbus_header1 "bus = ["
	#define _xbus_header2 "mpc.bus = ["
	if (xpc->version == 1)
		str=_xbus_header1,len=strlen(_xbus_header1);
	else
		str=_xbus_header2;len=strlen(_xbus_header2); 
	pos = ftell(fd),once = _XTRUE;

BUS:
	done = _XFALSE;
	do {
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,str,len)){
			done = _XTRUE;
			break;
		}
	} while (!feof(fd));
	if (!done) goto DONE;
	if (once){
	    i = 0, done = _XFALSE;
	    do {
			if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	        line[strlen(line)-1] = '\0';/* remove new-line */
		    if (!strncmp(line,"];",2)){
			    done = _XTRUE;
			    break;
		    }
		    i++;
	    } while (!feof(fd));
	    if (!done || !i) goto DONE;
	}
	if (once){	
		once = _XFALSE;fseek(fd,pos,0);goto BUS;
	}

	xpc->bus.elen = i; _xfree(xpc->bus.bucket);
	xpc->bus.bucket = (_xpowerflow_bus *)_xcalloc(xpc->bus.elen, sizeof(_xpowerflow_bus));
	i = 0;
	do {
		_xpowerflow_bus *bus = xpc->bus.bucket + i;
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,"];",2))
			break;

#if _XREAL == _XDOUBLE
  #define _xbus_format "\t%d\t%d\t%Lf\t%Lf\t%Lf\t%Lf\t%d\t%Lf\t%Lf\t%Lf\t%d\t%Lf\t%Lf[\t;]"
#else
  #define _xbus_format "\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%d\t%lf\t%lf\t%lf\t%d\t%lf\t%lf[\t;]"
#endif
		j = sscanf(line,_xbus_format,
			       &(bus->bus_i),
				   &(bus->bus_type),
				   &(bus->Pd),
				   &(bus->Qd),
				   &(bus->Gs),
				   &(bus->Bs),
				   &(bus->bus_area),
				   &(bus->Vm),
				   &(bus->Va),
				   &(bus->base_kV),
				   &(bus->zone),
				   &(bus->Vmax),
				   &(bus->Vmin));
		if (j != 13) goto DONE;
		i++;
	} while (i<xpc->bus.elen && !feof(fd));
	if (i < xpc->bus.elen) goto DONE;

	/* gen data */
    #define _xgen_header1 "gen = ["
	#define _xgen_header2 "mpc.gen = ["
	if (xpc->version == 1)
		str=_xgen_header1,len=strlen(_xgen_header1);
	else
		str=_xgen_header2;len=strlen(_xgen_header2); 
	pos = ftell(fd),once = _XTRUE;

GEN:
	done = _XFALSE;
	do {
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,str,len)){
			done = _XTRUE;
			break;
		}
	} while (!feof(fd));
	if (!done) goto DONE;
	if (once){
	    i = 0, done = _XFALSE;
	    do {
	        if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	        line[strlen(line)-1] = '\0';/* remove new-line */
		    if (!strncmp(line,"];",2)){
			    done = _XTRUE;
			    break;
		    }
		    i++;
	    } while (!feof(fd));
	    if (!done || !i) goto DONE;
	}
	if (once){
		once = _XFALSE;fseek(fd,pos,0);goto GEN;
	}

	xpc->gen.elen = i; _xfree(xpc->gen.bucket);
	xpc->gen.bucket = (_xpowerflow_gen *)_xcalloc(xpc->gen.elen, sizeof(_xpowerflow_gen));
	i = 0;
	do {
		_xpowerflow_gen *gen = xpc->gen.bucket + i;
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,"];",2))
			break;

#if _XREAL == _XDOUBLE
  #define _xgen_format1 "\t%d\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%d\t%Lf\t%Lf[\t;]"
  #define _xgen_format2 "%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf[\t;]"
#else
  #define _xgen_format1 "\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d\t%lf\t%lf[\t;]"
  #define _xgen_format2 "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf[\t;]"
#endif
		j = sscanf(line,_xgen_format1,
			       &(gen->gen_bus),
				   &(gen->Pg),
				   &(gen->Qg),
				   &(gen->Qmax),
				   &(gen->Qmin),
				   &(gen->Vg),
				   &(gen->mBase),
				   &(gen->gen_status),
				   &(gen->Pmax),
				   &(gen->Pmin));
		if (j != 10) goto DONE;
		if (xpc->version == 2){
			p = line, j = 0;
			while (j<11){
				if (*p == '\t') j++;
				p++;
			}
		    j = sscanf(p,_xgen_format2,
					   &(gen->Pc1),
					   &(gen->Pc2),
					   &(gen->Qc1min),
					   &(gen->Qc1max),
					   &(gen->Qc2min),
					   &(gen->Qc2max),
					   &(gen->ramp_agc),
					   &(gen->ramp_10),
					   &(gen->ramp_30),
					   &(gen->ramp_q),
					   &(gen->apf));
		    if (j != 11) goto DONE;
		}
		i++;
	} while (i<xpc->gen.elen && !feof(fd));
	if (i < xpc->gen.elen) goto DONE;

	/* branch data */
    #define _xbranch_header1 "branch = ["
	#define _xbranch_header2 "mpc.branch = ["
	if (xpc->version == 1)
		str=_xbranch_header1,len=strlen(_xbranch_header1);
	else
		str=_xbranch_header2;len=strlen(_xbranch_header2); 
	pos = ftell(fd),once = _XTRUE;

BRANCH:
	done = _XFALSE;
	do {
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,str,len)){
			done = _XTRUE;
			break;
		}
	} while (!feof(fd));
	if (!done) goto DONE;
	if (once){
	    i = 0, done = _XFALSE;
	    do {
	        if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	        line[strlen(line)-1] = '\0';/* remove new-line */
		    if (!strncmp(line,"];",2)){
			    done = _XTRUE;
			    break;
		    }
		    i++;
	    } while (!feof(fd));
	    if (!done || !i) goto DONE;
	}
	if (once){
		once = _XFALSE;fseek(fd,pos,0);goto BRANCH;
	}

	xpc->branch.elen = i; _xfree(xpc->branch.bucket);
	xpc->branch.bucket = (_xpowerflow_branch *)_xcalloc(xpc->branch.elen, sizeof(_xpowerflow_branch));
	i = 0;
	do {
		_xpowerflow_branch *branch = xpc->branch.bucket + i;
	    if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	    line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,"];",2))
			break;

#if _XREAL == _XDOUBLE
  #define _xbranch_format1 "\t%d\t%d\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%Lf\t%d[\t;]"
  #define _xbranch_format2 "%Lf\t%Lf[\t;]"
#else
  #define _xbranch_format1 "\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d[\t;]"
  #define _xbranch_format2 "%lf\t%lf[\t;]"
#endif
		j = sscanf(line,_xbranch_format1,
			       &(branch->f_bus),
				   &(branch->t_bus),
				   &(branch->br_r),
				   &(branch->br_x),
				   &(branch->br_b),
				   &(branch->rate_a),
				   &(branch->rate_b),
				   &(branch->rate_c),
				   &(branch->tap),
				   &(branch->shift),
				   &(branch->br_status));
		if (j != 11) goto DONE;
		if (xpc->version == 2){
			p = line, j = 0;
			while (j<12){
				if (*p == '\t') j++;
				p++;
			}
			j = sscanf(p,_xbranch_format2,&(branch->ang_min),&(branch->ang_max));
			if (j != 2) goto DONE;
		}
		i++;
	} while (i<xpc->branch.elen && !feof(fd));
	if (i < xpc->branch.elen) goto DONE;

	/* gencost data (optional)*/
    #define _xgencost_header1 "gencost = ["
	#define _xgencost_header2 "mpc.gencost = ["
	if (xpc->version == 1)
		str=_xgencost_header1,len=strlen(_xgencost_header1);
	else
		str=_xgencost_header2;len=strlen(_xgencost_header2); 
	pos = ftell(fd),once = _XTRUE, i = 0;

GENCOST:
	done = _XFALSE;
	while (!feof(fd)){
		fgets(line,sizeof(line),fd);
		line[strlen(line)-1] = '\0';/* remove new-line */
		if (!strncmp(line,str,len)){
			done = _XTRUE;
			break;
		}
	}
	if (done && once){
	    i = 0, done = _XFALSE;
	    do {
	        if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	        line[strlen(line)-1] = '\0';/* remove new-line */
		    if (!strncmp(line,"];",2)){
			    done = _XTRUE;
			    break;
		    }
		    i++;
	    } while (!feof(fd));
	    if (!done || !i) goto DONE;
		if (once){
		    once = _XFALSE;fseek(fd,pos,0);goto GENCOST;
	    }
	}
	i /= xpc->gen.elen;
	if (i == 1 || i == 2){
		i *= xpc->gen.elen;
	    xpc->gencost.elen = i; _xfree(xpc->gencost.bucket);
	    xpc->gencost.bucket = (_xpowerflow_gencost *)_xcalloc(xpc->gencost.elen, sizeof(_xpowerflow_gencost));
	    i = 0;
		do {
		    _xpowerflow_gencost *gencost = xpc->gencost.bucket + i;
	        if (fgets(line,sizeof(line),fd) == NULL) goto DONE;
	        line[strlen(line)-1] = '\0';/* remove new-line */
		    if (!strncmp(line,"];",2))
			    break;
			j = sscanf(line,"\t%d\t%d\t%d\t%d\t",
				       &(gencost->model),
					   &(gencost->startup),
					   &(gencost->shutdown),
					   &(gencost->ncost));
			if (j != 4) goto DONE;
			if (gencost->ncost > 0){
				gencost->cost = (_xreal *)_xcalloc(gencost->ncost,sizeof(_xreal));
				p = line, j = 0;
			    while (j<5){
				    if (*p == '\t') j++;
				    p++;
			    }
				for (j=0; j<gencost->ncost; j++){
					double val;
					if (sscanf(p,"%Lf",&val) != 1) goto DONE;
					gencost->cost[j] = (_xreal)val;

					if (j < gencost->ncost-1){
					    while (*p && *p != '\t') p++;
					    if (*p != '\t') goto DONE;
					    p++;
					}
				}
			}

		} while (i<xpc->gencost.elen && !feof(fd));
	}
	ret = _XTRUE;

DONE:
	fclose(fd);
	return ret;
}

static int _xbus_index(_xpowerflow_xpc *xpc, int bus_i){
	int lo,hi,mid,tmp,loop=_XTRUE;
	_xpowerflow_bus *bus;

	if (xpc == NULL || xpc->bus.bucket == NULL ||
		xpc->bus.ilen <= 0 || xpc->bus.ilen > xpc->bus.elen)
		return -1;

	/* connected & in-service bus */
	lo = 0; hi = xpc->bus.ilen - 1; mid = lo;

LOOP:
	while (lo <= hi){
		if (hi - lo <= 1){
			bus = xpc->bus.bucket + lo; if (bus->bus_i == bus_i) return lo;
			bus = xpc->bus.bucket + hi; if (bus->bus_i == bus_i) return hi;
			break;
		}

		mid = (hi + lo)/2;
		bus = xpc->bus.bucket + mid;
		if ((tmp = bus->bus_i - bus_i) > 0)
			hi = mid;
		else if (tmp < 0)
			lo = mid;
		else
			return mid;
	}

	if (loop){/* "out" staff */
		loop = _XFALSE;
		lo = xpc->bus.ilen; hi = xpc->bus.elen - 1; mid = lo;
		goto LOOP;
	}

	/*oops!!!*/
	return -1;
}

static int _xbus_func(const void *b1, const void *b2){
	_xpowerflow_bus *bus1 = (_xpowerflow_bus *)b1;
	_xpowerflow_bus *bus2 = (_xpowerflow_bus *)b2;

	if (bus1->bus_type == _XBUSTYPE_NONE){
		return bus2->bus_type == _XBUSTYPE_NONE ? bus1->bus_i - bus2->bus_i : 1;
	}
	return bus2->bus_type == _XBUSTYPE_NONE ? -1 : bus1->bus_i - bus2->bus_i;
}

static void _xgen_perm(_xpowerflow_xpc *xpc, _xperm *px){
	unsigned int i,start,size,old_i;
	_xpowerflow_gen tmp;

	if (!xpc || !px || px->size != xpc->gen.elen)
		_xerror(_XE_NULL,"_xgen_perm");

	start=0,size=px->size;
	while (start < size){
		old_i = start;
		i = px->pe[old_i];
		if (i >= size){
			start++;
			continue;
		}
		memcpy(&tmp,xpc->gen.bucket+start,sizeof(_xpowerflow_gen));
		while (_XTRUE){
			memcpy(xpc->gen.bucket+old_i,xpc->gen.bucket+i,sizeof(_xpowerflow_gen));
			px->pe[old_i] = i+size;
			old_i = i;
			i = px->pe[old_i];
			if (i >= size) break;
			if (i == start){
				memcpy(xpc->gen.bucket+old_i,&tmp,sizeof(_xpowerflow_gen));
				px->pe[old_i] = i+size;
				break;
			}
		}
		start++;
	}
	for (i = 0; i < size; i++)
        if (px->pe[i] < size)
            _xerror(_XE_BOUNDS,"_xgen_perm");
        else
            px->pe[i] = px->pe[i]-size;

}

static void _xgencost_perm(_xpowerflow_xpc *xpc, _xperm *px){
	unsigned int i,start,size,old_i,loop;
	_xpowerflow_gencost *from,tmp;

	if (!xpc || !px || px->size == 0 || px->size != xpc->gen.elen)
		_xerror(_XE_NULL,"_xgencost_perm");
	
	size = xpc->gencost.elen/xpc->gen.elen;
	if (size != 1 && size != 2)
		_xerror(_XE_SIZES,"_xgencost_perm");

	loop = size == 2;
	from = xpc->gencost.bucket;

LOOP:
	start=0,size=xpc->gen.elen;
	while (start < size){
		old_i = start;
		i = px->pe[old_i];
		if (i >= size){
			start++;
			continue;
		}
		memcpy(&tmp,from+start,sizeof(_xpowerflow_gencost));
		while (_XTRUE){
			memcpy(from+old_i,from+i,sizeof(_xpowerflow_gencost));
			px->pe[old_i] = i+size;
			old_i = i;
			i = px->pe[old_i];
			if (i >= size) break;
			if (i == start){
				memcpy(from+old_i,&tmp,sizeof(_xpowerflow_gencost));
				px->pe[old_i] = i+size;
				break;
			}
		}
		start++;
	}
	for (i = 0; i < size; i++)
        if (px->pe[i] < size)
            _xerror(_XE_BOUNDS,"_xgencost_perm");
        else
            px->pe[i] = px->pe[i]-size;

	if (loop){
		loop = _XFALSE;
		from = xpc->gencost.bucket + xpc->gen.elen;
		goto LOOP;
	}
}

/*! convert external to internal bus numbering */
int _xpowerflow_ext2int(_xpowerflow_xpc *xpc){
	int fr,to,max;
	_xivec *iv;
	_xperm *px;
	unsigned int i,j;
	_xpowerflow_bus *bus;
	_xpowerflow_gen *gen;
	_xpowerflow_branch *branch;
	_xpowerflow_areas *areas;

	if (!xpc)
		_xerror(_XE_NULL,"_xpowerflow_ext2int");
	if (xpc->order.state != 'e')
		return _XFALSE;

	/* determine which buses, branches, gens are connected & in-service */
	/* check that all buses have a valid BUS_TYPE */
	qsort(xpc->bus.bucket,xpc->bus.elen,sizeof(_xpowerflow_bus),_xbus_func);
	xpc->order.i2e = iv = _xivec_resize(xpc->order.i2e,xpc->bus.elen);
	for (i=j=0; i<xpc->bus.elen; i++){
		bus = xpc->bus.bucket + i;
		iv->ive[i] = bus->bus_i;

		switch (bus->bus_type){
		case _XBUSTYPE_PQ   :
		case _XBUSTYPE_PV   :
		case _XBUSTYPE_REF  : j++;break;
		case _XBUSTYPE_NONE : break;
		default : return _XFALSE;
		}
	}
	xpc->bus.ilen = j;
	/* bus is ready for internal except for numbering,
	 * but internal numbering can be used as index + 1
	 */

	/* gen status, move "out" staffs to end */
	iv = _xivec_get(xpc->gen.elen);
	for (i=j=0; i<xpc->gen.elen; i++){
		gen = xpc->gen.bucket + i;
		fr = _xbus_index(xpc,gen->gen_bus);
		if (fr < 0){
			_xivec_ifree(iv);
			return _XFALSE;
		}
		
		iv->ive[i] = fr+1;
		if (fr >= xpc->bus.ilen || gen->gen_status <= 0)
			iv->ive[i] = -(fr+1),j++;
	}
	xpc->gen.ilen = xpc->gen.elen - j;
	xpc->gencost.ilen = xpc->gencost.elen - xpc->gencost.elen/xpc->gen.elen*j;

	/* reorder gens & gencost in order of increasing bus number */
	px = _xperm_get(iv->dim);
	for (i=to=0; i<iv->dim; i++){
		if (iv->ive[i] < 0){/* "out" staff */
			px->pe[i] = xpc->gen.ilen + to,to++;
			continue;
		}
		max = iv->ive[i];
		for (j=fr=0; j<iv->dim; j++){
			if (j==i) continue;
			if (iv->ive[j] > 0 && iv->ive[j] < max) fr++;
		}
		px->pe[i] = fr;	
	}
	_xgen_perm(xpc,px);
	if (xpc->gencost.elen) _xgencost_perm(xpc,px);
	_xperm_pfree(px);

	/* branch status, move the "out" staffs to end */
	iv = _xivec_resize(iv,xpc->branch.elen);
	for (i=j=0; i<xpc->branch.elen; i++){
		branch = xpc->branch.bucket + i;
		fr = _xbus_index(xpc,branch->f_bus);
		to = _xbus_index(xpc,branch->t_bus);
		if (fr < 0 || to < 0){
			_xivec_ifree(iv);
			return _XFALSE;
		}
		iv->ive[i] = fr+1;
		if (branch->br_status <= 0 || 
			fr >= xpc->bus.ilen || 
			to >= xpc->bus.ilen)
			iv->ive[i] = -(fr+1),j++;
	}
	xpc->branch.ilen = xpc->branch.elen - j;
	if (j>0){
		_xpowerflow_branch bbuf;
		for (i=0; i<xpc->branch.elen-1; i++){
			if (iv->ive[i] > 0) continue;
			for (j=i+1; j<xpc->branch.elen; j++){
				if (iv->ive[j] < 0) continue;
				memcpy(&bbuf,xpc->branch.bucket+j,sizeof(_xpowerflow_branch));
				memmove(xpc->branch.bucket+i+1,
					    xpc->branch.bucket+i,
						sizeof(_xpowerflow_branch)*(j-i));
				memcpy(xpc->branch.bucket+i,&bbuf,sizeof(_xpowerflow_branch));
				break;
			}
		}
	}

	/* areas status ,move the "out" staffs to end */
	iv = _xivec_resize(iv,xpc->areas.elen);
	for (i=j=0; i<xpc->areas.elen; i++){
		areas = xpc->areas.bucket + i;
		fr = _xbus_index(xpc,areas->price_ref_bus);
		if (fr < 0){
			_xivec_ifree(iv);
			return _XFALSE;
		}
		iv->ive[i] = fr+1;
		if (fr >= xpc->bus.ilen) iv->ive[i] = -(fr+1),j++;
	}
	xpc->areas.ilen = xpc->areas.elen - j;
	if (j>0){
		_xpowerflow_areas abuf;
		for (i=0; i<xpc->areas.elen-1; i++){
			if (iv->ive[i] > 0) continue;
			for (j=i+1; j<xpc->areas.elen; j++){
				if (iv->ive[j] < 0) continue;
				memcpy(&abuf,xpc->areas.bucket+j,sizeof(_xpowerflow_areas));
				memmove(xpc->areas.bucket+i+1,
					    xpc->areas.bucket+i,
						sizeof(_xpowerflow_areas)*(j-i));
				memcpy(xpc->areas.bucket+i,&abuf,sizeof(_xpowerflow_areas));
				break;
			}
		}
	}
	_xivec_ifree(iv);

	/* update areas, branch, gen, bus with internal numbering */
	for (i=0; i<xpc->areas.elen; i++){
		areas = xpc->areas.bucket + i;
		fr = _xbus_index(xpc,areas->price_ref_bus);
		areas->price_ref_bus = fr + 1;
	}
	for (i=0; i<xpc->branch.elen; i++){
		branch = xpc->branch.bucket + i;
		fr = _xbus_index(xpc,branch->f_bus);
		to = _xbus_index(xpc,branch->t_bus);
		branch->f_bus = fr + 1;
		branch->t_bus = to + 1;
	}
	for (i=0; i<xpc->gen.elen; i++){
		gen = xpc->gen.bucket + i;
		fr = _xbus_index(xpc,gen->gen_bus);
		gen->gen_bus = fr + 1;
	}
	for (i=0; i<xpc->bus.elen; i++){/*must be the last*/
		bus = xpc->bus.bucket + i;
		bus->bus_i = i+1;
	}
	xpc->order.state = 'i';

    return _XTRUE;
}

/*! convert internal to external bus numbering */
int _xpowerflow_int2ext(_xpowerflow_xpc *xpc){
	unsigned int i;
	_xivec *iv;
	_xpowerflow_bus *bus;
	_xpowerflow_gen *gen;
	_xpowerflow_branch *branch;
	_xpowerflow_areas *areas;

	if (!xpc)
		_xerror(_XE_NULL,"_xpowerflow_int2ext");

	iv = xpc->order.i2e;
	if (xpc->order.state != 'i' || !iv || iv->dim != xpc->bus.elen)
		return _XFALSE;

	for (i=0; i<xpc->areas.elen; i++){
		areas = xpc->areas.bucket + i;
		areas->price_ref_bus = iv->ive[areas->price_ref_bus - 1];
	}
	for (i=0; i<xpc->branch.elen; i++){
		branch = xpc->branch.bucket + i;
		branch->f_bus = iv->ive[branch->f_bus - 1];
		branch->t_bus = iv->ive[branch->t_bus - 1];
	}
	for (i=0; i<xpc->gen.elen; i++){
		gen = xpc->gen.bucket + i;
		gen->gen_bus = iv->ive[gen->gen_bus - 1];
	}
	for (i=0; i<xpc->bus.elen; i++){
		bus = xpc->bus.bucket + i;
		bus->bus_i = iv->ive[bus->bus_i - 1];
	}
	_xivec_ifree(xpc->order.i2e);
	xpc->order.state = 'e';

	return _XTRUE;
}

/*! get the voltage&angle initial-values of bus*/
int _xpowerflow_v0(_xpowerflow_xpc *xpc, _xzvec **v0){
	int idx;
	unsigned int i;
    _xpowerflow_bus *bus;
    _xpowerflow_gen *gen;
    _xzvec *xv0 = *v0;
    
    if (!xpc)
    	_xerror(_XE_NULL,"_xpowerflow_v0");
	if (xpc->order.state != 'i' || xpc->bus.ilen == 0)
		return _XFALSE;

    xv0 = _xzvec_resize(xv0,xpc->bus.ilen); *v0 = xv0;

	for (i=0; i<xpc->bus.ilen; i++){
		bus = xpc->bus.bucket + i;

		xv0->ve[bus->bus_i-1].re = bus->Vm*cos(bus->Va*_XPI/180.0);
		xv0->ve[bus->bus_i-1].im = bus->Vm*sin(bus->Va*_XPI/180.0);
    }
    
	for (i=0; i<xpc->gen.ilen; i++){
		gen = xpc->gen.bucket + i;

    	if (gen->gen_status > 0){
			idx = gen->gen_bus - 1;
			if (idx < 0) return _XFALSE;

			bus = xpc->bus.bucket + idx;
    		xv0->ve[idx].re = gen->Vg*cos(bus->Va*_XPI/180.0);
    	    xv0->ve[idx].im = gen->Vg*sin(bus->Va*_XPI/180.0);
    	}
    }
    
    return _XTRUE;
}
                                 
/*!
 * generators with "out-of-service" status are treated as PQ buses with
 * zero generation (regardless of Pg/Qg values in gen). Expects BUS and
 * GEN have been converted to use internal consecutive bus numbering.
 */
int _xpowerflow_bustypes(_xpowerflow_xpc *xpc, int *ref, _xivec **pv, _xivec **pq){
    int idx;
    unsigned int i,nref,npv,npq,len,ret = _XFALSE;
    _xivec *xpv = *pv, *xpq = *pq, *bus_gen_status;
    _xpowerflow_bus *bus;
    _xpowerflow_gen *gen;
    
    if (!xpc || !ref)
    	_xerror(_XE_NULL,"_xpowerflow_bustypes");
    
	if (xpc->order.state != 'i' || xpc->bus.ilen == 0 || xpc->gen.ilen == 0)
		return _XFALSE;

    /* get generator status */
	bus_gen_status = _xivec_get(xpc->bus.ilen);
	for (i=0; i<xpc->gen.ilen; i++){
		gen = xpc->gen.bucket + i;
    	if (gen->gen_status > 0){
			idx = gen->gen_bus - 1;
			if (idx < 0 || idx >= xpc->bus.ilen){
    			_xivec_ifree(bus_gen_status);
    			return _XFALSE;
    		}
    		bus_gen_status->ive[idx] = 1;
    	}
    }

    *ref = 0; 
    xpv = _xivec_resize(xpv,xpc->gen.ilen); *pv = xpv; _xivec_zero(xpv);
	xpq = _xivec_resize(xpq,xpc->bus.ilen); *pq = xpq; _xivec_zero(xpq); 

    /* form index lists for slack, PV, and PQ buses */
    for (i=npv=npq=nref=0; i<xpc->bus.ilen; i++){
		bus = xpc->bus.bucket + i;
		if (bus->bus_type == _XBUSTYPE_PQ)
			xpq->ive[npq++] = bus->bus_i;
        else if (bus_gen_status->ive[i]){
        	switch (bus->bus_type) {
			case _XBUSTYPE_REF : *ref = bus->bus_i; break;
        	case _XBUSTYPE_PV  : xpv->ive[npv++] = bus->bus_i; break;
        	default : break;
            }
        }
		else 
			xpq->ive[npq++] = bus->bus_i;
    }
	_xivec_ifree(bus_gen_status);
	xpv->dim = npv; xpq->dim = npq;
    
    /* pick a new reference bus if for some reason there is none (may have been shut down) */
	if (*ref <= 0 && xpv->dim > 0){
		*ref = xpv->ive[0];/* use the first PV bus */
    	len = xpv->dim - 1; xpv->ive[0] = 0;
    	for (i=1; i<=len; i++)/* take it off PV list */
    	    xpv->ive[i-1] = xpv->ive[i];
    	xpv->dim = len;
    }
	ret = *ref > 0;
    
    return ret;
}

/*!
 * build the vector of complex bus power injections.
 * returns the vector of complex bus power injections, 
 * that is, generation minus load. Power is expressed
 * in per unit
 */
int _xpowerflow_make_sbus(_xpowerflow_xpc *xpc, _xzvec **sbus){
    int i,idx;
	double baseMVA;
    _xzvec *xsbus = *sbus;
    _xpowerflow_bus *bus;
    _xpowerflow_gen *gen;

	if (!xpc)
    	_xerror(_XE_NULL,"_xpowerflow_make_sbus");
    
	if (xpc->order.state != 'i' || xpc->bus.ilen == 0 || xpc->baseMVA <= 0.0f)
		return _XFALSE;
    	
	xsbus = _xzvec_resize(xsbus,xpc->bus.ilen); *sbus = xsbus; _xzvec_zero(xsbus);
    baseMVA = 1.0/xpc->baseMVA;

	for (i=0; i<xpc->bus.ilen; i++){/* power injected by loads */
		bus = xpc->bus.bucket + i;
    	xsbus->ve[i].re -= bus->Pd*baseMVA;
    	xsbus->ve[i].im -= bus->Qd*baseMVA;
    }
    
	for (i=0; i<xpc->gen.ilen; i++){/* power injected by generators */
		gen = xpc->gen.bucket + i;
    	if (gen->gen_status > 0){
			idx = gen->gen_bus - 1;
    		if (idx < 0) return _XFALSE;
    	    xsbus->ve[idx].re += gen->Pg*baseMVA;
    	    xsbus->ve[idx].im += gen->Qg*baseMVA;
        }
    }
    
    return _XTRUE;
}

/*! build the bus admittance matrix and branch admittance matrices */
int _xpowerflow_make_ybus(_xpowerflow_xpc *xpc, int nargout, _xzspmat *ybus, _xzspmat *yf, _xzspmat *yt){
    int i,m,n;
	_xpowerflow_bus *bus;
	_xpowerflow_branch *branch;
	_xcomplex tap,Bc,Ys,Ytt,Yff,Yft,Ytf;
    
	if (!xpc || !ybus || (nargout > 1 && (!yf || !yt)))
		_xerror(_XE_NULL,"_xpowerflow_make_ybus");

	if (xpc->order.state != 'i' || xpc->baseMVA <= 0.0f ||
		xpc->bus.ilen == 0 || xpc->branch.ilen == 0)
		return _XFALSE;

	ybus->re = _xspmat_resize(ybus->re,xpc->bus.ilen,xpc->bus.ilen);
	ybus->im = _xspmat_resize(ybus->im,xpc->bus.ilen,xpc->bus.ilen);
	_xspmat_zero(ybus->re);_xspmat_compact(ybus->re,_XMACHEPS);
	_xspmat_zero(ybus->im);_xspmat_compact(ybus->im,_XMACHEPS);
	if (nargout > 1){
		yf->re = _xspmat_resize(yf->re,xpc->branch.ilen,xpc->bus.ilen);
	    yf->im = _xspmat_resize(yf->im,xpc->branch.ilen,xpc->bus.ilen);
		_xspmat_zero(yf->re);_xspmat_compact(yf->re,_XMACHEPS);
		_xspmat_zero(yf->im);_xspmat_compact(yf->im,_XMACHEPS);

		yt->re = _xspmat_resize(yt->re,xpc->branch.ilen,xpc->bus.ilen);
	    yt->im = _xspmat_resize(yt->im,xpc->branch.ilen,xpc->bus.ilen);
		_xspmat_zero(yt->re);_xspmat_compact(yt->re,_XMACHEPS);
		_xspmat_zero(yt->im);_xspmat_compact(yt->im,_XMACHEPS);
    }
	
	/* compute shunt admittance */
	for (i=0; i<xpc->bus.ilen; i++){
		bus = xpc->bus.bucket + i;
		_xspmat_set_val(ybus->re,i,i,bus->Gs/xpc->baseMVA);
		_xspmat_set_val(ybus->im,i,i,bus->Bs/xpc->baseMVA);
	}

	for (i=0; i<xpc->branch.ilen; i++){
		branch = xpc->branch.bucket + i;
		m = branch->f_bus - 1;
		n = branch->t_bus - 1;
		if (m<0 || n<0) return _XFALSE;

		Bc.re = Bc.im = Ys.re = Ys.im = 0.0f;
		if (branch->br_status){
			Ys = _xzinv(_xzmake(branch->br_r,branch->br_x));
			Bc.im = branch->br_b/2.0;
		}
		if (branch->tap > 0.0f){
			tap.re = branch->tap*cos(branch->shift*_XPI/180.0);
			tap.im = branch->tap*sin(branch->shift*_XPI/180.0);
		}
		else
		    tap.re = 1.0f, tap.im = 0.0f;

		Ytt = _xzadd(Ys,Bc);
		Yff = _xzdiv(Ytt,_xzmlt(tap,_xzconj(tap)));
		Yft = _xzneg(_xzdiv(Ys,_xzconj(tap)));
		Ytf = _xzneg(_xzdiv(Ys,tap));

		/* build Ybus */
		_xspmat_set_val(ybus->re,m,m,_xspmat_get_val(ybus->re,m,m)+Yff.re);
		_xspmat_set_val(ybus->re,m,n,_xspmat_get_val(ybus->re,m,n)+Yft.re);
		_xspmat_set_val(ybus->re,n,m,_xspmat_get_val(ybus->re,n,m)+Ytf.re);
		_xspmat_set_val(ybus->re,n,n,_xspmat_get_val(ybus->re,n,n)+Ytt.re);

		_xspmat_set_val(ybus->im,m,m,_xspmat_get_val(ybus->im,m,m)+Yff.im);
		_xspmat_set_val(ybus->im,m,n,_xspmat_get_val(ybus->im,m,n)+Yft.im);
		_xspmat_set_val(ybus->im,n,m,_xspmat_get_val(ybus->im,n,m)+Ytf.im);
		_xspmat_set_val(ybus->im,n,n,_xspmat_get_val(ybus->im,n,n)+Ytt.im);

		/* build Yf and Yt such that Yf * V is the vector of complex branch
         * currents injected at each branch's "from" bus, and Yt is the same
         * for the "to" bus end
         */
		if (nargout > 1){
			_xspmat_set_val(yf->re,i,m,Yff.re);
		    _xspmat_set_val(yf->re,i,n,Yft.re);
		    _xspmat_set_val(yf->im,i,m,Yff.im);
		    _xspmat_set_val(yf->im,i,n,Yft.im);

			_xspmat_set_val(yt->re,i,m,Ytf.re);
		    _xspmat_set_val(yt->re,i,n,Ytt.re);
		    _xspmat_set_val(yt->im,i,m,Ytf.im);
		    _xspmat_set_val(yt->im,i,n,Ytt.im);
		}
	}
    
    return _XTRUE;
}

static _xcomplex _xznorm(_xcomplex z){
	double v = _xzabs(z);
	if (v != 0.0f){
		z.re /= v, z.im /= v;
	}
	return z;
}

/*!
 * compute partial derivatives of power injection w.r.t. voltage.
 */
int _xpowerflow_dsbus_dv(_xzvec *v, _xzspmat *ybus, _xzspmat *dvm, _xzspmat *dva){
    int i,j,k,n;
	_xcomplex vi,vj,ibus,yij,aij;
	_xsprow *rg,*rb;

	if (!ybus || !ybus->re || !ybus->im || !v)
		_xerror(_XE_NULL,"_xpowerflow_dsbus_dv");
	if (ybus->re->m != ybus->re->n ||
		ybus->im->m != ybus->im->n ||
		ybus->re->m != ybus->im->m ||
		ybus->re->n != v->dim)
		return _XFALSE;

	n = v->dim; 
	dvm->re = _xspmat_resize(dvm->re,n,n);
	dvm->im = _xspmat_resize(dvm->im,n,n);
	dva->re = _xspmat_resize(dva->re,n,n);
	dva->im = _xspmat_resize(dva->im,n,n);

	for (i=0; i<n; i++){
		vi = v->ve[i];
		/* Ibus = Ybus * V */
		ibus.re = ibus.im = 0.0f;
		rg = ybus->re->row + i;
		rb = ybus->im->row + i;
		for (j=0; j<rb->len; j++){
			_xrow_elt *belt = rb->elt + j;
			yij.im = belt->val,yij.re = 0.0f;
			for (k=0; k<rg->len; k++){
				_xrow_elt *gelt = rg->elt + k;
				if (gelt->col == belt->col){
					yij.re = gelt->val;
					break;
				}
			}
			k = belt->col;
			vj = v->ve[k];
			aij = _xzmlt(vi,_xzconj(_xzmlt(yij,_xznorm(vj))));
			
			_xspmat_set_val(dvm->re,i,k,aij.re);
			_xspmat_set_val(dvm->im,i,k,aij.im);
			
			aij = _xzmlt(vi,_xzconj(_xzmlt(yij,vj)));
			_xspmat_set_val(dva->re,i,k, aij.im);
			_xspmat_set_val(dva->im,i,k,-aij.re);

			yij = _xzmlt(yij,vj);
			ibus.re += yij.re, ibus.im += yij.im;
		}
		
		ibus = _xzconj(ibus);
		
		aij = _xzmlt(vi,ibus);
		_xspmat_set_val(dva->re,i,i,_xspmat_get_val(dva->re,i,i)-aij.im);
		_xspmat_set_val(dva->im,i,i,_xspmat_get_val(dva->im,i,i)+aij.re);
		
		aij = _xzmlt(ibus,_xznorm(vi));
		_xspmat_set_val(dvm->re,i,i,_xspmat_get_val(dvm->re,i,i)+aij.re);
		_xspmat_set_val(dvm->im,i,i,_xspmat_get_val(dvm->im,i,i)+aij.im);
	}

	return _XTRUE;
}

static _xzvec *_xpowerflow_mis(_xzspmat *ybus,_xzvec *sbus,_xzvec *v,_xzvec *mis){
	int i,j,k,n;
	_xcomplex z,x,yij;
	_xsprow *rg,*rb;

    if (!ybus || !ybus->re || !ybus->im || !sbus || !v ||
		ybus->re->m != ybus->re->n ||
		ybus->re->m != ybus->im->m ||
		ybus->re->m != ybus->im->n ||
		ybus->re->m != sbus->dim ||
		ybus->re->m != v->dim)
		_xerror(_XE_NULL,"_xpowerflow_mis");

	n = ybus->re->m;
	mis = _xzvec_resize(mis,n);

	for (i=0; i<n; i++){
		rg = ybus->re->row + i;
		rb = ybus->im->row + i;

		z.re = z.im = 0.0f;
		for (j=0; j<rb->len; j++){
			_xrow_elt *belt = rb->elt + j;
			yij.im = belt->val, yij.re = 0.0f;
			for (k=0; k<rg->len; k++){
				_xrow_elt *gelt = rg->elt + k;
				if (gelt->col == belt->col){
					yij.re = gelt->val;
					break;
				}
			}
			k = belt->col;
			x = v->ve[k];
			z = _xzadd(z,_xzmlt(yij,x));
		}
		x = v->ve[i];
		z = _xzmlt(x,_xzconj(z));
		mis->ve[i] = _xzsub(z,sbus->ve[i]);
	}

	return mis;
}

static _xvec *_xpowerflow_f(_xzvec *mis, _xivec *pv, _xivec *pq, _xvec *f){
	int m,n,k;
	unsigned int i;
	if (!mis || !pv || !pq)
		_xerror(_XE_NULL,"_xpowerflow_f");

	n = pv->dim;
	m = pv->dim + pq->dim * 2;
	f = _xvec_resize(f,m);

	/* V angle of pv buses */
	for (i=0; i<pv->dim; i++){
		k = pv->ive[i];
		f->ve[i] = (mis->ve[k-1]).re;
	}

	m = pv->dim,n = pv->dim + pq->dim;
	for (i=0; i<pq->dim; i++){
		k = pq->ive[i];
		f->ve[m+i] = (mis->ve[k-1]).re;/* V angle of pq buses */
		f->ve[n+i] = (mis->ve[k-1]).im;/* V mag of pq buses */
	}

	return f;	
}

static _xcomplex _xzprod(_xzspmat *zsp, _xzvec *zv, int row){
	unsigned int i,j,m,n;
	_xcomplex s,c;
	_xsprow *re,*im;

	if (!zsp || !zsp->im || !zsp->re || !zv)
		_xerror(_XE_NULL,"_xzprod");

	n = zv->dim;
	if (zsp->im->n != n || zsp->re->n != n ||
		zsp->re->n != n || zsp->re->n != n)
		_xerror(_XE_SIZES,"_xzprod");

	m = zsp->im->m;
	if (zsp->re->m != m || row >= m || row < 0)
		_xerror(_XE_SIZES,"_xzprod");

	im = zsp->im->row + row;
	re = zsp->re->row + row;

	s.im = s.re = 0.0f;
	for (i=0; i<im->len; i++){
		_xrow_elt *elt = im->elt + i;
		c.im = elt->val,c.re = 0.0f;

		for (j=0; j<re->len; j++){
			_xrow_elt *elt2 = re->elt + j;
			if (elt2->col == elt->col){
				c.re = elt2->val;
				break;
			}
		}
		c = _xzmlt(c,zv->ve[elt->col]);
		s = _xzadd(c,s);
	}

	return s;
}

/*!
 * solve the power flow using a Gauss-Seidel method
 */
int _xpowerflow_gausspf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout){
	int i,j,nb,converged;
	double normf,val;
	_xzvec *v=*vout,*mis; 
	_xvec *vm,*F;
	_xcomplex s,c;

	/* check */
	if (!ybus ||!ybus->re || !ybus->im || !sbus || !v0 || !pv || !pq || !opt)
		_xerror(_XE_NULL,"_xpowerflow_gausspf");

	nb = ybus->re->m;
    if (nb != ybus->re->n || nb != ybus->im->m || nb != ybus->im->n ||
		nb != sbus->dim || nb != v0->dim || pv->dim+pq->dim+1 != nb)
		_xerror(_XE_SIZES,"_xpowerflow_gausspf");

	/* initialize */
	converged=_XFALSE,opt->i=0;
	v =_xzvec_vcopy(v0,v),*vout = v;
	vm = _xzvec_abs(v,NULL);

	/* evaluate F(x0) */
	mis = _xpowerflow_mis(ybus,sbus,v,NULL);
	F = _xpowerflow_f(mis,pv,pq,NULL);
	normf = _xvec_vnorm_inf(F);
	if (normf < opt->tol){
		converged = _XTRUE;
	}

	/* do Gauss-Seidel iterations */
	while (!converged && opt->i<opt->max_it){
		opt->i++;
		
		/* update voltage */
	    for (i=0; i<pq->dim; i++){/* at PQ buses */
		    j = pq->ive[i]-1;

			c.re = _xspmat_get_val(ybus->re,j,j);
			c.im = _xspmat_get_val(ybus->im,j,j);

			s = _xzconj(_xzdiv(sbus->ve[j],v->ve[j]));
			s = _xzsub(s,_xzprod(ybus,v,j));
			s = _xzdiv(s,c);
			v->ve[j] = _xzadd(v->ve[j],s);
	    }
	    for (i=0; i<pv->dim; i++){/* at PV buses */
			j = pv->ive[i]-1;

			c.re = _xspmat_get_val(ybus->re,j,j);
			c.im = _xspmat_get_val(ybus->im,j,j);

			s = _xzprod(ybus,v,j);
			s = _xzmlt(v->ve[j],_xzconj(s));
			s.re = sbus->ve[j].re;

			s = _xzconj(_xzdiv(s,v->ve[j]));
			s = _xzsub(s,_xzprod(ybus,v,j));
			s = _xzdiv(s,c);
			v->ve[j] = _xzadd(v->ve[j],s);

			val = vm->ve[j]/_xzabs(v->ve[j]);
			v->ve[j].re *= val, v->ve[j].im *= val;
	    }
		for (i=0; i<pv->dim; i++){
			j = pv->ive[i]-1;
			val = vm->ve[j]/_xzabs(v->ve[j]);
			v->ve[j].re *= val, v->ve[j].im *= val;
		}
	    
		/* evaluate F(x) */
		mis = _xpowerflow_mis(ybus,sbus,v,mis);
	    F = _xpowerflow_f(mis,pv,pq,F);

		/* check for convergence */
	    normf = _xvec_vnorm_inf(F);
	    if (normf < opt->tol){
		    converged = _XTRUE;
			break;
	    }
	}

	/* deallocate */
	_xzvec_vfree(mis);
	_xvec_vfree(F);_xvec_vfree(vm);
    
	return converged;
}

/*!
 * solve the power flow using a full Newton's method
 */
int _xpowerflow_newtonpf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout){
	int i,j,m,n,nb,converged;
	double normf,val;
	_xzvec *v=*vout,*mis;
	_xivec *pv_pq=NULL; 
	_xvec *va,*vm,*F,*dx=NULL;
	_xperm *pivot=NULL;
	_xspmat *J=NULL;
	_xzspmat dvm={NULL,NULL}, dva={NULL,NULL};

	/* check */
	if (!ybus ||!ybus->re || !ybus->im || !sbus || !v0 || !pv || !pq || !opt)
		_xerror(_XE_NULL,"_xpowerflow_newtonpf");

	nb = ybus->re->m;
    if (nb != ybus->re->n || nb != ybus->im->m || nb != ybus->im->n ||
		nb != sbus->dim || nb != v0->dim || pv->dim+pq->dim+1 != nb)
		_xerror(_XE_SIZES,"_xpowerflow_newtonpf");

	/* initialize */
	converged=_XFALSE,opt->i=0;
	v =_xzvec_vcopy(v0,v),*vout = v;
	va = _xzvec_angle(v,NULL);
	vm = _xzvec_abs(v,NULL);

	pv_pq = _xivec_get(pv->dim+pq->dim);
	_xmem_copy(&(pv->ive[0]),&(pv_pq->ive[0]),pv->dim*sizeof(int));
	_xmem_copy(&(pq->ive[0]),&(pv_pq->ive[pv->dim]),pq->dim*sizeof(int));

	/* evaluate F(x0) */
	mis = _xpowerflow_mis(ybus,sbus,v,NULL);
	F = _xpowerflow_f(mis,pv,pq,NULL);
	normf = _xvec_vnorm_inf(F);
	if (normf < opt->tol){
		converged = _XTRUE;
	}

	/* do Newton iterations */
	while (!converged && opt->i<opt->max_it){
		opt->i++;

		/* evaluate Jacobian */
		if (_xpowerflow_dsbus_dv(v,ybus,&dvm,&dva) == _XFALSE)
			break;
		J = _xspmat_resize(J,pv->dim+2*pq->dim,pv->dim+2*pq->dim);
		for (i=0; i<pv_pq->dim; i++){
			n = pv_pq->ive[i]-1;
			for (j=0; j<pv_pq->dim; j++){/*j11*/
				m = pv_pq->ive[j]-1;
				val = _xspmat_get_val(dva.re,m,n);
				if (val != 0.0){
					_xspmat_set_val(J,j,i,val);
				}
			}
			for (j=0; j<pq->dim; j++){/*j21*/
				m = pq->ive[j]-1;
				val = _xspmat_get_val(dva.im,m,n);
				if (val != 0.0){
					_xspmat_set_val(J,j+pv_pq->dim,i,val);
				}
			}
		}
		for (i=0; i<pq->dim; i++){
			n = pq->ive[i]-1;
			for (j=0; j<pv_pq->dim; j++){/*j12*/
				m = pv_pq->ive[j]-1;
				val = _xspmat_get_val(dvm.re,m,n);
				if (val != 0.0){
					_xspmat_set_val(J,j,i+pv_pq->dim,val);
				}
			}
			for (j=0; j<pq->dim; j++){/*j22*/
				m = pq->ive[j]-1;
				val = _xspmat_get_val(dvm.im,m,n);
				if (val != 0.0){
					_xspmat_set_val(J,j+pv_pq->dim,i+pv_pq->dim,val);
				}
			}
		}
	
		/* compute update step*/
		if (!pivot && (pivot = _xperm_get(J->m)) == NULL)
			break;	
		_xspmat_lufactor(J, pivot, 0.5);
	 	dx = _xspmat_lusolve(J, pivot, F, dx);
		_xspmat_zero(J);_xspmat_compact(J,_XMACHEPS);
		
		/* update voltage */
	    for (i=0; i<pv->dim; i++){
			j = pv->ive[i]-1;
			va->ve[j] -= dx->ve[i];

			v->ve[j].re = vm->ve[j]*cos(va->ve[j]);
			v->ve[j].im = vm->ve[j]*sin(va->ve[j]);
	    }
	    for (i=0; i<pq->dim; i++){
		    j = pq->ive[i]-1;
			va->ve[j] -= dx->ve[pv->dim+i];
			vm->ve[j] -= dx->ve[pv->dim+pq->dim+i];
			
			v->ve[j].re = vm->ve[j]*cos(va->ve[j]);
			v->ve[j].im = vm->ve[j]*sin(va->ve[j]);
	    }
		vm = _xzvec_abs(v,vm);
		va = _xzvec_angle(v,va);
	    
		/* evaluate F(x) */
		mis = _xpowerflow_mis(ybus,sbus,v,mis);
	    F = _xpowerflow_f(mis,pv,pq,F);

		/* check for convergence */
	    normf = _xvec_vnorm_inf(F);
	    if (normf < opt->tol){
		    converged = _XTRUE;
			break;
	    }
	}

	/* deallocate */
	_xspmat_sfree(J);
	_xzvec_vfree(mis);
	_xperm_pfree(pivot);
	_xivec_ifree(pv_pq);
	_xzspmat_sfree(&dvm);_xzspmat_sfree(&dva);
	_xvec_vfree(F);_xvec_vfree(dx);_xvec_vfree(va);_xvec_vfree(vm);
    
	return converged;
}

/*!
 * build the FDPF matrices, B prime and B double prime
 */
int _xpowerflow_make_b(_xpowerflow_xpc *xpc, int nargout, int alg, _xspmat **bp, _xspmat **bpp){
	unsigned int i,ret;
	_xpowerflow_bus *bus;
	_xpowerflow_branch *branch;
	_xzspmat ybus={NULL,NULL};
	_xvec *vbs=NULL,*vbb=NULL,*tap=NULL,*br2=NULL,*br3=NULL,*shift=NULL;

	if (!xpc)
		_xerror(_XE_NULL,"_xpowerflow_make_b");
	if (xpc->order.state != 'i')
		return _XFALSE;

	/* form Bp (B prime) */
	vbs = _xvec_get(xpc->bus.ilen);
	for (i=0; i<xpc->bus.ilen; i++){/*zero out shunts at buses*/
		bus = xpc->bus.bucket + i;
		vbs->ve[i] = bus->Bs, bus->Bs = 0.0f;
	}

	vbb = _xvec_get(xpc->branch.ilen);
	tap = _xvec_get(xpc->branch.ilen);
	if (alg == _XPF_ALG_FDPQXB)
		br2 = _xvec_get(xpc->branch.ilen);
	for (i=0; i<xpc->branch.ilen; i++){
		branch = xpc->branch.bucket + i;

		vbb->ve[i] = branch->br_b, branch->br_b = 0.0f;/*zero out line charging shunts*/
		tap->ve[i] = branch->tap,branch->tap = 1.0f;/*cancel out taps */
		if (br2) br2->ve[i] = branch->br_r, branch->br_r = 0.0f;/*if XB method zero out line resistance*/
	}

	ret = _xpowerflow_make_ybus(xpc,1,&ybus,NULL,NULL);
	if (!ret){
		_xzspmat_sfree(&ybus);
		goto DONE;
	}

	_xspmat_sfree(*bp);_xspmat_sfree(ybus.re);
	*bp = _xspmat_smlt(ybus.im,-1.0,ybus.im);

	/* form Bpp (B double prime) */
	if (nargout == 2){
		for (i=0; i<xpc->bus.ilen; i++){
	        bus = xpc->bus.bucket + i;
	        bus->Bs = vbs->ve[i];
	    }
	    _xvec_vfree(vbs);

		ybus.im = ybus.re = NULL; shift = tap, tap = NULL;
		if (alg == _XPF_ALG_FDPQBX)
			br3 = _xvec_get(xpc->branch.ilen);

		for (i=0; i<xpc->branch.ilen; i++){
		    branch = xpc->branch.bucket + i;
			
			branch->br_b = vbb->ve[i];
			branch->tap  = shift->ve[i];
			if (br2) branch->br_r = br2->ve[i];
			if (br3) br3->ve[i] = branch->br_r,branch->br_r = 0.0f;/*if BX method zero out line resistance*/
			
			shift->ve[i] = branch->shift, branch->shift = 0.0f;/*zero out phase shifters*/
	    }
		_xvec_vfree(vbb);_xvec_vfree(tap);_xvec_vfree(br2);

		ret = _xpowerflow_make_ybus(xpc,1,&ybus,NULL,NULL);
	    if (!ret){
		    _xzspmat_sfree(&ybus);
		    goto DONE;
	    }

	    _xspmat_sfree(*bpp);_xspmat_sfree(ybus.re);
	    *bpp = _xspmat_smlt(ybus.im,-1.0,ybus.im);
	}

DONE:
	if (vbs){
	    for (i=0; i<xpc->bus.ilen; i++){
		    bus = xpc->bus.bucket + i;
		    bus->Bs = vbs->ve[i];
	    }
		_xvec_vfree(vbs);
	}
	if (vbb || tap || br2 || shift || br3){
		for (i=0; i<xpc->branch.ilen; i++){
		    branch = xpc->branch.bucket + i;
			if (vbb) branch->br_b = vbb->ve[i];
			if (tap) branch->tap = tap->ve[i];
			if (br2) branch->br_r = br2->ve[i];
			if (shift) branch->shift = shift->ve[i];
			if (br3) branch->br_r = br3->ve[i];
		}
		_xvec_vfree(vbb);_xvec_vfree(tap);_xvec_vfree(br2);
		_xvec_vfree(br3);_xvec_vfree(shift);
	}

	return ret;
}

/* get mismatch P&Q for fast decoupled method */
static void _xfdpf_pq(_xzvec *mis, _xvec *vm, _xivec *pv, _xivec *pq, _xvec **p, _xvec **q){
	unsigned int i,j;
	_xvec *xp = *p, *xq = *q;

	if (!mis || !vm || !pv || !pq)
		_xerror(_XE_NULL,"_xfdpf_pq");
	if (mis->dim != vm->dim || mis->dim != pv->dim+pq->dim+1)
		_xerror(_XE_SIZES,"_xfdpf_pq");

	xp = _xvec_resize(xp,pv->dim+pq->dim),*p = xp;
	xq = _xvec_resize(xq,pq->dim),*q = xq;

	for (i=0; i<pv->dim; i++){
		j = pv->ive[i]-1;
		xp->ve[i] = mis->ve[j].re/vm->ve[j];
	}
	for (i=0; i<pq->dim; i++){
		j = pq->ive[i]-1;
		xp->ve[pv->dim+i] = mis->ve[j].re/vm->ve[j];
		xq->ve[i] = mis->ve[j].im/vm->ve[j];
	}
}
/*!
 * solve the power flow using a fast decoupled method
 */
int _xpowerflow_fdpf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xspmat *bp, _xspmat *bpp, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout){
	int i,j,m,n,nb,converged;
	double normP,normQ,val;
	_xzvec *v=*vout,*mis; 
	_xvec *va,*vm,*P=NULL,*Q=NULL,*dva=NULL,*dvm=NULL;
	_xperm *pBp=NULL,*pBpp=NULL;
	_xspmat *Bp=NULL,*Bpp=NULL;

	/* check */
	if (!ybus ||!ybus->re || !ybus->im || !sbus || !v0 || !bp || !bpp || !pv || !pq || !opt)
		_xerror(_XE_NULL,"_xpowerflow_fdpf");

	nb = ybus->re->m;
    if (nb != ybus->re->n || nb != ybus->im->m || nb != ybus->im->n ||
		nb != sbus->dim || nb != v0->dim || nb != bp->m || nb != bp->n ||
		nb != bpp->m || nb != bpp->n || pv->dim+pq->dim+1 != nb)
		_xerror(_XE_SIZES,"_xpowerflow_fdpf");

	/* initialize */
	converged=_XFALSE,opt->i=0;
	v =_xzvec_vcopy(v0,v),*vout = v;
	va = _xzvec_angle(v,NULL);
	vm = _xzvec_abs(v,NULL);

	/* evaluate F(x0) */
	mis = _xpowerflow_mis(ybus,sbus,v,NULL);
	_xfdpf_pq(mis,vm,pv,pq,&P,&Q);
	normP = _xvec_vnorm_inf(P);
	normQ = _xvec_vnorm_inf(Q);
	if (normP < opt->tol && normQ < opt->tol){
		converged = _XTRUE;
	}

	if (!converged){
		/* reduce B matrices */
		Bp  = _xspmat_get(pv->dim+pq->dim,pv->dim+pq->dim,3);
		Bpp = _xspmat_get(pq->dim,pq->dim,3);
		for (i=0; i<pv->dim; i++){
			m = pv->ive[i]-1;
			for (j=0; j<pv->dim; j++){
				n = pv->ive[j]-1;
				val = _xspmat_get_val(bp,m,n);
				if (val != 0.0)
					_xspmat_set_val(Bp,i,j,val);
			}
			for (j=0; j<pq->dim; j++){
				n = pq->ive[j]-1;
				val = _xspmat_get_val(bp,m,n);
				if (val != 0.0)
					_xspmat_set_val(Bp,i,pv->dim+j,val);
			}
		}
		for (i=0; i<pq->dim; i++){
			m = pq->ive[i]-1;
			for (j=0; j<pv->dim; j++){
				n = pv->ive[j]-1;
				val = _xspmat_get_val(bp,m,n);
				if (val != 0.0)
					_xspmat_set_val(Bp,pv->dim+i,j,val);
			}
			for (j=0; j<pq->dim; j++){
				n = pq->ive[j]-1;
				val = _xspmat_get_val(bp,m,n);
				if (val != 0.0)
					_xspmat_set_val(Bp,pv->dim+i,pv->dim+j,val);

				val = _xspmat_get_val(bpp,m,n);
				if (val != 0.0)
					_xspmat_set_val(Bpp,i,j,val);
			}
		}

		/* factor B matrices */
		pBp  = _xperm_resize(pBp,Bp->m);	
		_xspmat_lufactor(Bp, pBp, 0.5);
		pBpp = _xperm_resize(pBpp,Bpp->m);	
		_xspmat_lufactor(Bpp, pBpp, 0.5);
	}

	/* do P and Q iterations */
	while (!converged && opt->i<opt->max_it){
		opt->i++;
		/* do P iteration, update Va */
		dva = _xspmat_lusolve(Bp,pBp,P,dva);
		/* update voltage */
		for (i=0; i<pv->dim; i++){
			j = pv->ive[i]-1;
			va->ve[j] -= dva->ve[i];
			v->ve[j].re = vm->ve[j]*cos(va->ve[j]);
			v->ve[j].im = vm->ve[j]*sin(va->ve[j]);
		}
		for (i=0; i<pq->dim; i++){
			j = pq->ive[i]-1;
			va->ve[j] -= dva->ve[pv->dim+i];
			v->ve[j].re = vm->ve[j]*cos(va->ve[j]);
			v->ve[j].im = vm->ve[j]*sin(va->ve[j]);
		}
		/* evaluate mismatch */
	    mis = _xpowerflow_mis(ybus,sbus,v,mis);
	    _xfdpf_pq(mis,vm,pv,pq,&P,&Q);
	    normP = _xvec_vnorm_inf(P);
	    normQ = _xvec_vnorm_inf(Q);
	    if (normP < opt->tol && normQ < opt->tol){
		    converged = _XTRUE;
			break;
	    }

		/* do Q iteration, update Vm */
		dvm = _xspmat_lusolve(Bpp,pBpp,Q,dvm);
		/* update voltage */
		for (i=0; i<pq->dim; i++){
			j = pq->ive[i]-1;
			vm->ve[j] -= dvm->ve[i];
			v->ve[j].re = vm->ve[j]*cos(va->ve[j]);
			v->ve[j].im = vm->ve[j]*sin(va->ve[j]);
		}
		/* evaluate mismatch */
	    mis = _xpowerflow_mis(ybus,sbus,v,mis);
	    _xfdpf_pq(mis,vm,pv,pq,&P,&Q);
	    normP = _xvec_vnorm_inf(P);
	    normQ = _xvec_vnorm_inf(Q);
	    if (normP < opt->tol && normQ < opt->tol){
		    converged = _XTRUE;
			break;
	    }
	}

	/* deallocate */
	_xzvec_vfree(mis); 
	_xvec_vfree(va);_xvec_vfree(vm);_xvec_vfree(P);
	_xvec_vfree(Q);_xvec_vfree(dva);_xvec_vfree(dvm);
	_xperm_pfree(pBp);_xperm_pfree(pBpp);
	_xspmat_sfree(Bp);_xspmat_sfree(Bpp);
    
	return converged;
}

/*! caculate branches loss */
_xzvec *_xpowerflow_loss(_xpowerflow_xpc *xpc, _xzvec *loss){
	unsigned int i;
	double val;
	_xcomplex s,vf,vt;
	_xpowerflow_bus *bus;
	_xpowerflow_branch *branch;

	if (!xpc)
		_xerror(_XE_NULL,"_xloss");

	if (xpc->order.state != 'i')
		return NULL;

	loss = _xzvec_resize(loss,xpc->branch.ilen);
	for (i=0; i<xpc->branch.ilen; i++){
		branch = xpc->branch.bucket + i;
		if (branch->br_status <=0 ){
			s.re = s.im = 0.0f;
			loss->ve[i] = s;
			continue;
		}

		s.re = 1.0f,s.im = 0.0f;
		if (branch->tap != 0.0)
			s.re = branch->tap*cos(branch->shift*_XPI/180.0),
			s.im = branch->tap*sin(branch->shift*_XPI/180.0);

		bus = xpc->bus.bucket + branch->f_bus - 1;
		vf.re = bus->Vm*cos(bus->Va*_XPI/180.0),
		vf.im = bus->Vm*sin(bus->Va*_XPI/180.0);
		s = _xzdiv(vf,s);

		bus = xpc->bus.bucket + branch->t_bus - 1;
		vt.re = bus->Vm*cos(bus->Va*_XPI/180.0),
		vt.im = bus->Vm*sin(bus->Va*_XPI/180.0);
		s = _xzsub(s,vt);

		val = _xzabs(s); val = val*val*xpc->baseMVA;
		s.re = branch->br_r, s.im = -branch->br_x;
		s = _xzinv(s);
		s.re *= val, s.im *= val;

		loss->ve[i] = s;
	}

	return loss;
}

/*! update bus,gen,branch data structures to match power flow solution */
int _xpowerflow_pfsoln(_xpowerflow_xpc *xpc,int ref,_xivec *pv,_xivec *pq,_xzspmat *ybus,_xzspmat *yf,_xzspmat *yt,_xzvec *v){
	unsigned int i,on,refgen=-1;
	double refPg;
	_xivec *ngg=NULL;
	_xpowerflow_bus *bus;
	_xpowerflow_gen *gen;
	_xpowerflow_branch *branch;
	_xcomplex sf,st;

	if (xpc->order.state != 'i')
		return _XFALSE;

	/* update bus voltages */
	for (i=0; i<v->dim; i++){
		bus = xpc->bus.bucket + i;
		bus->Vm = _xzabs(v->ve[i]);
		bus->Va = atan2(v->ve[i].im,v->ve[i].re)*180.0/_XPI;
	}

	/* update Qg for all gens and Pg for slack bus(es)*/
	ngg = _xivec_get(xpc->bus.ilen);
	for (i=on=0; i<xpc->gen.ilen; i++){
		gen = xpc->gen.bucket + i;
		gen->Qg = 0.0f;/* zero out all Qg */
		if (gen->gen_status <= 0)
			continue;

		on++;
		ngg->ive[gen->gen_bus-1] += 1;
		bus = xpc->bus.bucket + gen->gen_bus - 1;
		sf = _xzmlt(v->ve[gen->gen_bus-1],_xzconj(_xzprod(ybus,v,gen->gen_bus-1)));
		gen->Qg = sf.im*xpc->baseMVA + bus->Qd;
		if (gen->gen_bus == ref) refgen=i,refPg=sf.re;
	}
	/* at this point any buses with more than one generator will have
	 * the total Q dispatch for the bus assigned to each generator. This
	 * must be split between them. We do it first equally, then in proportion
	 * to the reactive range of the generator.
	 */
	if (on > 1){
		/* divide Qg by number of generators at the bus to distribute equally */
		int prop = _XFALSE;
		for (i=0; i<xpc->gen.ilen; i++){
			gen = xpc->gen.bucket + i;
			if (gen->gen_status <= 0 || ngg->ive[gen->gen_bus-1] <= 1)
				continue;
			prop = _XTRUE;
			gen->Qg /= ngg->ive[gen->gen_bus-1];
		}
		/* divide proportionally */
		if (prop){
			double Qg;
			_xvec *Qg_tot,*Qg_min,*Qg_max;
			Qg_tot = _xvec_get(xpc->bus.ilen);
			Qg_min = _xvec_get(xpc->bus.ilen);
			Qg_max = _xvec_get(xpc->bus.ilen);

			for (i=0; i<xpc->gen.ilen; i++){
			    gen = xpc->gen.bucket + i;
			    if (gen->gen_status <= 0 || ngg->ive[gen->gen_bus-1] <= 1)
				    continue;
			   
				Qg_tot->ve[gen->gen_bus-1] = gen->Qg * ngg->ive[gen->gen_bus-1];
				Qg_min->ve[gen->gen_bus-1]+= gen->Qmin;
				Qg_max->ve[gen->gen_bus-1]+= gen->Qmax;
		    }

			for (i=0; i<xpc->gen.ilen; i++){
				gen = xpc->gen.bucket + i;
				if (gen->gen_status <= 0 || ngg->ive[gen->gen_bus-1] <= 1 ||
					Qg_min->ve[gen->gen_bus-1] == Qg_max->ve[gen->gen_bus-1])
				    continue;
				Qg = (Qg_tot->ve[gen->gen_bus-1]-Qg_min->ve[gen->gen_bus-1])/
					 (Qg_max->ve[gen->gen_bus-1]-Qg_min->ve[gen->gen_bus-1]+_XMACHEPS);
				Qg *= (gen->Qmax-gen->Qmin);
				gen->Qg = gen->Qmin + Qg;
			}

			_xvec_vfree(Qg_tot);_xvec_vfree(Qg_min);_xvec_vfree(Qg_max);
		}
	}

	/* update Pg for slack gen (one slack only) */
	if (refgen < 0){
		_xivec_ifree(ngg);
		return _XFALSE;
	}
	gen = xpc->gen.bucket + refgen;
	bus = xpc->bus.bucket + ref - 1;
	gen->Pg = refPg*xpc->baseMVA + bus->Pd;
	if (ngg->ive[ref-1] > 1){/* more than one generator at this ref bus */
		for (i=0; i<xpc->gen.ilen; i++){
			_xpowerflow_gen *gen2 = xpc->gen.bucket + i;
			if (i == refgen || gen2->gen_bus != ref) continue;
			gen->Pg -= gen2->Pg;
		}
	}
	_xivec_ifree(ngg);

	/* update/compute branch power flows */
	for (i=0; i<xpc->branch.ilen; i++){
		branch = xpc->branch.bucket + i;
		if (branch->br_status <= 0){/* out-of-service branches */
			branch->Pf = branch->Qf = branch->Pt = branch->Qt = 0.0f;
			continue;
		}
		/* in-service branches */
		sf = _xzmlt(v->ve[branch->f_bus-1],_xzconj(_xzprod(yf,v,i)));
		st = _xzmlt(v->ve[branch->t_bus-1],_xzconj(_xzprod(yt,v,i)));
		branch->Pf = sf.re*xpc->baseMVA,branch->Qf = sf.im*xpc->baseMVA;
		branch->Pt = st.re*xpc->baseMVA,branch->Qt = st.im*xpc->baseMVA;
	}

    return _XTRUE;
}

/*! run a power flow */
int _xpowerflow_runpf(_xpowerflow_xpc *xpc, _xoption *opt){
	clock_t t0;
	unsigned int i,mx,mn;
	double varef0;
	int ret,ref,ref0,state,repeat;
	_xpowerflow_bus *bus;
	_xpowerflow_gen *gen;
	_xivec *pv=NULL,*pq=NULL,*limit=NULL;
	_xzvec *v0=NULL,*v=NULL,*sbus=NULL;
	_xspmat *bp=NULL,*bpp=NULL;
	_xzspmat ybus={NULL,NULL},yf={NULL,NULL},yt={NULL,NULL};

	if (!xpc)
		_xerror(_XE_NULL,"_xpowerflow_runpf");

	/* convert to internal indexing */
	state = xpc->order.state;
	if (state != 'i'){
	    ret = _xpowerflow_ext2int(xpc);
	    if (!ret) return _XFALSE;
	}

	/* get bus index lists of each type of bus */
	ret = _xpowerflow_bustypes(xpc,&ref,&pv,&pq);
	t0 = clock();if (!ret) goto DONE;

	/* initial state */
	ret = _xpowerflow_v0(xpc,&v0);
	if (!ret) goto DONE;

	/* save index and angle of original reference bus */
	if (opt->qlim){
		ref0 = ref;
		bus = xpc->bus.bucket + ref - 1;
		varef0 = bus->Va;
		limit = _xivec_get(xpc->gen.elen);
	}

	repeat=1;
	while (repeat){
		/* build admittance matrices */
		ret = _xpowerflow_make_ybus(xpc,3,&ybus,&yf,&yt);
		if (!ret) break;
		
		/* compute complex bus power injections (generation - load)*/
		ret = _xpowerflow_make_sbus(xpc, &sbus);
		if (!ret) break;

		/* run the power flow */
		if (opt->alg == _XPF_ALG_NEWTON){
			ret = _xpowerflow_newtonpf(&ybus,sbus,v0,pv,pq,opt,&v);
			if (!ret) break;
		}
		else if (opt->alg == _XPF_ALG_FDPQXB || opt->alg == _XPF_ALG_FDPQBX){
			ret = _xpowerflow_make_b(xpc,2,opt->alg,&bp,&bpp);
			if (!ret) break;

			ret = _xpowerflow_fdpf(&ybus,sbus,v0,bp,bpp,pv,pq,opt,&v);
			if (!ret) break;
		}
		else if (_XPF_ALG_GAUSS){
			ret = _xpowerflow_gausspf(&ybus,sbus,v0,pv,pq,opt,&v);
			if (!ret) break;
		}
		else {
			_xerror(_XE_UNKNOWN,"only Newton\'s method, fast-decoupled, and Gauss-Seidel power flow algorithms currently implemented.");
		}

		/* update data matrices with solution */
		ret = _xpowerflow_pfsoln(xpc,ref,pv,pq,&ybus,&yf,&yt,v);
		if (!ret) break;

		if (opt->qlim){/* enforce generator Q limits */
			mx = mn = 0;
			for (i=0; i<xpc->gen.elen; i++){
				gen = xpc->gen.bucket + i;
				if (gen->gen_status > 0){
					if (gen->Qg > gen->Qmax)
						mx++,limit->ive[i] = 2;/*exceeds upper Q limit*/
					else if (gen->Qg < gen->Qmin)
						mn++,limit->ive[i] = 1;/*exceeds lower Q limit*/
				}
			}
			if (mx > 0 || mn > 0){/*we have some Q limit violations*/
				int ref_temp;
				if (pv->dim == 0){/*infeasible problem */
					ret = _XFALSE;
					break;
				}
				if (opt->qlim == 2){/*fix largest violation, ignore the rest*/
					int k=-1,vio;double max;/*one at a time?*/
					for (i=0; i<limit->dim; i++){
						if (limit->ive[i] <= 0) continue;
						gen = xpc->gen.bucket + i;
						if (k < 0){
							k = i; 
							max = limit->ive[i] == 2 ? gen->Qg - gen->Qmax : gen->Qmin - gen->Qg;
						}
						else if (limit->ive[i] == 2){
							if (gen->Qg - gen->Qmax > max) k = i, max = gen->Qg - gen->Qmax;
						}
						else {
							if (gen->Qmin - gen->Qg > max) k = i, max = gen->Qmin - gen->Qg;
						}
					}
					vio = limit->ive[k];
					_xivec_zero(limit);limit->ive[k] = vio;
				}
				/*convert to PQ bus*/
				for (i=0; i<limit->dim; i++){
				    if (limit->ive[i] <= 0) continue;
					gen = xpc->gen.bucket + i;
					gen->Qg = limit->ive[i] == 2 ? gen->Qmax : gen->Qmin;
					gen->gen_status = 0;/*temporarily turn off gen*/
					bus = xpc->bus.bucket + gen->gen_bus - 1;
					bus->bus_type = _XBUSTYPE_PQ;/*set bus type to PQ*/
					bus->Pd -= gen->Pg, bus->Qd -= gen->Qg;
				}
				/*update bus index lists of each type of bus*/
				ref_temp = ref;
				ret = _xpowerflow_bustypes(xpc,&ref,&pv,&pq);
				if (!ret){
					ret = _XFALSE;
					break;
				}
			}
			else
			    repeat=0;
		}
		else
			repeat=0;/* don't enforce generator Q limits, once is enough */
	}
	if (opt->qlim && limit){/*restore injections from limited gens (those at Q limits)*/
		for (i=0; i<limit->dim; i++){
		    if (limit->ive[i] <= 0) continue;
			gen = xpc->gen.bucket + i;
			gen->Qg = limit->ive[i] == 2 ? gen->Qmax : gen->Qmin;
			gen->gen_status = 1;/*turn gen back on*/
			bus = xpc->bus.bucket + gen->gen_bus - 1;
			bus->Pd += gen->Pg, bus->Qd += gen->Qg;
		}
		if (ref != ref0){
			/* adjust voltage angles to make original ref bus correct */
			bus = xpc->bus.bucket + ref0 - 1;
			varef0 -= bus->Va;
			for (i=0; i<xpc->bus.ilen; i++){
				bus = xpc->bus.bucket + i;
				bus->Va += varef0;
			}
		}
	}

DONE:
	_xivec_ifree(pv);_xivec_ifree(pq);_xivec_ifree(limit);
	_xzvec_vfree(v0);_xzvec_vfree(v);_xzvec_vfree(sbus);
	_xspmat_sfree(bp);_xspmat_sfree(bpp);
	_xzspmat_sfree(&ybus);_xzspmat_sfree(&yf);_xzspmat_sfree(&yt);

	xpc->et = (clock() - t0);
	xpc->success = ret;
	/* convert back to original bus numbering */
	if (state != xpc->order.state) _xpowerflow_int2ext(xpc);

	return ret;
}

/*!
 * form the power flow Jacobian
 */
int _xpowerflow_make_jac(_xpowerflow_xpc *xpc, _xzspmat *ybus, _xzspmat *yf, _xzspmat *yt, _xspmat **jac){
	int m,n,ret,ref;
	unsigned int i,j;
	double val;
	 _xzvec *v=NULL;
	_xivec *pv=NULL,*pq=NULL,*pv_pq=NULL;
	_xzspmat dvm={NULL,NULL},dva={NULL,NULL};
	_xspmat *J=NULL;

	/* get bus index lists of each type of bus */
	ret = _xpowerflow_bustypes(xpc,&ref,&pv,&pq);
	if (!ret) goto DONE;
	
	pv_pq = _xivec_get(pv->dim+pq->dim);
	_xmem_copy(&(pv->ive[0]),&(pv_pq->ive[0]),pv->dim*sizeof(int));
	_xmem_copy(&(pq->ive[0]),&(pv_pq->ive[pv->dim]),pq->dim*sizeof(int));

	/* build ybus */
	ret = _xpowerflow_make_ybus(xpc,3,ybus,yf,yt);
	if (!ret) goto DONE;

	/* extract voltage */
	ret = _xpowerflow_v0(xpc,&v);
	if (!ret) goto DONE;

    /* build Jacobian */
	ret = _xpowerflow_dsbus_dv(v,ybus,&dvm,&dva);
	J = _xspmat_resize(J,pv->dim+2*pq->dim,pv->dim+2*pq->dim);
	for (i=0; i<pv_pq->dim; i++){
	    n = pv_pq->ive[i]-1;
        for (j=0; j<pv_pq->dim; j++){/*j11*/
		    m = pv_pq->ive[j]-1;
			val = _xspmat_get_val(dva.re,m,n);
			if (val != 0.0){
				_xspmat_set_val(J,j,i,val);
			}
		}
		for (j=0; j<pq->dim; j++){/*j21*/
			m = pq->ive[j]-1;
			val = _xspmat_get_val(dva.im,m,n);
			if (val != 0.0){
				_xspmat_set_val(J,j+pv_pq->dim,i,val);
			}
		}
	}
	for (i=0; i<pq->dim; i++){
		n = pq->ive[i]-1;
		for (j=0; j<pv_pq->dim; j++){/*j12*/
			m = pv_pq->ive[j]-1;
			val = _xspmat_get_val(dvm.re,m,n);
			if (val != 0.0){
				_xspmat_set_val(J,j,i+pv_pq->dim,val);
			}
		}
		for (j=0; j<pq->dim; j++){/*j22*/
			m = pq->ive[j]-1;
			val = _xspmat_get_val(dvm.im,m,n);
			if (val != 0.0){
				_xspmat_set_val(J,j+pv_pq->dim,i+pv_pq->dim,val);
			}
		}
	}

	*jac = J, ret = _XTRUE;

DONE:
	_xzvec_vfree(v);
	_xzspmat_sfree(&dvm);_xzspmat_sfree(&dva);
	_xivec_ifree(pv);_xivec_ifree(pq);_xivec_ifree(pv_pq);

	return ret;
}

/*!
 * print power flow results
 */
int _xpowerflow_printpf(_xpowerflow_xpc *xpc, FILE *fd){
	int i,j,ret,state;
	int out_any=1,out_bus=1,out_branch=1;
	_xivec *i2e=NULL;
	_xzvec *loss=NULL;

	if (!xpc)
		_xerror(_XE_NULL,"_xpowerflow_printpf");
	if (!fd) fd=stdout;

	/* convert to internal indexing */
	state = xpc->order.state;
	if (state != 'i'){
	    ret = _xpowerflow_ext2int(xpc);
	    if (!ret) return _XFALSE;
	}
	i2e = xpc->order.i2e;

	if (out_any){
       /* convergence & elapsed time */
        if (xpc->success)
            fprintf(fd,"\nConverged in %.3f seconds",(double)(xpc->et)/CLOCKS_PER_SEC);
        else
            fprintf(fd,"\nDid not converge (%.3f seconds)\n",(double)(xpc->et)/CLOCKS_PER_SEC);
	}

	loss = _xpowerflow_loss(xpc,loss);
    #define isload(gen) ((gen)->Pmin < 0.0 && (gen)->Pmax == 0.0)

	/* bus data */
	if (out_bus){
		_xcomplex g,l,sumg,suml;
        fprintf(fd,"\n================================================================================");
        fprintf(fd,"\n|     Bus Data                                                                 |");
        fprintf(fd,"\n================================================================================");
        fprintf(fd,"\n Bus      Voltage          Generation             Load        ");
		fprintf(fd,"\n  #   Mag(pu) Ang(deg)   P (MW)   Q (MVAr)   P (MW)   Q (MVAr)");
		fprintf(fd,"\n----- ------- --------  --------  --------  --------  --------");
		
		sumg.re = sumg.im = suml.re = suml.im = 0.0f;
		for (i=0; i<xpc->bus.ilen; i++){
			_xpowerflow_bus *bus = xpc->bus.bucket + i;
			fprintf(fd,"\n%5d%7.3f%9.3f",i+1,
					bus->Vm,
					bus->Va);
			if (bus->bus_type == _XBUSTYPE_REF)
				fprintf(fd,"*");
			else
				fprintf(fd," ");

			suml.re += bus->Pd,suml.im += bus->Qd;

			g.re = g.im = l.re = l.im = 0.0f;
			for (j=0; j<xpc->gen.ilen; j++){
				_xpowerflow_gen *gen = xpc->gen.bucket + j;
				if (gen->gen_status > 0 && gen->gen_bus == bus->bus_i){
					if (isload(gen))
                        l.re += gen->Pg, l.im += gen->Qg;
					else
					    g.re += gen->Pg, g.im += gen->Qg;
				}
			}
			sumg = _xzadd(sumg,g);
			suml = _xzsub(suml,l);

			if (g.re != 0.0 || g.im != 0.0)
				fprintf(fd,"%9.2f%10.2f",g.re,g.im);
			else
				fprintf(fd,"      -         -  ");

			if (bus->Pd != 0.0 || bus->Qd != 0.0 || l.re != 0.0 || l.im != 0.0){
				if (l.re != 0.0 || l.im != 0.0)
					fprintf(fd,"%10.2f*%9.2f*",bus->Pd - l.re,bus->Qd - l.im);
				else
					fprintf(fd,"%10.2f%10.2f ",bus->Pd,bus->Qd);
			}
			else
				fprintf(fd,"       -         -   ");
		}
		fprintf(fd,"\n                        --------  --------  --------  --------");
		fprintf(fd,"\n               Total: %9.2f %9.2f %9.2f %9.2f",sumg.re,sumg.im,
                suml.re,suml.im);
        fprintf(fd,"\n");
	}

	/* branch data */
	if (out_branch){
		_xcomplex sum = _xzvec_sum(loss);

        fprintf(fd,"\n================================================================================");
        fprintf(fd,"\n|     Branch Data                                                              |");
        fprintf(fd,"\n================================================================================");
        fprintf(fd,"\nBrnch   From   To    From Bus Injection   To Bus Injection     Loss (I^2 * Z)  ");
        fprintf(fd,"\n  #     Bus    Bus    P (MW)   Q (MVAr)   P (MW)   Q (MVAr)   P (MW)   Q (MVAr)");
        fprintf(fd,"\n-----  -----  -----  --------  --------  --------  --------  --------  --------");
		for (i=0; i<xpc->branch.ilen; i++){
			_xpowerflow_branch *branch = xpc->branch.bucket + i;
			fprintf(fd,"\n%4d%7d%7d%10.2f%10.2f%10.2f%10.2f%10.3f%10.2f",i+1,
				    i2e->ive[branch->f_bus-1],
					i2e->ive[branch->t_bus-1],
					branch->Pf,
					branch->Qf,
					branch->Pt,
					branch->Qt,
					loss->ve[i].re,
					loss->ve[i].im);
			        
		}
        fprintf(fd,"\n                                                             --------  --------");
        fprintf(fd,"\n                                                    Total:%10.3f%10.2f",sum.re, sum.im);
        fprintf(fd,"\n");
	}
	_xzvec_vfree(loss);

	/* convert back to original bus numbering */
	if (state != xpc->order.state) _xpowerflow_int2ext(xpc);

	return ret;
}

