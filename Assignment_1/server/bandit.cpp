#include "bandit.h"
#include <random>
#include <typeinfo>

using namespace std;

Bandit::Bandit(const BanditType type, const int &numArms, const vector<double> &means, const vector<double> &alpha, const vector<double> &beta, const vector<vector<double> > &histograms, const int &seed, const int &binCount)
  : type(type) {

  armMeans.clear();
  armAlpha.clear();
  armBeta.clear();
  armHistograms.clear();
     this->numArms = numArms;
  this->binCount = binCount;
  maxMean = -1.0;
  if (type == betaDistribution){
 

    for(int i = 0; i < numArms; i++){
      armAlpha.push_back(alpha[i]);
      armBeta.push_back(beta[i]);
 
      double currentArmMean = alpha[i]/(alpha[i] + beta[i]);
      armMeans.push_back(currentArmMean); 
      
    }
  }

  for(int a = 0; a < numArms && (type == bernoulli || type == betaDistribution); a++){
    if (type == bernoulli){

    
      armMeans.push_back(means[a]);
    }
    if(armMeans[a] > maxMean){
      maxMean = armMeans[a];
    }
  }

  double weightedMean = 0;
  double binWidth = 1.0/binCount;
  double left = 0;
  double right = left + binWidth;
  for(int a = 0; a < numArms && type == histogram; a++){
    for(int b = 0; b < binCount; b++){
      weightedMean += ((right+left)/2.0)*histograms[a][b];
      left = right;
      right += binWidth;  
      }
      left = 0;
      right = binWidth;
      if(weightedMean > maxMean)
        maxMean = weightedMean;
      weightedMean = 0;
    } 

  for(int a = 0; a < numArms && type == histogram; a++){
    armHistograms.push_back(histograms[a]);
  }
  
  gsl_rng* seedGenerator = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(seedGenerator, seed);

  ran.clear();
  for(int a = 0; a < numArms; a++){
    ran.push_back(gsl_rng_alloc(gsl_rng_mt19937));
    gsl_rng_set(ran[a], gsl_rng_get(seedGenerator));
  }

  gsl_rng_free(seedGenerator);

  cumulativeReward = 0;
  numTotalPulls = 0;
}

Bandit::~Bandit(){
  
  for(int a = 0; a < numArms; a++){
    gsl_rng_free(ran[a]);
  }
}
 
int Bandit::getNumArms(){

  return numArms;
}

unsigned long int Bandit::getNumTotalPulls(){

  return numTotalPulls;
}


double Bandit::pull(const int &armIndex){

  double reward = 0;
  double temp = 0;
  int iterator = 0;
  switch (type){
  case bernoulli:
    if(gsl_rng_uniform(ran[armIndex]) < armMeans[armIndex]){
      reward = 1.0;
    }
    break;
  case betaDistribution:
    {
      reward =  gsl_ran_beta(ran[armIndex], armAlpha[armIndex], armBeta[armIndex]);
      break;
      

    }
  case histogram:
    temp = gsl_rng_uniform(ran[armIndex]);
    while(temp - armHistograms[armIndex][iterator] > 0 && iterator < binCount)
    {
      temp = temp - armHistograms[armIndex][iterator];
      iterator++;
    }
    reward = (1.0/binCount)*(iterator + temp);
    break;
  default:
    reward = 0;
  }
  
  cumulativeReward += reward;
  numTotalPulls++;
    
  return reward;
}


double Bandit::getCumulativeReward(){

  return cumulativeReward;
}

double Bandit::getRegret(){
  // cout << "maxMean" << maxMean << " numTotalPulls" << numTotalPulls << " cumulativeReward" << cumulativeReward << endl;
  return ((numTotalPulls * maxMean) - cumulativeReward);
}

void Bandit::display(){

  // cout << "********************************\n";
  for(int a = 0; a < numArms; a++){
    // cout << a << ": " << armMeans[a] << "\n";
  }

  // cout << "max mean: " << maxMean << "\n";
  // cout << "********************************\n";
}

