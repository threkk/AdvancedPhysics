//
//  mobile.cpp
//  Assignment 3
//
//  Created by Alberto Martinez de Murga Ramirez, 500693826
//  and Jose Manuel Carmona Roldan, 500693836
//
//  This project tries to recreate a mobile for babies like this one http://imgur.com/XU3Alb3 It has one
//  central fixed point where the mobile is supposed to be hanging over the crib with four cables holding
//  the cross from the corners. From each corner of the cross hangs one spring with a particle to simulate
//  the ornaments.


#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include <math.h>
#include "app.h"
#include "timing.h"

// 10 particles: 1 fixed, 5 for the cross, 4 hanging in the springs.
// [0]: Fixed point in the middle.
// [1]: Central point of the cross.
// [2]: Right point of the cross.
// [3]: Left point of the cross.
// [4]: Front point of the cross.
// [5]: Rear point of the cross.
// [6]: Particle hanging at the right part.
// [7]: Particle hanging at the left part.
// [8]: Particle hanging at the front part.
// [9]: Particle hanging at the rear part.
#define PARTICLES_COUNT 10
#define ROD_COUNT 10
#define CABLE_COUNT 5
#define SPRING_COUNT 4

// Particle mass
#define PMAS 1

using namespace cyclone;

class Mobile : public MassAggregateApplication
{
    ParticleRod *rods;
    ParticleCableConstraint *cables;
    ParticleSpring *springs [SPRING_COUNT];
    
public:
    Mobile();
    virtual ~Mobile();
    
    virtual const char* getTitle();
    virtual void display();
    virtual void update();
    virtual void key(unsigned char key);
};


Mobile::Mobile() : MassAggregateApplication(PARTICLES_COUNT), cables(0), rods(0){

    // Fixed point, everthing hangs from here.
    // As it is fixed, it has no acceleration or velocity.
    particleArray[0].setPosition(0.0, 7.0, 0.0);
    particleArray[0].setVelocity(0.0, 0.0, 0.0);
    particleArray[0].setAcceleration(0.0, 0.0, 0.0);
    particleArray[0].clearAccumulator();
	
    // Normal points have velocity 0 by default but they are affected by gravity and have mass.
	// Central cross point
    particleArray[1].setPosition(0.0, 3.0, 0.0);
    particleArray[1].setVelocity(0.0, 0.0, 0.0);
    particleArray[1].setAcceleration(cyclone::Vector3::GRAVITY);
    particleArray[1].clearAccumulator();
	particleArray[1].setMass(PMAS);
    particleArray[1].setDamping(0.9f);
	
	// Corners of the cross and the particles which hang on it.
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
			particleArray[i].setPosition(3.0, 2.0, 0.0);
			break;
		case 7 :
			particleArray[i].setPosition(-3.0, 2.0, 0.0);
			break;
		case 8 :
			particleArray[i].setPosition(0.0, 2.0, 3.0);
			break;
		case 9 :
			particleArray[i].setPosition(0.0, 2.0, -3.0);
			break;

		}
        
		particleArray[i].setVelocity(0.0, 0.0, 0.0);
		particleArray[i].setAcceleration(cyclone::Vector3::GRAVITY);
		particleArray[i].clearAccumulator();
		particleArray[i].setMass(PMAS);
        particleArray[i].setDamping(0.9f);
    }

	
	// Create the rods.
	rods = new ParticleRod[ROD_COUNT];

    // The four rods which make the cross. All of them are connected to the middle.
	for (unsigned i = 0; i < 4; i++)
    {
		rods[i].particle[0] = &particleArray[1];
		rods[i].particle[1] = &particleArray[i+2];
		rods[i].length = 3.0;
		world.getContactGenerators().push_back(&rods[i]);
	}

    // Internal rods to keep the mobile integrity.
	rods[4].particle[0] = &particleArray[2];
	rods[4].particle[1] = &particleArray[3];
	rods[4].length = 6;
	world.getContactGenerators().push_back(&rods[4]);

	rods[5].particle[0] = &particleArray[4];
	rods[5].particle[1] = &particleArray[5];
	rods[5].length = 6;
	world.getContactGenerators().push_back(&rods[5]);

    // Rods from the center of the cross to the extremes.
	rods[6].particle[0] = &particleArray[2];
	rods[6].particle[1] = &particleArray[4];
	rods[6].length = sqrt(18.0);
	world.getContactGenerators().push_back(&rods[6]);

	rods[7].particle[0] = &particleArray[2];
	rods[7].particle[1] = &particleArray[5];
	rods[7].length = sqrt(18.0);
	world.getContactGenerators().push_back(&rods[7]);

	rods[8].particle[0] = &particleArray[3];
	rods[8].particle[1] = &particleArray[5];
	rods[8].length = sqrt(18.0);
	world.getContactGenerators().push_back(&rods[8]);

	rods[9].particle[0] = &particleArray[3];
	rods[9].particle[1] = &particleArray[4];
	rods[9].length = sqrt(18.0);
	world.getContactGenerators().push_back(&rods[9]);

	
	// Create the cables.
	cables = new ParticleCableConstraint[CABLE_COUNT];

    // Cables from the fixed point to the extremes of the cross.
	for (unsigned i = 0; i < 4; i++)
    {
		cables[i].particle = &particleArray[i+2];
		cables[i].anchor = Vector3(0,7,0);
		cables[i].maxLength = 5.0;
		cables[i].restitution = 0.3f;
		world.getContactGenerators().push_back(&cables[i]);
	}

    // Cable from the fixed point the center of the cross.
	cables[4].particle = &particleArray[1];
	cables[4].anchor = Vector3(0,7,0);
	cables[4].maxLength = 4.0;
	cables[4].restitution = 0.3f;
	world.getContactGenerators().push_back(&cables[4]);


	// Create the springs.
	for(unsigned i = 0; i < SPRING_COUNT; i++)
    {
        // 4 springs with different stiffness.
		springs[i] = new ParticleSpring(&particleArray[i+2],5+i*2,0.1);
        world.getForceRegistry().add(&particleArray[i+6], springs[i]);
	}
	
}

