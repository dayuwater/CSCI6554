//
//  Model.h
//  CSCI6554
//
//  Created by Tan Wang on 15/1/31.
//  
//

#ifndef CSCI6554_Model_h
#define CSCI6554_Model_h

#include <iostream>
#include <vector>
#include "Vector.h"
#include "util.h"
#define PI 3.14159265358979


class Model {
public:
    Model(){
        for(int i=0; i<3; i++){
            maxVert.set(-9999999,-9999999,-9999999);
        }
        for(int i=0; i<3; i++){
            minVert.set(9999999,9999999,9999999);
        }

       
    }
    // generate a besier surface
    void loadBModel(float Xpoints[], float Ypoints[], float Zpoints[]){
        // generate the points
       
       
        // set M
        float m[]={-1,3,-3,1,3,-6,3,0,-3,3,0,0,1,0,0,0};
        Matrix M(m,4,4);
        // get the transpose of M
        Matrix MT=M.transpose();
        // set G
        Matrix GX(Xpoints,4,4);
        Matrix GY(Ypoints,4,4);
        Matrix GZ(Zpoints,4,4);
        
        for(float i=0; i<1; i+=0.1){
            // set S
            float s[]={i*i*i,i*i,i,1};
            Matrix S(s,1,4);
            
            for(float j=0; j<1; j+=0.1){
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
                
                
                Vec vert;
                vert.set(xp, yp, zp);
                verts.push_back(vert);
                
                // find the maximum vertex
                (xp>maxVert.x()) ? maxVert.setX(xp) : placeHolder();
                (yp>maxVert.y()) ? maxVert.setY(yp) : placeHolder();
                (zp>maxVert.z()) ? maxVert.setZ(zp) : placeHolder();
                // find the minimum vertex
                (xp<minVert.x()) ? minVert.setX(xp) : placeHolder();
                (yp<minVert.y()) ? minVert.setY(yp) : placeHolder();
                (zp<minVert.z()) ? minVert.setZ(zp) : placeHolder();
                
            }
            
        }
        
        polys.resize(81);
        
        poly_center.resize(polys.size());
        surround_polys.resize(verts.size());
        int v=0;
        
        for(int j=0; j<polys.size(); j++){
            if(v%10==9){
                v++;
            }
            vector<int> poly;
            int s=4;
            
            poly.resize(s);
            Vec center;
            float minX=9999999;
            float minY=9999999;
            float minZ=9999999;
            float maxX=-9999999;
            float maxY=-9999999;
            float maxZ=-9999999;
            
            for(int k=0; k<s; k++){
                int a;
                if(k==0){
                    a=v+1;
                }
                else if(k==1){
                    a=v+2;
                    
                }
                else if(k==2){
                    a=v+12;
                    
                }
                else if(k==3){
                    a=v+11;
                    
                }
                //input >> a;
                poly[k]=a-1;
                surround_polys[a-1].push_back(j);
                (verts[a-1].x()<minX ? minX=verts[a-1].x() : 0 );
                (verts[a-1].y()<minY ? minY=verts[a-1].y() : 0 );
                (verts[a-1].z()<minZ ? minZ=verts[a-1].z() : 0 );
                (verts[a-1].x()>maxX ? maxX=verts[a-1].x() : 0 );
                (verts[a-1].y()>maxY ? maxY=verts[a-1].y() : 0 );
                (verts[a-1].z()>maxZ ? maxZ=verts[a-1].z() : 0 );
                center.set((minX+maxX)/2, (minY+maxY)/2,(minZ+maxZ)/2);
                
            }
            poly_center[j]=center;
            polys[j]=poly;
            v++;
        }
        
       
        
        setNormals();
        setCenter();
        setVertexNormals();
        setTexCoords();
        
        
        
    }

    void loadModel(istream& input){
        int vertex_count=0;
        int polys_count=0;
        string data;
        
        input >> data >> vertex_count >> polys_count;
        
        verts.resize(vertex_count);
        polys.resize(polys_count);
        poly_center.resize(polys_count);
        surround_polys.resize(vertex_count);
        
        for(int i=0; i<vertex_count; i++){
            Vec v;
            float a,b,c;
            input >> a >> b >> c;
            // find the maximum vertex
            (a>maxVert.x()) ? maxVert.setX(a) : placeHolder();
            (b>maxVert.y()) ? maxVert.setY(b) : placeHolder();
            (c>maxVert.z()) ? maxVert.setZ(c) : placeHolder();
            // find the minimum vertex
            (a<minVert.x()) ? minVert.setX(a) : placeHolder();
            (b<minVert.y()) ? minVert.setY(b) : placeHolder();
            (c<minVert.z()) ? minVert.setZ(c) : placeHolder();
            

            v.set(a, b, c);
            verts[i]=v;
            
        }
        
        for(int j=0; j<polys_count; j++){
            vector<int> poly;
            int s;
            input >> s;
            poly.resize(s);
            Vec center;
            float minX=9999999;
            float minY=9999999;
            float minZ=9999999;
            float maxX=-9999999;
            float maxY=-9999999;
            float maxZ=-9999999;
            for(int k=0; k<s; k++){
                int a;
                input >> a;
                poly[k]=a-1;
                surround_polys[a-1].push_back(j);
                (verts[a-1].x()<minX ? minX=verts[a-1].x() : 0 );
                (verts[a-1].y()<minY ? minY=verts[a-1].y() : 0 );
                (verts[a-1].z()<minZ ? minZ=verts[a-1].z() : 0 );
                (verts[a-1].x()>maxX ? maxX=verts[a-1].x() : 0 );
                (verts[a-1].y()>maxY ? maxY=verts[a-1].y() : 0 );
                (verts[a-1].z()>maxZ ? maxZ=verts[a-1].z() : 0 );
                center.set((minX+maxX)/2, (minY+maxY)/2,(minZ+maxZ)/2);
                
            }
            poly_center[j]=center;
            polys[j]=poly;
        }
        
        cout << vertex_count << endl;
        cout << polys_count << endl;
        
        setNormals();
        setCenter();
        setVertexNormals();
        setTexCoords();
        
        
        
    }
    
