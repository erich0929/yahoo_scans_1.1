#!/bin/sh

lastline=`wc -l ~/문서/stock/nyse.csv | awk '{print $1}'`
count=0
cat /dev/null > ./nyseInfo.h

echo 'STOCKINFO NYSE_LIST [] = {' >> ./nyseInfo.h

while read line
do
count=`expr $count + 1`
echo "$count / $lastline" | awk '{printf "\rIn working ... process : %s", $0}'
element=`echo "$line" | awk 'BEGIN {FS="\t"}; {printf "{\"%s\", \"%s.NS\"}", $1, $2}'`
if [ $lastline -eq $count ] ; then
	echo $element >> ./nyseInfo.h
else
	echo $element',' >> ./nyseInfo.h
fi

done < ~/문서/stock/nyse.csv

echo '};' >> ./nyseInfo.h
echo "\"nyseInfo.h\" C header file is created successfully in this directory!!" | awk '{printf "\r%s \n", $0}'
