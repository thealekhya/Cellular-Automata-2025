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

void generate_transitions(int rule, int n, bool periodic){
    int total_config= 1<<n;
    cout<<"Rule= "<<rule<<" n= "<<n<<" Boundary Condition= "<< (periodic? "Periodic": "Null")<<"\n";
    for(int i=0;i<total_config;++i){
        string initial = dectobin(i,n);
        string next_configuration= nextconfig(initial,rule,periodic);
        cout<<initial<<" -> "<<next_configuration<<"\n";
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
    generate_transitions(rule,n, bc);
    return 0;
}
