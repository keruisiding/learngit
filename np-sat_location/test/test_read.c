/* test_read.c
 */
#include <string.h>
#include "sat_location.h"
#include "unity.h"

void test_readrnxh()
{
    FILE* fp = NULL;
    fp = fopen("//Users//nyhuang//Desktop//intern//sat_location_proj//site247j.01n","r"); //eph file
    TEST_ASSERT_NOT_NULL_MESSAGE(fp,"Fail when opening the file");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,readrnxh(fp),"Fail when reading rinex header");
    fclose(fp); fp = NULL;
}

void test_readrnxnav()
{
    FILE* fp = NULL;
    fp = fopen("//Users//nyhuang//Desktop//intern//sat_location_proj//site247j.01n","r"); //eph file
    TEST_ASSERT_NOT_NULL_MESSAGE(fp,"Fail when opening the file");
    eph_t eph; eph.eph_isavailable = 1;
    readrnxh(fp);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,readrnxnav(fp,&eph),"Fail when reading rinex data");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,eph.svh,"Fail when reading rinex data");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0,eph.week,"Fail when reading rinex data");
    TEST_ASSERT_INT_WITHIN_MESSAGE(302400,302400,eph.toes,"Fail when reading rinex data");
    fclose(fp); fp = NULL;
}

void test_readalm()
{
    FILE* fp = NULL;
    fp = fopen("//Users//nyhuang//Desktop//intern//sat_location_proj//current.txt","r");
    TEST_ASSERT_NOT_NULL_MESSAGE(fp,"Fail when opening the file");
    eph_t eph; eph.eph_isavailable = 0;
    TEST_ASSERT_EQUAL_INT_MESSAGE(0,readalm(fp,&eph,1),"Fail when reading almanac data");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1,eph.svh,"Fail when reading almanac data");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(0,eph.week,"Fail when reading almanac data");
    TEST_ASSERT_INT_WITHIN_MESSAGE(302400,302400,eph.toes,"Fail when reading almanac data");
    fclose(fp); fp = NULL;
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_readrnxh);
    RUN_TEST(test_readrnxnav);
    RUN_TEST(test_readalm);
    return UNITY_END();
}