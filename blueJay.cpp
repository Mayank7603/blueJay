#include<bits/stdc++.h>
using namespace std;

class Person{       // created a class for storing the details of individual person

    public :
    string name;
    string position_id;
    int file_number;
    int last_active_date;
    int crnt_active_date;
    int crnt_hour;
    int cnst_days;
    int max_cnst_days;
    int prev_hour;
    int prev_min;

    Person(string name,int f, int l, int cA , int chr,string pId, int ph, int pm){      // parameterised constructor for initializing objects
        this->name=name;
        file_number=f;
        last_active_date=l;
        crnt_active_date=cA;
        crnt_hour=chr;
        position_id=pId;
        cnst_days=1;
        prev_hour=ph;
        prev_min=pm;
    }

};

void printInOutputFile(set<pair<string,string> >arr, string details)   // print function for printing values in output File 
{

    ofstream opFile("output.txt");  // creating an ofstream class object for printing in output.txt file 

    opFile<<details<<" \n"<<endl;
    for(auto &x : arr){
        opFile<<"Name : "<<x.first<<"\nPosition Id : "<<x.second<<"\n"<<endl;
    }
    opFile<<endl;
    
}

void print(set<pair<string,string> >arr, string details )   // print function for printing values in terminal 
{

    cout<<details<<" \n"<<endl;
    for(auto &x : arr){
        cout<<"Name : "<<x.first<<"\nPosition Id : "<<x.second<<"\n"<<endl;
    }
    cout<<endl;
    
}

bool inputFile(string fileName, vector<vector<string> > &data)      // input File function 
{

    ifstream file(fileName);       // creating ifstream class object for reading the file 

    if(!file)       // if we are unable to open file due to wrong address or corrupted file 
    {
        cout<<"Unable to open file "<<endl;
        return 0;
    }

    string line;
    while(getline(file,line))       // iterating all the rows present in the file 
    {
        istringstream newStream (line);
        string temp;
        vector<string>individual;
        while(getline(newStream,temp,','))      // separating all the column values present in the row and storing them into a vector named data 
        {
            individual.push_back(temp);
        }
        data.push_back(individual);
    }

    return 1;
}

void evaluationCase(unordered_map<int,Person *>mp,set<pair<string,string> >&arr)    // basic function for selecting users according to first case after computation
{

    for(auto & x:mp)
    {
        Person * temp=x.second;

        if(temp->max_cnst_days>=7)      // checking the condition who has worked for 7 consecutive days
        {
            arr.insert(make_pair(temp->name,temp->position_id));    // storing into set to remove duplicacy
        }

    }   
}

void computation(vector<vector<string> >&data ,set<pair<string,string> >&case1,set<pair<string,string> >&case2, set<pair<string,string> >&case3)    // main function for analyzing the stored data
{

    unordered_map<int,Person *>mp;  // creating a map for linking file Number to Person's class object 
    struct tm tm;                   // for extracting details of time string given in dataset

    for(auto &x:data)   // iterating all over the dataset
    {
        strptime(x[2].c_str() ,"%m/%d/%Y %I:%M:%S %p", &tm);        // stripping time into day, month, year, hour and minutes

        int fileNumber=atoi(x[9].c_str());      // storing basic details
        int currHour=atoi(x[4].c_str());
        int timeHour=tm.tm_hour;
        int timeMin=tm.tm_min;
        int currDate=tm.tm_mday;

        
        if(mp.count(fileNumber)>0)      // checking wheter we have created object for this person or not
        {
            Person * tempPerson=mp[fileNumber];

            if(currDate==tempPerson->crnt_active_date)  //checking for same day remaining shift by comparing date
            {
                if(tempPerson->crnt_hour+currHour>14)      // checking for condition that who has worked for more than 14 hours in a single shift
                {
                    case3.insert(make_pair(tempPerson->name,tempPerson->position_id));
                }
                tempPerson->crnt_hour+=currHour;       

                int hr=timeHour-tempPerson->prev_hour;      // checking for condition that who have less than 10 hours of time between shifts but greater than 1 hour
                int min=timeMin-tempPerson->prev_min;
                double finalElapsedTime=hr+(min/60.0);
                if(finalElapsedTime<=10 && finalElapsedTime>1)
                {
                    case2.insert(make_pair(tempPerson->name,tempPerson->position_id));
                }

            }

            else      // proceeding ahead to another day shifts
            {
                tempPerson->last_active_date=tempPerson->crnt_active_date;
                tempPerson->crnt_active_date=currDate;
                int checkDate=currDate-tempPerson->last_active_date;

                if(checkDate==1)        // updating values by checking whether we have consecutive days or not 
                {
                    tempPerson->cnst_days+=1;
                }
                else
                {
                    tempPerson->cnst_days=1;
                }

                tempPerson->max_cnst_days=max(tempPerson->max_cnst_days,tempPerson->cnst_days);     // updating the maximum consecutive days 
                tempPerson->crnt_hour=currHour;
                tempPerson->prev_hour=timeHour;
                tempPerson->prev_min=timeMin;
            }

            strptime(x[3].c_str() ,"%m/%d/%Y %I:%M:%S %p", &tm);
            int endDate=tm.tm_mday;
            if(endDate!=currDate)   //  Employee completed his shift on another day 
            {
                tempPerson->last_active_date=tempPerson->crnt_active_date;
                tempPerson->crnt_active_date=endDate;
                tempPerson->cnst_days+=1;
                tempPerson->max_cnst_days=max(tempPerson->max_cnst_days,tempPerson->cnst_days);     // updating the maximum consecutive days 
            }

        }
        
        else    // this case means that there is no object for the given object number 
        {
            string psId=(x[0]);
            string name=(x[7]+ " "+x[8]);
            int timeCardHours=atoi(x[4].c_str());

            if(timeCardHours>14)       // checking for condition that who has worked for more than 14 hours in a single shift
            {
                case3.insert(make_pair(name,psId));
            }

            Person *newPerson= new Person(name,fileNumber,-1,currDate,timeCardHours,psId,timeHour,timeMin);     // creating a new Person class object based on collected details 
            mp[fileNumber]=newPerson;
        }
    }

    evaluationCase(mp,case1);       // calling the evaluation function for who have worked for 7 consecutive days

}

int main(){

    // location of dataset in .csv format
    string fileName="testcase.csv";        
    vector<vector<string> > data;

    // Inputting the file in program 

    inputFile(fileName,data);   

    // for storing differnt conditions result 

    set<pair<string,string> >case1;     
    set<pair<string,string> >case2;
    set<pair<string,string> >case3;

    // Analyzing the dataset
    
    computation(data,case1,case2,case3);        
   

    // Printing the required  data by invoking the print function 

    string c1="Case 1 : who have worked for 7 consecutive days ";
    string c2="Case 2 :  who have less than 10 hours of time between shifts but greater than 1 hour";
    string c3="Case 3 : who has worked for more than 14 hours in a single shift";
    print(case1,c1);
    print(case2,c2);
    print(case3,c3);

}
