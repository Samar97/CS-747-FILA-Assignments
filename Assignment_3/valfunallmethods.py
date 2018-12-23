import sys

file_path = sys.argv[1]

V = []
e = []
alp = 7
lamb  = 0.65
# algo = "TD-L"
# algo = "TD-0"
algo = "Model Based"

# Reading the input from the file #
with open(file_path) as f:
	S = int(f.readline())
	A = int(f.readline())
	
	V = [0 for i in range(S)]
	e = [0 for i in range(S)]
	gamma = float(f.readline())
	temp = f.readline().split()
	state = int(temp[0])
	action = int(temp[1])
	reward = float(temp[2])
	temp = f.readline().split()
	count = 1
	alpha = alp/count
	if algo == "TD-L":
		# Online TD lambda algorithm
		while len(temp) != 1:
			next_state = int(temp[0])
			delta = reward + gamma*V[next_state] - V[state]
			e[state] = e[state] + 1
			for s in range(S):
				V[s] = V[s] + alpha*delta*e[s]
				e[s] = gamma*lamb*e[s]
			state = int(temp[0])
			reward = float(temp[2])
			count += 1
			alpha = alp/count
			temp = f.readline().split()
		next_state = int(temp[0])
		delta = reward + gamma*V[next_state] - V[state]
		e[state] = e[state] + 1
		for s in range(S):
			V[s] = V[s] + alpha*delta*e[s]
			e[s] = gamma*lamb*e[s]

	elif algo == "TD-0":
		# Online TD zero algorithm
		while len(temp) != 1:
			next_state = int(temp[0])
			delta = reward + gamma*V[next_state] - V[state]
			V[state] = V[state] + alpha*delta
			state = int(temp[0])
			reward = float(temp[2])
			count += 1
			alpha = alp/count
			temp = f.readline().split()
		next_state = int(temp[0])
		delta = reward + gamma*V[next_state] - V[state]
		V[state] = V[state] + alpha*delta
	
	elif algo == "Model Based":
		# Model based algorithm
		T  = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
		R  = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
		Rs = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
		Rc = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
		Tc = [[[0 for i in range(S)] for j in range(A)] for k in range(S)]
		P  = [[0 for i in range(A)] for j in range(S)] ##

		while len(temp) != 1:
			next_state = int(temp[0])
			Tc[state][action][next_state] += 1
			Rc[state][action][next_state] += 1
			P[state][action] += 1 ##
			Rs[state][action][next_state] += reward

			state  = int(temp[0])
			action = int(temp[1])
			reward = float(temp[2])
			temp = f.readline().split()
		next_state = int(temp[0])
		Tc[state][action][next_state] += 1
		Rc[state][action][next_state] += 1
		P[state][action] += 1 ##
		Rs[state][action][next_state] += reward
		for s in range(S):
			sumstate = float(sum(P[s]))
			for a in range(A):
				P[s][a] = float(P[s][a])/sumstate ##
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
		# Now since we have the estimates for R and T, we can find V using value iteration
		error = 1e-6
		diff = 1
		while diff > error:
			diff = 0
			for s in range(S):
				maxsofar = -float("inf")
				valfun = 0.0
				for a in range(A):
					const_term = sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])
					var_term = sum([ele[0]*gamma*ele[1] for ele in zip(T[s][a],V)])
					tempval = var_term + const_term
					if tempval > maxsofar:
						maxsofar = tempval
					valfun += P[s][a]*tempval
				# diff = max([diff,abs(maxsofar-V[s])])
				diff = max([diff,abs(valfun-V[s])])
				# V[s] = maxsofar
				V[s] = valfun

f.close()
for v in V:
	print(v)