import sys

ph = float(sys.argv[1])

S = 101
A = 51
gamma = 1
mdp_type = "episodic"
R = [[[0.0 for k in range(S)] for j in range(A)] for i in range(S)]
T = [[[0.0 for k in range(S)] for j in range(A)] for i in range(S)]

for s in range(S):
	max_act = min(s,100-s)
	for a in range(0,A):
		if a<=max_act:
			if a==0:
				## You can't just not play, it is of no use, for the game to end you anyway have to either go ahead or back, so action = 0 not allowed ## 
				T[s][a][s] = 0.0
			else:
				T[s][a][s+a] = ph
				T[s][a][s-a] = 1 - ph
			if s+a == 100:
				R[s][a][100] = 1.0


for s in range(S):
	for a in range(A):
		T[0][a][s] = 0.0
		R[0][a][s] = 0.0
		T[100][a][s] = 0.0
		R[100][a][s] = 0.0

# for a in range(A):
# 	T[0][a][0] = 1.0
# 	T[100][a][100] = 1.0

print(S)
print(A)
for s in range(S):
	for a in range(A):
		for sPrime in range(S):
			print(str(R[s][a][sPrime]),end="\t")
		print("")
for s in range(S):
	for a in range(A):
		for sPrime in range(S):
			print(str(T[s][a][sPrime]),end="\t")
		print("")
print(gamma)
print(mdp_type)