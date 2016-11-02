/* calc.c
calculate position and velocity of satellites
*/
#include "sat_location.h"

/* broadcast ephemeris to satellite position and clock bias --------------------
* compute satellite position and clock bias with broadcast ephemeris (gps,
* galileo, qzss)
* args   : double time      I   time (gpst)
*          eph_t *eph       I   broadcast ephemeris
*          double *rs       O   satellite position (ecef) {x,y,z} (m)
* return : 0 for correct
*-----------------------------------------------------------------------------*/
extern int eph2pos(double *curgpst,const eph_t *eph,double *rs)
{
    double tk,M,E,Ek,sinE,cosE,u,r,i,O,sin2u,cos2u,x,y,sinO,cosO,cosi,mu,omge,dt,time,dts;
    int n;
    if (eph->A<=0.0)
    {
        rs[0]=rs[1]=rs[2]=0.0;
        return -1;
    }
    time = curgpst[1];
	dt=timediff(time,eph->toes);
	dts=eph->f0+eph->f1*dt+eph->f2*dt*dt;
	time-=dts;
	dt=timediff(time,eph->toes);
	dts=eph->f0+eph->f1*dt+eph->f2*dt*dt;
	time-=dts;
    tk=timediff(time,eph->toes)+(curgpst[0]-(eph->week))*604800;

    mu=MU_GPS; omge=OMGE;

    M=eph->M0+(sqrt(mu/((eph->A)*(eph->A)*(eph->A)))+eph->deln)*tk;

    for (n=0,E=M,Ek=0.0;fabs(E-Ek)>RTOL_KEPLER;n++) {
        Ek=E; E-=(E-eph->e*sin(E)-M)/(1.0-eph->e*cos(E));
    }
    sinE=sin(E); cosE=cos(E);
    u=atan2(sqrt(1.0-eph->e*eph->e)*sinE,cosE-eph->e)+eph->omg;
    r=eph->A*(1.0-eph->e*cosE);
    i=eph->i0+eph->idot*tk;
    sin2u=sin(2.0*u); cos2u=cos(2.0*u);
    u+=eph->cus*sin2u+eph->cuc*cos2u;
    r+=eph->crs*sin2u+eph->crc*cos2u;
    i+=eph->cis*sin2u+eph->cic*cos2u;
    x=r*cos(u); y=r*sin(u); cosi=cos(i);

	O=eph->OMG0+(eph->OMGd-omge)*tk-omge*eph->toes;
    sinO=sin(O); cosO=cos(O);
    rs[0]=x*cosO-y*cosi*sinO;
    rs[1]=x*sinO+y*cosi*cosO;
    rs[2]=y*sin(i);
    return 0;
}

/* judge whether elevation angle > 15 deg ------------------------------------
* args   : double *rs          I   satellite position (ecef) {x,y,z} (m)
*          double *s           I   receiver position (ecef) {x,y,z} (m)
*          double *pos         I   receiver position (lla) {lat,lon,h} (m)
* return : 0 for false and 1 for true
*-----------------------------------------------------------------------------*/
extern int angle_judge(double *rs,double* r,double *pos)
{
    double del[3] = {rs[0]-r[0],rs[1]-r[1],rs[2]-r[2]}; //ecef
    double enu[3],sinlam,coslam,sinphi,cosphi; //enu
    sinlam = sin(pos[1]*D2R); coslam = cos(pos[1]*D2R);
    sinphi = sin(pos[0]*D2R); cosphi = cos(pos[0]*D2R);
    enu[0] = -sinlam*del[0]+coslam*del[1];
    enu[1] = -sinphi*coslam*del[0]-sinphi*sinlam*del[1]+cosphi*del[2];
    enu[2] = cosphi*coslam*del[0]+cosphi*sinlam*del[1]+sinphi*del[2];
    double sintheta = enu[2]/sqrt(SQR(enu[0])+SQR(enu[1])+SQR(enu[2]));

	if (sintheta>SIN_15)
		return 1;
	else
		return 0;
}

/* calculate position and velocity of satellites -----------------------------
* args   : eph_t *eph       I   satellite data
*          double *curgpst  I   current gpst {week,sec}
*          int sat          I   satellite number
*          double *pos      I   current position {lat,lon,h}
*          double *r        I   current position {x,y,z}
* return : 0 for unseen satellite and 1 for visible
*-----------------------------------------------------------------------------*/
extern int calc(eph_t *eph,double *curgpst,double *r,int sat,double* pos)
{
	if (sat==4) return 0;
	int i;
    double rs[6],nrs[3],dt=1E-3;
    /* calc pos */
    eph2pos(curgpst,eph,rs);
    if (angle_judge(rs,r,pos)==1)
	{
		curgpst[1] += dt;
		eph2pos(curgpst,eph,nrs);
		for (i=0;i<3;i++)
			rs[i+3] = (nrs[i]-rs[i])/dt;
        eph->x = rs[0];eph->y = rs[1];eph->z = rs[2];
		eph->vx = rs[3];eph->vy = rs[4];eph->vz = rs[5];
		return 1;
	}
	else
    {
        //printf("\nUnvisible!\n");
        return 0;
    }
}



