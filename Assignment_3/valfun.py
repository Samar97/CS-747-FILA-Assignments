import sys

file_path = sys.argv[1]

# The list holding the Value function values of the states
V = []

# Reading the input from the file #
with open(file_path) as f:
	# Reading the no. of states and the no. of actions and the gamma
	S = int(f.readline())
	A = int(f.readline())
	gamma = float(f.readline())
	
	# Initialising the values of value functions as 0
	V = [0 for i in range(S)]

	# Reading the first line of state,action and reward from the file
	temp = f.readline().split()
	state = int(temp[0])
	action = int(temp[1])
	reward = float(temp[2])
	temp = f.readline().split()

	# Using a Model based algorithm

	# Initialising the Transition, Reward
	T  = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
	R  = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]

	# The reward sum and the reward count
	Rs = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
	Rc = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]

	# The transition count
	Tc = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]

	# For storing the probability of taking an action a from state s, basically this takes care of stochastic policy
	P  = [[0 for i in range(A)] for j in range(S)]

	# Reading the rest of the lines one by one
	while len(temp) != 1:
		# next state is sprime for the last triplet
		next_state = int(temp[0])
		# Increasing the transition counts and the reward counts by 1
		Tc[state][action][next_state] += 1
		Rc[state][action][next_state] += 1
		# Now, P holds the no of times state,action was taken, later it will hold the probability
		P[state][action] += 1
		# Holding the cumulative reward for s,a,sprime
		Rs[state][action][next_state] += reward

		# Now, for this iteration, our state, action, reward are the current triplet values, we will get the sprime when we read the next line in the next iteration
		state  = int(temp[0])
		action = int(temp[1])
		reward = float(temp[2])

		# reading the next line at the end of loop
		temp = f.readline().split()

	# Handling the last line which has only one state, this is the sprime for our last triplet
	next_state = int(temp[0])
	# Doing the usual updates for the last case
	Tc[state][action][next_state] += 1
	Rc[state][action][next_state] += 1
	P[state][action] += 1
	Rs[state][action][next_state] += reward

	# Now to estimate T and R, if the no of counts or the sum of occurences is 0, we just assign 0 to the corresponding probabilities
	for s in range(S):
		sumstate = float(sum(P[s]))
		for a in range(A):
			if sumstate == 0:
				P[s][a] = 0
			else:
				P[s][a] = float(P[s][a])/sumstate
			transum = sum(Tc[s][a])
			for sp in range(S):
				if Rc[s][a][sp] == 0:
					R[s][a][sp] = 0
				else:
					R[s][a][sp] = Rs[s][a][sp]/Rc[s][a][sp]
				if transum == 0:
					T[s][a][sp] = 0
				else:
					T[s][a][sp] = Tc[s][a][sp]/transum
	# Now since we have the estimates for R and T, we can find V using value iteration by applying bellman's operator for the policy in the episode

	# Threshold for terminating
	error = 1e-7
	# diff holds the difference between current and last V estimates, for now it is 1, so that the loop runs at least once
	diff = 1

	while diff > error:
		diff = 0
		for s in range(S):
			valfun = 0.0
			for a in range(A):
				# Calculating the constant term of the bellman's equation's RHS
				const_term = sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])
				# Calculating the value function dependent term of bellman's equation's RHS
				var_term = sum([ele[0]*gamma*ele[1] for ele in zip(T[s][a],V)])
				# The total sum of the above two terms is the current estimate of the Q(s,a) value of this action
				tempval = var_term + const_term
				# We weigh this value by the probability(fraction of no. of times this action was taken at this state according to the episode's policy).
				# This was done to take care of stochastic policies.
				valfun += P[s][a]*tempval
			# The max difference between current estimate and last estimate of V[s] is stored in diff
			diff = max([diff,abs(valfun-V[s])])
			# V[s] is updated to hold the current estimate
			V[s] = valfun

f.close()
# Displaying the value function
for v in V:
	print(v)