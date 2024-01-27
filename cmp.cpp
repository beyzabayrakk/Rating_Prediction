#include<iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <map>
#include<queue>
#include<algorithm>
using namespace std;

struct user{
    int user_id,numberofratings;
};
struct item{
    int item_id,numberofratings;
};
bool user_comperative(user u1,user u2){
    return u1.numberofratings>u2.numberofratings;
}
bool item_comperative(item i1, item i2){
    return i1.numberofratings>i2.numberofratings;
}
class Prediction
{
public:
    Prediction(string trainpath, string testpath, string resultpath)
    {
        ReadData(trainpath);
        show10users();
        show10items();
        Write(testpath,resultpath);
    }

private:
    unordered_map<int, map<int,float>> UserList;
    unordered_map<int, map<int,float>> ItemList;

    void ReadData(string trainpath){
        string datastring;
        ifstream trainfile(trainpath);
        getline(trainfile, datastring);
        while (trainfile.peek()!= EOF)
        {
            getline(trainfile,datastring,',');
            int user_id = atoi(datastring.c_str());
            getline(trainfile,datastring,',');
            int item_id = atoi(datastring.c_str());
            getline(trainfile,datastring);
            float rating = atof(datastring.c_str());
            UserList[user_id][item_id] = rating;
            ItemList[item_id][user_id] = rating;
        }
    }

    float CosineSimilarity(vector<float> vec1, vector<float> vec2) // 0-1
    {
        if(vec1.size()<3) return-1;
        float res = 0, mag1 = 0, mag2 = 0;

        for (int i = 0; i < vec1.size(); i++)
        {
            res += vec1[i] * vec2[i];
            mag1 += vec1[i] * vec1[i];
            mag2 += vec2[i] * vec2[i];
        }
        mag2 = sqrt(mag2);
        mag1 = sqrt(mag1);
        return res / (mag1 * mag2);
    }

    float MakePredictionwithUBFC(int user_id, int item_id) {

        map<int,float> users = ItemList[item_id];
        map<int,float>::iterator i = users.begin();

        int number_of_ratings=0;
        float total_ratings=0;

        while(i != users.end()){
            if(UserSimilarity(user_id, i->first) > 0.50){
                number_of_ratings++;
                total_ratings+= i->second;
            }
            i++;
        }
        if(number_of_ratings>0)return total_ratings/number_of_ratings;
        else return 3.47;
    }
    
    

    

    float UserSimilarity(int user1,int user2){
        vector<float> vec1,vec2;
        map<int,float> user1map = UserList[user1]; // 1,4,7,34,345,5675,
        map<int,float>::iterator i1 = user1map.begin();
        map<int,float> user2map = UserList[user2]; //1,46,224,567,2345
        map<int,float>::iterator i2 = user2map.begin();

        while(i1 != user1map.end() && i2 != user2map.end()){
            if(i1->first== i2->first){
                vec1.push_back(i1->second);
                vec2.push_back(i2->second);
                i1++;
                i2++;
            }
            else if(i1->first < i2->first){
                i1++;
            }
            else i2++;
        }
        return CosineSimilarity(vec1,vec2);
    }
    float MakePredictionwithIBFC(int user_id, int item_id) {

        map<int,float> items = UserList[user_id];
        map<int,float>::iterator i = items.begin();

        int number_of_ratings=0;
        float total_ratings=0;

        while(i != items.end()){
            if(itemSimilarity(item_id, i->first) > 0.50){
                number_of_ratings++;
                total_ratings+= i->second;
            }
            i++;
        }
        if(number_of_ratings>0)return total_ratings/number_of_ratings;
        else return 3.47;
    }
    float itemSimilarity(int item1,int item2){
        vector<float> vec1,vec2;
        map<int,float> item1map = ItemList[item1]; // 1,4,7,34,345,5675,
        map<int,float>::iterator i1 = item1map.begin();
        map<int,float> item2map = ItemList[item2]; //1,46,224,567,2345
        map<int,float>::iterator i2 = item2map.begin();

        while(i1 != item1map.end() && i2 != item2map.end()){
            if(i1->first== i2->first){
                vec1.push_back(i1->second);
                vec2.push_back(i2->second);
                i1++;
                i2++;
            }
            else if(i1->first < i2->first){
                i1++;
            }
            else i2++;
        }
        return CosineSimilarity(vec1,vec2);
        }
    
    void Write(string testpath,string resultpath){
        string datastring;
        ifstream testfile(testpath);
        getline(testfile, datastring);
        ofstream resultfile(resultpath);
        resultfile<<"ID,Predicted\n";
        while( testfile.peek()!= EOF){
            getline(testfile, datastring,',');
            int id = atoi(datastring.c_str());
            getline(testfile, datastring,',');
            int user_id = atoi(datastring.c_str());
            getline(testfile, datastring);
            int item_id = atoi(datastring.c_str());
            resultfile<< id<<","<<(MakePredictionwithIBFC(user_id,item_id)+MakePredictionwithUBFC(user_id,item_id))/2<<"\n";

        }
    }
    void show10users (){
        vector<user>best;
        unordered_map<int, map<int, float>>::iterator  i=UserList.begin();
        while (i!=UserList.end())
        {
            user u ;
            u.user_id=i->first;
            u.numberofratings=i->second.size();
            best.push_back(u);
            i++;

        }
        sort(best.begin(),best.end(),user_comperative);
        for (int j = 0; j<10; j++){
            cout<<"user "<<best[j].user_id<<" = "<<best[j].numberofratings<<endl;
        }
        

    }
    void show10items (){
        vector<item>best;
        unordered_map<int, map<int, float>>::iterator  i=ItemList.begin();
        while (i!=ItemList.end())
        {
            item k ;
            k.item_id=i->first;
            k.numberofratings=i->second.size();
            best.push_back(k);
            i++;

        }
        sort(best.begin(),best.end(),item_comperative);
        for (int j = 0; j<10; j++){
            cout<<"movie "<<best[j].item_id<<" = "<<best[j].numberofratings<<endl;
        }
        

    }
    
    };

int main()
{
    Prediction pre("train.csv", "test.csv", "LastlastSubmission50.csv");

    return 0;
}
