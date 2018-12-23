#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <vector>
#include <random>
#include <string>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


// Included extra data structures/libraries if necessary
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <map>
// End

#define MAXHOSTNAME 256

using namespace std;

void options(){

	cout << "Usage:\n";
	cout << "bandit-agent\n"; 
	cout << "\t[--numArms numArms]\n";
	cout << "\t[--randomSeed randomSeed]\n";
	cout << "\t[--horizon horizon]\n";
	cout << "\t[--hostname hostname]\n";
	cout << "\t[--port port]\n";
	cout << "\t[--algorithm algorithm]\n";
	cout << "\t[--epsilon epsilon]\n";

}


/*
	Read command line arguments, and set the ones that are passed (the others remain default.)
*/
bool setRunParameters(int argc, char *argv[], int &numArms, int &randomSeed, unsigned long int &horizon, string &hostname, int &port, string &algorithm, double &epsilon){

	int ctr = 1;
	while(ctr < argc){

		//cout << string(argv[ctr]) << "\n";

		if(string(argv[ctr]) == "--help"){
			return false;//This should print options and exit.
		}
		else if(string(argv[ctr]) == "--numArms"){
			if(ctr == (argc - 1)){
	return false;
			}
			numArms = atoi(string(argv[ctr + 1]).c_str());
			ctr++;
		}
		else if(string(argv[ctr]) == "--randomSeed"){
			if(ctr == (argc - 1)){
	return false;
			}
			randomSeed = atoi(string(argv[ctr + 1]).c_str());
			ctr++;
		}
		else if(string(argv[ctr]) == "--horizon"){
			if(ctr == (argc - 1)){
	return false;
			}
			horizon = atoi(string(argv[ctr + 1]).c_str());
			ctr++;
		}
		else if(string(argv[ctr]) == "--hostname"){
			if(ctr == (argc - 1)){
	return false;
			}
			hostname = string(argv[ctr + 1]);
			ctr++;
		}
		else if(string(argv[ctr]) == "--port"){
			if(ctr == (argc - 1)){
	return false;
			}
			port = atoi(string(argv[ctr + 1]).c_str());
			ctr++;
		}
		else if(string(argv[ctr]) == "--algorithm"){
			if(ctr == (argc - 1)){
	return false;
			}
			algorithm = string(argv[ctr + 1]);
			ctr++;
		}
		 else if(string(argv[ctr]) == "--epsilon"){
			if(ctr == (argc - 1)){
	return false;
			}
			epsilon = atof(string(argv[ctr + 1]).c_str());
			ctr++;
		}
		else{
			return false;
		}

		ctr++;
	}

	return true;
}

/* ============================================================================= */
/* Write your algorithms here */

vector<int> num_pulls;
vector<double> average_reward;
vector<double> cumulative_reward;
vector<double> alpha;
vector<double> beta;

long aseed = 0;
long eseed = 1;
int prev_arm = 0;
gsl_rng* arm_seed = gsl_rng_alloc(gsl_rng_mt19937);
gsl_rng* epsilon_seed = gsl_rng_alloc(gsl_rng_mt19937);

int epsilon_greedy(double epsilon, int pulls, float reward, int numArms){
	int arm_to_pull = 0;
	
	if(pulls == 0){
		// Initialisation steps
		for(int i=0; i<numArms; i++){
			num_pulls.push_back(0);
			average_reward.push_back(1.0);
			cumulative_reward.push_back(0.0);
		}
		// Randomly choose an arm for the first time
		double random_arm = gsl_rng_uniform(arm_seed);
		arm_to_pull = int(random_arm*numArms);
		prev_arm = arm_to_pull;
		return arm_to_pull;
	}
	// Update step
	num_pulls[prev_arm]++;
	cumulative_reward[prev_arm]+=reward;
	average_reward[prev_arm] = cumulative_reward[prev_arm]/(num_pulls[prev_arm]*1.0);

	// Toss for exploitation vs exploitation
	double toss = gsl_rng_uniform(epsilon_seed);

	if(toss<epsilon){
		// Randomly choose an arm
		double random_arm = gsl_rng_uniform(arm_seed);
		arm_to_pull = int(random_arm*numArms);
		prev_arm = arm_to_pull;
		return arm_to_pull;	
	}

	// Choose the best arm
	arm_to_pull = distance(average_reward.begin(),max_element(average_reward.begin(),average_reward.end()));
	prev_arm = arm_to_pull;
	return arm_to_pull;
}

