# This was not asked as a part of the submission #
# Extra Script to generate the graphs, need to have the files carefully placed in the appropriate directory and directories named 0.2, 0.4, 0.6 and 0.8 to accomodate the graphs #
for ph in 0.2 0.4 0.6 0.8
do
	echo "Doing for ph = "$ph
	echo "Making MDP file for the gambler's problem"
	./encodeGambler.sh $ph > mdpFile
	echo "Applying Linear Programming"
	./planner.sh --mdp ../mdpFile --algorithm lp > gambler_lp
	echo "Applying Howard's Policy Iteration"
	./planner.sh --mdp ../mdpFile --algorithm hpi > gambler_hpi
	echo "Plotting Graphs"
	python3 plot_gambler.py gambler_lp
	python3 plot_gambler.py gambler_hpi
	echo "Done"
	mv *.png $ph
done;