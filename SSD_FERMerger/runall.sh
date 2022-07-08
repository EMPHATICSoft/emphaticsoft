#! /bin/sh -f

for runfile in `ls $MERGER_INPUT_DIR`
do
#		echo $runfile
	export station=`echo $runfile | cut -c 17`
	export subfile=`echo $runfile | cut -c 26`
	if [[ $station -eq 0 ]] && [[ $subfile -eq 0 ]]
	then
		export arun=`echo $runfile | cut -c 22-24`
		if [[ $arun -gt 450 ]]
		then
			export size=0
			for rawfile in `ls $MERGER_INPUT_DIR/RawDataSaver0FER*_Run$arun*`
			do
				let "size += `ls -s $rawfile | awk '{print $1}'`"
#				echo $rawfile $size
			done
#			echo $size
# If size is too large, that's from SSD fluctuation and is bad data at least in the last spill. 
# Splitter now reads in all data at once and cannot handle over ~1 G data. 	
# Do not plan to use these runs for now.
			if [[ $size -lt 1000000 ]]
			then
				echo Splitting run $arun
				./Splitter $arun
			fi
		fi
	fi
done
