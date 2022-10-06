#!/bin/bash

g++ main.cpp -o main

if [ $? -ne 0 ]; then
	echo Compile Error
	exit 1
fi

cnt_ok=0
cnt_fail=0

for dir in $(ls | grep grad)
do
	for id in $(ls $dir | grep .in | cut -d"." -f1 | sort -n)
	do
		echo -n Running $dir / $id:" "
		start_time=$(date +%s%N)
		./main < $dir/$id.in > $dir/$id.out
		return_code=$?
		end_time=$(date +%s%N)
		milsecs=$(( (end_time - start_time ) / 1000000 ))
		if [ $return_code -ne 0 ]; then
			echo Runtime Error
			((cnt_fail=cnt_fail+1))
		elif [ $milsecs -ge 2000 ]; then
			echo Time Limit Exceeded
			((cnt_fail=cnt_fail+1))
		else
			if diff $dir/$id.out $dir/$id.ans; then
				echo OK
				((cnt_ok=cnt_ok+1))
			else
				echo Wrong Answer
				((cnt_fail=cnt_fail+1))
			fi
			rm $dir/$id.out
		fi
	done
done

echo OK: $cnt_ok Fail: $cnt_fail
