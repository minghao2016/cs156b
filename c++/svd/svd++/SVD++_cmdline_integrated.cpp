#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <ctime>
#include <algorithm>

using namespace std;

// is sushkov using his mac or not?
const int MAC = 0;

// 123: 98291669
// 123_86%of4: 99473899
// 2: 1965045
const int NUM_POINTS = 1965045;

// Filenames
const char dataFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123_86%of4.txt";
const char dataFilePathUM[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123_86%of4.txt";
const char outOfSampleFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(valid).txt";
const char qualFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char dataFilePath5[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data5.txt";
const char aggrFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data_7%of4(aggr).dta";
const char aggrOutFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/aggregation/svd++/integratedSVD++_50f_aggr";
const char outputFilePath[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD++/integratedSVD++_50f_qual.txt";

const char outputUserFeatureFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/userFeatures_SVD++_40f_version1.txt";
const char outputMovieFeatureFile[] = "/home/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/movieFeatures_SVD++_40f_version1.txt";

const char dataFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data123.txt";
const char dataFilePathUMMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/um/data123.txt";
const char outOfSampleFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/data4.txt";
const char qualFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/mu/qual.dta";
const char outputFilePathMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/submissions/SVD/SVDreg_40f_min150e_K=0.02.txt";

const char outputUserFeatureFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/user/userFeatures_SVD++_50f_K=0.02_min150e.txt";
const char outputMovieFeatureFileMAC[] = "/Users/msushkov/Dropbox/Caltech/Caltech Classes/CS/CS 156b/data/feature_matrices/movie/movieFeatures_SVD++_50f_K=0.02_min150e.txt";

// constants
const float GLOBAL_AVG_MOVIE_RATING = 3.60951619727;
const int NUM_USERS = 458293;
const int NUM_MOVIES = 17770;

const int NUM_FEATURES = 50;
const int NUM_EPOCHS = 15;

float LEARNING_RATE_W = 0.001;
float LEARNING_RATE_Y = 0.001;
float LEARNING_RATE_USER = 0.006;
float LEARNING_RATE_MOVIE = 0.011;
float LRATE_ub = 0.012;
float LRATE_mb = 0.003;

float LAMBDA_W = 0.015;
float K_user = 0.08;
float K_movie = 0.006;
float LAMBDA_Y = 0.03;
float LAMBDA_ub = 0.03;
float LAMBDA_mb = 0.001;

const float LEARNING_RATE_DECREASE_RATE = 0.9;
const float EPSILON = 0.0001;
const float VARIANCE_RATIO = 25;

// defines a single training data point
class DataPoint {
public:
    int user;
    int movie;
    float rating;

    DataPoint() {
        this->user = 0;
        this->movie = 0;
        this->rating = 0;
    }

    DataPoint(int user, int movie, float rating) {
        this->user = user;
        this->movie = movie;
        this->rating = rating;
    }
};

class MovieRating {
public:
    int movie;
    int rating;

    MovieRating(int movie, int rating) {
        this->movie = movie;
        this->rating = rating;
    }
};

// a vector of datapoints to hold data
//vector<DataPoint*> *trainingData = new vector<DataPoint*>;
//vector<DataPoint> **trainingData = new vector<DataPoint>*[NUM_USERS]; // an array of vector pointers to DataPoint pointers

int *movies = new int[NUM_POINTS];
int *users = new int[NUM_POINTS];
int *ratings = new int[NUM_POINTS];
int *times = new int[NUM_POINTS];

// feature matrices (2D arrays); feature k for user u is userFeatures[u][k]
float **userFeatures = new float*[NUM_USERS];
float **movieFeatures = new float*[NUM_MOVIES];

// arrays to hold the average ratings for each movie and for each user
float *avgUserRatings = new float[NUM_USERS];
float *avgMovieRatings = new float[NUM_MOVIES];

// the average offset for a given user
float *avgUserOffset = new float[NUM_USERS];
float *avgMovieOffset = new float[NUM_MOVIES];

// b_u and b_i
float *userBias = new float[NUM_USERS];
float *movieBias = new float[NUM_MOVIES];

// For each user, the number of movies they rated
int *movieRatingsByUser = new int[NUM_USERS];

// the inverse square root of the number of movie ratings
float *norm = new float[NUM_USERS];

float *normW = new float[NUM_USERS];

// stores the y-values of each y[movie][feature]
float **y = new float*[NUM_MOVIES];
//float **y = new float*[NUM_FEATURES];

// stores the sum of the y's: sumY[user][feature]
float **sumY = new float*[NUM_USERS];

// which movies has this user seen?
vector<int> **seenByUser = new vector<int>*[NUM_USERS];

float **w = new float*[NUM_MOVIES];
float *sumW = new float[NUM_USERS];

//float **c = new float*[NUM_MOVIES];
//float **sumC = new float*[NUM_USERS];

void computeNorms() {
    for (int i = 0; i < NUM_USERS; i++) {
        int numMovies = seenByUser[i]->size();
        //int numMovies = trainingData[i]->size();

        numMovies++;

        if (numMovies > 1)
            normW[i] = pow(numMovies, -0.5);
        else
            normW[i] = 1;

        int numMovies2 = movieRatingsByUser[i];

        numMovies2++;

        if (numMovies2 > 1)
            norm[i] = pow(numMovies2, -0.5);
        else
            norm[i] = 1;
    }
}

// returns a random value between -0.01 and 0.01
float getRandom() {
	//return 10.0 / 10.0 * ((float) rand() / ((float) RAND_MAX)) - 0.0;
	int num = ((int) rand()) % 100;
	return 2.0 * num / 10000.0 - 0.01;
}

// initialize each element of the feature matrices
void initialize() {
    for (int i = 0; i < NUM_POINTS; i++) {
        movies[i] = 0;
        users[i] = 0;
        ratings[i] = 0;
        times[i] = 0;
    }

    // initialize the user feature matrix
    for (int i = 0; i < NUM_USERS; i++)
        userFeatures[i] = new float[NUM_FEATURES];
    for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            //float rand = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES[0]) + getRandom();
            userFeatures[i][j] = getRandom();
            //userFeatures[i][j] = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES) + getRandom();
        }
    }
        
    // initialize the movie feature matrix
    for (int i = 0; i < NUM_MOVIES; i++)
        movieFeatures[i] = new float[NUM_FEATURES];
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            //float rand = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES[0]) + getRandom();
            movieFeatures[i][j] = getRandom();
            //movieFeatures[i][j] = sqrt(GLOBAL_AVG_MOVIE_RATING / (float) NUM_FEATURES) + getRandom();
        }
    }

    for (int i = 0; i < NUM_USERS; i++) {
        userBias[i] = 0.0;
        movieRatingsByUser[i] = 0;
        norm[i] = 0.0;
        normW[i] = 0.0;
    }

    for (int i = 0; i < NUM_MOVIES; i++)
        movieBias[i] = 0.0;

    // initialize y
    for (int i = 0; i < NUM_MOVIES; i++)
        y[i] = new float[NUM_FEATURES];

    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int j = 0; j < NUM_FEATURES; j++) {
            y[i][j] = getRandom();
            //y[i][j] = 0;
        }
    }
        
    for (int i = 0; i < NUM_USERS; i++) {
        sumY[i] = new float[NUM_FEATURES];

        //sumW[i] = new float[NUM_FEATURES];
        sumW[i] = 0;

        //sumC[i] = new float[NUM_FEATURES];

        for (int j = 0; j < NUM_FEATURES; j++) {
            sumY[i][j] = 0.0;
            //sumW[i][j] = 0.0;
            //sumC[i][j] = 0.0;
        }
    }

    for (int i = 0; i < NUM_MOVIES; i++) {
        w[i] = new float[NUM_MOVIES];
        //c[i] = new float[NUM_MOVIES];

        for (int j = 0; j < NUM_MOVIES; j++) {
            w[i][j] = 0;
        }
    }
        
    //for (int i = 0; i < NUM_USERS; i++)
        //trainingData[i] = new vector<DataPoint>;

    for (int i = 0; i < NUM_USERS; i++)
        seenByUser[i] = new vector<int>;
}

