import sys
import numpy as np
from random import randint
from random import random

# Modelling the gridworld #
rows = 7
cols = 10
actions = int(sys.argv[1])

start = [3,0]
goal  = [3,7]

wind = [0,0,0,1,1,1,2,2,1,0]

stochastic = bool(int(sys.argv[2]))

gamma = 1

# action_map = {
# 	0 : "n",
# 	1 : "e",
# 	2 : "w",
# 	3 : "s",
# 	4 : "ne",
# 	5 : "nw",
# 	6 : "se",
# 	7 : "sw",
# }

def get_next_state_reward(state,action):

	next_state = state
	reward = -1
	curr_wind = wind[state[1]];

	if action == 0:
		next_state = [state[0]-1,state[1]]
	elif action == 1:
		next_state = [state[0],state[1]+1]
	elif action == 2:
		next_state = [state[0],state[1]-1]
	elif action == 3:
		next_state = [state[0]+1,state[1]]
	elif action == 4:
		next_state = [state[0]-1,state[1]+1]
	elif action == 5:
		next_state = [state[0]-1,state[1]-1]
	elif action == 6:
		next_state = [state[0]+1,state[1]+1]
	elif action == 7:
		next_state = [state[0]+1,state[1]-1]
	else:
		print("Invalid action encountered")
		exit()

	if stochastic:
		curr_wind += randint(-1,1)

	next_state[0] = min(max(0,next_state[0]-curr_wind),rows-1)
	next_state[1] = min(max(0,next_state[1]),cols-1)

	if next_state == goal:
		reward = 0

	return next_state,reward


	
# Sarsa Learning algorithm #
Q = [[[0 for a in range(actions)] for j in range(cols)] for i in range(rows)]
alpha   = 0.5
epsilon = 0.1
episodes = 170

# Epsilon Greedy action selection #
def epsilon_greedy(state):
	toss = random()
	if toss < epsilon:
		return randint(0,actions-1)
	else:
		return np.argmax(Q[state[0]][state[1]])


sum_steps = [0 for i in range(episodes)]
N = 10

# Running the process for N times (= 10 by default)
for run in range(N):
	cumulative_time_steps = 0
	# Looping through the episodes
	for episode in range(episodes):
		state = start[:]
		action = epsilon_greedy(state)
		time_steps = 0

		while not(state == goal):
			next_state,reward = get_next_state_reward(state,action)
			next_action = epsilon_greedy(next_state)
			Q[state[0]][state[1]][action] += alpha*(reward + gamma*Q[next_state[0]][next_state[1]][next_action] - Q[state[0]][state[1]][action]) 
			state = next_state[:]
			action = next_action
			time_steps += 1
		
		cumulative_time_steps += time_steps
		sum_steps[episode] += cumulative_time_steps
	Q = [[[0 for a in range(actions)] for j in range(cols)] for i in range(rows)]

for episode in range(episodes):
	print((sum_steps[episode]*1.0)/(1.0*N))