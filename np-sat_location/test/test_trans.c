/* test_trans.c
 */
#include <string.h>
#include "sat_location.h"
#include "unity.h"

#define ISYEAP(x) ((x%400==0) || (x%4==0 && x%100!=0))

void test_timediff()
{
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(timediff(302401,0),-302399,"Wrong when calculating time difference");
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(timediff(0,302401),302399,"Wrong when calculating time difference");
}

void test_str2time()
{
    char s[1028];
    strcpy(s," 7 01  9  4  9 59 44.0  .394901260734D-03  .387672116631D-10  .000000000000D+00");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,str2time(s,0,19),"Fail when decoding toc field from rinex");
    strcpy(s,"                                                            END OF HEADER       ");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,str2time(s,0,19),"Fail when decoding toc field from rinex");
    strcpy(s,"     .228000000000D+03 -.138750000000D+02  .543415492579D-08 -.101085380239D+01");
    TEST_ASSERT_EQUAL_INT_MESSAGE(-1,str2time(s,0,19),"Fail when decoding toc field from rinex");
}

void test_str2num()
{
    char s[1028];
    strcpy(s,"  .228000000000D+03 -.138750000000D+02  .543415492579D-08 -.101085380239D+01");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(1E-3,228.000,str2num(s,0,19),"Fail when decoding data field from rinex");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(1E-3,-13.875,str2num(s+19,0,19),"Fail when decoding data field from rinex");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(1E-3,0.000,str2num(s+38,0,19),"Fail when decoding data field from rinex");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(1E-3,-1.011,str2num(s+57,0,19),"Fail when decoding data field from rinex");
    strcpy(s,"Right Ascen at Week(rad):   0.1615259674E+001");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(1E-3,1.615,str2num(s,27,18),"Fail when decoding data field from rinex");
}

void test_trans()
{
    double ep[7] = {2016,10,21,15,18,0,8},curgpst[2];
    double pos[3] = {0,45,1000},r[3];
    double cor_r[3] = {4510731,4510731,0};
    double cor_gpst[2] = {1919,458297};
    trans(ep,pos,r,curgpst);
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,r[0],cor_r[0],"Wrong x");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,r[1],cor_r[1],"Wrong y");
    TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(0.1,r[2],cor_r[2],"Wrong z");
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(curgpst[0],cor_gpst[0],"Wrong gpst week");
    TEST_ASSERT_EQUAL_DOUBLE_MESSAGE(curgpst[1],cor_gpst[1],"Wrong gpst seconds");
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_timediff);
    RUN_TEST(test_str2time);
    RUN_TEST(test_str2num);
    RUN_TEST(test_trans);
    return UNITY_END();
}