    void placeHolder(){
        
    }
    void setCenter(){
        center.set((getMaxVert().x()+getMinVert().x())/2, (getMaxVert().y()+getMinVert().y())/2, (getMaxVert().z()+getMinVert().z())/2);
    }
    
    vector<Vec> getVerts(){
        return verts;
    }
    
    vector<vector<int>> getPolys(){
        return polys;
    }
    
    void setNormals(){
        normals.resize(polys.size());
        for(int i=0; i<polys.size(); i++){
            Vec a=verts[polys[i][2]].sub(verts[polys[i][1]]);
            Vec b=verts[polys[i][1]].sub(verts[polys[i][0]]);
            normals[i]=a.cross(b);
            
        }
    }
    void setVertexNormals(){
        vertex_normal.resize(verts.size());
        for(int i=0; i<verts.size(); i++){
            Vec sum; // the sum of the normals of surrounding polygons
            for(int j=0; j<surround_polys[i].size(); j++){
                sum=sum.add(normals[surround_polys[i][j]]);
                
            }
            Vec avg=sum.normalize(); // the average normals
            vertex_normal[i]=avg;
        }
        
    }
    
    void setTexCoords(){
        tex_coords.resize(verts.size());
        
        // set the radius of the intermediate cylinder texture mapping
        float r=1.5*(getMaxVert().sub(getCenter())).length();
        // set h
        // ball=15, house=3
        float h=15*getMaxVert().z();

        for(int i=0; i<verts.size(); i++){
            // theta->z->h
            tex_coords[i].resize(3);
            // nxt+a=rcos(d) ->x
            // nyt+b=rsin(d) ->y
            // nzt+c=z    ->z
            Vec v=getVertNormals()[i];
            float nx=v.x(); // x vertex normal of the vertex
            float ny=v.y();
            float nz=v.z();
            float a=getVerts()[i].x();
            float b=getVerts()[i].y();
            float c=getVerts()[i].z();
            
            // x^2+y^2=r^2
            // ((nx)t+a)^2+((ny)t+b)^2-r^2=0
            float factora=nx*nx+ny*ny;
            float factorb=2*a*nx+2*b*ny;
            float factorc=a*a+b*b-r*r;
            // solve the equation, get the t
            vector<float> result=solveX2(factora, factorb, factorc);
            float t=(result[0]>0) ? result[0] : result[1] ;
            // get theta (d)
            float d=atan((ny*t+b)/(nx*t+a));
            (d <0)? d+=2*PI : d;
            // get z
            float z=nz*t+c;
            // set the tex coord
            tex_coords[i][0]=d;
            tex_coords[i][1]=z;
            tex_coords[i][2]=h;
            
            
            
            
            
            
            
            
           
            
            
            
            

            
        }
        
        
        
    }
    vector<vector<float>> getTexCoords(){
        return tex_coords;
    }
    
    vector<Vec> getNormals(){
        return normals;
    }
    
    vector<Vec> getVertNormals(){
        return vertex_normal;
    }
    
    Vec getMaxVert(){
        return maxVert;
    }
    
    Vec getMinVert(){
        return minVert;
    }
    Vec getCenter(){
        return center;
    }
    vector<Vec> getPolyCenter(){
        return poly_center;
    }
    vector<vector<int>> getSur(){
        return surround_polys;
    }
    
    
protected:
    vector<Vec> verts;  // Vertex table
    vector<vector<int>> polys;  // Polygon table
    vector<Vec> normals; // the normals of the polygons
    vector<Vec> vertex_normal; // the normal of the verticies
    vector<vector<int>> surround_polys; // the surrounding polygons of verticies
    vector<Vec> poly_center; // the center of each polygon
    vector<vector<float>> tex_coords; // the texture coordinates of each vertex
    Vec maxVert;
    Vec minVert;
    Vec center;
    
    
};

#endif
