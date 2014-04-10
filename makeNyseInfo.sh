#!/bin/sh

lastline=`wc -l ~/문서/stock/nyse.csv | awk '{print $1}'`
count=0
cat /dev/null > ./nyseInfo.h

echo 'STOCKINFO NYSE_LIST [] = {' >> ./nyseInfo.h

while read line
do
count=`expr $count + 1`
element=`echo "$line" | awk 'BEGIN {FS="\t"}; {printf "{\"%s\", \"%s.NS\"}", $1, $2}'`
if [ $lastline -eq $count ] ; then
	echo $element >> ./nyseInfo.h
else
	echo $element',' >> ./nyseInfo.h
fi

done < ~/문서/stock/nyse.csv

echo '};' >> ./nyseInfo.h
