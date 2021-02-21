//
// Created by ayelet on 24/06/2020.
//

// INCLUDES
#include <iostream>
#include "RecommenderSystem.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <set>

// DEFINES
#define SUCCESS (0)
#define FAILURE (1)
#define MINUS_INF (-10000)
#define ERROR_INPUT_FILE ("Unable to open file " + filePath)
#define USER_NOT_FOUND ("USER NOT FOUND")

// USINGS
using std::string;
using std::vector;

/**
 * @brief default constructor.
 */
RecommenderSystem::RecommenderSystem()
= default;

/**
 * @brief constructs the recommender system with movies attributes and user ranks.
 * @param moviesAttributes: map with movies as keys and ranks (ints) as values.
 * @param userRanks: map with users as keys and ranks (ints) as values.
 */
RecommenderSystem::RecommenderSystem(map<string, vector<double>> moviesAttributes, map<string,
                                     vector<double>> userRanks)
{
    moviesAttributes = this->_moviesAttributes;
    userRanks = this->_userRanks;
}

/**
 * @brief open files.
 * @param inFile: file to opem
 * @param filePath: path
 */
void RecommenderSystem:: _openFile(std::ifstream& inFile, const string& filePath)
{
    inFile.open(filePath);
    if (inFile.is_open())
    {
        return;
    }
    else
    {
        throw std::runtime_error(ERROR_INPUT_FILE);
        exit(FAILURE);
    }
}


/**
 * @brief get two strings - a full string, and a suffix and checks if full string ends with suffix.
 * @param fullString: string to check if ends with.
 * @param suffix: suffix to check.
 * @return true if ends with given suffix, else false.
 */
bool endsWith(string const &fullString, string const &suffix)
{
    if (fullString.length() >= suffix.length())
    {
        return (0 == fullString.compare(fullString.length() - suffix.length(), suffix.length(),
                suffix));
    }
    else
    {
        return false;
    }
}

/**
 * @brief parses movies attributes file into a map of strings and vector of ints.
 * @param inFile: attributes file.
 */
void RecommenderSystem::_processAttributeFile(std::ifstream &inFile) noexcept (false)
{
    string line;
   while (getline(inFile, line))
   {
       if (inFile.good())
       {
           string word = "";
           string movieName;
            for (auto ch : line)
            {
                if ((ch == ' ' || ch == '\r') && (word != "1" && word != "2" && word != "3" &&
                    word != "4" && word != "5" && word != "6" && word != "7" && word != "8"
                    && word != "9" && word != "10" && word !="NA" && !endsWith (word, "\r")))
                {
                    movieName = word;
                    word = "";
                }
                else if ((ch == ' ' || ch == '\r') && (word.length() == 1 || word == "10" ||
                         (endsWith(word, "\r"))))
                {
                    std::stringstream toConvert(word);
                    int val = 0;
                    toConvert >> val;
                    this->_moviesAttributes[movieName].push_back(val);
                    word = "";
                }
                else
                {
                    word = word + ch;
                }

            }
       }
   }
    if (inFile.eof())
    {
        return;
    }
}

void RecommenderSystem::_processUsersFile(std::ifstream &inFile) noexcept(false)
{
    string line;
    // get first line and put it into a vector.
    getline(inFile, line);
    if (inFile.good())
    {
        string word = "";
        for (auto ch : line)
        {
            if (ch == ' ' || ch == '\r')
            {
                this->_moviesRankedByUsers.push_back(word);
                word = "";
            }
            else
            {
                word = word + ch;
            }
        }
    }
    // get the other lines and put it into map.
    while (getline(inFile, line))
    {
        {
            string word = "";
            string userName;
            for (auto ch : line)
            {
                if ((ch == ' ' || ch == '\r') && (word != "1" && word != "2" && word != "3" &&
                    word != "4" && word != "5" && word != "6" && word != "7" && word != "8"
                    && word != "9" && word != "10" && word !="NA" && !endsWith (word, "\r")))
                {
                    userName = word;
                    word = "";
                }
                else if ((ch == ' ' || ch == '\r') && (word.length() == 1 || word == "10" || endsWith(word, "\r")))
                {
                    std::stringstream toConvert(word);
                    int val = 0;
                    toConvert >> val;
                    this->_userRanks[userName].push_back(val);
                    word = "";
                }
                else if ((ch == ' ' || ch == '\r') && word == "NA")
                {
                    this->_userRanks[userName].push_back(0);
                    word = "";
                }
                else
                {
                    word = word + ch;
                }
            }
        }
    }
    if (inFile.eof())
    {
        return;
    }
}

/**
 * @brief: gets two strings with path to files input, and constructs the reccomeder system.
 * @param moviesAttributesFilePath: text input file contains movies ranks.
 * @param userRanksFilePath: text input file contains users ranks.
 * @return: 0 on success, 1 on failure.
 */
int RecommenderSystem::loadData(string moviesAttributesFilePath, string userRanksFilePath)
{
    std::ifstream attributeFile;     //open attributeFile
    _openFile(attributeFile, moviesAttributesFilePath);
    _processAttributeFile(attributeFile);
    attributeFile.close();
    std::ifstream usersFile; // open usersFile.
    _openFile(usersFile, userRanksFilePath);
    _processUsersFile(usersFile);
    usersFile.close();
    return SUCCESS;
}

/**
 * @brief calculates a vector's norm, and returns it.
 * @param vector: vector to find norm.
 * @return: norm.
 */