// helper function that splits a string
vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim))
        elems.push_back(item);
    return elems;
}

// helper function to split a string
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    return split(s, delim, elems);
}

float predictRating(int movie, int user) {
	float result = GLOBAL_AVG_MOVIE_RATING + userBias[user] + movieBias[movie] + norm[user] * sumW[user];
    
    // find the dot product
    for (int f = 0; f < NUM_FEATURES; f++)
        result += (movieFeatures[movie][f] * (userFeatures[user][f] + normW[user] * sumY[user][f]));

    return result;
}

float predictRating_training(int movie, float userBiasArg, float movieBiasArg, float *sumYArray, float normArg, float *userFeatureVec, float *movieFeatureVec) {
    float result = GLOBAL_AVG_MOVIE_RATING + userBiasArg + movieBiasArg;

    // find the dot product
    for (int f = 0; f < NUM_FEATURES; f++)
        result += movieFeatureVec[f] * (userFeatureVec[f] + normArg * sumYArray[f]);

    return result;
}

// compute the in-sample error
float computeInSample(int numFeatures) {
    float error = 0;
    int size = 0;

    // go through each training point and calculate the error for it
    /*for (int i = 0; i < NUM_USERS; i++) {
        for (int j = 0; j < trainingData[i]->size(); j++) {
            DataPoint currData = trainingData[i]->at(j);
            size++;

            int currUser = currData.user;
            int currMovie = currData.movie;
            float actualRating = currData.rating;

            float predicted = predictRating(currMovie - 1, currUser - 1);

            // make sure the rating is between 1 and 5
            if (predicted > 5)
                predicted = 5;
            else if (predicted < 1)
                predicted = 1;

            error += (predicted - actualRating) * (predicted - actualRating);
        }
    }*/

    for (int i = 0; i < NUM_POINTS; i++) {
        int currUser = users[i];
        int currMovie = movies[i];
        float actualRating = ratings[i];

        float predicted = predictRating(currMovie - 1, currUser - 1);

        // make sure the rating is between 1 and 5
        if (predicted > 5)
            predicted = 5;
        else if (predicted < 1)
            predicted = 1;

        error += (predicted - actualRating) * (predicted - actualRating);
        
    }

    return sqrt(error / NUM_POINTS);
}

