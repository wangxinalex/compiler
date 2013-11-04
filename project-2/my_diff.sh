#!/bin/bash
#========================================================================
#   FileName: my_diff.sh
#     Author: wangxinalex
#      Email: wangxinalex@gmail.com
#   HomePage: http://singo.10ss.me
# LastChange: 2013-11-03 22:21:49
#========================================================================
test ! -d ./my_out &&  mkdir ./my_out
test ! -d ./my_diff &&  mkdir ./my_diff
point=0
for i in `seq 1 49`
do
	./parsetest ../testcases/test$i.tig &>./my_out/my_test$i.out
	diff ./my_out/my_test$i.out ../refs-2/test$i.out > ./my_diff/test$i.diff
	char=`wc ./my_diff/test$i.diff|awk '{print $3}'`
	if [ $char == 0 ]; then
		point=$(($point+1))
	fi
done
echo "point = $point"
