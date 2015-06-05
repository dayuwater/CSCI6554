//
//  Matrix.h
//  CSCI6554
//
//  Created by Tan Wang on 15/1/25.
//  
//

#ifndef CSCI6554_Matrix_h
#define CSCI6554_Matrix_h

#include <iostream>
#include <vector>
#include "util.h"

using namespace std;

class Matrix {
public:
    
    
    // default constructor, create an identity 4*4 matrix
    
    Matrix(){
        mat.resize(4);
        for(int i=0; i<4; i++){
            // create a row
            vector<float> temp;
            temp.resize(4);
            // set all elements in the row to 0
            for(int j=0; j<4; j++){
                temp[j]=0;
            }
            // set the pivot element to 1
            temp[i]=1;
            // add the row to the matrix
            mat[i]=temp;
            
        }
        
        
        
    }
    // create an identity n*n matrix
    Matrix(int n){
        mat.resize(n);
        for(int i=0; i<n; i++){
            // create a row
            vector<float> temp;
            temp.resize(n);
            // set all elements in the row to 0
            for(int j=0; j<n; j++){
                temp[j]=0;
            }
            // set the pivot element to 1
            temp[i]=1;
            // add the row to the matrix
            mat[i]=temp;
            
        }
 
    }
    // create a rectangular matrix with specific value and specific size
    // n=array of the elements, a=the row number of matrix, b=the column number of matrix
    Matrix(float n[],int a, int b){
        mat.resize(a);
        for(int i=0; i<a; i++){
            // create a row
            vector<float> temp;
            temp.resize(b);
            // set all elements in the row to the value
            for(int j=0; j<b; j++){
                temp[j]=n[b*i+j];
            }
            
            // add the row to the matrix
            mat[i]=temp;
            
        }

    }
    // modify the matrix. a=row number, b=column number, n=new value
    void modify(int a, int b, float n){
        if(a>=mat.size()){
            cout << "There is no Row "<< a <<endl;
        }
        else if(b>=mat[a].size()){
            cout << "There is no Column "<< b << "in Row "<< a<< endl;
        }
        else{
            mat[a][b]=n;
        }
    }
    // scalar product
    Matrix scale(float factor, int mode){
        Matrix r;
        for(int i=0; i<numRows(); i++){
            for(int j=0; j<numColumns(); j++){
                if(mode==0x93){
                    r.modify(i,j,get(i,j)*factor);
                }
                else if(mode==0x94){
                    r.modify(i,j,get(i,j)/factor);
                }
                else if(mode==0x91){
                    r.modify(i,j,get(i,j)+factor);
                }
                else if(mode==0x92){
                    r.modify(i,j,get(i,j)-factor);
                }
            }
        }
        return r;
    }
    Matrix scale(float factor){
        return scale(factor,0x93);
    }
    // add two matrices together, mode=0x91 is add, mode=0x92 is subtract, mode=0x93 is multiplication by number, mode=0x94 is division
    Matrix add(Matrix b, int mode){
        Matrix r(b.numColumns());
        if(numRows()!=b.numRows()||numColumns()!=b.numColumns()){
            cout << "Cannot Add" << endl;
            return r;
        }
        else{
            for(int i=0; i<numRows(); i++){
                for(int j=0; j<numColumns(); j++){
                    switch(mode){
                        case 0x91:
                            r.modify(i,j,get(i,j)+b.get(i, j));
                            break;
                        case 0x92:
                            r.modify(i,j,get(i,j)-b.get(i, j));
                            break;
                        case 0x93:
                            r.modify(i,j,get(i,j)*b.get(i, j));
                            break;
                        case 0x94:
                            r.modify(i,j,get(i,j)/b.get(i, j));
                            break;
                    
                    }
                    
                }
            }
            return r;
        }
        
        
    }
    Matrix add(Matrix b){
        return add(b,0x91);
    }
    // multiplication
    Matrix multiply(Matrix b){
        Matrix r;
        if(numColumns()!=b.numRows()){
            cout << "Cannot Multiply" << endl;
        }
        else{
            float n[numRows()*b.numColumns()];
            int c=0;
            for(int i=0; i<numRows(); i++){
                for(int j=0; j<b.numColumns(); j++){
                    float temp=0.0;
                    for(int k=0; k<numColumns(); k++){
                        temp+=get(i,k)*b.get(k,j);
                    }
                    n[c]=temp;
                    c++;
                    
                }
            }
            r=Matrix(n,numRows(),b.numColumns());
            
        }
        return r;
    }
    // copy this matrix
    Matrix copy(){
        
        // create a mask matrix with all entries 0
        float f[numRows()*numColumns()];
        for(int i=0; i<numRows()*numColumns(); i++){
            f[i]=0;
        }
        Matrix b(f,numRows(),numColumns());
        Matrix e(f,numRows(),numColumns());// target matrix
        // add a 0 matrix, which means copy the matrix
        e=add(b);
        cout << e.get(3,3) << endl;
        return e;
        
    }
    // transpose
    Matrix transpose(){
        float r[numRows()*numColumns()];
        for(int i=0; i<numRows()*numColumns(); i++){
            r[i]=0;
        }
        Matrix a(r,numColumns(),numRows());
        for(int i=0; i<numColumns(); i++){
            for(int j=0; j<numRows(); j++){
                a.modify(i, j, get(j,i));
            }
        }
        return a;
    }
    // determinant
    float determinant(){
        return det(*this);
    }
    float det(Matrix a){
        // check if the matrix has a determinant
        if(a.numRows()!=a.numColumns()){
            cout << "This matrix is not a square matrix, cannot compute its determinant." << endl;
            return 0.0;
        }
        // Base Case: If the matrix is 2*2, the determinant can be directly calculated
        else if(a.numRows()==2){
            return a.get(0,0)*a.get(1,1)-a.get(0,1)*a.get(1,0);
        }
        // Induction: If the matrix is greater than 2*2, use the regualar recursion algorithm
        float result=0.0;
        for(int i=0; i<a.numColumns(); i++){
            float f[(a.numRows()-1)*(a.numColumns()-1)];
            int findex=0;
            for(int j=1; j<a.numRows(); j++){
                for(int k=0; k<a.numColumns(); k++){
                    if(k!=i){
                        f[findex]=a.get(j,k);
                        findex++;
                    }
                }
            }
            Matrix r(f,a.numRows()-1,a.numColumns()-1);
            //r.toString();
            if(i%2==0){
                result+=a.get(0,i)*det(r);
                //cout << a.get(0,i)<< "  "<<det(r);

            }
            else{
                result-=a.get(0,i)*det(r);
                //cout << a.get(0,i)<< "  "<<det(r);
            }
            
            
        }
        return result;
    }
    
