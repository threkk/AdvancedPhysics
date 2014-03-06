//
//  mobile.cpp
//  Assignment 3
//
//  Created by Alberto Mtnez on 05/03/14.
//  Copyright (c) 2014 Alberto Mtnez. All rights reserved.
//

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../../app.h"
#include "../../timing.h"

#include <stdio.h>

/*
 * 10 particles: 1 fixed, 5 for the cross, 4 hanging in the springs.
 */
#define PARTICLES_COUNT 10
#define ROD_COUNT 6
#define CABLE_COUNT 5
#define SPRING_COUNT 4

#define PMAS 4

using namespace cyclone;

class Mobile : public MassAggregateApplication
{
    ParticleRod *rods;
    ParticleCableConstraint *cables;
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
// Constructor
Mobile::Mobile() : MassAggregateApplication(PARTICLES_COUNT), cables(0), rods(0), springs(0){


    // Fixed point, everthing hangs from here.
    particleArray[0].setPosition(0.0, 7.0, 0.0);
    particleArray[0].setVelocity(0.0, 0.0, 0.0);
    particleArray[0].setAcceleration(0.0, 0.0, 0.0);
    particleArray[0].clearAccumulator();
	
	// Central cross point
    particleArray[1].setPosition(0.0, 3.0, 0.0);
    particleArray[1].setVelocity(0.0, 0.0, 0.0);
    particleArray[1].setAcceleration(cyclone::Vector3::GRAVITY);
    particleArray[1].clearAccumulator();
	particleArray[1].setMass(PMAS);
    particleArray[1].setDamping(0.9f);

	
	// Corners of the cross
	for (int i = 2; i < 10; i++)
    {
		switch(i){

		case 2 :
			particleArray[i].setPosition(3.0, 4.0, 0.0);
			break;
		case 3 :
			particleArray[i].setPosition(-3.0, 4.0, 0.0);
			break;
		case 4 :
			particleArray[i].setPosition(0.0, 4.0, 3.0);
			break;
		case 5 :
			particleArray[i].setPosition(0.0, 4.0, -3.0);
			break;
		case 6 :
			particleArray[i].setPosition(3.0, 0.0, 0.0);
			break;
		case 7 :
			particleArray[i].setPosition(-3.0, 0.0, 0.0);
			break;
		case 8 :
			particleArray[i].setPosition(0.0, 0.0, 3.0);
			break;
		case 9 :
			particleArray[i].setPosition(0.0, 0.0, -3.0);
			break;

		}
		particleArray[i].setVelocity(0.0, 0.0, 0.0);
		particleArray[i].setAcceleration(cyclone::Vector3::GRAVITY);
		particleArray[i].clearAccumulator();
		particleArray[i].setMass(PMAS);
        particleArray[i].setDamping(0.9f);
    }

	
	// Create the rods
	rods = new ParticleRod[ROD_COUNT];

	for (unsigned i = 0; i < 4; i++){
		rods[i].particle[0] = &particleArray[1];
		rods[i].particle[1] = &particleArray[i+2];
		rods[i].length = 3;
		world.getContactGenerators().push_back(&rods[i]);
	}

	rods[4].particle[0] = &particleArray[2];
	rods[4].particle[1] = &particleArray[3];
	rods[4].length = 6;
	world.getContactGenerators().push_back(&rods[4]);

	rods[5].particle[0] = &particleArray[4];
	rods[5].particle[1] = &particleArray[5];
	rods[5].length = 6;
	world.getContactGenerators().push_back(&rods[5]);

	
	// Create the cables
	cables = new ParticleCableConstraint[CABLE_COUNT];

	for (unsigned i = 0; i < 4; i++){
		cables[i].particle = &particleArray[i+2];
		cables[i].anchor = Vector3(0,7,0);
		cables[i].maxLength = 5.0;
		cables[i].restitution = 0.3f;
		world.getContactGenerators().push_back(&cables[i]);
	}

	cables[4].particle = &particleArray[1];
	cables[4].anchor = Vector3(0,7,0);
	cables[4].maxLength = 4.0;
	cables[4].restitution = 0.3f;
	world.getContactGenerators().push_back(&cables[4]);


	// Create the springs
	springs = new ParticleAnchoredSpring[SPRING_COUNT];

	
    
}

// Destructor
Mobile::~Mobile()
{
    if(rods) delete[] rods;
    if(cables) delete[] cables;
    if(springs) delete[] springs;
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

		const cyclone::Vector3 &p0 = cables[i].particle->getPosition();
        const cyclone::Vector3 &p1 = cables[i].anchor;
        glVertex3f(p0.x, p0.y, p0.z);
        glVertex3f(p1.x, p1.y, p1.z);
    }
    
	
	/*
    // Red springs
    glColor3f(1, 0, 0);
    for (unsigned i = 0; i < SPRING_COUNT; i++)
    {
        const Vector3 *p = springs[i].getAnchor();
        glVertex3f(p->x, p->y, p->z);
    }
	*/
 
    glEnd();
}



//TODO: Define key input.
// Handles key input
void Mobile::key(unsigned char key)
{
	switch(key)
    {
    case 'w': case 'W':
        
        break;
    case 'a': case 'A':
        
        break;
    case 'd': case 'D':
        
        break;

    default:
        MassAggregateApplication::key(key);
    }
}


const char* Mobile::getTitle()
{
    return "Assignment 3 > Mobile";
}

Application* getApplication()
{
    return new Mobile();
}
