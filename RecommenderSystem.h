//
// Created by ayelet on 24/06/2020.
//

#ifndef EX5_RECOMMENDERSYSTEM_H
#define EX5_RECOMMENDERSYSTEM_H

#include <string>
#include <vector>
#include <map>
using std::string;
using std::vector;
using std::map;

/**
 * @brief This class represents a movies recommender system.
 */
class RecommenderSystem
{
public:

    /**
     * @brief default constructor.
     */
    RecommenderSystem();

    /**
     * @brief constructor: constructs the recommender system with movies attributes and user ranks.
     * @param moviesAttributes: map with movies as keys and ranks (ints) as values.
     * @param userRanks: map with users as keys and ranks (ints) as values.
     */
    RecommenderSystem(map<string, vector<double>> moviesAttributes, map<string, vector<double>>
                      userRanks);



    /**
     * @brief gets two strings with path to files input, and constructs the reccomeder system.
     * @param moviesAttributesFilePath: text input file contains movies ranks.
     * @param userRanksFilePath: text input file contains users ranks.
     * @return: 0 on success, 1 on failure.
     */
    int loadData(string moviesAttributesFilePath, string userRanksFilePath);
    /**
     * @brief gets a username, finds it favorsVector and returns a reccomended movie for user.
     * @param userName: user's username.
     * @return: a string represents the mose recommended movie for user (if there is no such user
     * exist, returns "USER NOT FOUND".
     */
    string recommendByContent(const string& userName);
    /**
     * @brief gets a username and a movie that user is willing to watch, and predics the rank it
     * will get by user.
     * @param movieName: string
     * @param userName: string
     * @param k: positive integer
     * @return the predicted score for movie (if moviename or username does not exist, returns -1.
     */
    double predictMovieScoreForUser(string movieName, string userName, int k);
    /**
     * @brief gets a username and int k, and recommends a movie for user.
     * @param userName: string
     * @param k: a posotive integer.
     * @return: recommended movie name.
     */
    string recommendByCF(const string& userName, int k);
private:
    map<string, vector<double>> _moviesAttributes;
    map<string, vector<double>> _userRanks;
    vector<string> _moviesRankedByUsers;
    void _openFile(std::ifstream& inFile, const string& filePath);
    void _processAttributeFile(std::ifstream &inFile);
    void _processUsersFile(std::ifstream &inFile);
    double _getNorm(vector<double> const& vector);
};


#endif //EX5_RECOMMENDERSYSTEM_H
