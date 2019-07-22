#!/bin/sh
#sim.sh
detailed=false
get_alg=false
get_quantum=false
quantum=1
algorithm=""
for fn in "$@"
do
    if [ "$fn" == "-d" ] #if block to process flags
	then #begin if statement
		detailed=true #state that we want detailed information
	elif [ "$fn" == "-a" ]
	then
		get_alg=true #state that the next variable is the algorithm choice
	fi

	if [ $get_alg == true -a $fn != "-a" ]
	then
		algorithm="$fn" #get algorithm name
		if [ $algorithm == "RR" ] #check if algorithm is RR, because then we are looking for time quantum
		then
			get_quantum=true
		fi
		get_alg=false #stop looking for algorithm
	elif [ $get_quantum == true -a $fn != "RR" ]
	then
		quantum=$fn
		get_quantum=false
	else
		fileName=$fn
	fi
done
###########################################################################find out which algorithm to execute

if [ "$algorithm" == "RR" ] 
then
    echo "RR executed"
	./RR "$quantum" "$fileName"
elif [ "$algorithm" == "SJN" ]
then
    echo "SJN executed"
	./SJN "$fileName"
elif [ "$algorithm" == "SRTN" ]
then
    echo "SRTN executed"
	./SRTN "$fileName"
else
	echo "FCFS executed"
	./FCFS "$fileName"
fi
#############################################################################get info from file
if [ "$detailed" == false -a "$fileName" != "" ] #if the detailed flag is set
then
	line=0 #must find out number of lines dynamically so that I can find CPUTime more easily
	totalCPUTime=0
	CPUTime=0
	idleTime=0
    switches=0
    switchTime=0
	input="AlgorithmResults.txt"
    while read -a var #then calculate the CPUUsage
    do
        if [ "${var[0]}" == "CPUTime:" ]
        then
        totalCPUTime=${var[1]}
        elif [ "${var[0]}" == "Number" ]
        then
            switches=${var[3]}
        elif [ "${var[0]}" == "Switch" ]
        then
            switchTime=${var[2]}
        else
            if [ "${var[2]}" == "Time:" ]
                then
                echo ""
            else
                ((CPUTime=CPUTime+${var[2]}))
        fi
    fi
    done < "$input"
	echo "Time CPU allocated = $CPUTime"
	echo "Total Time = $totalCPUTime"
	CPUUsage=$(echo "scale=2; $CPUTime/$totalCPUTime*100" |bc) #try to get rid of trailing zeroes
	echo "CPU Usage: %$CPUUsage"
    echo "Number of Switches: $switches"
    echo "Switch Time: $switchTime"
elif [ "$fileName" != "" ] #else output all result from file to console
	then
	line=0 #must find out number of lines dynamically so that I can find CPUTime more easily
	totalCPUTime=0
	CPUTime=0
	idleTime=0
	switches=0
	switchTime=0
	input="AlgorithmResults.txt"
	while read var #first output results
	do
		echo "$var"
	done < "$input"
		
	while read -a var #then calculate the CPUUsage
	do
		if [ "${var[0]}" == "CPUTime:" ]
		then
			totalCPUTime=${var[1]}
        elif [ "${var[0]}" == "Number" ]
        then
            switches=${var[2]}
        elif [ "${var[0]}" == "Switch" ]
        then
            switchTime=${var[3]}
		else
			if [ "${var[2]}" == "Time:" ]
			then
				echo ""
			else
				((CPUTime=CPUTime+${var[2]}))
			fi
		fi
	done < "$input"
	echo "Time CPU allocated = $CPUTime"
	echo "Total Time = $totalCPUTime"
	CPUUsage=$(echo "scale=2; $CPUTime/$totalCPUTime*100" |bc) #try to get rid of trailing zeroes
	echo "CPU Usage: %$CPUUsage"
    echo "Number of Switches: $switches"
    echo "Switch Time: $switchTime"
else
	echo "Filename never passed\n"
fi
