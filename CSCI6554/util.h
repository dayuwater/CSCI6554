//
//  util.h
//  CSCI6554
//
//  Created by Tan Wang on 15/3/29.
//  //

#ifndef CSCI6554_util_h
#define CSCI6554_util_h
#include <vector>
using namespace std;


// solve ax2+bx+c=0

 vector<float> solveX2(float a, float b, float c){
     vector<float> result;
     result.resize(2);
     float delta=b*b-4*a*c;
     if(delta<0){
         cout << "This equation has no real solution" << endl;
         result[0]=0;
         result[1]=0;
         
     }
     else{
         result[0]=(-b+sqrt(delta))/2/a;
         result[1]=(-b-sqrt(delta))/2/a;
     }
    
     return result;
    
}

float absolute(float a){
    if(a<0){
        return -a;
    }
    else {
        return a;
    }
}

float round6(float a){
    float b=a*1000000;
    int c=round(b);
    
    return c/1000000.0;
}


#endif
