#include "sat_location.h"

const int EPH_ISAVAILABLE = 1;

int main()
{
    int i,j;
	eph_t eph[33],tmp_eph;
	double pos[3];/* input pos: {lat,lon,h} example: {39.983246,116.315509,57}*/
	double ep[7];/* input time: {year,month,day,hour,min,sec,timezone} example: {2001,9,4,9,59,44,8}*/
	/* input data */
	for (i=0;i<3;i++) scanf("%lf",&pos[i]);
	for (i=0;i<7;i++) scanf("%lf",&ep[i]);
	/* read sat data */
	for (i=1;i<=32;i++) eph[i].eph_isavailable = 0;
	if (EPH_ISAVAILABLE)
	{
		FILE* fpp = NULL;
		fpp = fopen("./resources/site247j.01n","r"); //eph file
		readrnxh(fpp);
		for(i=0;i<6;i++) 
		{
			readrnxnav(fpp,&tmp_eph);
			eph[tmp_eph.sat] = tmp_eph;
			eph[tmp_eph.sat].eph_isavailable = 1;
		}
		fclose(fpp); fpp = NULL;
	}
	FILE *fp = NULL;
	fp = fopen("./resources/current.txt","r"); //alm file
	for (i=1;i<=32;i++) readalm(fp,&eph[i],i);
	fclose(fp); fp = NULL;
	/* calculate pos and velovity*/
	double r[3];/* ecef {x,y,z} */
	double curgpst[2];/* current gpst */
	trans(ep,pos,r,curgpst);
    int cnt = 0;
    for (j=1;j<=32;j++)
    {
        if (calc(&eph[j],curgpst,r,j,pos))
        {
            printf("sat%d:\n",j);
            printf("x=%lf;y=%lf;z=%lf\n",eph[j].x,eph[j].y,eph[j].z);
            printf("vx=%lf;vy=%lf;vz=%lf\n\n",eph[j].vx,eph[j].vy,eph[j].vz);
            cnt++;
        }
	}
    printf("number of available satellite:%d\n",cnt);
	return 0;
}
