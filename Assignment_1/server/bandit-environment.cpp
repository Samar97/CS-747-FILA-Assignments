#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "bandit.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>


using namespace std;

void options(){

  cout << "Usage:\n";
  cout << "bandit-environment\n"; 
  cout << "\t[--numArms numArms]\n";
  cout << "\t[--randomSeed readInstanceFilerandomSeed]\n";
  cout << "\t[--horizon horizon]\n";
  cout << "\t[--banditFile banditFile]\n";
  cout << "\t[--port port]\n";
}


/*
  Read command line arguments, and set the ones that are passed (the others remain default.)
*/
bool setRunParameters(int argc, char *argv[], int &numArms, int &randomSeed, unsigned long int &horizon, string &banditFile, int &port){

  int ctr = 1;
  while(ctr < argc){

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
    else if(string(argv[ctr]) == "--banditFile"){
      if(ctr == (argc - 1)){
	return false;
      }
      banditFile = string(argv[ctr + 1]);
      ctr++;
    }
    else if(string(argv[ctr]) == "--port"){
      if(ctr == (argc - 1)){
	return false;
      }
      port = atoi(string(argv[ctr + 1]).c_str());
      ctr++;
    }
    else{
      return false;
    }

    ctr++;
  }

  return true;
}

int connectWithAgent(const int &port, int &clientSock){

  int serverSock=socket(AF_INET, SOCK_STREAM, 0);

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(port);
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));

  listen(serverSock,4);

  // cout << "Waiting for client to connect.\n";

  sockaddr_in clientAddr;
  socklen_t sin_size=sizeof(struct sockaddr_in);
  clientSock=accept(serverSock,(struct sockaddr*)&clientAddr, &sin_size);

  // cout << "Client connected.\n";

  return 0;
}


int getArmFromAgent(int &clientSock){

  char recvBuf[256];

  int arm = -1;

  //  cout << "Waiting for client message...\n"; /// Already Commented
  if(recv(clientSock, recvBuf, 256, 0) > 0){
    // cout << "Received message: " << recvBuf << ".\n";
    sscanf(recvBuf, "%d", &arm);
  }
  else{
    cout << "Did not receive message.\n";
  }
  
  return arm;
}

void giveRewardToAgent(int &clientSock, const double &reward, const unsigned long int &pulls){
  // cout << "Sending reward " << reward << "; pulls = " << pulls << ".\n";
  char sendBuf[256];
  char sept=',';
  sprintf(sendBuf, "%f%c%lu",reward,sept,pulls);
  if(send(clientSock, sendBuf, strlen(sendBuf) + 1, MSG_NOSIGNAL) < 0){
    cout << "Error sending message.\n";
  }

}

BanditType readInstanceFile(string banditFile, const int &numArms, vector<double> &means, vector<double> &alpha, vector<double> &beta, vector<vector<double> > &histograms, int &binCount){
  fstream file;
  file.open(banditFile.c_str(), fstream::in);

  BanditType type;
  string banditType;
  file >> banditType;
  
  if (banditType.compare("betaDistribution") == 0){
    alpha.resize(numArms);
    beta.resize(numArms);
    for(int a = 0; a < numArms; a++){
      file >> alpha[a] >> beta[a];
    }
    type = betaDistribution;
  }
  else if (banditType.compare("histogram") == 0){
    file >> binCount;
    histograms.resize(numArms);
    for(int a = 0; a < numArms; a++){
      vector<double> histogram;
      histogram.resize(binCount);
      for (int bin = 0; bin < binCount; bin++){
        file >> histogram[bin];
      }
      histograms[a] = histogram;
    }
    type = histogram;
  }
  else if (banditType.compare("bernoulli") == 0){
    means.resize(numArms);
    for(int a = 0; a < numArms; a++){
      file >> means[a];
    }
    type = bernoulli;
  }
  else{
    cout << "Bad instance file :(" << endl;
    return invalid;
  }
  file.close();
  return type;
}

int main(int argc, char *argv[]){
 
  // Run Parameter defaults.
  int numArms = 5;
  int randomSeed = time(0);
  unsigned long int horizon = 1000;
  string banditFile = "";
  int port = 5000;
 

  //Set from command line, if any.
  if(!(setRunParameters(argc, argv, numArms, randomSeed, horizon, banditFile, port))){
    //Error parsing command line.
    options();
    return 1;
  }
 
  int binCount = 0;
  vector<double> means;
  vector<double> alpha;
  vector<double> beta;
  vector<vector<double> > histograms;
 BanditType bandit_type = readInstanceFile(banditFile, numArms, means, alpha, beta, histograms, binCount);
  
  if (bandit_type == invalid) return 1;
  /*
  cout << "Num Arms: " << numArms << "\n";
  cout << "Random Seed: " << randomSeed << "\n";
  cout << "Horizon: " << horizon << "\n";
  cout << "Bandit File: " << banditFile << "\n";
  cout << "Means:";
  for(int a = 0; a < numArms; a++){
    cout << "\t" <<means[a];
  }
  cout << "\n";
  cout << "Port: " << port << "\n";
  */

  // cout << bandit_type;
      Bandit *bandit = new Bandit(bandit_type, numArms, means, alpha, beta, histograms, randomSeed, binCount);  



  /// k_steps is the no of steps after which regret is printed
  int k_steps = 10;

  int clientSock = -1;
  connectWithAgent(port, clientSock);
  for(unsigned int i = 0; i < horizon; i++){
    
    int armToPull = getArmFromAgent(clientSock);
    if(armToPull < 0){
      armToPull = i % numArms;
      cout << "Arm not received from agent. Sampling arm as per round robin.\n";
    }

    double reward = bandit->pull(armToPull);
    giveRewardToAgent(clientSock, reward, (i + 1));

    /// Extra Code for getting regret every k steps
    
    if(i%k_steps==1){
      double regret = bandit->getRegret();
      cout << regret << ';';
    }
    /// End of extra code
  }
  double regret = bandit->getRegret();
  cout << regret << "\n";
  // cout << "Regret = " << regret << "\n";
  // cout << "Terminating.\n";
  
  delete bandit;

  return 0;
}

