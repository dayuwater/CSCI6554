//
//  main.cpp
//  CSCI6554
//
//  Created by Tan Wang on 15/1/25.
//
//

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

#include "Matrix.h"
#include "Vector.h"
#include "Model.h"
#include "texture.h"
#include "util.h"

// vector/matrix operation modes
#define ADD 0x91
#define SUBTRACT 0x92
#define MULT 0x93
#define DIVIDE 0x94
// program parameters
#define INITIAL_WIDTH 600
#define INITIAL_HEIGHT 600
#define INITIAL_X_POS 100
#define INITIAL_Y_POS 100
#define WINDOW_NAME  "Project 5"
#define REC_LOC "/Users/dayuwater/Documents/6554/CSCI6554/Resources/"

// shading options, for now use Phong illumination model for all options
#define CONSTANT_SHADING 0x901 // each polygon has a random color
#define FAKE_GOUROUD 0x905 // each vertex has a random color, linear interpolate between verticies, basically Gouroud shading with no illumination model
#define CHANGING_SHADING 0x902 // each edge has a random color, linear interpolate between edges
#define GOUROUD_SHADING 0x903 // the base color is same as constant shading with Gouroud shading
#define PHONG_SHADING 0x904 // the base color is same as constant shading with Phong shading
#define SHADING 0x999 // under development

#define AMBIENT_RATE 0.8
#define DIFFUSE_RATE 0.8

// 2.2: Faked Gouroud Shading
// 2.3: Constant Shading with Phong illu
// 2.4: Gouroud Shading with Phong illu
// 3: Phong Shading...



using namespace std;
GLsizei window_width;
GLsizei window_height;
Model house;
Model car;
vector<Model> models; // the vector to store models
vector<Matrix> model_transforms; // store all the model transformations of models in corresponding order
vector<Matrix> res_mat; // resulting matrix for models

list<Matrix> mat_cur; // current processing matricies
list<Matrix> mat_cur2;
list<Matrix> result;  // result matricies after transformations
list<Matrix> result_model; // result matricies after model transformations
list<Matrix> result_center; // result matricies of center of polygons after model transformations
vector<Vec> vec_res; // result verticies after transformations
vector<vector<Vec>> verts_after; // store the verticies of models after model transformation
vector<vector<Vec>> cents_after; // store the center of polygons after model transformation
vector<vector<int>> front_polys; // store the front side of a model
vector<vector<int>> front_poly2; // store the front side of a model
vector<vector<vector<float>>> poly_color; // store the color of models
vector<vector<vector<float>>> vertex_color; // store the color of verticies

vector<vector<vector<int>>> front_poly; // store the front side of a model
vector<vector<int>> front_poly_num; // store the number of the front polygons

vector<vector<vector<float>>> edge_table; // the edge table used in scanline conversion
vector<vector<float>> active_edge_table; // the active edge table
vector<vector<int>> cpoly; // current processing polygons

vector<vector<float>> Zbuffer; // the z buffer
vector<vector<float>> shadowZBuffer; // the shadow z buffer
vector<vector<float>> currentshadowZBuffer; // the shadow z buffer for current state

vector<vector<float>> lights; // the parameters of lights
vector<Vec> light_pos; // the position of lights

vector<Texture> textures;
vector<vector<vector<int>>> tex_coords; // the texture coordinates in xy

int mode=5;
int shadingMode=PHONG_SHADING;



Vec viewPoint;  // from ( camera position)
Vec viewDirect; // to (p ref)
Vec viewNormal;

float near; // near clipping pane
float far; // far clipping pane
float height; // the height of the clipping pane



Matrix r; // the current viewing transformations
Matrix rinv; // the inverse viewing transformations
Matrix rb; // used for second model

Matrix l; // The "viewing transformation" for the light
Matrix linv; // the inverse of the light matrix


Vec u;
Vec v;
Vec n;

Vec ul;
Vec vl;
Vec nl;

float timecounter=0;

// playground, use this function to test algorithms
void ZBuffer1(){
    float m[]={1,2,3,4,0,5,6,7,0,0,8,9,0,0,0,10};
    Matrix a(m,4,4);
    a=a.scale(3);
    Matrix b=a.inverse();
    Matrix c=a.multiply(b);
    float d=999.987675;
    float e=666.789012;
    d=round6(d);
    e=round6(e);
    
}




void init(){
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    //gluOrtho2D(-2, 2, -2, 2);
    
    string file= REC_LOC+string("ball.d");
    
    ifstream ifs(file.c_str());
    
    
    string file2= REC_LOC+string("car.d");
    
    ifstream ifs2(file2.c_str());
    
    string file3= REC_LOC+string("ball.d");
    
    ifstream ifs3(file3.c_str());
    
    float pointx[]={10,20,30,40,600,500,400,300,100,120,140,160,480,460,440,420};
    float pointy[]={600,590,580,570,120,140,160,180,332,338,412,426,76,65,287,433};
    float pointz[]={568,432,312,213,1,1,1,1,8,8,8,8,9,9,9,9};
    
    // load the model
    //house.loadModel(ifs);
    //car.loadModel(ifs2);
    models.resize(3);
    front_poly.resize(models.size());
    res_mat.resize(models.size());
    verts_after.resize(models.size());
    cents_after.resize(models.size());
    

    
    models[0].loadModel(ifs);
    models[1].loadBModel(pointz,pointx,pointy);
    models[2].loadBModel(pointy, pointy, pointz);
    
    // initialze the vectors to store the verticies after model transformation
    for(int i=0; i<models.size(); i++){
        verts_after[i].resize(models[i].getVerts().size());
        cents_after[i].resize(models[i].getPolyCenter().size());
    }
    // initialize the viewing specifications
    
        viewDirect.set(1, 9, 1);
    viewPoint.set(9,1,9);
    viewNormal.set(1, 0, 0);
    
    near=0.1;
    far=100;
    height=0.04;
    
    // initialize the light
    int numLights=1;
    lights.resize(numLights);
    light_pos.resize(numLights);
    for(int i=0; i<numLights; i++){
        vector<float> light; // current light
        light.resize(3);
        //use white light to faciliate debugging
        light[0]=1.0;
        light[1]=1.0;
        light[2]=1.0;
        // set the light position
        Vec pos;
        pos.set(0.0000001, 0, 30);
        // add the lights to the vectors
        lights[i]=light;
        light_pos[i]=pos;
    }
    
    
    ZBuffer1();
    srand(time(NULL));
    // set up textures
    textures.resize(5);
    
    
    
    
}
// set up the model transformation of models
void modelSetup(){
    // model transform matrix for house
    float g[]={ 6, 0, 0, 0,
        0, 6, 0, 0,
        0, 0, 6, 0,
        0, 0, 0, 1};
    Matrix mg(g, 4, 4);
    
    float f[]={ 1, 0, 0, -models[0].getCenter().x(),
        0, 1, 0, -models[0].getCenter().y(),
        0, 0, 1, -models[0].getCenter().z(),
        0, 0, 0, 1};
    Matrix mf(f, 4, 4);
    //Matrix mf;
    Matrix a=mg.multiply(mf);
    model_transforms.push_back(a);
    
    // model transform matrix for house
    float g2[]={ 0.06, 0, 0, 0,
        0, 0.06, 0, 0,
        0, 0, 0.06, 0,
        0, 0, 0, 1};
    Matrix mg2(g2, 4, 4);
    
    float f2[]={ 1, 0, 0, -models[1].getCenter().x(),
        0, 1, 0, -models[1].getCenter().y(),
        0, 0, 1, -models[1].getCenter().z(),
        0, 0, 0, 1};
    //Matrix mf2;
    Matrix mf2(f2, 4, 4);
    Matrix a2=mg2.multiply(mf2);
    model_transforms.push_back(a2);

    
    // model transform matrix for car
    float e[]={0.06, 0, 0, 0,
        0, 0.06, 0, 0,
        0, 0, 0.06, 0,
        0, 0, 0, 1};
    Matrix me(e, 4, 4);
    float f3[]={ 1, 0, 0, -models[2].getCenter().x(),
        0, 1, 0, -models[2].getCenter().y(),
        0, 0, 1, -models[2].getCenter().z(),
        0, 0, 0, 1};
    Matrix mf3(f3,4,4);
    Matrix a3=me.multiply(mf3);

    model_transforms.push_back(a3);
    

    
    
}

