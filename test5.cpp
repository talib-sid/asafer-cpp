#include <iostream>
using namespace std;

int main(){
    // int *p = new int;
    // *p = 5; // this is a valid assignment
    // cout << *p << endl; // garbage value
    // cout << p << endl; // but memory defined-well

    int *p;
    *p = 5; // this is a valid assignment
    cout << *p << endl; // garbage value
    cout << p << endl; // but memory defined-well
}
