/*
read.c
read data from ephemeris and almanac data
*/
#include "sat_location.h"

/* string to number ------------------------------------------------------------
* convert substring in string to number
* args   : char   *s        I   string ("... nnn.nnn ...")
*          int    i,n       I   substring position and width
* return : converted number (0.0:error)
*-----------------------------------------------------------------------------*/
extern double str2num(const char *s, int i, int n)
{
    double value;
    char str[256],*p=str;

    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<n) return 0.0;
    for (s+=i;*s&&--n>=0;s++) *p++=*s=='d'||*s=='D'?'E':*s; *p='\0';
    return sscanf(str,"%lf",&value)==1?value:0.0;
}

/* string to time --------------------------------------------------------------
* read substring in string to time
* args   : char   *s        I   string ("... yyyy mm dd hh mm ss ...")
*          int    i,n       I   substring position and width
* return : status (0:ok,0>:error)
*-----------------------------------------------------------------------------*/
extern int str2time(const char *s,int i,int n)
{
    double ep[6];
    char str[256],*p=str;

    if (i<0||(int)strlen(s)<i||(int)sizeof(str)-1<i) return -1;
    for (s+=i;*s&&--n>=0;) *p++=*s++; *p='\0';
    if (sscanf(str,"%lf %lf %lf %lf %lf %lf",ep,ep+1,ep+2,ep+3,ep+4,ep+5)<6)
        return -1;
    if (ep[0]<100.0) ep[0]+=ep[0]<80.0?2000.0:1900.0;
    return 0;
}

/* decode ephemeris ----------------------------------------------------------*/
static int decode_eph(int sat,const double *data,eph_t *eph)
{
    /* svh and toc ignored*/
	eph->sat=sat;

    eph->f0=data[0];
    eph->f1=data[1];
    eph->f2=data[2];

    eph->A=SQR(data[10]); eph->e=data[ 8]; eph->i0  =data[15]; eph->OMG0=data[13];
    eph->omg =data[17]; eph->M0 =data[ 6]; eph->deln=data[ 5]; eph->OMGd=data[18];
    eph->idot=data[19]; eph->crc=data[16]; eph->crs =data[ 4]; eph->cuc =data[ 7];
    eph->cus =data[ 9]; eph->cic=data[12]; eph->cis =data[14];

    eph->toes=     data[11];      /* toe (s) in gps week */
    eph->week=(int)data[21];      /* gps week */

    return 0;
}

/* read data from alm file ----------------------------------------------------------*/
extern int readalm(FILE *fp,eph_t *eph,int sat)
{
	int i;
	double almdata[13],data[64];
	char buff[MAXALMLEN];
	/* PRN4 has retired */
	eph->svh = 1;
	if (sat==4)	return 0;
	/* skip if eph is available */
	if (eph->eph_isavailable!=0) {
		for (i=0;i<15;i++)
			fgets(buff,MAXALMLEN,fp);
		return 0;
	}
	/* read sat ID field */
	fgets(buff,MAXALMLEN,fp);
	/* decode data field */
	for (i=0;i<13;i++) {
		fgets(buff,MAXALMLEN,fp);
		almdata[i] = str2num(buff,27,18);
	}
	/* read line feed */
	if (sat!=32) fgets(buff,MAXALMLEN,fp);
	/* almdata->data */
	memset(data,0.0,sizeof(data));
	int data_map[12] = {24,8,11,15,18,10,13,17,6,0,1,21};
	for (i=1;i<=12;i++)
		data[data_map[i-1]] = almdata[i];
	decode_eph(sat,data,eph);

	return 0;
}

 /* read rinex header ---------------------------------------------------------*/
extern int readrnxh(FILE *fp)
{
    char buff[MAXRNXLEN],*label=buff+60;

    while (fgets(buff,MAXRNXLEN,fp)) {

        if (strlen(buff)<=60) continue;

        else if (strstr(label,"RINEX VERSION / TYPE")) {
            continue;
        }
        else if (strstr(label,"PGM / RUN BY / DATE")) continue;
        else if (strstr(label,"COMMENT")) { /* opt */

            /* read cnes wl satellite fractional bias */
            if (strstr(buff,"WIDELANE SATELLITE FRACTIONAL BIASES")||
                strstr(buff,"WIDELANE SATELLITE FRACTIONNAL BIASES")) {
            }
            continue;
        }
        if (strstr(label,"END OF HEADER")) return 0;
    }
    return 1;
}

/* read rinex navigation data -------------------------------------------*/
extern int readrnxnav(FILE *fp, eph_t *eph)
{
    double data[64];
    int i=0,j,sat=0,sp=3;
    char buff[MAXRNXLEN],*p;
    eph->svh = 1;
    while (fgets(buff,MAXRNXLEN,fp)) {

        if (i==0) {
			sat=(int)str2num(buff,0,2);
            /* decode toc field */
            if (str2time(buff+sp,0,19)) {
                printf("rinex nav toc error: %23.23s\n",buff);
                return -1;
            }
            /* decode data fields */
            for (j=0,p=buff+sp+19;j<3;j++,p+=19) {
                data[i++]=str2num(p,0,19);
            }
        }
        else {
            /* decode data fields */
            for (j=0,p=buff+sp;j<4;j++,p+=19) {
                data[i++]=str2num(p,0,19);
            }
            /* decode ephemeris */
            if (i>=31) {
                return decode_eph(sat,data,eph);
            }
        }
    }
    return -1;
}
