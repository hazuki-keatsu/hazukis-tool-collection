#include "split.hpp"
#include <iostream>
using namespace std;

int main()
{
    string test = "a,b,d,c";
    vector<string> output = hazuki::split<vector<string>>(test,",");
    for (auto &i : output)
    {
        cout << i << " ";
    }
    cout << endl;
}