// compute the out of sample error
float computeOutOfSample(int numFeatures) {
    float error = 0;
    int numPoints = 0;

    string line;
    ifstream outOfSample;
    
    if (MAC == 0)
    	outOfSample.open(outOfSampleFile, ios::in);
    else
    	outOfSample.open(outOfSampleFileMAC, ios::in);

    // go through the input data line by line
    while (getline(outOfSample, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;
        int actualRating = -1;
        int actualTime = -1;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    user = val;
                else if (count == 1)
                    movie = val;
                else if (count == 3)
                    actualRating = val;
                else if (count == 4)
                    actualTime = val;
                count++;
            }
        }

        float predictedRating = predictRating(movie - 1, user - 1);

        // make sure the rating is between 1 and 5
        if (predictedRating > 5)
            predictedRating = 5;
        else if (predictedRating < 1)
            predictedRating = 1;
            
        assert(actualRating != -1);

        // get the squared error for this point
        error += (predictedRating - actualRating) * (predictedRating - actualRating);

        numPoints++;
    }

    outOfSample.close();

    return sqrt(error / numPoints);
}

// iterate through the epochs and all the data points for each epoch
/*void learn() {
    float Eout = 10000;
    float prevEout = 1000000;

    int i = 0;

    // go N times through the data
    //for (int i = 0; i < NUM_EPOCHS; i++) {
    while (i < NUM_EPOCHS || (prevEout - Eout) > EPSILON) {
        const clock_t begin_time = clock();

        for (int p = 0; p < NUM_USERS; p++) {

            float tempSumY[NUM_FEATURES];

            vector<DataPoint> *currUserVector = trainingData[p];
            int size = currUserVector->size();

            float *currSumY = sumY[p];

            // for each movie the user rated
            for (int j = 0; j < size; j++) {
            
                DataPoint currData = currUserVector->at(j);
                int user = currData.user;
                int movie = currData.movie;
                int rating = currData.rating;
                
                // train the SVD on each point (will go through all features)
                float *userFeature = userFeatures[user - 1];
                float *movieFeature = movieFeatures[movie - 1];

                float userBiasVal = userBias[user - 1];
                float movieBiasVal = movieBias[movie - 1];

                //float error = rating - predictRating(movie - 1, user - 1);
                float error = rating - predictRating_training(movie - 1, userBiasVal, movieBiasVal, currSumY, norm[user - 1], userFeature, movieFeature);

                //predictRating_training(int movie, float userBiasArg, float *sumYArray, float normArg, float *userFeatureVec)

                userBias[user - 1] += (LRATE_ub * (error - LAMBDA_ub * userBiasVal));
                movieBias[movie - 1] += (LRATE_mb * (error - LAMBDA_mb * movieBiasVal));

                // iterate over features 0 through N - 1
                for (int k = 0; k < NUM_FEATURES; k++) {
                    float oldUserFeature = userFeature[k];
                    float oldMovieFeature = movieFeature[k];

                    userFeature[k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K * oldUserFeature));
                    movieFeature[k] += (LEARNING_RATE_MOVIE * (error * (oldUserFeature + norm[user - 1] * currSumY[k]) - K * oldMovieFeature));
                    tempSumY[k] += (error * norm[user - 1] * oldMovieFeature);

                } // end for loop over features
                
            } // end movie loop

            // for each movie the user rated
            for (int j = 0; j < size; j++) {
                int movie = currUserVector->at(j).movie;
                float *currY = y[movie - 1];

                for (int k = 0; k < NUM_FEATURES; k++) {                   
                    float update = LEARNING_RATE_Y * (tempSumY[k] - LAMBDA_Y * currY[k]);
                    currY[k] += update;
                    currSumY[k] += update;
                }
                
            } // end movie loop
            
        } // end user loop

        // decrease the learning rates
        LEARNING_RATE_Y *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_USER *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_MOVIE *= LEARNING_RATE_DECREASE_RATE;
        LRATE_ub *= LEARNING_RATE_DECREASE_RATE;
        LRATE_mb *= LEARNING_RATE_DECREASE_RATE;

        // only check Eout every 5 iterations
        if (i % 3 == 0) {
            prevEout = Eout;
            Eout = computeOutOfSample(NUM_FEATURES);
            cout << "    Eout = " << Eout << endl;
        }

        cout << "Finished epoch: " << i + 1 << " out of " << NUM_EPOCHS << endl;
        cout << " --> " << float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;

        i++;

    } // end epoch loop

    // print out the results
    cout << endl << "RESULTS: (K, # of features, Ein, Eout)" << endl;
    cout << K << ", " << NUM_FEATURES << ", " << computeInSample(NUM_FEATURES) << ", " << computeOutOfSample(NUM_FEATURES) << endl;
}*/

