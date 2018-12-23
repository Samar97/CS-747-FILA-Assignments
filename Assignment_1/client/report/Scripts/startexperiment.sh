#!/bin/bash

PWD=`pwd`


port=5001
nRuns=100
hostname="localhost"

# Allowed values for algorithm parameter(case-sensitive)
# 1. epsilon-greedy 
# 2. UCB 
# 3. KL-UCB 
# 4. Thompson-Sampling
# 5. rr


algorithm="KL-UCB"
horizon=10000
epsilon=0.25
numArms=5
randomSeed=1

banditFile="$PWD/data/instance-bernoulli-5.txt"


SERVERDIR=./server
CLIENTDIR=./client

OUTPUTFILE=$PWD/serverlog.txt

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

