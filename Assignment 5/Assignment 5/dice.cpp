//
//  dice.cpp
//  Assignment 5
//
//  Created by Alberto Martinez de Murga Ramirez, 500693826
//  and Jose Manuel Carmona Roldan, 500693836

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"

#include <stdio.h>


class Die : public cyclone::CollisionBox
{
    
public:
    Die()
    {
        body = new cyclone::RigidBody;
    }

    ~Die()
    {
        delete body;
    }

    void render()
    {
        GLfloat mat[16];
        body->getGLTransform(mat);

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x*2, halfSize.y*2, halfSize.z*2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    void setState(const cyclone::Vector3 &position, const cyclone::Vector3 &velocity)
    {
        body->setPosition(position);
        body->setOrientation(1,0,0,0);
        body->setVelocity(velocity);
        body->setRotation(cyclone::Vector3(0,0,0));
        halfSize = cyclone::Vector3(2,2,2);

        cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 10.0;
        body->setMass(mass);

        cyclone::Matrix3 tensor;
        tensor.setBlockInertiaTensor(halfSize, mass);
        body->setInertiaTensor(tensor);

        body->setLinearDamping(0.95f);
        body->setAngularDamping(0.8f);
        body->clearAccumulators();
        body->setAcceleration(0,-10.0f,0);

        body->setCanSleep(false);
        body->setAwake();

        body->calculateDerivedData();
        calculateInternals();
    }
};


/**
 * The main demo class definition.
 */
class DiceDemo : public RigidBodyApplication
{

	/** Number of dice */
    const static unsigned DICE = 3;

	/** Holds the dice data. */
    Die diceData[DICE];

    /** Detonates the explosion. */
    void fire();

    /** Resets the position of all the boxes and primes the explosion. */
    virtual void reset();

    /** Processes the contact generation code. */
    virtual void generateContacts();

    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

public:
    /** Creates a new demo object. */
    DiceDemo();

    /** Sets up the rendering. */
    virtual void initGraphics();

    /** Returns the window title for the demo. */
    virtual const char* getTitle();

    /** Display the particle positions. */
    virtual void display();

    /** Handles a key press. */
    virtual void key(unsigned char key);

    /** Handle a mouse drag */
    virtual void mouseDrag(int x, int y);
};

// Method definitions
DiceDemo::DiceDemo() : RigidBodyApplication()
{
	pauseSimulation = false;

    // Reset the position of the boxes
    reset();
}

const char* DiceDemo::getTitle()
{
    return "Assignment 5 - Advanced Physics";
}

void DiceDemo::fire()
{
    
}

void DiceDemo::reset()
{

	// Initialise the boxes
	cyclone::real x = 0;
	cyclone::real y = 10;
    cyclone::real z = 10;
	int i=0;

	for (Die *die = diceData; die < diceData + DICE; die++)
    {
		cyclone::Vector3* position = new cyclone::Vector3(x,y,z);
		cyclone::Vector3* velocity = new cyclone::Vector3(0,-5,-10.0);
        die->setState(*position, *velocity);
		i++;
    }

}

void DiceDemo::generateContacts()
{

    // Create the ground plane data
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0,1,0);
    plane.offset = 0;

    // Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.3;
    cData.tolerance = (cyclone::real)0.1;


	// Check collisions
    for (Die *die = diceData; die < diceData + DICE; die++)
    {
		// Check for collisions with the ground plane
        if (!cData.hasMoreContacts()) return;
        cyclone::CollisionDetector::boxAndHalfSpace(*die, plane, &cData);
	}

   
}

void DiceDemo::updateObjects(cyclone::real duration)
{
    // Update the dice
    for (Die *die = diceData; die < diceData + DICE; die++)
    {
        // Run the physics
        die->body->integrate(duration);
        die->calculateInternals();
    }
}


void DiceDemo::initGraphics()
{
    GLfloat lightAmbient[] = {0.8f,0.8f,0.8f,1.0f};
    GLfloat lightDiffuse[] = {0.9f,0.95f,1.0f,1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_LIGHT0);

    Application::initGraphics();
}

void DiceDemo::display()
{
    const static GLfloat lightPosition[] = {1,-1,0,0};
    const static GLfloat lightPositionMirror[] = {1,1,0,0};

    // Clear the viewport and set the camera direction
    RigidBodyApplication::display();


    // Draw some scale circles
    glColor3f(0.75, 0.75, 0.75);
    for (unsigned i = 1; i < 20; i++)
    {
        glBegin(GL_LINE_LOOP);
        for (unsigned j = 0; j < 32; j++)
        {
            float theta = 3.1415926f * j / 16.0f;
            glVertex3f(i*cosf(theta),0.0f,i*sinf(theta));
        }
        glEnd();
    }
    glBegin(GL_LINES);
    glVertex3f(-20,0,0);
    glVertex3f(20,0,0);
    glVertex3f(0,0,-20);
    glVertex3f(0,0,20);
    glEnd();



	glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1,0,0);
    
    // Render the dice    
    for (Die *die = diceData; die < diceData + DICE; die++)
    {
        die->render();
    }

	glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

	 // Render the description
    glColor3f(0.0f, 0.0f, 0.0f);
    char text[256];
    sprintf(text, "Press R to launch the dice again.");
    renderText(10.0f, 10.0f, text);

    // Finish the frame, rendering any additional information
    drawDebug();
}

void DiceDemo::mouseDrag(int x, int y)
{
    RigidBodyApplication::mouseDrag(x, y);

    // Remember the position
    last_x = x;
    last_y = y;
}


void DiceDemo::key(unsigned char key)
{
    switch(key)
    {
    case 'e': case 'E':
       
        return;

	case 'r': case 'R':
       reset();
       return;

    case 't': case 'T':
       
        return;

    case 'w': case 'W':
       
        return;
    }

    RigidBodyApplication::key(key);
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new DiceDemo();
}