// Destructor
Mobile::~Mobile()
{
    if(rods) delete[] rods;
    if(cables) delete[] cables;
    if(springs) delete[] &springs;
}

void Mobile::update()
{
    MassAggregateApplication::update();
}


void Mobile::display()
{
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
    
	
	
    // Red springs
    // The spring is indeed a constraint so as we know the particles we just have to draw a line that shows that constraint.
    glColor3f(1, 0, 0);
    for (unsigned i = 0; i < SPRING_COUNT; i++)
    {
		glVertex3f(particleArray[2+i].getPosition().x, particleArray[2+i].getPosition().y, particleArray[2+i].getPosition().z);
        glVertex3f(particleArray[6+i].getPosition().x, particleArray[6+i].getPosition().y, particleArray[6+i].getPosition().z);

    }
	
    glEnd();
}

// Handles key input
void Mobile::key(unsigned char key)
{
	switch(key)
    {
            // Jumps a little bit the cross.
    case 'w': case 'W':
		particleArray[1].setPosition(Vector3(0,5,0));
		for(unsigned i = 2; i<6; i++)
        {
			Vector3 pos = particleArray[i].getPosition();
			pos.y = pos.y + 2.0;
			particleArray[i].setPosition(pos);
		}
        break;
            
            // Moves the cross to the left.
    case 'a': case 'A':
		for(unsigned i = 2; i<6; i++)
        {
			Vector3 pos = particleArray[i].getPosition();
			pos.x = pos.x - 1.0;
			particleArray[i].setPosition(pos);
		}
        break;
            
            // Moves the cross to the right.
    case 'd': case 'D':
		for(unsigned i = 2; i<6; i++)
        {
			Vector3 pos = particleArray[i].getPosition();
			pos.x = pos.x + 1.0;
			particleArray[i].setPosition(pos);
		}
        break;

    default:
        MassAggregateApplication::key(key);
    }
}


const char* Mobile::getTitle()
{
    return "Assignment 3 - Mobile";
}

Application* getApplication()
{
    return new Mobile();
}