void learn() {
    computeNorms();

    float Eout = 10000;
    float prevEout = 1000000;

    int iterations = 0;

    int prevUser = 1;

    // go N times through the data
    while (iterations < NUM_EPOCHS || (prevEout - Eout) > EPSILON) {
        const clock_t begin_time = clock();

        int countForCurrUser = 0;

        vector<MovieRating> moviesRatedByCurrUser;

        //float tempSumW[NUM_FEATURES];
        float tempSumY[NUM_FEATURES];
        //float tempSumC[NUM_FEATURES];

        for (int i = 0; i < NUM_FEATURES; i++) {
            tempSumY[i] = 0.0;
            //tempSumC[i] = 0.0;
        }

        // go through each point
        for (int p = 0; p < NUM_POINTS; p++) {
            int user = users[p];
            int movie = movies[p];
            int rating = ratings[p];
            int numMovies = movieRatingsByUser[user - 1];
                
            float *userFeature = userFeatures[user - 1];
            float *movieFeature = movieFeatures[movie - 1];

            float userBiasVal = userBias[user - 1];
            float movieBiasVal = movieBias[movie - 1];

            float *currSumY = sumY[user - 1];

            float error = rating - predictRating(movie - 1, user - 1);
            //float error = rating - predictRating_training(movie - 1, userBiasVal, movieBiasVal, currSumY, norm[user - 1], userFeature, movieFeature);

            userBias[user - 1] += (LRATE_ub * (error - LAMBDA_ub * userBiasVal));
            movieBias[movie - 1] += (LRATE_mb * (error - LAMBDA_mb * movieBiasVal));

            // the user changed
            if (user != prevUser) {
                countForCurrUser = 1;
                moviesRatedByCurrUser.clear();
            } else {
                countForCurrUser++;
            }

            moviesRatedByCurrUser.push_back(MovieRating(movie, rating));

            // iterate over features 0 through N - 1
            for (int k = 0; k < NUM_FEATURES; k++) {
                float oldUserFeature = userFeature[k];
                float oldMovieFeature = movieFeature[k];

                userFeature[k] += (LEARNING_RATE_USER * (error * oldMovieFeature - K_user * oldUserFeature));
                movieFeature[k] += (LEARNING_RATE_MOVIE * (error * (oldUserFeature + norm[user - 1] * currSumY[k]) - K_movie * oldMovieFeature));
                tempSumY[k] += (error * norm[user - 1] * oldMovieFeature);

                //tempSumC[k] += (error * norm[user - 1]);
            }

            // if we had just finished going through all the movies this user has seen
            if (countForCurrUser == numMovies) { // user is going to change on the next data point

                // for each movie the user rated
                for (int j = 0; j < numMovies; j++) {
                    int movie_j = moviesRatedByCurrUser.at(j).movie;

                    double deviation = moviesRatedByCurrUser.at(j).rating - (GLOBAL_AVG_MOVIE_RATING + avgUserOffset[user - 1] + avgMovieOffset[movie_j - 1]);
                    double updateW = error * normW[user - 1] * deviation;

                    double wCache = w[movie - 1][movie_j - 1];

                    w[movie - 1][movie_j - 1] += LEARNING_RATE_W * (updateW - LAMBDA_W * wCache);

                    sumW[user - 1] += deviation * w[movie - 1][movie_j - 1];

                } // end movie loop

                for (int j = 0; j < seenByUser[user - 1]->size(); j++) {
                    float *currY = y[seenByUser[user - 1]->at(j) - 1];

                    for (int k = 0; k < NUM_FEATURES; k++) {
                        float updateY = LEARNING_RATE_Y * (tempSumY[k] - LAMBDA_Y * currY[k]);
                        
                        currY[k] += updateY;
                        currSumY[k] += updateY;
                    }
                }

            } // end if

            //} // end for loop over features

            // reset the temp sum array before the next user
            if (countForCurrUser == numMovies) {
                for (int i = 0; i < NUM_FEATURES; i++) {
                    tempSumY[i] = 0.0;
                    //tempSumW[i] = 0.0;
                }
            }

            prevUser = user;

            //if (p % 10000000 == 0)
                //cout << p / 1000000 << " million" << endl;

        } // end points loop

        // decrease the learning rates
        LEARNING_RATE_Y *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_USER *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_MOVIE *= LEARNING_RATE_DECREASE_RATE;
        LRATE_ub *= LEARNING_RATE_DECREASE_RATE;
        LRATE_mb *= LEARNING_RATE_DECREASE_RATE;
        LEARNING_RATE_W *= LEARNING_RATE_DECREASE_RATE;

        // only check Eout every few iterations
        if (iterations % 3 == 0) {
            prevEout = Eout;
            Eout = computeOutOfSample(NUM_FEATURES);
            cout << "    Eout = " << Eout << endl;
        }

        cout << "Finished epoch: " << iterations + 1 << " out of " << NUM_EPOCHS << endl;
        cout << " --> " << float(clock() - begin_time) / CLOCKS_PER_SEC << " seconds" << endl;

        iterations++;

    } // end epoch loop

    // print out the results
    cout << endl << "RESULTS: (# of features, Ein, Eout)" << endl;
    cout << NUM_FEATURES << ", " << computeInSample(NUM_FEATURES) << ", " << computeOutOfSample(NUM_FEATURES) << endl;
}

