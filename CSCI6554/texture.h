//
//  texture.h
//  CSCI6554
//
//  Created by Tan Wang on 15/3/28.
//
//

#ifndef CSCI6554_texture_h
#define CSCI6554_texture_h


#include <iostream>
#include <vector>
#include <list>
#include <math.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <fstream>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include "util.h"
#include "Vector.h"
#include "Matrix.h"

#define PI 3.14159265358979

class Texture {
public:
    Texture(){
        
    }
   

    Texture(int height, int width){
        // initialize the texture image
        teximage.resize(height);
        for(int i=0; i<teximage.size(); i++){
            teximage[i].resize(width);
            for(int j=0; j<teximage[i].size(); j++){
                teximage[i][j].resize(3);
            }
        }
        srand(time(NULL));
        // initialize the noise
        noise.resize(height*4);
        for(int i=0; i<noise.size(); i++){
            noise[i].resize(width*4);
            for(int j=0; j<noise[i].size(); j++){
                noise[i][j]=(rand()%100)/100.0;
            }
            
        }
        
        
    }
       // get the color using the texture coordinates directly
    vector<float> getColor(int x, int y){
        vector<float> result;
        result.resize(3);
        if(x>=getHeight()) x=getHeight()-1;
        if(x<0)  x=0;
        if(y>=getWidth()) y=getWidth()-1;
        if(y<0) y=0;
        result[0]=teximage[x][y][0];
        result[1]=teximage[x][y][1];
        result[2]=teximage[x][y][2];
        
        
        return result;
    }
    // get the color using cylinder intermediate texture mapping
    vector<float> getColor(float theta, float z,float h){
        int x=getX(theta,z,h);
        int y=getY(theta,z,h);
        return getColor(x,y);
        
        
    }
    void setColor(int x, int y, float r, float g, float b){
        if(x>=getHeight()){
            x=599;
        }
        if(x<0){
            x=0;
        }
        if(y>=getWidth()){
            y=599;
        }
        if(y<0){
            y=0;
        }
        teximage[y][x][0]=r;
        teximage[y][x][1]=g;
        teximage[y][x][2]=b;
        
    }
    float getX(float theta, float z, float h){
        float result=0;
        result=getHeight()/2+z/h*getHeight();
        if(result>getHeight()){
            result=getHeight();
        }
        else if(result<0){
            result=0;
        }
        return result;
    }
    float getY(float theta, float z, float h){
        float result=0;
        result=theta/2/PI*getWidth();
        if(result>getWidth()){
            result=getWidth();
        }
        else if(result<0){
            result=0;
        }
        return result;
    }
    float getHeight(){
        return teximage.size();
    }
    float getWidth(){
        if(getHeight()!=0){
            return teximage[0].size();
        }
        else {
            return 0;
        }
    }
    
    // Mode 1: Pure color
    void pureColor(float r, float g, float b){
        setTurbulance(2);
        for(int i=0; i<teximage.size(); i++){
            for(int j=0; j<teximage[i].size(); j++){
                teximage[i][j][0]=r-turbulance[i][j];
                teximage[i][j][1]=g+2*turbulance[i][j];
                teximage[i][j][2]=b+3*turbulance[i][j];
                
            }
        }
        
    }
    // Mode 2: Swirl
    // r=n(theta) in polar coordinates
    // r1=color inside the curve
    // r2=color outside the curve
    
    
    void swirl(float n, float width,float r1, float g1, float b1, float r2,float g2,float b2){
        float theta=0;
        float r=n*theta;
        float rmax=(n+0.2)*(theta+width);
        float rmin=(n-0.2)*(theta-width);
        float x=getWidth()/2+r*cos(theta);
        float y=getHeight()/2+r*sin(theta);
        float xmin=getWidth()/2+rmin*cos(theta);
        float ymin=getHeight()/2+rmin*sin(theta);
        float xmax=getWidth()/2+rmax*cos(theta);
        float ymax=getHeight()/2+rmax*sin(theta);
        float xs=x-width;
        float xl=x+width;
        float ys=y-width;
        float yl=y+width;
        // initialize to the outside color
        pureColor(r2, g2, b2);
        setTurbulance(2);
        
        while(xl<getWidth()&&xs>0&&yl<getHeight()&&ys>0){
            for(x=xs; x<xl; x++){
                for(y=ys; y<yl; y++){
                    teximage[x][y][0]=r1+turbulance[x][y];
                    teximage[x][y][1]=g1+turbulance[x][y];
                    teximage[x][y][2]=b1+turbulance[x][y];
                }
            }
            theta+=0.2;
            r=n*theta;
            x=getWidth()/2+r*cos(theta);
            y=getHeight()/2+r*sin(theta);
            xs=x-width*(theta/100.0);
            xl=x+width*(theta/100.0);
            ys=y-width*(theta/100.0);
            yl=y+width*(theta/100.0);
            
            
        }
        /*x=xmax; y=ymax;
        theta=0;
        while(x<getWidth()&&x>0&&y<getHeight()&&y>0){
            
            teximage[x][y][0]=r1-0.5;
            teximage[x][y][1]=g1;
            teximage[x][y][2]=b1;
            theta+=0.1;
            r=(n+0.2)*(theta+width);
            x=getWidth()/2+r*cos(theta);
            y=getHeight()/2+r*sin(theta);
            
            
        }
        x=xmin; y=ymin;
        theta=0;
        while(x<getWidth()&&x>0&&y<getHeight()&&y>0){
            teximage[x][y][0]=r1;
            teximage[x][y][1]=g1;
            teximage[x][y][2]=b1+0.5;
            theta+=0.1;
            r=(n-0.2)*(theta-width);
            x=getWidth()/2+r*cos(theta);
            y=getHeight()/2+r*sin(theta);
            
            
        }*/


        
        
    }
    
