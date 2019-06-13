/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) models for system 
** information.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "xpower-array.h"

void _xdump_company(FILE *fp, void *ptr){
    _xmodel_company *cp;
    if (!ptr){
        (void) fprintf(fp,"\t## company    [0x00000000] =>NULL\n");
    }
    else {
        cp = (_xmodel_company *)ptr;
        (void) fprintf(fp,"\t## company    [0x%08lx] =>\n",(long)cp);
        (void) fprintf(fp,"\t#id           [0x%08lx] = %d\n",(long)&(cp->id),(int)(cp->id));
        (void) fprintf(fp,"\t#load_mw      [0x%08lx] = %-14.9g\n",(long)&(cp->load_mw),cp->load_mw);                          
        (void) fprintf(fp,"\t#load_mvar    [0x%08lx] = %-14.9g\n",(long)&(cp->load_mvar),cp->load_mvar);                        
        (void) fprintf(fp,"\t#power_factor [0x%08lx] = %-14.9g\n",(long)&(cp->power_factor),cp->power_factor);                   
        (void) fprintf(fp,"\t#gen_mw       [0x%08lx] = %-14.9g\n",(long)&(cp->gen_mw),cp->gen_mw);                      
        (void) fprintf(fp,"\t#gen_mvar     [0x%08lx] = %-14.9g\n",(long)&(cp->gen_mvar),cp->gen_mvar);                    
        (void) fprintf(fp,"\t#charge_mvar  [0x%08lx] = %-14.9g\n",(long)&(cp->charge_mvar),cp->charge_mvar);                  
        (void) fprintf(fp,"\t#shunt_mvar   [0x%08lx] = %-14.9g\n",(long)&(cp->shunt_mvar),cp->shunt_mvar);                   
        (void) fprintf(fp,"\t#ich_sched    [0x%08lx] = %-14.9g\n",(long)&(cp->ich_sched),cp->ich_sched);                 
        (void) fprintf(fp,"\t#ich_mw       [0x%08lx] = %-14.9g\n",(long)&(cp->ich_mw),cp->ich_mw);         
        (void) fprintf(fp,"\t#ich_mvar     [0x%08lx] = %-14.9g\n",(long)&(cp->ich_mvar),cp->ich_mvar);       
        (void) fprintf(fp,"\t#intern_ich   [0x%08lx] = %-14.9g\n",(long)&(cp->intern_ich),cp->intern_ich);     
        (void) fprintf(fp,"\t#loss_mw      [0x%08lx] = %-14.9g\n",(long)&(cp->loss_mw),cp->loss_mw);                        
        (void) fprintf(fp,"\t#loss_mvar    [0x%08lx] = %-14.9g\n",(long)&(cp->loss_mvar),cp->loss_mvar);                    
        (void) fprintf(fp,"\t#penalty      [0x%08lx] = %-14.9g\n",(long)&(cp->penalty),cp->penalty);       
    }
}

void _xdump_substation(FILE *fp, void *ptr){
    _xmodel_substation *sp;
    if (!ptr){
        (void) fprintf(fp,"\t## substation [0x00000000] =>NULL\n");
    }
    else {
        sp = (_xmodel_substation *)ptr;
        (void) fprintf(fp,"\t## substation [0x%08lx] =>\n",(long)sp);
        (void) fprintf(fp,"\t#id           [0x%08lx] = %d\n",(long)&(sp->id),(int)(sp->id));
        (void) fprintf(fp,"\t#company      [0x%08lx] = %d\n",(long)&(sp->company),(int)(sp->company));
        (void) fprintf(fp,"\t#loadarea     [0x%08lx] = %d\n",(long)&(sp->loadarea),(int)(sp->loadarea));
        (void) fprintf(fp,"\t#type         [0x%08lx] = %d\n",(long)&(sp->type),sp->type);
        (void) fprintf(fp,"\t#load_mw      [0x%08lx] = %-14.9g\n",(long)&(sp->load_mw),sp->load_mw);
        (void) fprintf(fp,"\t#load_mvar    [0x%08lx] = %-14.9g\n",(long)&(sp->load_mvar),sp->load_mvar);
        (void) fprintf(fp,"\t#power_factor [0x%08lx] = %-14.9g\n",(long)&(sp->power_factor),sp->power_factor);
        (void) fprintf(fp,"\t#gen_mw       [0x%08lx] = %-14.9g\n",(long)&(sp->gen_mw),sp->gen_mw);
        (void) fprintf(fp,"\t#gen_mvar     [0x%08lx] = %-14.9g\n",(long)&(sp->gen_mvar),sp->gen_mvar);
        (void) fprintf(fp,"\t#shunt_mvar   [0x%08lx] = %-14.9g\n",(long)&(sp->shunt_mvar),sp->shunt_mvar);
        
    }
}

