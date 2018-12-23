# Python script to plot the graphs given the value function and the optimal policy in the format as specified in the PS #
import matplotlib.pyplot as plt
import sys

file_path = sys.argv[1]
algo_type = file_path.split('_')[1].split('.')[0]
S = 101
x = [i for i in range(S)]

value = []
policy = []

with open(file_path) as f:
	for i in range(S):
		temp = f.readline()[:-1].split()
		value += [temp[0]]
		policy += [temp[1]]

plt.plot(x, value) 
plt.savefig("Value_"+algo_type+".png")
plt.close()
plt.plot(x, policy)
plt.savefig("Policy_"+algo_type+".png")
