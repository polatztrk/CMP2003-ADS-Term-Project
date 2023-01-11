#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include "createAllList.h"
#include "ratePredictor.h"


int main() {
    //Pair-vector-vector which stores pairs of ratings. One side of the rating pair is prediction_user's rating and
    //the other side is the rating of every other user who have watched the predicted_item.
    vector<vector<pair<float, float> > > rating_pair_vector;
    string file_name = "train.csv";
    string file2_name = "test.csv";
    //A list created from train.csv with indices corresponding to User IDs.
    vector<vector<pair<float, float> > > user_based_all_list_train = createAllList(file_name, 0);
    //A list created from test.csv with indices corresponding to IDs (the order, from top to down).
    vector<vector<pair<float, float> > > user_based_all_list_test = createAllList(file2_name, 0);
    //A list created from train.csv with indices corresponding to Item IDs.
    vector<vector<pair<float, float> > > item_based_all_list_train = createAllList(file_name, 1);
    //Lists created from user_based_all_list_train and item_based_all_list_train with indices corresponding to Users/Items
    //and elements to number of Items watched/People have watched.
    vector<int> all_users_item_count, all_items_user_count;
    //A more accurate rating pair vector. It only has requiredQuality or higher amount of elements in every sub-vector.
    vector<vector<pair<float, float> > > quality_rating_pair_vector;
    //Vectors for store results in the final
    vector<float> container;
    vector<float> container2;
    vector<float> finalcontainer;


    //all_users_item_count generator.
    for (int j = 0; j < user_based_all_list_train.size(); j++) {
        all_users_item_count.push_back(user_based_all_list_train[j].size());
    }
    //all_items_user_count generator.
    for (int j = 0; j < item_based_all_list_train.size(); j++) {
        all_items_user_count.push_back(item_based_all_list_train[j].size());
    }
    auto start = chrono::high_resolution_clock::now();
    //predictionIndex is equal to "ID" in the test file. It is the "row number" to find which userID we are working with.
    for (int predictionIndex = 0; predictionIndex < user_based_all_list_test.size(); predictionIndex++) {
        //UserID is being assigned to predicted_user.
        int predicted_user = user_based_all_list_test[predictionIndex][0].first;
        //ItemID is being assigned to predicted_item.
        int predicted_item = user_based_all_list_test[predictionIndex][0].second;
        //Amount of films which "UserID" has watched is being assigned to predicted_users_item_count.


        for (int h = 0; h < item_based_all_list_train[predicted_item].size(); h++) {
            //Two indices to compare two users' items.
            int i = 0, j = 0;
            //Temporary vector being pushed back into rating_pair_vector to create a vector to hold pairs.
            vector<pair<float, float> > temp;
            rating_pair_vector.push_back(temp);
            //quality_rating_pair_vector.push_back(temp);
            //The rating which was given by the "userID" to predicted_item is being stored with a "token" -1.
            rating_pair_vector[h].push_back(make_pair(-1, item_based_all_list_train[predicted_item][h].second));
            //The algorithm to compare two users Items, very quickly.
            while (i < user_based_all_list_train[predicted_user].size() &&
                   j < user_based_all_list_train[item_based_all_list_train[predicted_item][h].first].size()) {
                while (user_based_all_list_train[predicted_user][i].first >
                       user_based_all_list_train[item_based_all_list_train[predicted_item][h].first][j].first &&
                       j < user_based_all_list_train[item_based_all_list_train[predicted_item][h].first].size()) {
                    j++;
                }
                while (user_based_all_list_train[predicted_user][i].first <
                       user_based_all_list_train[item_based_all_list_train[predicted_item][h].first][j].first &&
                       i < user_based_all_list_train[predicted_user].size()) {
                    i++;
                }
                if (user_based_all_list_train[predicted_user][i].first ==
                    user_based_all_list_train[item_based_all_list_train[predicted_item][h].first][j].first) {
                    rating_pair_vector[h].push_back(make_pair(user_based_all_list_train[predicted_user][i].second,
                                                              user_based_all_list_train[item_based_all_list_train[predicted_item][h].first][j].second));
                    j++;
                }
            }

        }

        //These vectors take the result of two different mode of finalFunction
        container.push_back(finalFunction(rating_pair_vector,item_based_all_list_train[predicted_item].size(),0));
        container2.push_back(finalFunction(rating_pair_vector,item_based_all_list_train[predicted_item].size(),1));
        //finalcontainer.push_back(finalFunction(rating_pair_vector,item_based_all_list_train[predicted_item].size(),1));

        quality_rating_pair_vector.clear();
        rating_pair_vector.clear();
    }


    //If we want to use third scenario we use this for loop
    //Simply take the average of these two vectors
    for (int i = 0; i < container.size(); ++i) {
        finalcontainer.push_back((container[i]+container2[i])/2);
    }

    //And we write results to the submission.csv
    fstream submissionfile;
    submissionfile.open("submission.csv",ios::out | ios::app);

    submissionfile <<"ID"<<","<<"Predicted"<<endl;
    for (int j = 0; j < finalcontainer.size(); ++j) {
        submissionfile <<j<<","<<finalcontainer[j]<< endl;
    }

    submissionfile.close();
    /*for(auto x: finalcontainer){
        cout<<x<<endl;
    }*/

    auto stop = chrono::high_resolution_clock::now();
    auto duration = duration_cast<chrono::microseconds>(stop - start);
    cout << duration.count() <<" microseconds"<< endl;

    return 0;
}