// computes the average ratings for each user (uses UM data set)
// also computes the user offset
void getAvgUserRatings() {
    string line;

    // get the UM data file
    ifstream dataFile;
    if (MAC == 0)
    	dataFile.open(dataFilePathUM, ios::in);
    else
    	dataFile.open(dataFilePathUMMAC, ios::in);
    
    int numLinesSoFar = 0;

    int prevUser = 1;
    float totalRating = 0;
    int index = 0; // counts the # of movies each user rated

    float currTotalUserOffset = 0;

    // go through the input data line by line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;

        int currUser = -1;
        int currMovie = -1;
        int currRating = -1;
        int currTime = -1;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    currUser = val;
                else if (count == 1)
                    currMovie = val;
                else if (count == 3)
                    currRating = val;
                //else if (count == 4)
                    //currTime = val;
                count++;
            }
        }
        
        //DataPoint *currPoint = new DataPoint(currUser, currMovie, currRating);
        //DataPoint currPoint(currUser, currMovie, currRating);

        movies[numLinesSoFar] = currMovie;
        users[numLinesSoFar] = currUser;
        ratings[numLinesSoFar] = currRating;
        times[numLinesSoFar] = currTime;

        // if the user changes
        if (prevUser != currUser) {
            //avgUserRatings[prevUser - 1] = totalRating / index;
            avgUserRatings[prevUser - 1] = (GLOBAL_AVG_MOVIE_RATING * VARIANCE_RATIO + totalRating) / (VARIANCE_RATIO + index);
            totalRating = currRating;

            avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
            currTotalUserOffset = currRating - avgMovieRatings[currMovie - 1];
            //currTotalUserOffset = currRating - GLOBAL_AVG_MOVIE_RATING;
            movieRatingsByUser[prevUser - 1] = index;
            
            // we see a new user, so he starts off with the only rating we have seen for him
            //moviesByUser[currUser - 1]->push_back(currMovie);
            //trainingData[currUser - 1]->push_back(currPoint);

            index = 1;
        } else { // if we keep seeing data for the same user
            index++;
            totalRating += currRating;
            currTotalUserOffset += (currRating - avgMovieRatings[currMovie - 1]);
            //currTotalUserOffset += (currRating - GLOBAL_AVG_MOVIE_RATING);
            
            //moviesByUser[currUser - 1]->push_back(currMovie);
            //trainingData[currUser - 1]->push_back(currPoint);
        }

        prevUser = currUser;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << "Computing avg user ratings: " << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with end of file
    avgUserRatings[prevUser - 1] = totalRating / index;
    avgUserOffset[prevUser - 1] = currTotalUserOffset / index;
    movieRatingsByUser[prevUser - 1] = index;

    dataFile.close();
}

