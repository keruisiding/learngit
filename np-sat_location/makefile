obj = read.o trans.o calc.o sat_location.o
head = sat_location.h
cc = gcc
VPATH = src

sat_location:$(obj)
		cc -o sat_location $(obj)
$(obj):$(head)

.PHONY:clean
clean:
		rm sat_location $(obj)