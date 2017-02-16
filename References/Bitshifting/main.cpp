#include <map>
#include <cmath>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

#define pb push_back

int main()
{
    for(int i=0;i<8;++i)
    {
        int x = (i & 4)>>2;
        int y = (i & 2)>>1;
        int z = i & 1;

        int n = (x<<2)|(y<<1)|z;

        cout<<x<<y<<z<<"\t"<<n<<endl;
    }
    return 0;
}
