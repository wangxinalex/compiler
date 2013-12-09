#!/bin/bash
#===============================================================================
#
#          FILE:  my_diff.sh
# 
#         USAGE:  ./my_diff.sh 
# 
#   DESCRIPTION:  a little shell script to check the passed testcases
# 
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR:  wangxinalex, wangxinalex@gmail.com
#       COMPANY:  
#       VERSION:  1.0
#       CREATED:  12/05/13 18:47:24 CST
#      REVISION:  ---
#===============================================================================

point=0

test ! -d ./my_diff && mkdir ./my_diff
test ! -d ./my_out && mkdir ./my_out
for file in `seq 1 49`
do
	./a.out testcases/test$file.tig &> ./my_out/test$file.out
	diff ./my_out/test$file.out ./refs-3/test$file.out > ./my_diff/test$file.diff
   if [ ! -s ./my_diff/test$file.diff ]; then
	   echo "pass test$file.tig"
	   point=$(($point + 1))
   else
	   echo "fail test$file.tig"
   fi	   
done
echo "point = $point"
