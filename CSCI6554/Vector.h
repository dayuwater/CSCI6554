//
//  Vector.h
//  CSCI6554
//
//  Created by Tan Wang on 15/1/25.
//  
//

#ifndef CSCI6554_Vector_h
#define CSCI6554_Vector_h


#include <iostream>
#include <vector>
#include <math.h>

using namespace std;
// use for vectors for 3 variables for now
class Vec {
public:
    // default constructor, create a vector with 3 variables with initial value of 0
    Vec(){
        vec.resize(3);
        for(int i=0; i<3; i++){
            vec[i]=0;
        }
    }
    
    // get the elements from the vector
    float x(){
        return vec[0];
    }
    float y(){
        return vec[1];
    }
    float z(){
        return vec[2];
    }
    // set the elements
    void setX(float a){
        vec[0]=a;
    }
    void setY(float a){
        vec[1]=a;
    }
    void setZ(float a){
        vec[2]=a;
    }
    void set(float a, float b, float c){
        vec[0]=a;
        vec[1]=b;
        vec[2]=c;
    }
    // basic operations
    Vec add(Vec b){
        Vec r;
        r.setX(x()+b.x());
        r.setY(y()+b.y());
        r.setZ(z()+b.z());
        return r;
    }
    
    Vec sub(Vec b){
        Vec r;
        r.setX(x()-b.x());
        r.setY(y()-b.y());
        r.setZ(z()-b.z());
        return r;
    }
    
    Vec mult(Vec b){
        Vec r;
        r.setX(x()*b.x());
        r.setY(y()*b.y());
        r.setZ(z()*b.z());
        return r;
    }
    
    Vec div(Vec b){
        Vec r;
        r.setX(x()/b.x());
        r.setY(y()/b.y());
        r.setZ(z()/b.z());
        return r;
    }
    
    Vec scale(float a){
        Vec r;
        r.set(x()*a, y()*a, z()*a);
        return r;
    }
    
    // advanced operations
    float dot(Vec b){
        
        return x()*b.x()+y()*b.y()+z()*b.z();
    }
    // if the vector is a vector, this is the length of the vector
    // if the vector is a point, this is the distance from the point to original
    float length(){
        return sqrt(x()*x()+y()*y()+z()*z());
    }
    // calculate the distance of two points
    float distance(Vec b){
        float xx=x()-b.x();
        float yy=y()-b.y();
        float zz=z()-b.z();
        return sqrt(xx*xx+yy*yy+zz*zz);
    }
    // normalize the vector
    Vec normalize(){
        Vec r;
        if(length()==0){
            r.set(0,0,0);
            return r;
        }
        
        r.set(x()/length(), y()/length(), z()/length());
        return r;
    }
    Vec cross(Vec b){
        Vec r;
        r.set(y()*b.z()-z()*b.y(),z()*b.x()-x()*b.z(),x()*b.y()-y()*b.x() );
        return r;
    }



    
    // print the vector
    void toString(){
        cout << "-------" << endl;
        cout << "Printing Vector" << endl;
        cout << "<";
        for(int i=0; i<vec.size(); i++){
            cout << vec[i] << "\t" ;
        }
        cout << ">" << endl;
    }
protected:
    // the field for vector
    vector<float> vec;
};

#endif
