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

/*
 * 10 particles: 1 fixed, 5 for the cross, 4 hanging in the springs.
 */
#define PARTICLES_COUNT 10
#define ROD_COUNT 4
#define CABLE_COUNT 4
#define SPRING_COUNT 4

using namespace cyclone;

class Mobile : public MassAggregateApplication
{
    ParticleRod *rods;
    ParticleCable *cables;
    ParticleAnchoredSpring *springs;
    
public:
    Mobile();
    virtual ~Mobile();
    
    virtual const char* getTitle();
    virtual void display();
    virtual void update();
    virtual void key(unsigned char key);
};


// TODO: Crear la figura.
Mobile::Mobile() : MassAggregateApplication(PARTICLES_COUNT){
    
    rods = new ParticleRod[ROD_COUNT];
    cables = new ParticleCable[CABLE_COUNT];
    springs = new ParticleAnchoredSpring[SPRING_COUNT];
    
    // Fixed point, everthing hangs from here.
    particleArray[0].setPosition(0.0f, 0.0f, 0.0f);
    particleArray[0].setVelocity(0.0f, 0.0f, 0.0f);
    particleArray[0].setAcceleration(0.0f, 0.0f, 0.0f);
    particleArray[0].clearAccumulator();
    
    
}

//TODO: Decidir que se mueve.
void Mobile::key(unsigned char key)
{
    switch(key)
    {
        default:
            MassAggregateApplication::key(key);
    }
}

// TODO: Añadir las fuerzas. No las estamos teniendo en cuenta hasta ahora.
void Mobile::update()
{
    MassAggregateApplication::update();
    
}


void Mobile::display()
{
    
    /*
     * Es posible que haya que cambiar esto mas adelante cuando veamos exactamente como se comportan
     * los muelles y como funcionan las fuerzas.
     */
    MassAggregateApplication::display();

    glBegin(GL_LINES);
    
    // Blue rods
    glColor3f(0,0,1);
    for (unsigned i = 0; i < ROD_COUNT; i++)
    {
        Particle **particles = rods[i].particle;
        const Vector3 &p0 = particles[0]->getPosition();
        const Vector3 &p1 = particles[1]->getPosition();
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
    
    // Green cables
    glColor3f(0,1,0);
    for (unsigned i = 0; i < CABLE_COUNT; i++)
    {
        Particle **particles = cables[i].particle;
        const Vector3 &p0 = particles[0]->getPosition();
        const Vector3 &p1 = particles[1]->getPosition();
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
    
    // Red springs
    glColor3f(1, 0, 0);
    for (unsigned i = 0; i < SPRING_COUNT; i++)
    {
        const Vector3 *p = springs[i].getAnchor();
        glVertex3f(p->x, p->y, p->z);
    }
 
    glEnd();
}

Mobile::~Mobile()
{
    if(rods) delete[] rods;
    if(cables) delete[] cables;
    if(springs) delete[] springs;
}

const char* Mobile::getTitle()
{
    return "Assignment 3 > Mobile";
}

Application* getApplication()
{
    return new Mobile();
}