    // Mode 3: Strips
    void strips(int start, int n){
        
        for(int x=0; x<getHeight(); x++){
            for(int y=0; y<getWidth(); y++){
                if(((x+start)%n)>(n/2)){
                    teximage[x][y][0]=1;
                    teximage[x][y][1]=1;
                    teximage[x][y][2]=1;
                    
                    
                }
                else{
                    teximage[x][y][0]=0;
                    teximage[x][y][1]=0;
                    teximage[x][y][2]=0;

                    
                }
            }
        }
        
    }
    
    // Mode 4: Perlin Noise
    void perlin(){
        setTurbulance(2);
        for(int i=0; i<teximage.size(); i++){
            for(int j=0; j<teximage[i].size(); j++){
                    teximage[i][j][0]=sin((i+j+100/turbulance[i][j])/30);
                    teximage[i][j][1]=sin((i+j+100/turbulance[i][j])/30);
                    teximage[i][j][2]=sin((i+j+100/turbulance[i][j])/30);
                /*teximage[i][j][0]=absolute(sin(50*i))-absolute(sin(50*j));
                teximage[i][j][1]=absolute(sin(50*i))-absolute(sin(50*j));
                teximage[i][j][2]=absolute(sin(50*i))-absolute(sin(50*j));*/
                /*teximage[i][j][0]=sin(50*i+turbulance[i][j])-sin(50*j+turbulance[i][j]);
                teximage[i][j][1]=sin(50*i+turbulance[i][j])-sin(50*j+turbulance[i][j]);

                teximage[i][j][2]=sin(50*i+turbulance[i][j])-sin(50*j+turbulance[i][j]);*/

                
                    
         
            }
            
        }

        
        //assert(false);
        
        
    }
    
    // Mode 5: Plants
    void stem(int  startX, int endX, int startY, int endY, int factor){
        float slope=(endY-startY)/(endX-startX);
        int slo=2;
        if(slope<0){
            slo=1;
        }
        float length=sqrt((endY-startY)*(endY-startY)+(endX-startX)*(endX-startX));
        for(int i=startX ;i<=endX ; i++){
            int j=startY+(i-startX)*slope;
            setColor(i+rand()%10, j+rand()%10, factor/7.0, 1-factor/7.0, factor/7.0);
        }
        float theta=rand()%360*(PI/180);
        float ftx=(endX-startX)/3+startX;
        float fty=(endY-startY)/3+startY;
        float stx=2*(endX-startX)/3+startX;
        float sty=2*(endY-startY)/3+startY;
        float end1x, end1y,end2x,end2y;
        float nextLength=length/3;
        end1x=ftx-nextLength;
        end1y=fty+pow(-1,slo)*nextLength;
        end2x=stx+nextLength;
        end2y=sty-pow(-1,slo)*nextLength;
        if(end1x>ftx){
        for(int i=ftx; i<=end1x; i++){
            int j=fty-pow(-1,slo)*(i-ftx);
            setColor(i, j, factor/7.0, 1-factor/7.0, factor/7.0) ;
        }
        }
        else{
            for(int i=end1x; i<=ftx; i++){
                int j=end1y-pow(-1,slo)*(i-end1x);
                setColor(i, j, factor/7.0, 1-factor/7.0, factor/7.0) ;
            }
        }
        if(end2x>stx){
        for(int i=stx; i<=end2x; i++){
            int j=sty-pow(-1,slo)*(i-stx);
           setColor(i, j, factor/7.0, 1-factor/7.0, factor/7.0) ;
        }
        }
        else{
            for(int i=end2x; i<=stx; i++){
                int j=end2y-pow(-1,slo)*(i-end2x);
                setColor(i, j, factor/7.0, 1-factor/7.0, factor/7.0) ;
            }
        }
        if(factor<5){
            stem(ftx,end1x,fty,end1y,factor+1);
            stem(stx,end2x,sty,end2y,factor+1);
        }
        
        
    }
    void plants(){
        /*for(int i=0; i<8 ; i++){
            stem(10*i,400,10*i,400,i%7+1);
        }*/
        /*stem(0,600,0,600,4);
        stem(0,600,600,0,1);*/
        stem(0,400,300,300,1);
        
    }
    
