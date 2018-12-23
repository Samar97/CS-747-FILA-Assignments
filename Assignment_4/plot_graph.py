# Python script to plot the graphs #
import matplotlib.pyplot as plt
import sys

episodes = 170

for count in range(3):
	file_path = sys.argv[count+1]
	time = [0]
	with open(file_path) as f:
		for i in range(episodes):
			temp = f.readline()[:-1].split()
			time += [int(float(temp[0]))]

	y = [i for i in range(episodes+1)]
	plt.plot(time, y, label=file_path)
	
plt.legend(loc='upper left')
plt.savefig("Graph.png")
plt.close()