// reads data in (MU) and also computes avg ratings for each movie
// also computes the movie offset
void getData() {
    string line;
    ifstream dataFile;
    
    if (MAC)
    	dataFile.open(dataFilePathMAC, ios::in);
    else
    	dataFile.open(dataFilePath, ios::in);
    
    int numLinesSoFar = 0;

    int prevMovie = 1;
    float totalRating = 0;
    int index = 0; // counts the # of users that watched each movie

    float currTotalMovieOffset = 0;

    // go through the input data line by line
    while (getline(dataFile, line)) {
        // where are we in the current line?
        int count = 0;
        
        int currUser = -1;
        int currMovie = -1;
        float currRating = -1;

        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    currUser = val;
                else if (count == 1)
                    currMovie = val;
                else if (count == 3)
                    currRating = val;
                count++;
            }
        }
        // add the point to our data vector
        //trainingData->push_back(currPoint);

        // to calculate the average ratings for the movies
        if (prevMovie != currMovie) {
            //avgMovieRatings[prevMovie - 1] = totalRating / index;
            avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
            
            totalRating = currRating;

            avgMovieOffset[prevMovie - 1] = currTotalMovieOffset / index;

            //currTotalMovieOffset = currPoint->rating - avgUserRatings[currPoint->user - 1];
            currTotalMovieOffset = currRating - GLOBAL_AVG_MOVIE_RATING;

            index = 1;
        } else {
            if (currRating != 0) {
                index++;
                totalRating += currRating;
                //currTotalMovieOffset += (currPoint->rating - avgUserRatings[currPoint->user - 1]);
                currTotalMovieOffset += (currRating - GLOBAL_AVG_MOVIE_RATING);
            }
        }

        prevMovie = currMovie;
            
        numLinesSoFar++;
        if (numLinesSoFar % 1000000 == 0)
            cout << numLinesSoFar / 1000000 << " million" << endl;
    }    

    // deals with last line with respect to calculating the average movie rating
    //avgMovieRatings[prevMovie - 1] = totalRating / index;
    avgMovieRatings[prevMovie - 1] = (VARIANCE_RATIO * GLOBAL_AVG_MOVIE_RATING + totalRating) / (VARIANCE_RATIO + index);
    
    avgMovieOffset[prevMovie - 1] = currTotalMovieOffset / index;

    dataFile.close();

    // at this point we have the avg movie ratings filled out
    // now calculate the avg user ratings
    getAvgUserRatings();
}

void outputVectors() {
    cout << "Output vectors" << endl;

    // output files
    ofstream outputFileUser;
    
    if (MAC == 0)
	    outputFileUser.open(outputUserFeatureFile, ios::out);
	else
		outputFileUser.open(outputUserFeatureFileMAC, ios::out);
	
    ofstream outputFileMovie;
    if (MAC == 0)
	    outputFileMovie.open(outputMovieFeatureFile, ios::out);
    else
    	outputFileMovie.open(outputMovieFeatureFileMAC, ios::out);
    
    // output the user features
    for (int i = 0; i < NUM_USERS; i++) {
        for (int k = 0; k < NUM_FEATURES; k++) {
            outputFileUser << userFeatures[i][k] << " ";
        }

        outputFileUser << endl;
    }

    // output the movie features
    for (int i = 0; i < NUM_MOVIES; i++) {
        for (int k = 0; k < NUM_FEATURES; k++) {
            outputFileMovie << movieFeatures[i][k] << " ";
        }

        outputFileMovie << endl;
    }

    outputFileMovie.close();
    outputFileUser.close();
}