    // Mode 6: Bezier Curves
    void bCurve(int p1x, int p1y, int p2x, int p2y, int p3x, int p3y, int p4x, int p4y){
        for(float i=0; i<1; i+=0.01){
            // get the x
            float x=p1x*pow(1-i,3)+p2x*pow(1-i,2)*3*i+p3x*pow(i,2)*(1-i)*3+p4x*pow(i,3);
            
            // get the y
            float y=p1y*pow(1-i,3)+p2y*pow(1-i,2)*3*i+p3y*pow(i,2)*(1-i)*3+p4y*pow(i,3);
            // draw the point
            setColor(x, y, 1, 0, 0);
        }
        
    }
    void drawLine(int startx, int starty, int endx, int endy,float r, float g,float b){
        if(endx==startx){
            if(endy<starty){
                swap(endy,starty);
                
            }
            for(int i=starty; i<=endy; i++){
                setColor(startx, i, r, g, b);
            }
        }
        
        else{
            float slope=(endy-starty)/(endx-startx);
            if(endx<startx){
                swap(startx,endx);
            
            }
        
            for(int i=startx; i<=endx; i++){
                int y=startx+slope*(i-startx);
                setColor(i,y,r,g,b);
            }
        }
    }
    
    // Mode 7: Bezier Surface
    void bSurface(float Xpoints[], float Ypoints[], float Zpoints[]){
        vector<int> xps;
        vector<int> yps;
        vector<int> zps;
        // set M
        float m[]={-1,3,-3,1,3,-6,3,0,-3,3,0,0,1,0,0,0};
        Matrix M(m,4,4);
        // get the transpose of M
        Matrix MT=M.transpose();
        // set G
        Matrix GX(Xpoints,4,4);
        Matrix GY(Ypoints,4,4);
        Matrix GZ(Zpoints,4,4);
        
        for(float i=0; i<1; i+=0.01){
            // set S
            float s[]={i*i*i,i*i,i,1};
            Matrix S(s,1,4);
            
            for(float j=0; j<1; j+=0.01){
                // set T
                float t[]={j*j*j,j*j,j,1};
                Matrix T(t,4,1);
                // calculate the x
                Matrix X1=S.multiply(M);
                Matrix X2=X1.multiply(GX);
                Matrix X3=X2.multiply(MT);
                Matrix X=X3.multiply(T);
                float xp=X.get(0, 0);
                // calculate the y
                Matrix Y1=S.multiply(M);
                Matrix Y2=Y1.multiply(GY);
                Matrix Y3=Y2.multiply(MT);
                Matrix Y=Y3.multiply(T);
                 float yp=Y.get(0, 0);
                // calculate the z
                Matrix Z1=S.multiply(M);
                Matrix Z2=Z1.multiply(GZ);
                Matrix Z3=Z2.multiply(MT);
                Matrix Z=Z3.multiply(T);
                 float zp=Z.get(0, 0);
                
                setColor(xp, yp, xp/600, yp/600, zp/600);
                xps.push_back(xp);
                yps.push_back(yp);
                zps.push_back(zp);
                
            }
            xps.push_back(99999);
            yps.push_back(99999);
            zps.push_back(99999);
        }
        for(int i=0; i<xps.size()-1; i++){
            int startx=xps[i];
            int starty=yps[i];
            int startz=zps[i];
            int endx=xps[i+1];
            int endy=yps[i+1];
            int endz=zps[i+1];
            if(startx!=99999&&endx!=99999){
                //drawLine(startx, starty, endx, endy, startx/600, starty/600, startz/600);
            }
            
        }
        
    }


    
    
    
private:
    // store the color of each pixel in the texture
    vector<vector<vector<float>>> teximage;
    vector<vector<float>> noise; // the random number used for perlin noise
    vector<vector<float>> turbulance; // the turbulance used ofr perlin noise
    // 0=horizontal turbulance
    // 1=vertical turbulance
    // 2=both turbulance
    void setTurbulance(int mode){
       
        turbulance.resize(getHeight());
        
        for(int i=0; i<turbulance.size(); i++){
            
            turbulance[i].resize(getWidth());
            for(int j=0; j<turbulance[i].size(); j++){
                    //turbulance[i][j]=noise[i][j];
                if(mode==0){
                    for(int k=1; k<=getWidth(); k*=2){
                        turbulance[i][j]+=(noise[i][(j*k)%(noise[i].size())]/k);
                    }
                
                }
                else if(mode==1){
                    for(int k=1; k<=getWidth(); k*=2){
                        turbulance[i][j]+=(noise[(i*k)%(noise.size())][j]/k);
                    }
                    
                }
                else if(mode==2){
                    for(int k=1; k<=getWidth(); k*=2){
                        turbulance[i][j]+=(noise[(i*k)%(noise.size())][(j*k)%(noise[i].size())]/k);
                    }
                    
                }

                
            }
        }
        
    }
    
};





#endif
