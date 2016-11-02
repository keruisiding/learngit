/* test_calc.c
 */
#include <string.h>
#include "sat_location.h"
#include "unity.h"


void test_epoch2time()
{
    double ep[7] = {1970,1,1,0,0,0,0}, t;
    int i;
    for (i=-12;i<=12;i++)
    {
        ep[6] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(-i*3600.0,t,"Wrong time (timezone)");
    }
    ep[6] = 0;
    for (i=0;i<60;i++)
    {
        ep[5] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(i,t,"Wrong time (seconds)");
    }
    ep[5] = 0.0;
    for (i=0;i<60;i++)
    {
        ep[4] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(i*60,t,"Wrong time (minutes)");
    }
    ep[4] = 0;
    for (i=0;i<23;i++)
    {
        ep[3] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(i*3600,t,"Wrong time (hours)");
    }
    ep[3] = 0;
    for (i=1;i<=365;i++)
    {
        ep[2] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE((i-1)*86400,t,"Wrong time (days)");
    }
    ep[2] = 1;
    const int doy[]={1,32,60,91,121,152,182,213,244,274,305,335};
    for (i=1;i<=12;i++)
    {
        ep[1] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE((doy[i-1]-1)*86400,t,"Wrong time (months)");
    }
    ep[1] = 1;
    for (i=1970;i<=2099;i++)
    {
        ep[0] = i; epoch2time(&t,ep);
        TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(((i-1970)*365+(i-1969)/4)*86400,t,"Wrong time (years)");
    }
}

void ephinit(eph_t *eph)
{
    eph->sat = 1;
    eph->toes = 244800;
    eph->A = SQR(5153.65531);
    eph->e = 0.005912038265;
    eph->i0 = 0.9848407943;
    eph->OMG0 = 1.038062244;
    eph->omg = -1.717457876;
    eph->M0 = -1.064739758;
    eph->deln = 4.249105564E-9;
    eph->idot = 7.422851194E-51;
    eph->OMGd = -8.151768125E-9;
    eph->cuc = 3.054738045E-7;
    eph->cus = 2.237036824E-6;
    eph->crc = 350.53125;
    eph->crs = 2.53125;
    eph->cic = -8.381903172E-8;
    eph->cis = 8.940696716E-8;
    eph->week = 0;
    eph->f0 = 0.0;
    eph->f1 = 0.0;
    eph->f2 = 0.0;
    eph->eph_isavailable = 1;
}

void test_eph2pos()
{
    eph_t eph;
    ephinit(&eph);
    double curgpst[2] = {0,239050.7223}, rs[3];
    double cor_rs[3] = {13780293.30,-20230949.12,10441947.44};
    eph2pos(curgpst,&eph,rs);
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,rs[0],cor_rs[0],"Wrong x");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,rs[1],cor_rs[1],"Wrong y");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,rs[2],cor_rs[2],"Wrong z");
}

void test_angel_judge()
{
    //waiting for data
}

void test_calc()
{
    eph_t eph;
    ephinit(&eph);
    double curgpst[2] = {0,239050.7223};
    double pos[3] = {23.241,-55.739,330.63}, r[3] = {3302700,-4848700,2502600};
    double cor_rs[6] = {13780293.30,-20230949.12,10441947.44,1117.116,-681.974,-2850.309};
    calc(&eph,curgpst,r,1,pos);
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.x,cor_rs[0],"Wrong x");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.y,cor_rs[1],"Wrong y");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.z,cor_rs[2],"Wrong z");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.vx,cor_rs[3],"Wrong vx");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.vy,cor_rs[4],"Wrong vy");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,eph.vz,cor_rs[5],"Wrong vz");
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_epoch2time);
    RUN_TEST(test_eph2pos);
    RUN_TEST(test_angel_judge);
    RUN_TEST(test_calc);
    return UNITY_END();
}