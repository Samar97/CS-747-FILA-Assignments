import sys
import numpy as np
from pulp import *
import argparse

# Taking the first argument to be the absolute path of the file and the second to be the type of algorithm to be applied to solve
parser = argparse.ArgumentParser()
parser.add_argument("--mdp",type=str)
parser.add_argument("--algorithm",type=str)
args = parser.parse_args()
file_path = args.mdp
algo = args.algorithm

mdp_type = ""
S = 0
A = 0
R = []
T = []
gamma = 0

# Reading the input from the file #
with open(file_path) as f:
	S = int(f.readline())
	A = int(f.readline())

	R = [[[] for j in range(A)] for i in range(S)]
	T = [[[] for j in range(A)] for i in range(S)]

	for s in range(S):
		for a in range(A):
			R[s][a] = [float(i) for i in f.readline()[:-1].split()]

	for s in range(S):
		for a in range(A):
			T[s][a] = [float(i) for i in f.readline()[:-1].split()]

	gamma = float(f.readline())
	mdp_type = f.readline()[:-1]
f.close()
# End of reading input #

# Data structures for the values and the policies of the states #
V = [0 for i in range(S)]
P = [0 for i in range(S)]

if algo == "lp":
	# Solving by posing the Markov Decision Process(MDP) as a Linear Programming Problem(LPP) #

	# Initialising the problem #
	mdp = pulp.LpProblem("MDP",pulp.LpMinimize)
	
	# Adding the decision variables of the LPP - the value function of each state #
	for i in range(S):
		var_name = 'V'+str(i);
		V[i] = pulp.LpVariable(var_name,cat='Continuous')
		# # Taking care of extra cases where the terminal state is not the last state #
		# if mdp_type == "episodic":
		# 	is_terminal = True
		# 	# Checking if all the coefficients are zero to handle for cases where the state is terminal and may not occur at the end #
		# 	for a in range(A):
		# 		if T[i][a][i] != 1.0:
		# 			is_terminal = False
		# 			break
		# 	if is_terminal:
		# 		V[i] = pulp.LpVariable(var_name,cat='Continuous',lowBound=0.0,upBound=0.0)

	# Taking care of the episodic mdps by constraining the terminal state to have the value 0 #
	if mdp_type == "episodic":
		V[S-1] = pulp.LpVariable('V'+str(S-1),cat='Continuous',lowBound=0.0,upBound=0.0)

	# Adding the objective function in our LPP
	mdp += lpSum(V), "Sum of value function is maximized"

	# Adding the constraints in our LPP # 
	for s in range(S):
		for a in range(A):
			const_term = sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])
			var_term = [-ele[0]*gamma*ele[1] for ele in zip(T[s][a],V)]
			var_term[s] = (1-T[s][a][s]*gamma)*V[s]
			mdp += lpSum(var_term) >= const_term

	# Solving the LPP #
	mdp.solve()
	
	# Finding the optimal policy from the optimal values #
	for s in range(S):
		opt_val = V[s].varValue
		min_val = float("inf")
		opt_act = 0
		for a in range(A):
			const_term = sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])
			var_term = [ele[0]*gamma*ele[1].varValue for ele in zip(T[s][a],V)]
			val = const_term + sum(var_term)
			diff = abs(val-opt_val)
			if(diff<min_val):
				min_val = diff
				opt_act = a
		P[s] = opt_act

	# Printing the optimal values along with the optimal actions at each state #
	for s in range(S):
		print("%.10f %d"%(V[s].varValue, P[s]))

elif algo == "hpi":
	# Solving by policy iteration #
	change = True
	while(change):
		change = False
		for s in range(S):
			max_val = -float("inf")
			opt_act = 0
			for a in range(A):
				const_term = sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])
				var_term = sum([ele[0]*gamma*ele[1] for ele in zip(T[s][a],V)])
				val = const_term + var_term
				if(val>max_val):
					max_val = val
					opt_act = a

			if opt_act != P[s] and abs(max_val-V[s])>1e-7:
				change = True
				P[s] = opt_act

		const_vec = []
		coeff_mat = []
		for s in range(S):
			a =	P[s]
			const_vec += [sum([ele[0]*ele[1] for ele in zip(R[s][a],T[s][a])])]
			coeff_vec  = list(map(lambda x: -x*gamma, T[s][a]))
			coeff_vec[s] = 1 - T[s][a][s]*gamma
			# Taking care of the episodic MDPs by redefining the equation corresponding to the terminal state #
			if mdp_type == "episodic":
				# is_terminal = True
				# # Checking if all the coefficients are zero to handle for cases where the state is terminal and may not occur at the end #
				# for q in range(S):
				# 	if coeff_vec[q] != 0:
				# 		is_terminal = False
				# 		break
				# if s == S - 1 or is_terminal:
				if s == S - 1:
					# const_vec[s] = 0
					# coeff_vec = [0 for s in range(S)]
					coeff_vec[s] = 1
					# The first two lines were commented out because the input ensures that #
					# the transition probabilities from a terminal to a non terminal state is zero and also... #
					# the rewards are zero for transitions from the terminal state, hence the constant term is ensured to be zero #

			coeff_mat += [coeff_vec]
		const_vec = np.array(const_vec)
		coeff_mat = np.array(coeff_mat)
		V = list(np.linalg.solve(coeff_mat,const_vec))

	# Printing the optimal values along with the optimal actions at each state #
	for s in range(S):
		print("%.12f %d"%(V[s], P[s]))

else:
	print("Algorithm not recognised \nIt should be one of 'hpi' or 'lp'")