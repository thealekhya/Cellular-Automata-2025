#include<bits/stdc++.h>
#include<string>
using namespace std;

string dectobin(int num, int n){
    string s="";
    if(num==0){
        for(int i=0;i<n;i++){
            s.push_back('0');
        }
    }
    while(num>0){
        s.push_back(char('0'+ num%2));
        num=num/2;
    }
    while(s.length()<n) s.push_back('0');
    reverse(s.begin(),s.end());
    return s;
}

string nextconfig(const string& ic, int rule, bool periodic){
    int n=ic.length();
    string ecarule= dectobin(rule,8);
    string next="";
    for(int i=0;i<n;i++){
        char left = (i==0)?((periodic)? ic[n-1]: '0') : ic[i-1];
        char mid=ic[i];
        char right = (i==n-1)? ((periodic)? ic[0]: '0') : ic[i+1];
        int pattern = ((left-'0')<<2) | ((mid -'0')<<1) | (right-'0');
        next+=ecarule[pattern];
    }
    return next;
}

void cycles(int rule, int n, bool periodic){
    unordered_set<string> visited;
    vector<int>cycles_length;
    int total_config= 1<<n;

    for(int i=0;i<total_config;i++){
        string start=dectobin(i,n);
        if(visited.count(start)) continue;

        unordered_map<string,int> seen;
        int count=0;
        string current=start;
        while(!seen.count(current)){
            seen[current]=count++;
            current=(periodic)?nextconfig(current,rule,periodic):nextconfig(current,rule,0);
        }

        int cycle_start=seen[current];
        int length=count-cycle_start;
        
        current=start;
        for(int i=0;i<count;i++){
            visited.insert(current);
            current=(periodic)?nextconfig(current,rule,periodic):nextconfig(current,rule,0);
        }

        cycles_length.push_back(length);
    }

    cout<<"Number of cycles: "<<cycles_length.size()<<endl;
    sort(cycles_length.begin(),cycles_length.end());
    for(int i=0; i<cycles_length.size();++i){
        cout<<"Length of cycle"<<i+1<<": "<<cycles_length[i]<<endl;
    }
} 

int main(){
    int rule, n;
    bool bc;
    cout<<"Enter the ECA Rule: ";
    cin>>rule;
    cout<<"Enter n: ";
    cin>>n;
    cout<<"Enter the Boundary Condition(Press '1' for Periodic and '0' for Null): ";
    cin>>bc;
    
    if(bc) cycles(rule,n,1);
    else cycles(rule,n,0);

    return 0;
}