void setUp(){
    // set up model transformations
    modelSetup();
    // clear the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // set up textures
    for(int i=0; i<textures.size(); i++){
        Texture a(window_height, window_width);
        textures[i]=a;
    }
    textures[0].pureColor(1, 0, 0);
    textures[1].swirl(2, 5, 1, 0, 0, 0, 1, 0);
    textures[2].strips(100,200);
    textures[2].perlin();
    textures[3].plants();
    float pointx[]={10,20,30,40,600,500,400,300,100,120,140,160,480,460,440,420};
    float pointy[]={600,590,580,570,120,140,160,180,332,338,412,426,76,65,287,433};
    float pointz[]={568,432,312,213,1,1,1,1,8,8,8,8,9,9,9,9};
    for(int i=0; i<16; i++){
        pointx[i]=(int)(pointx[i]+timecounter)%600;
        pointy[i]=(int)(pointy[i]+timecounter)%600;
        pointz[i]=(int)(pointz[i]+timecounter)%600;
    }
    textures[4].bSurface(pointx, pointy, pointz);
    textures[4].bSurface(pointy, pointx, pointz);
    
    
    // set up texcoords
    tex_coords.resize(3);
    for(int i=0; i<models.size(); i++){
        tex_coords[i].resize(models[i].getTexCoords().size());
        for(int j=0; j<tex_coords[i].size(); j++){
            tex_coords[i][j].resize(2);
            float theta=models[i].getTexCoords()[j][0];
            float z=models[i].getTexCoords()[j][1];
            float h=models[i].getTexCoords()[j][2];
            tex_coords[i][j][0]=textures[1].getY(theta, z, h);
            
            tex_coords[i][j][1]=textures[1].getX(theta, z, h);
          
            
            
        }
    }

    // set the color
    glColor3f(1.0,0.0,0.0);
    front_poly.resize(models.size());
    front_poly_num.resize(models.size());
    // initialize the z-buffer
    Zbuffer.resize(window_height);
    shadowZBuffer.resize(window_height);
    for(int i=0; i<Zbuffer.size(); i++){
        Zbuffer[i].resize(window_width);
        for(int j=0; j<Zbuffer[i].size(); j++){
            Zbuffer[i][j]=1.0;
        }
    }
    for(int i=0; i<shadowZBuffer.size(); i++){
        shadowZBuffer[i].resize(window_width);
        for(int j=0; j<shadowZBuffer[i].size(); j++){
            shadowZBuffer[i][j]=1.0;
        }
    }
    
    
    // set viewing position
    // x-front/back
    // z-left/right
    // y-up/down
    viewDirect.set(0, 0, 0);
    //viewDirect.set(0, 0, 0);
    viewPoint.set(30.000000001,-30.0,60.0);
    viewNormal.set(0, 0, 1);
    
    // calculate u,v,n for camera
    n=viewDirect.sub(viewPoint); // u=p ref-c  (z direction)
    n=n.normalize();
    
    u=n.cross(viewNormal);  // u=n x normal  (x direction)
    u=u.normalize();
    v=u.cross(n);  // v= u x n  (y direction)
    
    // calculate u,v,n for light
    nl=viewDirect.sub(light_pos[0]); // u=p ref-c  (z direction)
    nl=nl.normalize();
    
    ul=nl.cross(viewNormal);  // u=n x normal  (x direction)
    ul=ul.normalize();
    vl=ul.cross(nl);  // v= u x n  (y direction)
    
    // remove back faces
    for(int j=0; j<models.size(); j++){
        cpoly=models[j].getPolys();
        for(int i=0; i<cpoly.size(); i++){
            Vec view=viewPoint.sub(viewDirect);
            Vec np=models[j].getNormals()[i];
            float dp=view.dot(np);
            if(dp>0){
                front_poly[j].push_back(cpoly[i]);
                front_poly_num[j].push_back(i);
            }
        //front_polys.push_back(cpoly[i]);
        }
    
    }
    // initialize the edge table, vertex color (Gouroud Shading) and polygon color (Constant Shading) table
    edge_table.resize(window_height);
    poly_color.resize(models.size());
    vertex_color.resize(models.size());
    
    
    
    
    

    
}

