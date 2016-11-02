/* sat location.h
*/
#include <stdio.h>
#include <string.h>
#include <math.h>

#define SQR(x)   ((x)*(x))

#define RE_WGS84    6378137.0           /* earth semimajor axis (WGS84) (m) */
#define FE_WGS84    (1.0/298.257223563) /* earth flattening (WGS84) */
#define MU_GPS   3.9860050E14     /* gravitational constant         ref [1] */
#define OMGE        7.2921151467E-5     /* earth angular velocity (IS-GPS) (rad/s) */
#define RTOL_KEPLER 1E-14         /* relative tolerance for Kepler equation */
#define SIN_15  0.2588190451 /* sin(15.0 deg) */
#define MAXALMLEN 50    /* max number of obs type in almanac */
#define MAXOBSTYPE  64                  /* max number of obs type in RINEX */
#define MAXRNXLEN   (16*MAXOBSTYPE+4)   /* max rinex record length */
#define MAXLEAPS    64                  /* max number of leap seconds table */
#define PI 3.1415926535898
#define D2R         (PI/180.0)          /* deg to rad */

typedef struct {
    int sat;            /* satellite number */
    int svh;            /* SV health (0:ok) */
    int week;           /* GPS/QZS: gps week, GAL: galileo week */
    double toe,toc; 	/* Toe,Toc */
                        /* SV orbit parameters */
    double A,e,i0,OMG0,omg,M0,deln,OMGd,idot;
    double crc,crs,cuc,cus,cic,cis;
    int toes;        /* Toe (s) in week */
    double f0,f1,f2;    /* SV clock parameters (af0,af1,af2) */
    int eph_isavailable; /* whether there is eph data for calculating. If not, use almanac instead */
	double x,y,z,vx,vy,vz; /* pos and velocity */
} eph_t;

extern int str2time(const char *s,int i,int n);
extern double str2num(const char *s, int i, int n);
extern int readalm(FILE *fp,eph_t *eph,int sat);
extern int readrnxh(FILE *fp);
extern int readrnxnav(FILE *fp, eph_t *eph);
extern int calc(eph_t *eph,double *curgpst,double *r,int sat,double* pos);
extern double timediff (double t1,double t2);
extern int trans(double *ep,double *pos,double *r,double *curgpst);
extern int epoch2time(double *time,const double *ep);
extern int eph2pos(double *curgpst,const eph_t *eph,double *rs);
extern int angle_judge(double *rs,double* r,double *pos);