int ucb(double epsilon, int pulls, float reward, int numArms){
	int arm_to_pull = 0;	
	if(pulls == 0){
		// Initialisation steps
		for(int i=0; i<numArms; i++){
			num_pulls.push_back(0);
			average_reward.push_back(1.0);
			cumulative_reward.push_back(0.0);
		}
		prev_arm = 0;
		return 0;
	}
	// Update step
	num_pulls[prev_arm]++;
	cumulative_reward[prev_arm]+=reward;
	average_reward[prev_arm] = cumulative_reward[prev_arm]/(num_pulls[prev_arm]*1.0);

	// For pull no.s 0 to n-1
	if(pulls<numArms){
		prev_arm = pulls;
		return pulls;
	}

	// Choose the arm with best ucb
	double ucb_val = 0;
	arm_to_pull = 0;
	for(int i=0; i<numArms; i++){
		double value = average_reward[i] + sqrt(2*log(pulls)/num_pulls[i]);
		if(value > ucb_val){
			ucb_val = value;
			arm_to_pull = i;
		}
	}
	prev_arm = arm_to_pull;
	return arm_to_pull;
}

int thompson(double epsilon, int pulls, float reward, int numArms){
	int arm_to_pull = 0;
	if(pulls == 0){
		// Initialisation steps
		for(int i=0; i<numArms; i++){
			alpha.push_back(1.0);
			beta.push_back(1.0);	
		}
	}
	else{
		// Update steps
		alpha[prev_arm] += reward;
		beta[prev_arm] += (1.0-reward);
	}
	// Beta Sampling and then choosing accordingly
	double alpha_beta_sample = 0;
	for(int i=0; i<numArms; i++){
		double sample = gsl_ran_beta(arm_seed, alpha[i], beta[i]);
		if(sample > alpha_beta_sample){
			alpha_beta_sample = sample;
			arm_to_pull = i;
		}
	}
	prev_arm = arm_to_pull;
	return arm_to_pull;
}

double solve_newton_raphson(int index, int tot_pulls){
	double thresh = 1e-5;
	double p = average_reward[index];
	int arm_pull = num_pulls[index];
	double q = 0.99999;
	double constant_term = (log(tot_pulls) + 3.0*log(log(tot_pulls)))/(arm_pull*1.0);
	// Handle corner cases here
	double variable_term = 0;
	if(p==0){
		variable_term = log(1.0/(1-q));
	}
	else if(p==1.0){
		return 1.0;
	}
	else{
		variable_term = p*log(p/q) + (1-p)*log((1-p)/(1-q)); 
	}
	double error = variable_term - constant_term;
	while(error > thresh){
		double differential_term = 0.0;
		// Exactly solving for p = 0
		if(p==0.0){
			q = 1.0 - exp(-constant_term);
			return q;
		}
		// Newton raphson update and limiting the value of q
		differential_term = (1-p)/(1-q) - p/q;
		// cout<<p<<" "<<q<<" "<<error<<" "<<differential_term<<" "<<constant_term<<" "<<variable_term<<endl;
		q = max(p, double(min(1.0, q - error/differential_term)));
		if(q==1.0){
			q = 0.99999;
		}
		variable_term = p*log(p/q) + (1-p)*log((1-p)/(1-q));
		error = variable_term - constant_term;
		// cout<<p<<" "<<q<<" "<<error<<" "<<differential_term<<" "<<constant_term<<" "<<variable_term<<endl;
	}
	// cout<<endl;
	return q;
}