void _xdump_section(FILE *fp, void *ptr){
    _xmodel_section *sp;
    if (!ptr){
        (void) fprintf(fp,"\t## section  [0x00000000] =>NULL\n");
    }
    else {
        sp = (_xmodel_section *)ptr;
        (void) fprintf(fp,"\t## section  [0x%08lx] =>\n",(long)sp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = [0x%08lx]\n",(long)&(sp->id),(int)(sp->id));
        (void) fprintf(fp,"\t#company    [0x%08lx] = %d\n",(long)&(sp->company),(int)(sp->company));
        (void) fprintf(fp,"\t#substation [0x%08lx] = %d\n",(long)&(sp->substation),(int)(sp->substation));
        (void) fprintf(fp,"\t#breakers   [0x%08lx] = [0x%08lx]\n",(long)&(sp->breakers),(long)(sp->breakers));
        (void) fprintf(fp,"\t#shunts     [0x%08lx] = [0x%08lx]\n",(long)&(sp->shunts),sp->shunts);
        (void) fprintf(fp,"\t#generators [0x%08lx] = [0x%08lx]\n",(long)&(sp->generators),sp->generators);
        (void) fprintf(fp,"\t#nodes      [0x%08lx] = [0x%08lx]\n",(long)&(sp->nodes),sp->nodes);
        (void) fprintf(fp,"\t#loads      [0x%08lx] = [0x%08lx]\n",(long)&(sp->loads),sp->loads);
        (void) fprintf(fp,"\t#scaps      [0x%08lx] = [0x%08lx]\n",(long)&(sp->scaps),sp->scaps);
        (void) fprintf(fp,"\t#voltages   [0x%08lx] = [0x%08lx]\n",(long)&(sp->voltages),sp->voltages);
        (void) fprintf(fp,"\t#svcs       [0x%08lx] = [0x%08lx]\n",(long)&(sp->svcs),sp->svcs);
        (void) fprintf(fp,"\t#couplers   [0x%08lx] = [0x%08lx]\n",(long)&(sp->couplers),sp->couplers);
        (void) fprintf(fp,"\t#nominal    [0x%08lx] = %-14.9g\n",(long)&(sp->nominal),sp->nominal);
        (void) fprintf(fp,"\t#desired    [0x%08lx] = %-14.9g\n",(long)&(sp->desired),sp->desired);
        (void) fprintf(fp,"\t#high       [0x%08lx] = %-14.9g\n",(long)&(sp->high),sp->high);
        (void) fprintf(fp,"\t#low        [0x%08lx] = %-14.9g\n",(long)&(sp->low),sp->low);
        (void) fprintf(fp,"\t#ehigh      [0x%08lx] = %-14.9g\n",(long)&(sp->ehigh),sp->ehigh);
        (void) fprintf(fp,"\t#elow       [0x%08lx] = %-14.9g\n",(long)&(sp->elow),sp->elow);
        (void) fprintf(fp,"\t#buses      [0x%08lx] = %d\n",(long)&(sp->buses),sp->buses);
        (void) fprintf(fp,"\t#status     [0x%08lx] = %08x\n",(long)&(sp->status),*(int *)(&(sp->status)));
    }
}

void _xdump_node(FILE *fp, void *ptr){
    _xmodel_node *np;
    if (!ptr){
        (void) fprintf(fp,"\t## node   [0x00000000] =>NULL\n");
    }
    else {
        np = (_xmodel_node *)ptr;
        (void) fprintf(fp,"\t## node   [0x%08lx] =>\n",(long)np);
        (void) fprintf(fp,"\t#id       [0x%08lx] = %d\n",(long)&(np->id),(int)(np->id));
        (void) fprintf(fp,"\t#section  [0x%08lx] = %d\n",(long)&(np->section),(int)(np->section));
        (void) fprintf(fp,"\t#velement [0x%08lx] = [0x%08lx]:[0x%08lx]\n",(long)&(np->velement),
                      (long)(np->velement.prev),
                      (long)(np->velement.next));
        (void) fprintf(fp,"\t#busbar   [0x%08lx] = %d\n",(long)&(np->busbar),(int)(np->busbar));
        (void) fprintf(fp,"\t#assign   [0x%08lx] = %d\n",(long)&(np->assign),(int)(np->assign));
    }
}

void _xdump_breaker(FILE *fp, void *ptr){
    _xmodel_breaker *bp;
    if (!ptr){
        (void) fprintf(fp,"\t## breaker [0x00000000] =>NULL\n");
    }
    else {
        bp = (_xmodel_breaker *)ptr;
        (void) fprintf(fp,"\t## breaker [0x%08lx] =>\n",(long)bp);
        (void) fprintf(fp,"\t#id        [0x%08lx] = %d\n",(long)&(bp->id),(int)(bp->id));
        (void) fprintf(fp,"\t#section   [0x%08lx] = %d\n",(long)&(bp->section),(int)(bp->section));
        (void) fprintf(fp,"\t#fnode     [0x%08lx] = %d\n",(long)&(bp->fnode),(int)(bp->fnode));
        (void) fprintf(fp,"\t#tnode     [0x%08lx] = %d\n",(long)&(bp->tnode),(int)(bp->tnode));
        (void) fprintf(fp,"\t#velement  [0x%08lx] = [0x%08lx]:[0x%08lx]\n",(long)&(bp->velement),
                      (long)(bp->velement.prev),
                      (long)(bp->velement.next));
        (void) fprintf(fp,"\t#meas      [0x%08lx] = %d\n",(long)&(bp->meas),bp->meas);
        (void) fprintf(fp,"\t#status    [0x%08lx] = %08x\n",(long)&(bp->status),*(int *)(&(bp->status)));
        (void) fprintf(fp,"\t#old       [0x%08lx] = %08x\n",(long)&(bp->old),*(int *)(&(bp->old)));
        (void) fprintf(fp,"\t#display   [0x%08lx] = %08x\n",(long)&(bp->display),*(int *)(&(bp->display)));
    }
}

void _xdump_coupler(FILE *fp, void *ptr){
    _xmodel_coupler *cp;
    if (!ptr){
        (void) fprintf(fp,"\t## coupler [0x00000000] =>NULL\n");
    }
    else {
        cp = (_xmodel_coupler *)ptr;
        (void) fprintf(fp,"\t## coupler [0x%08lx] =>\n",(long)cp);
        (void) fprintf(fp,"\t#id        [0x%08lx] = %d\n",(long)&(cp->id),(int)(cp->id));
        (void) fprintf(fp,"\t#breaker   [0x%08lx] = %d\n",(long)&(cp->breaker),(int)(cp->breaker));
        (void) fprintf(fp,"\t#velement  [0x%08lx] = [0x%08lx]:[0x%08lx]\n",(long)&(cp->velement),
                      (long)(cp->velement.prev),
                      (long)(cp->velement.next));
        (void) fprintf(fp,"\t#meas      [0x%08lx] = %d\n",(long)&(cp->meas),(int)(cp->meas));
        (void) fprintf(fp,"\t#status    [0x%08lx] = %08x\n",(long)&(cp->status),*((int *)&(cp->status)));
        (void) fprintf(fp,"\t#flow_mw   [0x%08lx] = %-14.9g\n",(long)&(cp->flow_mw),cp->flow_mw);
        (void) fprintf(fp,"\t#flow_mvar [0x%08lx] = %-14.9g\n",(long)&(cp->flow_mvar),cp->flow_mvar);
    }
}

void _xdump_loadarea(FILE *fp, void *ptr){
    _xmodel_loadarea *lp;
    if (!ptr){
        (void) fprintf(fp,"\t## loadarea [0x00000000] =>NULL\n");
    }
    else {
        lp = (_xmodel_loadarea *)ptr;
        (void) fprintf(fp,"\t## loadarea [0x%08lx] =>\n",(long)lp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(lp->id),(int)(lp->id));
        (void) fprintf(fp,"\t#meas_mw    [0x%08lx] = %-14.9g\n",(long)&(lp->meas_mw),lp->meas_mw);
        (void) fprintf(fp,"\t#ble_mw     [0x%08lx] = %-14.9g\n",(long)&(lp->ble_mw),lp->ble_mw);
        (void) fprintf(fp,"\t#est_mw     [0x%08lx] = %-14.9g\n",(long)&(lp->est_mw),lp->est_mw);
        (void) fprintf(fp,"\t#des_mw     [0x%08lx] = %-14.9g\n",(long)&(lp->des_mw),lp->des_mw);
        (void) fprintf(fp,"\t#dts_mw     [0x%08lx] = %-14.9g\n",(long)&(lp->dts_mw),lp->dts_mw);
        (void) fprintf(fp,"\t#meas       [0x%08lx] = %d\n",(long)&(lp->meas),lp->meas);
        (void) fprintf(fp,"\t#status     [0x%08lx] = %08x\n",(long)&(lp->status),*((int *)&(lp->status)));
    }
}

void _xdump_load(FILE *fp, void *ptr){
    _xmodel_load *lp;
    if (!ptr){
        (void) fprintf(fp,"\t## load [0x00000000] =>NULL\n");
    }
    else {
        lp = (_xmodel_load *)ptr;
        (void) fprintf(fp,"\t## load [0x%08lx] =>\n",(long)lp);
        (void) fprintf(fp,"\t#id     [0x%08lx] = %d\n",(long)&(lp->id),(int)(lp->id));
        /*##TODO*/
    }
}

void _xdump_generator(FILE *fp, void *ptr){
    _xmodel_generator *gp;
    if (!ptr){
        (void) fprintf(fp,"\t## generator [0x00000000] =>NULL\n");
    }
    else {
        gp = (_xmodel_generator *)ptr;
        (void) fprintf(fp,"\t## generator [0x%08lx] =>\n",(long)gp);
        (void) fprintf(fp,"\t#id          [0x%08lx] = %d\n",(long)&(gp->id),(int)(gp->id));
        /*##TODO*/
    }
}

void _xdump_svc(FILE *fp, void *ptr){
    _xmodel_svc *sp;
    if (!ptr){
        (void) fprintf(fp,"\t## svc [0x00000000] =>NULL\n");
    }
    else {
        sp = (_xmodel_svc *)ptr;
        (void) fprintf(fp,"\t## svc [0x%08lx] =>\n",(long)sp);
        (void) fprintf(fp,"\t#id    [0x%08lx] = %d\n",(long)&(sp->id),(int)(sp->id));
        /*##TODO*/
    }
}

void _xdump_branch(FILE *fp, void *ptr){
    _xmodel_branch *bp;
    if (!ptr){
        (void) fprintf(fp,"\t## branch [0x00000000] =>NULL\n");
    }
    else {
        bp = (_xmodel_branch *)ptr;
        (void) fprintf(fp,"\t## branch [0x%08lx] =>\n",(long)bp);
        (void) fprintf(fp,"\t#id       [0x%08lx] = %d\n",(long)&(bp->id),(int)(bp->id));
        /*##TODO*/
    }
}

void _xdump_residual(FILE *fp, void *ptr){
    _xmodel_residual *rp;
    if (!ptr){
        (void) fprintf(fp,"\t## residual [0x00000000] =>NULL\n");
    }
    else {
        rp = (_xmodel_residual *)ptr;
        (void) fprintf(fp,"\t## residual [0x%08lx] =>\n",(long)rp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(rp->id),(int)(rp->id));
        /*##TODO*/
    }
}

void _xdump_shunt(FILE *fp, void *ptr){
    _xmodel_shunt *sp;
    if (!ptr){
        (void) fprintf(fp,"\t## shunt [0x00000000] =>NULL\n");
    }
    else {
        sp = (_xmodel_shunt *)ptr;
        (void) fprintf(fp,"\t## shunt [0x%08lx] =>\n",(long)sp);
        (void) fprintf(fp,"\t#id      [0x%08lx] = %d\n",(long)&(sp->id),(int)(sp->id));
        /*##TODO*/
    }
}

void _xdump_capsegment(FILE *fp, void *ptr){
    _xmodel_capsegment *cp;
    if (!ptr){
        (void) fprintf(fp,"\t## capsegment [0x00000000] =>NULL\n");
    }
    else {
        cp = (_xmodel_capsegment *)ptr;
        (void) fprintf(fp,"\t## capsegment [0x%08lx] =>\n",(long)cp);
        (void) fprintf(fp,"\t#id           [0x%08lx] = %d\n",(long)&(cp->id),(int)(cp->id));
        /*##TODO*/
    }
}

void _xdump_capacitor(FILE *fp, void *ptr){
    _xmodel_capacitor *cp;
    if (!ptr){
        (void) fprintf(fp,"\t## capacitor [0x00000000] =>NULL\n");
    }
    else {
        cp = (_xmodel_capacitor *)ptr;
        (void) fprintf(fp,"\t## capacitor [0x%08lx] =>\n",(long)cp);
        (void) fprintf(fp,"\t#id          [0x%08lx] = %d\n",(long)&(cp->id),(int)(cp->id));
        /*##TODO*/
    }
}

void _xdump_tieline(FILE *fp, void *ptr){
    _xmodel_tieline *tp;
    if (!ptr){
        (void) fprintf(fp,"\t## tieline [0x00000000] =>NULL\n");
    }
    else {
        tp = (_xmodel_tieline *)ptr;
        (void) fprintf(fp,"\t## tieline [0x%08lx] =>\n",(long)tp);
        (void) fprintf(fp,"\t#id        [0x%08lx] = %d\n",(long)&(tp->id),(int)(tp->id));
        /*##TODO*/
    }
}

void _xdump_vxformer(FILE *fp, void *ptr){
    _xmodel_vxformer *vp;
    if (!ptr){
        (void) fprintf(fp,"\t## vxformer [0x00000000] =>NULL\n");
    }
    else {
        vp = (_xmodel_vxformer *)ptr;
        (void) fprintf(fp,"\t## vxformer [0x%08lx] =>\n",(long)vp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(vp->id),(int)(vp->id));
        /*##TODO*/
    }
}

void _xdump_pxformer(FILE *fp, void *ptr){
    _xmodel_pxformer *pp;
    if (!ptr){
        (void) fprintf(fp,"\t## pxformer [0x00000000] =>NULL\n");
    }
    else {
        pp = (_xmodel_pxformer *)ptr;
        (void) fprintf(fp,"\t## pxformer [0x%08lx] =>\n",(long)pp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(pp->id),(int)(pp->id));
        /*##TODO*/
    }
}

void _xdump_interface(FILE *fp, void *ptr){
    _xmodel_interface *ip;
    if (!ptr){
        (void) fprintf(fp,"\t## interface [0x00000000] =>NULL\n");
    }
    else {
        ip = (_xmodel_interface *)ptr;
        (void) fprintf(fp,"\t## interface [0x%08lx] =>\n",(long)ip);
        (void) fprintf(fp,"\t#id          [0x%08lx] = %d\n",(long)&(ip->id),(int)(ip->id));
        /*##TODO*/
    }
}

void _xdump_icomponent(FILE *fp, void *ptr){
    _xmodel_icomponent *ip;
    if (!ptr){
        (void) fprintf(fp,"\t## icomponent [0x00000000] =>NULL\n");
    }
    else {
        ip = (_xmodel_icomponent *)ptr;
        (void) fprintf(fp,"\t## icomponent [0x%08lx] =>\n",(long)ip);
        (void) fprintf(fp,"\t#id           [0x%08lx] = %d\n",(long)&(ip->id),(int)(ip->id));
        /*##TODO*/
    }
}

void _xdump_voltage(FILE *fp, void *ptr){
    _xmodel_voltage *vp;
    if (!ptr){
        (void) fprintf(fp,"\t## voltage [0x00000000] =>NULL\n");
    }
    else {
        vp = (_xmodel_voltage *)ptr;
        (void) fprintf(fp,"\t## voltage [0x%08lx] =>\n",(long)vp);
        (void) fprintf(fp,"\t#id        [0x%08lx] = %d\n",(long)&(vp->id),(int)(vp->id));
        /*##TODO*/
    }
}

void _xdump_tap(FILE *fp, void *ptr){
    _xmodel_tap *tp;
    if (!ptr){
        (void) fprintf(fp,"\t## tap [0x00000000] =>NULL\n");
    }
    else {
        tp = (_xmodel_tap *)ptr;
        (void) fprintf(fp,"\t## tap [0x%08lx] =>\n",(long)tp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(tp->id),(int)(tp->id));
        /*##TODO*/
    }
}

void _xdump_swingbus(FILE *fp, void *ptr){
    _xmodel_swingbus *sp;
    if (!ptr){
        (void) fprintf(fp,"\t## swingbus [0x00000000] =>NULL\n");
    }
    else {
        sp = (_xmodel_swingbus *)ptr;
        (void) fprintf(fp,"\t## swingbus [0x%08lx] =>\n",(long)sp);
        (void) fprintf(fp,"\t#id         [0x%08lx] = %d\n",(long)&(sp->id),(int)(sp->id));
        /*##TODO*/
    }
}

void _xdump_motor(FILE *fp, void *ptr){
    _xmodel_motor *mp;
    if (!ptr){
        (void) fprintf(fp,"\t## motor [0x00000000] =>NULL\n");
    }
    else {
        mp = (_xmodel_motor *)ptr;
        (void) fprintf(fp,"\t## motor [0x%08lx] =>\n",(long)mp);
        (void) fprintf(fp,"\t#id      [0x%08lx] = %d\n",(long)&(mp->id),(int)(mp->id));
        /*##TODO*/
    }
}

void _xdump_case(FILE *fp, void *ptr){
    _xmodel_case *cp;
    if (!ptr){
        (void) fprintf(fp,"\t## case [0x00000000] =>NULL\n");
    }
    else {
        cp = (_xmodel_case *)ptr;
        (void) fprintf(fp,"\t## case [0x%08lx] =>\n",(long)cp);
        (void) fprintf(fp,"\t#id     [0x%08lx] = %d\n",(long)&(cp->id),(int)(cp->id));
        /*##TODO*/
    }
}
