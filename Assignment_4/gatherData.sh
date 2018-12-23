python3 WindyGridWorld.py 4 0 > normal
python3 WindyGridWorld.py 8 0 > king
python3 WindyGridWorld.py 8 1 > stochastic

python3 plot_graph.py normal stochastic king
python3 plot_graph_separate.py normal stochastic king
