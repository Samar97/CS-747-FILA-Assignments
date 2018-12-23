#ifndef BANDIT_H
#define BANDIT_H

#include <iostream>
#include <vector>
#include <random>

#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"

using namespace std;

enum BanditType {bernoulli, betaDistribution, histogram, invalid};

class Bandit{

 private:

  int numArms;
  int binCount;
  vector<double> armMeans;
  vector<double> armAlpha;
  vector<double> armBeta;
  vector<vector<double> > armHistograms;
  vector<gsl_rng*> ran;

  double maxMean;
  double cumulativeReward;
  unsigned long int numTotalPulls;

  BanditType type;
  
 public:

  Bandit(const BanditType type, const int &numArms, const vector<double> &means, const vector<double> &alpha, const vector<double> &beta, const vector<vector<double> > &histograms, const int &seed, const int &binCount);  
  ~Bandit();
  
  int getNumArms();
  unsigned long int getNumTotalPulls();

  double pull(const int &armIndex);

  double getCumulativeReward();
  double getRegret();

  void display();
};

#endif

