/* trans.c
time and coordinate transformation
*/
#include "sat_location.h"

const double leaps[MAXLEAPS+1][7]={ /* leap seconds (y,m,d,h,m,s,utc-gpst) */
    {2015,7,1,0,0,0,-17},
    {2012,7,1,0,0,0,-16},
    {2009,1,1,0,0,0,-15},
    {2006,1,1,0,0,0,-14},
    {1999,1,1,0,0,0,-13},
    {1997,7,1,0,0,0,-12},
    {1996,1,1,0,0,0,-11},
    {1994,7,1,0,0,0,-10},
    {1993,7,1,0,0,0, -9},
    {1992,7,1,0,0,0, -8},
    {1991,1,1,0,0,0, -7},
    {1990,1,1,0,0,0, -6},
    {1988,1,1,0,0,0, -5},
    {1985,7,1,0,0,0, -4},
    {1983,7,1,0,0,0, -3},
    {1982,7,1,0,0,0, -2},
    {1981,7,1,0,0,0, -1},
    {0}
};

/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to real time(s)
* args   : double *ep       I   day/time {year,month,day,hour,min,sec,timezone}
*          double *time     O   real time(s)
* return : 0 for correct
*-----------------------------------------------------------------------------*/
extern int epoch2time(double *time,const double *ep)
{
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
	int days,year=(int)ep[0],mon=(int)ep[1],day=(int)ep[2];
    if (year<1970||mon<1||12<mon) return 1;
    /* leap year if year%4==0 in 1901-2099 */
    days=(year-1970)*365+(year-1969)/4+doy[mon-1]+day-2+(year%4==0&&mon>=3?1:0);
    *time=(int)days*86400+(int)(ep[3]-ep[6])*3600+ep[4]*60+ep[5];
    return 0;
}

/* calculate gps time(sec) difference -------------------------------------------
* args   : double t1,t2     I   gps time(sec)
* return : time difference
*-----------------------------------------------------------------------------*/
extern double timediff(double t1,double t2)
{
	 double half_week=302400,tt;
	 tt=t1-t2;
	 if (tt>half_week)
		 tt=tt-2*half_week;
	 else if (tt<-half_week)
		 tt=tt+2*half_week;
	 return tt;
}

/* time and coordinate transformation -------------------------------------------
* local time->gpst & local position->ecef
* args   : double *ep       I   day/time {year,month,day,hour,min,sec,timezone}
*          double *pos      I   pos {lat,lon,height}
*          double *r        O   ecef coordinate {x,y,z}
*          double *curgpst  O   current gpst {week,sec}
* return : 0 for correct
*-----------------------------------------------------------------------------*/
extern int trans(double *ep,double *pos,double *r,double *curgpst)
{
    int i;
    /* pos2ecef */
    double sinp=sin(pos[0]*D2R),cosp=cos(pos[0]*D2R),sinl=sin(pos[1]*D2R),cosl=cos(pos[1]*D2R);
    double e2=FE_WGS84*(2.0-FE_WGS84),v=RE_WGS84/sqrt(1.0-e2*sinp*sinp);
    r[0]=(v+pos[2])*cosp*cosl;
    r[1]=(v+pos[2])*cosp*sinl;
    r[2]=(v*(1.0-e2)+pos[2])*sinp;
    /* time2gpst */
    const static double gpst0[] = {1980,1,6,0,0,0,0}; /* gps time reference */
    double t0; epoch2time(&t0,gpst0);
    double t; epoch2time(&t,ep);
    /* leap seconds */
    for (i=0;leaps[i][0]>0;i++)
    {
        double tt; epoch2time(&tt,leaps[i]);
        if (t-tt>=0.0)
        {
            t -= leaps[i][6];
            break;
        }
    }
    double sec = t-t0;
    int week = (int)(sec/604800);
    sec = (double)(sec-week*604800);
    curgpst[0] = week;
    curgpst[1] = sec;
    return 0;
}