void setMatrix(){
    // screen
    float scr[]={0.8333, 0, 0, 0,
        0, 0.8333, 0,0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    Matrix mscr(scr, 4,4);
    r=r.multiply(mscr);
    
    
    // perspective
    float per[]={ near/height, 0, 0, 0,
        0, near/height, 0, 0,
        0, 0, far/(far-near), -near*far/(far-near),
        0, 0, 1, 0};
    
    Matrix mper(per,4,4);
    
    r=r.multiply(mper);
    
    
    
    // viewing matrix
    float c[]={ u.x(), u.y(), u.z(), 0,
        v.x(), v.y(), v.z(), 0,
        n.x(), n.y(), n.z(), 0,
        0,  0,  0,      1     };
    Matrix mc(c,4,4);
    float d[]= {1, 0, 0, -viewPoint.x(),
        0, 1, 0, -viewPoint.y(),
        0, 0, 1, -viewPoint.z(),
        0, 0, 0, 1  };
    Matrix md(d,4,4);
    
    r=r.multiply(mc);
    r=r.multiply(md);
    
    
    
    
    // viewing matrix for light
    float cl[]={ ul.x(), ul.y(), ul.z(), 0,
        vl.x(), vl.y(), vl.z(), 0,
        nl.x(), nl.y(), nl.z(), 0,
        0,  0,  0,      1     };
    Matrix mcl(cl,4,4);
    float dl[]= {1, 0, 0, -viewPoint.x(),
        0, 1, 0, -viewPoint.y(),
        0, 0, 1, -viewPoint.z(),
        0, 0, 0, 1  };
    Matrix mdl(dl,4,4);
    l=l.multiply(mscr);
    l=l.multiply(mper);
    l=l.multiply(mcl);
    l=l.multiply(mdl);

    
    // model transformations
    
    for(int i=0; i<res_mat.size(); i++){
        res_mat[i]=r.multiply(model_transforms[i]);
    }
    
    
    //cout << "rrrr" << endl;
    //r.toString();
    rinv=r.inverse();
    float rinvv[]={0.000  , -0.399 ,-199.820 , 199.465,
        0.480  ,  0.000  ,  0.000  ,  0.000,
        0.000  ,  0.266 ,-299.730 , 299.198,
        0.000  ,  0.000  , -9.991  , 10.001};
    Matrix v(rinvv,4,4);
    
    linv=v;
    
    Matrix k= r.multiply(v);
    //cout << "llll" << endl;
    //l.toString();
    //linv=l.inverse();
    //linv.modify(1, 1, 0);
    
    
    

    
}

void vertexProcess1(int j){
   
    // take all the verticies as matricies and store them in a queue
    for(int i=0; i<models[j].getVerts().size();i++){
        float x=models[j].getVerts()[i].x();
        float y=models[j].getVerts()[i].y();
        float z=models[j].getVerts()[i].z();
        
        
        /*x=-1+2*(x-min)/(max-min);
         y=-1+2*(y-min)/(max-min);
         z=-1+2*(z-min)/(max-min);*/
        
        /*x=(x-min)/(max-min);
         y=(y-min)/(max-min);
         z=(z-min)/(max-min);*/
        
        float f[4]={x,y,z,1};
        
        Matrix m(f,4,1);
        mat_cur.push_back(m);
        
    }
    // process the center of polygons as well
    for(int i=0; i<models[j].getPolyCenter().size(); i++){
        float x=models[j].getPolyCenter()[i].x();
        float y=models[j].getPolyCenter()[i].y();
        float z=models[j].getPolyCenter()[i].z();
        float f[4]={x,y,z,1};
        
        Matrix m(f,4,1);
        mat_cur2.push_back(m);

    }
    
    
    
    // processing matricies and take the output to the resulting queue
    while(!mat_cur.empty()){
        Matrix p; // current processing matrix
        Matrix q; // store the matrix after model transformation
        p=mat_cur.front();
        mat_cur.pop_front();
        // processing
        q=p;
        p=res_mat[j].multiply(p);
        q=model_transforms[j].multiply(q);
        // process end
        result.push_back(p);
        result_model.push_back(q);
        
    }
    while(!mat_cur2.empty()){
        Matrix p; // current processing matrix
        p=mat_cur2.front();
        mat_cur2.pop_front();
        // processing
        
        
        p=model_transforms[j].multiply(p);
        // process end
        result_center.push_back(p);
        
        
    }

    
    // convert the resulting matricies back to verticies
    int k=0;
    while(!result.empty()){
        
        Matrix p;
        Matrix q;
        p=result.front();
        q=result_model.front();
        result.pop_front();
        result_model.pop_front();
        Vec v;
        float d=p.get(3,0);
        float a=p.get(0,0)/d;
        float b=p.get(1,0)/d;
        float c=p.get(2,0)/d;
        v.set(a, b, c);
        vec_res.push_back(v);
        Vec w;
        d=q.get(3,0);
        a=q.get(0,0)/d;
        b=q.get(1,0)/d;
        c=q.get(2,0)/d;
        w.set(a, b, c);
        verts_after[j][k]=w;
        k++;
        
    }
    k=0;
    while(!result_center.empty()){
        
        Matrix p;
        p=result_center.front();
        
        result_center.pop_front();
        
        Vec v;
        float d=p.get(3,0);
        float a=p.get(0,0)/d;
        float b=p.get(1,0)/d;
        float c=p.get(2,0)/d;
        v.set(a, b, c);
        
      
        cents_after[j][k]=v;
        k++;
        
    }

    glColor3f(0, 1, 0);

    
    
}
//vector<vector<float>> et;

void Shader(float r, float g, float b){
    
}

void vertexColors(int j,int mode){
    float r,g,b;
    
        vertex_color[j].resize(models[j].getVerts().size());
        for(int i=0; i<models[j].getVerts().size(); i++){
            // get the base color (Ka, Ks, Kd)
            if(mode==SHADING){
            if(i==0){
                r=1.0;
                g=0.0;
                b=0.0;
            }
            else if(i==1){
                r=0.0;
                g=1.0;
                b=0.0;
            }
            else if(i==2){
                r=0.0;
                g=0.0;
                b=1.0;
            }

            else if(i==3){
                r=1.0;
                g=1.0;
                b=0.0;
            }
            else if(i==4){
                r=0.0;
                g=1.0;
                b=1.0;
            }

            else if(i==5){
                r=1.0;
                g=0.0;
                b=1.0;
            }

            else{
                r=1.0;
                g=1.0;
                b=1.0;
            }
            }
            else if(mode==GOUROUD_SHADING||mode==PHONG_SHADING){
                r=1.0;
                g=1.0;
                b=1.0;
            }
            else if(mode==FAKE_GOUROUD){
                r=1.0-0.05*(rand()%20);
                g=1.0-0.05*(rand()%20);
                b=1.0-0.05*(rand()%20);
            }
            // use for Gouraud Shading
            float rr=0.0, rg=0.0, rb=0.0;
            // add ambient light
            for(int k=0; k<lights.size(); k++){
                rr+=r*lights[k][0]*AMBIENT_RATE;
                rg+=g*lights[k][1]*AMBIENT_RATE;
                rb+=b*lights[k][2]*AMBIENT_RATE;
            }
            Vec norm=models[j].getVertNormals()[i];
            Vec vert=verts_after[j][i];
            // add diffuse light
            for(int k=0; k<lights.size(); k++){
                float NdotL;
                Vec lit;
                lit=light_pos[k].sub(vert).normalize();
                NdotL=norm.dot(lit);
                
                NdotL= (NdotL>0 ? NdotL : 0);
                rr+=r*lights[k][0]*DIFFUSE_RATE*(NdotL);
                rg+=g*lights[k][1]*DIFFUSE_RATE*(NdotL);
                rb+=b*lights[k][2]*DIFFUSE_RATE*(NdotL);
                //assert(i!=2);
            }

            
            // add specular light
            for(int k=0; k<lights.size(); k++){
                
                Vec lit; // L
                Vec half; // H
                Vec view;
                
                
                float NdotH;
                
                
                lit=light_pos[k].sub(vert);
                view=viewPoint.sub(vert);
                half=lit.add(view).normalize();
                
                
                NdotH=norm.dot(half);
                
                NdotH= (NdotH>0 ? NdotH : 0);
                rr+=lights[k][0]*pow(NdotH,30);
                rg+=lights[k][1]*pow(NdotH,30);
                rb+=lights[k][2]*pow(NdotH,30);
                //assert(i!=2);
            }

            
            
            r=rr;
            g=rg;
            b=rb;
            vector<float> color;
            color.resize(3);
            color[0]=r;
            color[1]=g;
            color[2]=b;
            vertex_color[j][i]=color;

            
        }
    }

    


void drawVertex1(int k, int mode){
    glColor3f(1, 0, 0);
    for(int i=0; i<front_poly[k].size(); i++){
        // initialize the polygon color table
        poly_color[k].resize(front_poly[k].size());
        glBegin(GL_LINES);
        vector<int> current=front_poly[k][i];
        float cr,cg,cb; // the color of one verticies of an edge or the color of the edge
        float crn = 0.0,cgn=0.0,cbn=0.0; // the color of the other vertex of an edge
        float normx=0.0, normy=0.0, normz=0.0; // the normal of the vertex of an edge
        float normxn=0.0, normyn=0.0, normzn=0.0; // the normal of the next vertex of an edge
        int tx=0, ty=0; // the tex coordinate
        int txn=0, tyn=0; // the tex coordinate of the other vertex

        // set the initial color (ks,kd,ka)
        cr=1.0;
        cg=1.0;
        cb=1.0;
        // constant shading
        // ambient light (Ia)
        if(mode==CONSTANT_SHADING){
        float rr=0.0,rg=0.0,rb=0.0;
        Vec norm; // N
        Vec center;
        
        norm=models[k].getNormals()[front_poly_num[k][i]].normalize();
        center=cents_after[k][front_poly_num[k][i]];
        
        
        for(int j=0; j<lights.size(); j++){
            rr+=cr*lights[j][0]*AMBIENT_RATE;
            rg+=cg*lights[j][1]*AMBIENT_RATE;
            rb+=cb*lights[j][2]*AMBIENT_RATE;
        }
        // diffuse light (Id)
        for(int j=0; j<lights.size(); j++){
            float NdotL;
            Vec lit;
            lit=light_pos[j].sub(center).normalize();
            NdotL=norm.dot(lit);
            
            NdotL= (NdotL>0 ? NdotL : 0);
            rr+=cr*lights[j][0]*DIFFUSE_RATE*(NdotL);
            rg+=cg*lights[j][1]*DIFFUSE_RATE*(NdotL);
            rb+=cb*lights[j][2]*DIFFUSE_RATE*(NdotL);
            //assert(i!=2);
        }
        // specular light (Is)
        for(int j=0; j<lights.size(); j++){
            
            Vec lit; // L
            Vec half; // H
            Vec view;
            
            
            float NdotH;
            
           
            lit=light_pos[j].sub(center);
            view=viewPoint.sub(center);
            half=lit.add(view).normalize();
            
            
            NdotH=norm.dot(half);
            
            NdotH= (NdotH>0 ? NdotH : 0);
            rr+=lights[j][0]*pow(NdotH,1);
            rg+=lights[j][1]*pow(NdotH,1);
            rb+=lights[j][2]*pow(NdotH,1);
            //assert(i!=2);
        }

        
        cr=rr;
        cg=rg;
        cb=rb;
        }

        vector<float> pcolor;
        pcolor.resize(3);
        pcolor[0]=cr;
        pcolor[1]=cg;
        pcolor[2]=cb;
        poly_color[k][i]=pcolor;
        
         glColor3f(cr,cg, cb);
        // set the edges
        for(int j=0; j<current.size(); j++){
            float x=vec_res[current[j]].x();
            float y=vec_res[current[j]].y();
            float z=vec_res[current[j]].z();
            float normx=models[k].getVertNormals()[current[j]].x();
            float normy=models[k].getVertNormals()[current[j]].y();

            float normz=models[k].getVertNormals()[current[j]].z();
            
            tx=tex_coords[k][current[j]][0];
            ty=tex_coords[k][current[j]][1];
            

            if(mode!=CONSTANT_SHADING&&mode!=CHANGING_SHADING&&mode!=PHONG_SHADING){
                cr=vertex_color[k][current[j]][0];
                cg=vertex_color[k][current[j]][1];
                cb=vertex_color[k][current[j]][2];
                
                
            }
            float xn,yn,zn;
            if(j<current.size()-1){
                //glVertex3f(x/window_width*300,y/window_height*300,z/window_height);
                
                //glVertex3f(x,y,z);
                
                 xn=vec_res[current[j+1]].x();
                 yn=vec_res[current[j+1]].y();
                 zn=vec_res[current[j+1]].z();
                normxn=models[k].getVertNormals()[current[j+1]].x();
                normyn=models[k].getVertNormals()[current[j+1]].y();

                normzn=models[k].getVertNormals()[current[j+1]].z();
                txn=tex_coords[k][current[j+1]][0];
                tyn=tex_coords[k][current[j+1]][1];

                
                
                //glVertex3f(xn/window_width*300, yn/window_height*300, zn/window_height);
                if(mode!=CONSTANT_SHADING&&mode!=CHANGING_SHADING&&mode!=PHONG_SHADING){
                    crn=vertex_color[k][current[j+1]][0];
                    cgn=vertex_color[k][current[j+1]][1];
                    cbn=vertex_color[k][current[j+1]][2];
                }

                
                
                
            }
            else{
                //glVertex3f(x,y,z);
                
                xn=vec_res[current[0]].x();
                yn=vec_res[current[0]].y();
                zn=vec_res[current[0]].z();
                
                normxn=models[k].getVertNormals()[current[0]].x();
                normyn=models[k].getVertNormals()[current[0]].y();
                
                normzn=models[k].getVertNormals()[current[0]].z();
                
                txn=tex_coords[k][current[0]][0];
                tyn=tex_coords[k][current[0]][1];
                
                //glVertex3f(xn/window_width*300, yn/window_height*300, zn/window_height);
                if(mode!=CONSTANT_SHADING&&mode!=CHANGING_SHADING&&mode!=PHONG_SHADING){
                    crn=vertex_color[k][current[0]][0];
                    cgn=vertex_color[k][current[0]][1];
                    cbn=vertex_color[k][current[0]][2];
                }

                
                
            }
            //glVertex3f(xn, yn, zn);
            // put into edge table
            // set the min and max
            
            float ymin=min(y,yn);
            //(ymin < 0) ? ymin =0 : ymin=ymin;
            float ymax=max(y,yn);
            (ymax >= window_height-2) ? ymax=window_height-2 : ymax=ymax;
            float xmin=min(x,xn);
            //(xmin < 0) ? xmin =0 : xmin=xmin;
            float xmax=max(x,xn);
             (xmax >= window_width-2) ? xmax=window_width-2 : xmax=xmax;
            float zmin=min(z,zn);
            float zmax=max(z,zn);
            float crmin=min(cr,crn);
            float crmax=max(cr,crn);
            float cgmin=min(cg,cgn);
            float cgmax=max(cg,cgn);
            float cbmin=min(cb,cbn);
            float cbmax=max(cb,cbn);
            
            float nxmax=max(normx,normxn);
            float nxmin=min(normx,normxn);
            float nymax=max(normy,normyn);
            float nymin=min(normy,normyn);
            float nzmax=max(normz,normzn);
            float nzmin=min(normz,normzn);
            
            float txmin=min(tx,txn);
            float tymin=min(ty,tyn);
            float txmax=max(tx,txn);
            float tymax=max(ty,tyn);

            assert(ymax<window_height-1);
            
            // The format of edge vector:
            // ymax->xmin->dx/dy->the red color value of the bottom of the edge color->green->blue->zmin->dz/dy->model number(k)->the polygon number(i) of that object->dr/dy( how much the red color change as y increases) ->dg/dy->db/dy->x normal of the bottom of the edge->y normal    -> z normal->dxn/dy->dyn/dy->dzn/dy->txmin->dtx/dy->tymin->dty/dy
            vector<float> edge;
            
            edge.resize(23);
            edge[0]=ymax;
            edge[1]=xmin;
            edge[2]=(xn-x)/(yn-y);
            if(edge[2]<0){
                edge[1]=xmax;
            }
            edge[3]=crmin;
            edge[4]=cgmin;
            edge[5]=cbmin;
            
            edge[6]=zmin;
            edge[7]=(zn-z)/(yn-y);
            if(edge[7]<0){
                edge[6]=zmax;
            }
            edge[8]=k;
            edge[9]=i;
            edge[10]=(crn-cr)/(yn-y);
            if(edge[10]<0){
                edge[3]=crmax;
            }
            edge[11]=(cgn-cg)/(yn-y);
            if(edge[11]<0){
                edge[4]=cgmax;
            }
            edge[12]=(cbn-cb)/(yn-y);
            if(edge[12]<0){
                edge[5]=cbmax;
            }
            edge[13]=nxmin;
            edge[14]=nymin;
            edge[15]=nzmin;
            edge[16]=(normxn-normx)/(yn-y);
            if(edge[16]<0){
                edge[13]=nxmax;
            }
            edge[17]=(normyn-normy)/(yn-y);
            if(edge[17]<0){
                edge[14]=nymax;
            }

            edge[18]=(normzn-normz)/(yn-y);
            if(edge[18]<0){
                edge[15]=nzmax;
            }
            edge[19]=txmin;
            edge[21]=tymin;
            edge[20]=(txn-tx)/(yn-y);
            edge[22]=(tyn-ty)/(yn-y);
            if(edge[20]<0){
                edge[19]=txmax;
            }
            if(edge[22]<0){
                edge[21]=tymax;
            }

            

            
            
            
            // push the edge into the edge table
            int ypos=((ymin+1)/2)*window_height;
            if(ypos<window_height&&ypos>=0){
                edge_table[ypos].push_back(edge);
            }
            //et.push_back(edge);

            
        }
        
        glEnd();
    }
    
    
    
    
    vec_res.resize(0);
    

    
}

// sort the active edge table
bool compare(vector<float> a, vector<float> b){
    if(a[1]>=b[1]){
        return false;
    }
    else{
        return true;
    }
}
// check if the edge table is empty or not
bool checkEdgeTable(){
    cout << "Checking edge table" << endl;
    for(int i=0; i<edge_table.size(); i++){
        if(edge_table[i].size()!=0){
            cout << i << endl;
            return true;
        }
    }
    return false;
}
// only use for debug
void allEdges(){
    glColor3f(1.0, 0.0, 0.0);
    vector<int> rr;
    for(int i=0; i<edge_table.size(); i++){
        if(edge_table[i].size()!=0){
            rr.push_back(i);
           
        }
    }
    for(int i=0; i<rr.size(); i++){
        for(float j=-1; j<1; j+=2.0/window_width){
            glBegin(GL_POINTS);
            glVertex2d(j,-1+2.0*rr[i]/(window_height+.0));
            glEnd();
        }
    }
    //assert(1+1==3);
}
void buildEdge(int order,int mode){
    // initialize the current z buffer
    currentshadowZBuffer.resize(window_height);
    for(int i=0; i<currentshadowZBuffer.size(); i++){
        currentshadowZBuffer[i].resize(window_width);
        for(int j=0; j<currentshadowZBuffer[i].size(); j++){
            currentshadowZBuffer[i][j]=1.0;
        }
    }
    //allEdges();
    float y=0; // the y value used for drawing
    int ye=0; // the sequence number for the edge table and Z buffer
    // find the first non-empty ymin
    
    for(int i=0; i<edge_table.size(); i++){
        if(edge_table[i].size()!=0){
            ye=i;
            y=(i+.0)/window_height*2.0-1;
            break;
        }
    }
    
    active_edge_table.resize(0);
    while((active_edge_table.size()!=0||checkEdgeTable())&&ye<window_height){
        
    // push the edges to the active edge table
    for(int i=0; i<edge_table[ye].size(); i++){
        vector<float> vec=edge_table[ye][i];
        
        active_edge_table.push_back(vec);
        
    }
        //assert(ye!=199);
    edge_table[ye].resize(0);
    // sort the edges
        
    sort(active_edge_table.begin(),active_edge_table.end(),compare);
    //assert(ye!=200);
    // fill the pixels
        
        bool draw=true; // draw if true, not draw if false, odd-even rule
        
        vector<vector<float>> current_polygon;
    for(int i=0; i<active_edge_table.size()-1; i++){
        
        float rs,gs,bs; // start color
        float rf,gf,bf; // finishing color
        float startz=active_edge_table[i][6]; // start z value
        float endz=active_edge_table[i+1][6]; // end z value
        float startx=active_edge_table[i][1]; // start x value
        float endx=active_edge_table[i+1][1]; // end x value
        //assert(ye!=200||i!=5);
        float starto=active_edge_table[i][8]; // the corresponding model of the start edge(deprecated for now)
        float endo=active_edge_table[i+1][8];// t he corresponding model of the end edge(deprecated for now)
        float startxn=active_edge_table[i][13];
        float endxn=active_edge_table[i+1][13];
        float startyn=active_edge_table[i][14];
        float endyn=active_edge_table[i+1][14];
        float startzn=active_edge_table[i][15];
        float endzn=active_edge_table[i+1][15];
        
        float starttx=active_edge_table[i][19];
        float endtx=active_edge_table[i+1][19];
        float startty=active_edge_table[i][21];
        float endty=active_edge_table[i+1][21];

        // if the two edges are the same, swap them so that the edge correspound to the same
        // polygon of last edge is processed first
        if(mode==CONSTANT_SHADING||mode==PHONG_SHADING){
            if(absolute(startx-endx)<2.0/window_width&&i>0){
                if(active_edge_table[i-1][9]!=active_edge_table[i][9]){
                    if(active_edge_table[i-1][8]==active_edge_table[i][8]){
                        swap(active_edge_table[i],active_edge_table[i+1]);
                    }

                
                }
            
            }
        }
        if(mode==CONSTANT_SHADING||mode==CHANGING_SHADING||mode==PHONG_SHADING){
        // set the start color. The color is from the polygon color table
        rs=poly_color[active_edge_table[i][8]][active_edge_table[i][9]][0];
        gs=poly_color[active_edge_table[i][8]][active_edge_table[i][9]][1];
        bs=poly_color[active_edge_table[i][8]][active_edge_table[i][9]][2];
        //rs=active_edge_table[i][3];
        //gs=active_edge_table[i][4];
        //bs=active_edge_table[i][5];
        
        // set the end color
        rf=poly_color[active_edge_table[i+1][8]][active_edge_table[i+1][9]][0];
        gf=poly_color[active_edge_table[i+1][8]][active_edge_table[i+1][9]][1];
        bf=poly_color[active_edge_table[i+1][8]][active_edge_table[i+1][9]][2];
        }
        else{
            rs=active_edge_table[i][3];
            gs=active_edge_table[i][4];
            bs=active_edge_table[i][5];
            rf=active_edge_table[i+1][3];
            gf=active_edge_table[i+1][4];
            bf=active_edge_table[i+1][5];
        }
        // find the next edge that is closer to the screen than the current edge
        /*while(((endz>startz+0.0001))&&(k<active_edge_table.size()-1)){
            k++;
            

            vector<float> cur;
            cur.resize(4);
            cur[0]=rf;
            cur[1]=gf;
            cur[2]=bf;
            cur[3]=endz;
            current_polygon.push_back(cur);
            rf=active_edge_table[k][3];
            gf=active_edge_table[k][4];
            bf=active_edge_table[k][5];
            

            endo=active_edge_table[k][8];
            endz=active_edge_table[k][6];
        }*/
        
       
       
        
        //rf=active_edge_table[k][3];
       // gf=active_edge_table[k][4];
       // bf=active_edge_table[k][5];
        
        int c=0;
        bool check=false;
        //assert(ye!=200);
        if(draw){
            float j=startx;
            float t=j;
        for(j=startx; j<endx; j+=1.0/window_width){
            glBegin(GL_POINTS);
            Matrix v=r;
            float r,g,b; // the color value to be filled in currect pixel
            // set up code for shading techniques using linear interpolation
            // for now only the start color is using
            float z; // the z value to be filled in z-buffer
            z=startz+(endz-startz)/(endx-startx)*(j-startx); // linear interpolate z
            int tecx;
            int tecy;
            tecx=starttx+(endtx-starttx)/(endx-startx)*(j-startx);
            tecy=startty+(endty-startty)/(endx-startx)*(j-startx);
            if(tecx>textures[1].getHeight()){
                tecx=textures[1].getHeight();
            }
            if(tecy>textures[1].getWidth()){
                tecy=textures[1].getWidth();
            }
            if(tecx<0){
                tecx=0;
            }
            if(tecy<0){
                tecy=0;
            }
            int t=((j+1)/2)*window_width;
            // if the z value is closer, draw the pixel and set the z value in the z buffer
            //assert(ye!=200);
            // if the shadow z is closer, draw the pixel, else, don't draw the pixel
            if(z<=Zbuffer[ye][t]){
                // when constant shading is used
                if(mode==CONSTANT_SHADING){
                    glColor3f(rs,gs,bs); // draw pixel
                   float ri=textures[1].getColor(tecx, tecy)[0];
                    float gi=textures[1].getColor(tecx, tecy)[1];
                    float bi=textures[1].getColor(tecx, tecy)[2];
                    
                    glColor3f(rs*ri,gs*gi,bs*bi);
                    
                }
                // when Gouraud Shading is used
                else if(mode==GOUROUD_SHADING||mode==FAKE_GOUROUD){
                    r=rs+(rf-rs)/(endx-startx)*(j-startx);
                    g=gs+(gf-gs)/(endx-startx)*(j-startx);
                    b=bs+(bf-bs)/(endx-startx)*(j-startx);
                    glColor3f(r,g,b);
                    float ri=textures[1].getColor(tecx, tecy)[0];
                    float gi=textures[1].getColor(tecx, tecy)[1];
                    float bi=textures[1].getColor(tecx, tecy)[2];
                    glColor3f(ri*r,gi*g,bi*b);

                }
                // when Phong Shading is used
                else if(mode==PHONG_SHADING){
                    float rr=0.0, rg=0.0, rb=0.0;
                    float nx, ny, nz;
                    float ri, gi, bi;
                    
                    Vec norm; // normal
                    Vec lit; // light
                    Vec lit2;
                    Vec vp; // the point in world coordinate
                    Vec half; // H
                    Vec view; // V
                    // set the normal
                    nx=startxn+(endxn-startxn)/(endx-startx)*(j-startx);
                    ny=startyn+(endyn-startyn)/(endx-startx)*(j-startx);
                    nz=startzn+(endzn-startzn)/(endx-startx)*(j-startx);
                    norm.set(nx, ny, nz);
                    norm=norm.normalize();
                    // get the viewing point by inverse transformation
                    // j=current x in screen coord
                    // y=current y in screen coord
                    // z=current z in screen coord
                    float f[]={j,y,z,1};
                    Matrix m(f,4,1);
                    Matrix res=rinv.multiply(m);
                    float d=res.get(3,0);
                    vp.set(res.get(0,0)/d,res.get(1,0)/d,res.get(2,0)/d);
                    // set the light
                    lit=light_pos[0].sub(vp).normalize();
                    lit2=light_pos[0].sub(vp);
                    // set the view vector
                    view=viewPoint.sub(vp);
                    half=view.add(lit2).normalize();
                    float NdotL=norm.dot(lit);
                    NdotL= (NdotL>0 ? NdotL : 0);
                    float NdotH=norm.dot(half);
                    NdotH= (NdotH>0 ? NdotH : 0);
                    if(order==0){
                    ri=textures[3].getColor(tecx, tecy)[0];
                    gi=textures[3].getColor(tecx, tecy)[1];
                    bi=textures[3].getColor(tecx, tecy)[2];
                    }
                    else if(order==1){
                        ri=textures[1].getColor(tecx, tecy)[0];
                        gi=textures[1].getColor(tecx, tecy)[1];
                        bi=textures[1].getColor(tecx, tecy)[2];

                    }
                    else{
                        ri=textures[2].getColor(tecx, tecy)[0];
                        gi=textures[2].getColor(tecx, tecy)[1];
                        bi=textures[2].getColor(tecx, tecy)[2];

                    }
                    
                    
                    // add ambient
                    for(int k=0; k<lights.size(); k++){
                        rr+=ri*lights[k][0]*AMBIENT_RATE;
                        rg+=gi*lights[k][1]*AMBIENT_RATE;
                        rb+=bi*lights[k][2]*AMBIENT_RATE;
                        
                    }
                    // add diffuse
                    
                    for(int k=0; k<lights.size(); k++){
                        rr+=ri*lights[k][0]*DIFFUSE_RATE*NdotL;
                        rg+=gi*lights[k][1]*DIFFUSE_RATE*NdotL;
                        rb+=bi*lights[k][2]*DIFFUSE_RATE*NdotL;
                    }
                    // add specular
                    for(int k=0; k<lights.size(); k++){
                        rr+=lights[k][0]*pow(NdotH,30);
                        rg+=lights[k][1]*pow(NdotH,30);
                        rb+=lights[k][2]*pow(NdotH,30);
                    }

                    
                    
                    // set the final color
                    ri=rr;
                    gi=rg;
                    bi=rb;
                    r=ri;
                    g=gi;
                    b=bi;
                     glColor3f(ri,gi,bi); // draw pixel
                }
                
                
                Zbuffer[ye][t]=z; // set z
                // calculate the shadow z and store it
                // get the viewing point by inverse transformation
                // j=current x in screen coord
                // y=current y in screen coord
                // z=current z in screen coord
                glVertex3f(j, y, 0);
                
                
            }
            j=j;
            y=y;
            z=z;
            float f[]={j,y,z,1};
            Matrix m(f,4,1);
            Matrix lm;
            
            lm=l.multiply(rinv);
            
            Matrix res=lm.multiply(m);
            
            float d=res.get(3,0);
            float lz=res.get(2,0);
            float lx=res.get(0,0)/d;
            float ly=res.get(1,0)/d;
            //assert(ye!=50||t!=200);
            lz=lz/d; // current shadow z
            //assert(lz==0);
            int gx=((lx+1)/2)*window_width;
            int gy=((ly+1)/2)*window_height;
            (gx >= window_width)? gx=window_width-1 : gx=gx;
            (gy >= window_height) ? gy=window_height-1 : gy=gy;
            if(shadowZBuffer[gy][gx]>=lz){
                if(shadowZBuffer[gy][gx]<1){
                    //glColor3f(r*AMBIENT_RATE, g*AMBIENT_RATE, b*AMBIENT_RATE);
                    //glVertex3f(lx, ly, 0);
                }
                shadowZBuffer[gy][gx]=lz;
                
                
            }
            else{
                /*glColor3f(r*AMBIENT_RATE, g*AMBIENT_RATE, b*AMBIENT_RATE);
                glVertex3f(lx, ly, 0);*/
            }
            
            // assert(ye!=260||i!=0);
            //glColor3f(rs,gs,bs); // draw pixel
            
            glEnd();
            
        }
            /*if(t==j){
                c++;
                check=false;
            }
            else{
                check=true;
            }*/
            
        }
        if(c<2){
            draw=!draw;
        }
        else{
            if(check==true){
                c=0;
                draw=!draw;
            }
        }
        
        
        
        
        
        
    }
    
    // remove the edges where y=ymax
    int count=0;
    for(int i=0; i<active_edge_table.size(); i++){
        if(y>=active_edge_table[i][0]-2.0/window_height){
            active_edge_table[i][1]=999999;
            count++;
        }
    }
    sort(active_edge_table.begin(),active_edge_table.end(),compare);
        
    active_edge_table.resize(active_edge_table.size()-count);
    // increment all the variables
    for(int i=0; i<active_edge_table.size(); i++){
        active_edge_table[i][1]+=active_edge_table[i][2]*(2.0/window_height);
        active_edge_table[i][6]+=active_edge_table[i][7]*(2.0/window_height);
        active_edge_table[i][3]+=active_edge_table[i][10]*(2.0/window_height);
        active_edge_table[i][4]+=active_edge_table[i][11]*(2.0/window_height);
         active_edge_table[i][5]+=active_edge_table[i][12]*(2.0/window_height);
        active_edge_table[i][13]+=active_edge_table[i][16]*(2.0/window_height);
        active_edge_table[i][14]+=active_edge_table[i][17]*(2.0/window_height);
        active_edge_table[i][15]+=active_edge_table[i][18]*(2.0/window_height);
        // 19 21
        active_edge_table[i][19]+=active_edge_table[i][20]*(2.0/window_height);
        active_edge_table[i][21]+=active_edge_table[i][22]*(2.0/window_height);
        
        
    }
    // sort AET again
    sort(active_edge_table.begin(),active_edge_table.end(),compare);
    // increment y by 1 pixel
    y+=2.0/window_height;
    ye+=1;
        
        
    }
    
    
}

void shadow(){
    for(int i=0; i<shadowZBuffer.size(); i++){
        for(int j=0; j<shadowZBuffer[i].size(); j++){
            if(currentshadowZBuffer[i][j]<=shadowZBuffer[i][j]){
                shadowZBuffer[i][j]=currentshadowZBuffer[i][j];
                
            }
            else if(currentshadowZBuffer[i][j]!=1){
                assert(false);
                float gx=(j+.0)/window_width*2.0-1;;
                float gy=(i+.0)/window_height*2.0-1;
                glBegin(GL_POINTS);
                glColor3f(0, 0, 0);
                glVertex3f(gx, gy, 0);
                glEnd();
                
            }
        }
    }
    currentshadowZBuffer.resize(0);
    
}

// reset the temporary values for refreshing the buffer
void rinseAndRepeat(){
    r=Matrix();
    rb=Matrix();
    l=Matrix();
    u=Vec();
    v=Vec();
    n=Vec();
    ul=Vec();
    vl=Vec();
    nl=Vec();
    edge_table.resize(0);
    //et.resize(0);
    poly_color.resize(0);
    front_poly.resize(0);
    front_poly_num.resize(0);
    Zbuffer.resize(0);
    shadowZBuffer.resize(0);
    vertex_color.resize(0);
}

void draw_scene(void){
    
    // Step 1: Set up
    setUp();
    // Step 2: Set the Matrices
   
    if(mode==0){
        setMatrix();
    for(int i=0; i<models.size(); i++){
    // Step 3: Vertex Processing
        vertexProcess1(i);
    // Step: Generate Vertex Colors
        vertexColors(i,shadingMode);
    // Step 4: drawing the resulting verticies and build edge table
        
        drawVertex1(i, shadingMode);
        // Step 5: Scan line conversion
        buildEdge(i,shadingMode);
        // Step 6: Apply shadow Z-buffer Algorithm
        //shadow();
        
    }
        
    }
    else if(mode==1){
        for(int i=0; i<window_height; i++){
            for(int j=0; j<window_width; j++){
                glBegin(GL_POINTS);
                vector<float> c=textures[0].getColor(i,j);
                
                glColor3f(c[0], c[1], c[2]);
                glVertex2d(-1+2*(i+.0)/window_height,-1+2*(j+.0)/window_width );
                glEnd();
            }
        }

        
    }
    
    else if(mode==2){
        for(int i=0; i<window_height; i++){
            for(int j=0; j<window_width; j++){
                glBegin(GL_POINTS);
                vector<float> c=textures[1].getColor(i,j);
                
                glColor3f(c[0], c[1], c[2]);
                glVertex2d(-1+2*(i+.0)/window_height,-1+2*(j+.0)/window_width );
                glEnd();
            }
        }
        
        
    }
    else if(mode==4){
        for(int i=0; i<window_height; i++){
            for(int j=0; j<window_width; j++){
                glBegin(GL_POINTS);
                vector<float> c=textures[2].getColor(i,j);
                
                glColor3f(c[0], c[1], c[2]);
                glVertex2d(-1+2*(i+.0)/window_height,-1+2*(j+.0)/window_width );
                glEnd();
            }
        }
        
        
    }
    else if(mode==5){
        for(int i=0; i<window_height; i++){
            for(int j=0; j<window_width; j++){
                glBegin(GL_POINTS);
                vector<float> c=textures[3].getColor(i,j);
                
                glColor3f(c[0], c[1], c[2]);
                glVertex2d(-1+2*(i+.0)/window_height,-1+2*(j+.0)/window_width );
                glEnd();
            }
        }
        
        
    }
    else if(mode==6){
        for(int i=0; i<window_height; i++){
            for(int j=0; j<window_width; j++){
                glBegin(GL_POINTS);
                vector<float> c=textures[4].getColor(i,j);
                
                glColor3f(c[0], c[1], c[2]);
                //glVertex2d(-1+2*(i+.0)/window_height,-1+2*(j+.0)/window_width );
                glVertex2d(-1+2*(j+.0)/window_width,-1+2*(i+.0)/window_height );
                glEnd();
            }
        }
        
        
    }



    else if(mode==3){
        for(int z=-300; z<300; z++){
            for(float theta=0; theta<6.28; theta+=0.001){
                glBegin(GL_POINTS);
                vector<float> c=textures[1].getColor(theta,z,600);
                
                glColor3f(c[0], c[1], c[2]);
                float x=window_height/2+z/600.0*window_height;

                float y=theta/2.0/PI*window_width;
                glVertex2d(-1+2*(x+.0)/window_height,-1+2*(y+.0)/window_width );
                glEnd();
            }
        }
        
        
    }


    // Step 6: Scan line conversion and painting (deprecated for now)
       // Step 7: Z-buffer (deprecated for now, currently use as playground)
    
    // test for point drawing used in scan line conversion
    
    /*glBegin(GL_POINTS);
    
    for(float i=-120.0; i<150.0; i++){
        glColor3f((i+120)/270, 0, 1-(i+120)/270);
        glVertex2f(300/window_width, i/window_height);
    }
    glEnd();*/

    
    
    /*
    // draw x, y,z-axis
    float fxp[4]={1,0,0,1};
    Matrix mfxp(fxp,4,1);
    float fxn[4]={-1,0,0,1};
    Matrix mfxn(fxn,4,1);
    
    float fyp[4]={0,1,0,1};
    Matrix mfyp(fyp,4,1);
    
    float fyn[4]={0,-1,0,1};
    Matrix mfyn(fyn,4,1);
    
    float fzp[4]={0,0,1,1};
    Matrix mfzp(fzp,4,1);
    
    float fzn[4]={0,0,-1,1};
    Matrix mfzn(fzn,4,1);

    
    
    // process the axis
    mfxn=r.multiply(mfxn);
    mfxp=r.multiply(mfxp);
    mfyn=r.multiply(mfyn);
    mfyp=r.multiply(mfyp);
    mfzn=r.multiply(mfzn);
    mfzp=r.multiply(mfzp);
    
    // draw the axis
    glBegin(GL_LINES);
    glColor3f(0, 1, 0); // x=green
    glVertex2f(mfxn.get(0, 0)/mfxn.get(3, 0), mfxn.get(1, 0)/mfxn.get(3, 0));
    
    glVertex2f(mfxp.get(0, 0)/mfxp.get(3, 0), mfxp.get(1, 0)/mfxp.get(3, 0));
    glColor3f(1, 1, 0); // y= yellow
    glVertex2f(mfyn.get(0, 0)/mfyn.get(3, 0), mfyn.get(1, 0)/mfyn.get(3, 0));
    glVertex2f(mfyp.get(0, 0)/mfyp.get(3, 0), mfyp.get(1, 0)/mfyp.get(3, 0));
    glColor3f(0, 1, 1); // z= cyan
    glVertex2f(mfzn.get(0, 0)/mfzn.get(3, 0), mfzn.get(1, 0)/mfzn.get(3, 0));
    glVertex2f(mfzp.get(0, 0)/mfzp.get(3, 0), mfzp.get(1, 0)/mfzp.get(3, 0));
   */
    
    // Final Step: Clean up and ready for next display
    rinseAndRepeat();
    glutSwapBuffers();
    
}

void resize_scene(GLsizei width, GLsizei height){
    
    glViewport(0, 0, width, height);
    window_width=width;
    window_height=height;
    
    
}

void update(){
    glutPostRedisplay();
    timecounter+=10;
    //cout << timecounter << endl;
    
}

void keyboard(unsigned char key, int x, int y){
    if(key=='1'){
        mode=1;
        
    }
    else if(key=='v'){
        mode=0;
    }
    else if(key=='2'){
        mode=2;
    }
    else if(key=='3'){
        mode=3;
    }
    else if(key=='4'){
        mode=4;
    }
    else if(key=='5'){
        mode=5;
    }
    else if(key=='6'){
        mode=6;
    }
    else if(key=='c'){
        shadingMode=CONSTANT_SHADING;
    }
    else if(key=='g'){
        shadingMode=GOUROUD_SHADING;
        
    }
    else if(key=='p'){
        shadingMode=PHONG_SHADING;
    }

    
}

void handle_mouse_click(int button, int state, int x, int y){
    
}

void handle_mouse_motion(int x, int y){
    
}




int main(int argc, char ** argv) {
    // insert code here...
    
    
    glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA|GLUT_ALPHA|GLUT_DEPTH);
    
    glutInitWindowSize(INITIAL_WIDTH, INITIAL_HEIGHT);
    glutInitWindowPosition(INITIAL_X_POS, INITIAL_Y_POS);
    glutCreateWindow(WINDOW_NAME);
    
    glutDisplayFunc(draw_scene);
    glutReshapeFunc(resize_scene);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(update);
    glutMouseFunc(handle_mouse_click);
    glutMotionFunc(handle_mouse_motion);
    init();
    glutMainLoop();
    
    return 0;
}
