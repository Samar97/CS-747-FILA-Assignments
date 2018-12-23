#!/bin/bash
PWD=`pwd`

port=5001
nRuns=100
hostname="localhost"

SERVERDIR=./server
CLIENTDIR=./client

declare -a algorithm_list=("epsilon-greedy" "KL-UCB" "UCB" "Thompson-Sampling")
declare -a horizon_list=(1000 10000)
declare -a instance_five_list=("betaDist_5.txt" "instance-histogram-5.txt" "instance-bernoulli-5.txt")
declare -a instance_twenty_five_list=("betaDist_25.txt" "instance-histogram-25.txt" "instance-bernoulli-25.txt")
declare -a epsilon_list=(0.2 0.3 0.4 0.5)


for algorithm in "${algorithm_list[@]}"
do
	for horizon in "${horizon_list[@]}"
	do
		for instance in "${instance_five_list[@]}"
		do
			banditFile="$PWD/data/$instance"
			echo "$banditFile"
			numArms=5
			if [ "$algorithm" = "epsilon-greedy" ]; then
				for epsilon in "${epsilon_list[@]}"
				do
					NAME=${algorithm}_${horizon}_${epsilon}_${instance}
					OUTPUTFILE=$PWD/$NAME
					echo "$OUTPUTFILE"
					for ((i = 1; i <= 100; i++)); do

						randomSeed="$i"
						
						pushd $SERVERDIR
						cmd="./startserver.sh $numArms $horizon $port $banditFile $randomSeed $OUTPUTFILE &"
						# echo $cmd
						$cmd 
						popd

						sleep 1

						pushd $CLIENTDIR
						cmd="./startclient.sh $numArms $horizon $hostname $port $randomSeed $algorithm $epsilon&"
						#echo $cmd
						$cmd > /dev/null 
						popd

					done
				done
			fi
			epsilon=0.1
			NAME=${algorithm}_${horizon}_${epsilon}_${instance}
			OUTPUTFILE=$PWD/$NAME
			echo "$OUTPUTFILE"
			for ((i = 1; i <= 100; i++)); do

				randomSeed="$i"
				
				pushd $SERVERDIR
				cmd="./startserver.sh $numArms $horizon $port $banditFile $randomSeed $OUTPUTFILE &"
				# echo $cmd
				$cmd 
				popd

				sleep 1

				pushd $CLIENTDIR
				cmd="./startclient.sh $numArms $horizon $hostname $port $randomSeed $algorithm $epsilon&"
				#echo $cmd
				$cmd > /dev/null 
				popd
				
			done
		done
	done
done

for algorithm in "${algorithm_list[@]}"
do
	for horizon in "${horizon_list[@]}"
	do
		for instance in "${instance_twenty_five_list[@]}"
		do
			banditFile="$PWD/data/$instance"
			echo "$banditFile"
			numArms=25
			if [ "$algorithm" = "epsilon-greedy" ]; then
				for epsilon in "${epsilon_list[@]}"
				do
					NAME=${algorithm}_${horizon}_${epsilon}_${instance}
					OUTPUTFILE=$PWD/$NAME
					echo "$OUTPUTFILE"
					for ((i = 1; i <= 100; i++)); do

						randomSeed="$i"
						
						pushd $SERVERDIR
						cmd="./startserver.sh $numArms $horizon $port $banditFile $randomSeed $OUTPUTFILE &"
						# echo $cmd
						$cmd 
						popd

						sleep 1

						pushd $CLIENTDIR
						cmd="./startclient.sh $numArms $horizon $hostname $port $randomSeed $algorithm $epsilon&"
						#echo $cmd
						$cmd > /dev/null 
						popd

					done
				done
			fi
			epsilon=0.1
			NAME=${algorithm}_${horizon}_${epsilon}_${instance}
			OUTPUTFILE=$PWD/$NAME
			echo "$OUTPUTFILE"
			for ((i = 1; i <= 100; i++)); do

				randomSeed="$i"
				
				pushd $SERVERDIR
				cmd="./startserver.sh $numArms $horizon $port $banditFile $randomSeed $OUTPUTFILE &"
				# echo $cmd
				$cmd 
				popd

				sleep 1

				pushd $CLIENTDIR
				cmd="./startclient.sh $numArms $horizon $hostname $port $randomSeed $algorithm $epsilon&"
				#echo $cmd
				$cmd > /dev/null 
				popd
				
			done
		done
	done
done