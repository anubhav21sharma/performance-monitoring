#!/bin/bash

function makeIPMap(){
	sampleFile=$1
	awk 'BEGIN{total=0}
		{map[$1]++;total++}
		END{for (var in map){printf("%s\t%.1f\t%ld\n", var,(map[var]/total)*100,total)}}' $sampleFile | sort -nr -k2 | sed -n '/^4/p'	
}

if [[ ! -e $1 || -z $1 ]];
then
	echo "Binary does not exist : $1"
	exit -1
fi

sampleFile='sample'
cat /dev/pmuIoctlFile | head -n -1 > $sampleFile

userProgram="$1"
makeIPMap $sampleFile > ipMap
objdump -D --no-show-raw-insn $userProgram > objectDump


cat objectDump | sed -n '/Disassembly of section \.text/,/Disassembly/p' | grep "<.*>:" | tr -d '<>:' | awk --non-decimal-data -F' ' '{key="0x"$1; map[key+0]=$2}END{for(key in map){print key, map[key]}}' | sort -n -k1 > fnDataSorted

cat ipMap | cut -f1 -d'	' | 
	while read line; 
	do 
		add=$line
		funcName=`cat fnDataSorted | awk --non-decimal-data -F' ' '{val="0x""'"$add"'";  val=val+0; if(val<=$1){print last; exit} last=$2}'`
		echo -e `grep $line <(grep '^4' ipMap) | cut -f2- -d'	'` "\t $funcName \t" `grep $line: objectDump` | tr ':' '\t'
	done > pmu.ip.data

cat pmu.ip.data | awk -F'\t' '{map[$2]+=$1}END{for(key in map){print map[key],key}}' | sort -nr -k1 | column -t > pmu.fn.data
#rm ipMap objectDump fnDataSorted $sampleFile
