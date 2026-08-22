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

bool reversibilty(int rule, int n, bool periodic){
    unordered_map<string,string> mpp;
    int total_config=1<<n;
    for(int i=0; i<total_config;i++){
        string initial= dectobin(i,n);
        string next;
        if(periodic){
            next=nextconfig(initial,rule,periodic);
        }
        else if(!periodic){
            next=nextconfig(initial,rule,0);
        }
        if(mpp.count(next)) return false;
        mpp[next]=initial;
    }
    return true;
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
    if(reversibilty(rule,n,bc)){
        cout<<"The ECA is reversible.\n";
    }
    else cout<<"The ECA is not reversible.\n";
    return 0;
}
