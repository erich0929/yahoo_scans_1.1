#!/bin/sh

lastline=`wc -l ~/문서/stock/krx.csv | awk '{print $1}'`
count=0
cat /dev/null > ./kospiInfo.h

echo 'STOCKINFO KOSPI_LIST [] = {' >> ./kospiInfo.h

while read line
do
count=`expr $count + 1`
element=`echo "$line" | awk 'BEGIN {FS="\t"}; {printf "{\"%s\", \"%s.KS\"}", $1, $2}'`
if [ $lastline -eq $count ] ; then
	echo $element >> ./kospiInfo.h
else
	echo $element',' >> ./kospiInfo.h
fi

done < ~/문서/stock/krx.csv

echo '};' >> ./kospiInfo.h