int kl_ucb(double epsilon, int pulls, float reward, int numArms){
	int arm_to_pull = 0;	
	if(pulls == 0){
		// Initialisation steps
		for(int i=0; i<numArms; i++){
			num_pulls.push_back(0);
			average_reward.push_back(1.0);
			cumulative_reward.push_back(0.0);
		}
		prev_arm = 0;
		return 0;
	}
	// Update step
	num_pulls[prev_arm]++;
	cumulative_reward[prev_arm]+=reward;
	average_reward[prev_arm] = cumulative_reward[prev_arm]/(num_pulls[prev_arm]*1.0);

	// For pull no.s 0 to n-1
	if(pulls<numArms){
		prev_arm = pulls;
		return pulls;
	}

	// Choose the arm with best ucb
	double kl_ucb_val = 0;
	arm_to_pull = 0;
	for(int i=0; i<numArms; i++){
		double q_value = solve_newton_raphson(i, pulls);
		// cout<<q_value<<" ";
		if(q_value > kl_ucb_val){
			kl_ucb_val = q_value;
			arm_to_pull = i;
		}
	}
	// cout<<endl;
	prev_arm = arm_to_pull;
	return arm_to_pull;
}

int sampleArm(string algorithm, double epsilon, int pulls, float reward, int numArms){
	if(algorithm.compare("rr") == 0){
		return(pulls % numArms);
	}
	else if(algorithm.compare("epsilon-greedy") == 0){
		return epsilon_greedy(epsilon,pulls,reward,numArms);
	}
	else if(algorithm.compare("UCB") == 0){
		return ucb(epsilon,pulls,reward,numArms);
	}
	else if(algorithm.compare("KL-UCB") == 0){
		return kl_ucb(epsilon,pulls,reward,numArms);
	}
	else if(algorithm.compare("Thompson-Sampling") == 0){
		return thompson(epsilon,pulls,reward,numArms);
	}
	else{
		return -1;
	}
}

/* ============================================================================= */


int main(int argc, char *argv[]){
	// Run Parameter defaults.
	int numArms = 5;
	int randomSeed = time(0);
	unsigned long int horizon = 200;
	string hostname = "localhost";
	int port = 5000;
	string algorithm="random";
	double epsilon=0.0;

	// Extra code added
	gsl_rng_set(arm_seed, aseed);
	gsl_rng_set(epsilon_seed, eseed);
	// End of extra code

	//Set from command line, if any.
	if(!(setRunParameters(argc, argv, numArms, randomSeed, horizon, hostname, port, algorithm, epsilon))){
		//Error parsing command line.
		options();
		return 1;
	}

	struct sockaddr_in remoteSocketInfo;
	struct hostent *hPtr;
	int socketHandle;

	bzero(&remoteSocketInfo, sizeof(sockaddr_in));
	
	if((hPtr = gethostbyname((char*)(hostname.c_str()))) == NULL){
		cerr << "System DNS name resolution not configured properly." << "\n";
		cerr << "Error number: " << ECONNREFUSED << "\n";
		exit(EXIT_FAILURE);
	}

	if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		close(socketHandle);
		exit(EXIT_FAILURE);
	}

	memcpy((char *)&remoteSocketInfo.sin_addr, hPtr->h_addr, hPtr->h_length);
	remoteSocketInfo.sin_family = AF_INET;
	remoteSocketInfo.sin_port = htons((u_short)port);

	if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(sockaddr_in)) < 0){
		//code added
		cout<<"connection problem"<<".\n";
		close(socketHandle);
		exit(EXIT_FAILURE);
	}


	char sendBuf[256];
	char recvBuf[256];

	float reward = 0;
	unsigned long int pulls=0;
	int armToPull = sampleArm(algorithm, epsilon, pulls, reward, numArms);
	
	sprintf(sendBuf, "%d", armToPull);

	cout << "Sending action " << armToPull << ".\n";
	while(send(socketHandle, sendBuf, strlen(sendBuf)+1, MSG_NOSIGNAL) >= 0){

		char temp;
		recv(socketHandle, recvBuf, 256, 0);
		sscanf(recvBuf, "%f %c %lu", &reward, &temp, &pulls);
		cout << "Received reward " << reward << ".\n";
		cout<<"Num of  pulls "<<pulls<<".\n";


		armToPull = sampleArm(algorithm, epsilon, pulls, reward, numArms);

		sprintf(sendBuf, "%d", armToPull);
		cout << "Sending action " << armToPull << ".\n";
	}
	
	close(socketHandle);

	cout << "Terminating.\n";
	// for(int i=0; i<num_pulls.size(); i++){
	// 	cout<<num_pulls[i]<<endl;
	// }
	// Added code
	gsl_rng_free(arm_seed);
	gsl_rng_free(epsilon_seed);
	// End of added code

	return 0;
}
					