double RecommenderSystem::_getNorm(vector<double> const &vector)
{
    double squared = 0.0;
    for (double coor : vector)
    {
        squared += coor * coor;
    }
    return sqrt(squared);
}

/**
 * @brief gets a username, finds it favorsVector and returns a reccomended movie for user.
 * @param userName: user's username.
 * @return a string represents the mose recommended movie for user (if there is no such user
 * exist, returns "USER NOT FOUND".
 */
string RecommenderSystem::recommendByContent(const string& userName)
{
    try
    {
        vector<double> ratingVec(_userRanks.at(userName));
        // find user in map.
        int sum = (std::accumulate(ratingVec.begin(), ratingVec.end(), 0.0));
        int alreadyWatchedCounter = 0;
        for (double &rank : ratingVec)
        {
            if (rank != 0)
            {
                alreadyWatchedCounter++;
            }
        }
        auto average = (double) ((double)sum / alreadyWatchedCounter);
        //normalization
        for (double & rank : ratingVec)
        {
            if (rank != 0)
            {
                rank -= average;
            }
        }
        vector<double > favorsVec(_moviesAttributes[_moviesRankedByUsers[0]].size());
        for (double i = 0; i < ratingVec.size(); i++)
        {
            if (ratingVec[i] != 0 && _userRanks[userName][i] != 0) // user has already watched the
                // movie
            {
                vector<double> normalizeAttribute(_moviesAttributes[_moviesRankedByUsers[i]]);
                for (double &rank : normalizeAttribute)
                {
                    rank *= ratingVec[i];
                }
                for (double j = 0; j < favorsVec.size(); j++)
                {
                    favorsVec[j] += normalizeAttribute[j];
                }

            }
        }
        string recommended;
        double bestSimilarity = MINUS_INF;
        double favorsVecNorm = _getNorm(favorsVec);
        for (double i = 0; i < ratingVec.size(); i++)
        {
            if (ratingVec[i] == 0 && _userRanks[userName][i] == 0) // movie is still not watched by user.
            {
                vector<double> candidate(_moviesAttributes[this->_moviesRankedByUsers[i]]);
                double candidateNorm = _getNorm(candidate);
                double innerProd = std::inner_product(favorsVec.begin(), favorsVec.end(),
                        candidate.begin(), 0.0);
                double similarityFactor = innerProd / (favorsVecNorm * candidateNorm);
                if (similarityFactor > bestSimilarity)
                {
                    bestSimilarity = similarityFactor;
                    recommended = _moviesRankedByUsers[i];
                }
            }

        }
        return recommended;

    }
    catch(const std::exception &e)
    {
        return USER_NOT_FOUND;
    }

}

/**
 * @brief gets a username and a movie that user is willing to watch, and predics the rank it
 * will get by user.
 * @param movieName: string
 * @param userName: string
 * @param k: positive integer
 * @return the predicted score for movie (if moviename or username does not exist, returns -1.
 */
double RecommenderSystem::predictMovieScoreForUser(string movieName, string userName, int k)
{
    try
    {
        vector<double> usersRatingVec(this->_userRanks.at(userName));
        vector<std::pair<double, string>> toSort(usersRatingVec.size());
        const vector<double> unseenMovieRate(_moviesAttributes.at(movieName));
        double normOfMovie = _getNorm(unseenMovieRate);
        for (double i = 0; i < usersRatingVec.size(); i++)
        {
            if (usersRatingVec[i] != 0)
            {
                vector<double> seen(this->_moviesAttributes[this->_moviesRankedByUsers[i]]);
                double seenNorm = _getNorm(seen);
                double innerProd = std::inner_product(unseenMovieRate.begin(), unseenMovieRate.end
                (), seen.begin(), 0.0);
                double similarityFactor = innerProd / (normOfMovie * seenNorm);
                toSort[i].second = _moviesRankedByUsers[i];
                toSort[i].first = similarityFactor;
            }
        }
        std::sort(toSort.begin(), toSort.end());
        double sumWithRanks = 0;
        double sumOfSimilarities = 0;
        for (double i = toSort.size() - 1; i >= toSort.size() - k; i--)
        {
            int index = -1; // add exception maybe
            for (double j = 0; j < this->_moviesRankedByUsers.size(); j++)
            {
                if (toSort[i].second == this->_moviesRankedByUsers[j])
                {
                    index = j;
                }
            }
            double usersRank = usersRatingVec[index];
            sumWithRanks += (usersRank * toSort[i].first);
            sumOfSimilarities += toSort[i].first;
        }


        return (sumWithRanks / sumOfSimilarities);

    }
    catch(const std::exception &e)
    {
        return -1;
    }

}

/**
 * @brief gets a username and int k, and recommends a movie for user.
 * @param userName: string
 * @param k: a positive integer.
 * @return recommended movie name.
 */
string RecommenderSystem::recommendByCF(const string& userName, int k)
{
    try
    {
        vector<double> usersRatingVec(this->_userRanks.at(userName).begin(), this->_userRanks.at
                (userName).end());
        double bestScored = 0;
        string recommended;
        for (double i = 0; i < usersRatingVec.size(); i++)
        {
            if (usersRatingVec[i] == 0)
            {
                double tempScore = predictMovieScoreForUser(this->_moviesRankedByUsers[i], userName,
                        k);
                if (tempScore > bestScored)
                {
                    bestScored = tempScore;
                    recommended = this->_moviesRankedByUsers[i];
                }
            }
        }
        return recommended;
    }
    catch(const std::exception &e)
    {
        return USER_NOT_FOUND;
    }

}