    // inverse, can also use this function to solve linear equations
   //  Common Case: get the inverse of a n*n matrix
    Matrix inverse(){
        Matrix a(numRows());
        return solve(a);
    }

    // Alternative use: get the solution of an linear equation
    Matrix solve(Matrix aug){
        Matrix a=copy();
        Matrix g=aug;
        cout << aug.get(1,0) << endl;
        if(numColumns()!=numRows()){
            cout << "Cannot compute inverse for this matrix because this is not a square matrix." << endl;
            return *this; // return the matrix itself
        }
        if(determinant()==0){
            cout << "Cannot compute inverse because this is not invertible." <<endl;
            return *this; // return the matrix itself
        }
        // Step 1: get the echlon form
        // current pivot column
        for(int i=0; i<a.numRows(); i++){
            // divide all the elements in a row by the number in pivot column
            for(int j=i; j<a.numRows(); j++){
                float pivotColumn=a.get(j,i);
                if(pivotColumn!=0){
                
                    for(int k=0; k<a.numColumns(); k++){
                        a.modify(j, k, a.get(j, k)/pivotColumn);
                        g.modify(j, k, g.get(j,k)/pivotColumn);
                    }
                }
            }
            // subtract the pivot row with other rows
            for(int j=i+1; j<a.numRows(); j++){
                if(a.get(j,i)!=0){
                    for(int k=0; k<a.numColumns(); k++){
                        a.modify(j, k, a.get(j,k)-a.get(i,k));
                        g.modify(j, k, g.get(j,k)-g.get(i,k));

                    }
                }
            }
               
            
            
        }
        
        // Step 2: get the reduced echlon form (identity matrix)
        for(int i=a.numRows()-1; i>-1; i--){
            for(int j=i; j>-1; j--){
                float pivotColumn=a.get(j,i);
                if(pivotColumn!=0){
                    for(int k=0; k<a.numColumns(); k++){
                        a.modify(j, k, a.get(j, k)/pivotColumn);
                        g.modify(j, k, g.get(j, k)/pivotColumn);
                    }
                }
            }
            for(int j=i-1; j>-1; j--){
                if(a.get(j,i)!=0){
                    for(int k=0; k<a.numColumns(); k++){
                        a.modify(j, k, a.get(j,k)-a.get(i,k));
                        g.modify(j, k, g.get(j,k)-g.get(i,k));
                        
                    }
                }
            }

            
        }
        
        //a.toString();
        //g.toString();
        
        
        
        return g;
    }
    // get the size of the matrix
    int numRows(){
        return mat.size();
    }
    int numColumns(){
        return mat[0].size();
    }
    // get the elements in a specific position
    // a=row, b=column
    float get(int a, int b){
        if(a>=numRows()||b>=numColumns()){
            //cout << "There is no such place in the matrix" << endl;
            return 0.0;
        }
        else{
            return mat[a][b];
        }
        
    }
    // print out the matrix
    void toString(){
        cout << "--------" << endl;
        cout << "Printing Matrix" << endl;
        for(int i=0; i<mat.size(); i++){
            cout << "[" ;
            for(int j=0; j<mat[i].size(); j++){
                cout << mat[i][j] << "\t";
            }
            cout << "]" << endl;
        }
        cout << "-------"<<endl;
        
    }
    
protected:
   // fields
    vector<vector<float>> mat; // the vector array to store the matrix
    
};


#endif
