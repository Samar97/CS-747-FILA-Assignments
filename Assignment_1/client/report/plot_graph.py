from matplotlib import pyplot as plt
import numpy as np
import sys
import os

arm_list = ['5','25']
horizon_list = ['1000','10000']
algo_list = ['betaDist', 'instance-bernoulli', 'instance-histogram']
for arm in arm_list:
	for horizon in horizon_list:
		for algo in algo_list:
			fpath = './arms-' + arm + '/h'+horizon+'/'+algo+'/'

			fig = plt.figure()
			colors = ['b','g','r','c','m','y','k','orange']
			count = 0
			ax = 0
			for file_name in os.listdir(fpath):
				sum_run_list = np.genfromtxt(os.path.join(fpath,file_name),delimiter=';')
				avg_run_list = sum_run_list.mean(axis=0)

				x = [i*10+1 for i in range(len(avg_run_list))]
				y = avg_run_list

				ax = fig.add_subplot(111)
				algo_name = file_name.split('_')[0]
				if algo_name == "epsilon-greedy":
					algo_name = algo_name + file_name.split('_')[2]
				ax.plot(x,y, c=colors[count],label=algo_name)
				count+=1
			if count == 0:
				continue
			# # Shrink current axis by 20%
			# box = ax.get_position()
			# ax.set_position([box.x0, box.y0, box.width * 0.8, box.height])
			# Put a legend to the right of the current axis
			# ax.legend(loc='top left', bbox_to_anchor=(1, 0.5))

			file_name = fpath[2:-1].split('/')
			file_name = '_'.join(file_name)
			ax.legend(loc='upper left')
			ax.set_title(file_name)
			ax.set_xlabel('No. of Pulls')
			ax.set_ylabel('Expected Cumulative Regret')
			plt.savefig(file_name)
			plt.close(fig)
print "Done"