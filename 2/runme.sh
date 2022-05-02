#!/bin/bash
rm -f myfile.lck statistics
make -s locker
if [[ $1 == "--crash" ]]
then
	./locker myfile &
	rm -f myfile.lck
	wait
	exit
fi

count=10
pids=()
for (( i=0; i<$count; i++ ))
do
	./locker myfile &
	pids+=($!)
done
sleep 5m
completedTasks=0
for pid in ${pids[@]}
do
	kill -SIGINT $pid
	if [[ $? -eq 0 ]]
	then
		((completedTasks++))
	fi
done

resultFileName="results.txt"
echo "Closing programms with SIGINT:" >"$resultFileName"
echo "Expected - " $count >>"$resultFileName"
echo "Actual - " $completedTasks >>"$resultFileName"
echo "-----" >>"$resultFileName"
echo "Check statics file lines count" >>"$resultFileName"
echo "Expected - " $count >>"$resultFileName"
echo "Actual - " $(cat statistics | wc -l) >>"$resultFileName"
echo "-----" >>"$resultFileName"
echo "Check even distribution:" >>"$resultFileName"
echo "Expected: almost equal numbers" >>"$resultFileName"
echo "Actual:\n" $(cat statistics) >>"$resultFileName"