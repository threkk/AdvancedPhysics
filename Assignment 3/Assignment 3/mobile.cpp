//
//  mobile.cpp
//  Assignment 3
//
//  Created by Alberto Mtnez on 05/03/14.
//  Copyright (c) 2014 Alberto Mtnez. All rights reserved.
//

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"

#include <stdio.h>


#define ROD_COUNT 4
#define SPRING_COUNT 4

using namespace cyclone;

class Mobile : public MassAggregateApplication
{
    ParticleRod *rods;
    
    
public:
    Mobile();
    virtual ~Mobile();
    
    virtual const char* getTitle();
    virtual void display();
    virtual void update();
    virtual void key(unsigned char key);
};


Mobile::Mobile() : MassAggregateApplication(5){
    
    rods = new ParticleRod[ROD_COUNT];
    
    particles[0]
    for (unsigned i = 1; i < 5; i++) {
        particles[i]
    }
    
}

Mobile::~Mobile(){
    if(rods) delete[] rods;
}

void Mobile::display()
{
    MassAggregateApplication::display();
    
    glBegin(GL_LINES);
    glColor3f(0,0,1);
    
    const Vector3 &p = parts->getPosition();
    
    glVertex3f(p.x,p.y,p.z);
    
/*    for (unsigned i = 0; i < ROD_COUNT; i++)
    {
        cyclone::Particle **particles = rods[i].particle;
        const cyclone::Vector3 &p0 = particles[0]->getPosition();
        const cyclone::Vector3 &p1 = particles[1]->getPosition();
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
    
    glColor3f(0,1,0);
    for (unsigned i = 0; i < CABLE_COUNT; i++)
    {
        cyclone::Particle **particles = cables[i].particle;
        const cyclone::Vector3 &p0 = particles[0]->getPosition();
        const cyclone::Vector3 &p1 = particles[1]->getPosition();
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
    
    glColor3f(0.7f, 0.7f, 0.7f);
    for (unsigned i = 0; i < SUPPORT_COUNT; i++)
    {
        const cyclone::Vector3 &p0 = supports[i].particle->getPosition();
        const cyclone::Vector3 &p1 = supports[i].anchor;
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
 */
    glEnd();
    
    glColor3f(1,0,0);
    glPushMatrix();
//  glTranslatef(massDisplayPos.x, massDisplayPos.y+0.25f, massDisplayPos.z);
    glutSolidSphere(0.25f, 20, 10);
    glPopMatrix();
}

void Mobile::update()
{
    MassAggregateApplication::update();
    
}

const char* Mobile::getTitle()
{
    return "Assignment 3 > Mobile";
}

void Mobile::key(unsigned char key)
{
    switch(key)
    {

            
        default:
            MassAggregateApplication::key(key);
    }
}


Application* getApplication()
{
    return new Mobile();
}