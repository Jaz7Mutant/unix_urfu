#!/bin/bash
make -s A_creator
make -s sparce_maker

./A_creator
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

./sparce_maker A B
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

gzip -kf A
gzip -kf B

gzip -cd B.gz | ./sparce_maker C
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

./sparce_maker A D -b 100
exitCode=$?
if [[ $exitCode -ne 0 ]]
then
	exit -1
fi

blockSize=$(stat A --printf=%B)
blockCount=$(stat A --printf=%b)
echo A size = $((blockSize*blockCount))
blockCount=$(stat B --printf=%b)
echo B size = $((blockSize*blockCount))
blockCount=$(stat A.gz --printf=%b)
echo A.gz size = $((blockSize*blockCount))
blockCount=$(stat B.gz --printf=%b)
echo B.gz size = $((blockSize*blockCount))
blockCount=$(stat C --printf=%b)
echo C size = $((blockSize*blockCount))
blockCount=$(stat D --printf=%b)
echo D size = $((blockSize*blockCount))