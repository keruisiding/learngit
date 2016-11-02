# sat_location

sat_location.c是程序的入口,输入所在位置的坐标{经度,纬度,高度}以及时间{年,月,日,时,分,秒,时区},可以计算出可视天空中的卫星数目,他们的坐标以及他们的速度.

current.txt是历书文件,用于当星历不能提供所有卫星的信息时替代星历进行精度较低的估算.current.txt可以在http://www.navcen.uscg.gov/?pageName=gpsAlmanacs下载.

10/21

增加了测试文件test.c.因为缺少计算仰角的数据,所以没有测试angel_judge(calc.c)函数.

在unity中使用了double类型.

修了原来代码里的bug,包括跳秒,时区,GPS周差和度与弧度转换的问题.

10/25

将测试文件分成test_calc.c,test_trans.c,test_read.c,分别对应源文件calc.c,trans.c,read.c.新建文件夹src,resources,test存放源文件,数据文件,代码文件.

增加了makefile.

nyhuang

2016/10/25