void outputAggr() {
    // output file
    ofstream outputFile;
    outputFile.open(aggrOutFilePath, ios::out);

    
    // aggregation file
    string line;
    ifstream aggrFile;
    aggrFile.open(aggrFilePath, ios::in);

    // go through each line of the qual file
    while (getline(aggrFile, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;

        // read the values on the current line one by one
        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    user = val;
                else if (count == 1)
                    movie = val;
                count++;
            }
        }

        // calculate the rating for user, movie
        float predictedScore = predictRating(movie - 1, user - 1);

        // make sure the rating is between 1 and 5
        if (predictedScore > 5)
            predictedScore = 5;
        else if (predictedScore < 1)
            predictedScore = 1;

        // write to file
        outputFile << predictedScore << endl;
    }

    aggrFile.close();
    outputFile.close();
}

// writes data out
void outputResults() {
    // output file
    ofstream outputFile;
    if (MAC)
    	outputFile.open(outputFilePathMAC, ios::out);
    else
    	outputFile.open(outputFilePath, ios::out);
    
    // qual file
    string line;
    ifstream qualFile;
    if (MAC)
    	qualFile.open(qualFilePathMAC, ios::in);
    else
    	qualFile.open(qualFilePath, ios::in);
    
    // go through each line of the qual file
    while (getline(qualFile, line)) {
        // where are we in the current line?
        int count = 0;

        int user = -1;
        int movie = -1;

        // read the values on the current line one by one
        istringstream lineIn(line);
        while (lineIn) {
            int val = 0;
            if (lineIn >> val) {
                if (count == 0)
                    user = val;
                else if (count == 1)
                    movie = val;
                count++;
            }
        }

        // calculate the rating for user, movie
        float predictedScore = predictRating(movie - 1, user - 1);

        // make sure the rating is between 1 and 5
        if (predictedScore > 5)
            predictedScore = 5;
        else if (predictedScore < 1)
            predictedScore = 1;

        // write to file
        outputFile << predictedScore << endl;
    }

    qualFile.close();
    outputFile.close();

    //outputAggr();
    //outputVectors();
}

void cleanup() {
    for (int i = 0; i < NUM_USERS; i++)
        delete[] userFeatures[i];
        
    for (int i = 0; i < NUM_MOVIES; i++)
        delete[] movieFeatures[i];
        
    delete[] userFeatures;
    delete[] movieFeatures;

    

    /*delete[] avgMovieRatings;
    delete[] avgUserRatings;

    delete[] avgUserOffset;
    delete[] avgMovieOffset;*/

    //delete[] userBias;
    //delete[] movieBias;

    //delete[] movieRatingsByUser;

    //delete[] norm;

    
}

// K-value, # features, # epochs
int main(int argc, char *argv[]) {
	srand((unsigned) time(0));

    initialize();

    if (argc != 11) {
        cout << "Usage: ./SVD++ [LEARNING_RATE_Y] [LEARNING_RATE_USER] [LEARNING_RATE_MOVIE] [LRATE_ub] [LRATE_mb] [K_user] [K_movie] [LAMBDA_Y] [LAMBDA_ub] [LAMBDA_mb]" << endl;
        return 1;
    } else {
        LEARNING_RATE_Y = atof(argv[1]);
        LEARNING_RATE_USER = atof(argv[2]);
        LEARNING_RATE_MOVIE = atof(argv[3]);
        LRATE_ub = atof(argv[4]);
        LRATE_mb = atof(argv[5]);
        K_user = atof(argv[6]);
        K_movie = atof(argv[7]);
        LAMBDA_Y = atof(argv[8]);
        LAMBDA_ub = atof(argv[9]);
        LAMBDA_mb = atof(argv[10]);
    }


    getData();
    cout << "Done with getData()" << endl;
    
    // run the SVD algorithm
    learn();
    cout << "Done learning" << endl;

    //outputResults();
    cout << "Done with outputResults()" << endl;

    //cleanup();
    cout << "DONE!" << endl;
    
    return 0;
}
