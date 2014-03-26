//
//  dice.cpp
//  Assignment 5
//
//  Created by Alberto Martinez de Murga Ramirez, 500693826
//  and Jose Manuel Carmona Roldan, 500693836

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "../../app.h"
#include "../../timing.h"

#include <stdio.h>
#include <time.h>


class Dice : public cyclone::CollisionBox
{
    
public:
    Dice()
    {
        body = new cyclone::RigidBody;
    }

    ~Dice()
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
        drawCube(0.5f);
        glPopMatrix();
    }

    void setState(const cyclone::Vector3 &position, const cyclone::Vector3 &velocity, const cyclone::Vector3 &rotation)
    {
        body->setPosition(position);
        body->setOrientation(1,0,0,0);
        body->setVelocity(velocity);
        body->setRotation(rotation);
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
    
    void drawCube(float size)
    {
        // Define vertices in counter-clockwise (CCW) order with normal pointing out
        glBegin(GL_QUADS);
        // Top green face (y = size)
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f( size, size, -size);
        glVertex3f(-size, size, -size);
        glVertex3f(-size, size,  size);
        glVertex3f( size, size,  size);
        
        // Bottom orange face (y = -size)
        glColor3f(1.0f, 0.5f, 0.0f);
        glVertex3f( size, -size,  size);
        glVertex3f(-size, -size,  size);
        glVertex3f(-size, -size, -size);
        glVertex3f( size, -size, -size);
        
        // Front red face  (z = size)
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f( size,  size, size);
        glVertex3f(-size,  size, size);
        glVertex3f(-size, -size, size);
        glVertex3f( size, -size, size);
        
        // Back yellow face (z = -size)
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f( size, -size, -size);
        glVertex3f(-size, -size, -size);
        glVertex3f(-size,  size, -size);
        glVertex3f( size,  size, -size);
        
        // Left blue face (x = -size)
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-size,  size,  size);
        glVertex3f(-size,  size, -size);
        glVertex3f(-size, -size, -size);
        glVertex3f(-size, -size,  size);
        
        // Right magenta face (x = size)
        glColor3f(1.0f, 0.0f, 1.0f);
        glVertex3f(size,  size, -size);
        glVertex3f(size,  size,  size);
        glVertex3f(size, -size,  size);
        glVertex3f(size, -size, -size);
        glEnd();
        
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
    Dice dices[DICE];

    /** Resets the position of all the boxes and primes the explosion. */
    virtual void reset();

    /** Processes the contact generation code. */
    virtual void generateContacts();

    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

	/* Generates the contacts of the dice with a plane. */
	unsigned diceAndHalfSpace(const Dice &dice, const cyclone::CollisionPlane &plane, cyclone::CollisionData *data);

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
	srand(time(NULL));
    reset();
}

const char* DiceDemo::getTitle()
{
    return "Assignment 5 - Advanced Physics";
}

void DiceDemo::reset()
{

	// Initialise the boxes
    
    for (unsigned i = 0; i < DICE; i++) {
        
        cyclone::real x = rand() % 10;
        cyclone::real y = 10 + rand() % 20;
        cyclone::real z = 10 + rand() % 20;
        cyclone::Vector3* position = new cyclone::Vector3(x,y,z);
        cyclone::Vector3* velocity = new cyclone::Vector3(0,-10.0,-10.0);


		x = -1 - rand() % 5;
        y = 1 + rand() % 5;
		cyclone::Vector3* rotation = new cyclone::Vector3(x,y,0);
        dices[i] = *new Dice();
        dices[i].setState(*position,*velocity, *rotation);
        
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
    for (unsigned i = 0; i < DICE; i++) {
        if(!cData.hasMoreContacts()) return;
        //cyclone::CollisionDetector::boxAndHalfSpace(dices[i], plane, &cData);
		diceAndHalfSpace(dices[i], plane, &cData);
        for (int j = i - 1; j >= 0; j--) {
            cyclone::CollisionDetector::boxAndBox(dices[i], dices[j], &cData);
        }
    }

   
}

void DiceDemo::updateObjects(cyclone::real duration)
{
    // Update the dice
    for (unsigned i = 0; i < DICE; i++) {
        dices[i].body->integrate(duration);
        dices[i].calculateInternals();
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
    const static GLfloat lightPosition[] = {-100,100,1,0};
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

    // Render the dice    
    for (unsigned i = 0; i < DICE; i++) {
        dices[i].render();
    }

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
	case 'r': case 'R':
       reset();
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


unsigned DiceDemo::diceAndHalfSpace(const Dice &dice, const cyclone::CollisionPlane &plane, cyclone::CollisionData *data)
{
    // Make sure we have contacts
    if (data->contactsLeft <= 0) return 0;

    // Check for intersection
    if (!cyclone::IntersectionTests::boxAndHalfSpace(dice, plane))
    {
        return 0;
    }

    // We have an intersection, so find the intersection points. We can make
    // do with only checking vertices. If the box is resting on a plane
    // or on an edge, it will be reported as four or two contact points.

    // Go through each combination of + and - for each half-size
	cyclone::real offset = 0.7;
    static cyclone::real mults[24][3] = {{offset,1,1}, {1,offset,1}, {1,1,offset},
										 {-offset,1,1}, {-1,offset,1}, {-1,1,offset},
										 {offset,-1,1}, {1,-offset,1}, {1,-1,offset},
										 {-offset,-1,1}, {-1,-offset,1}, {-1,-1,offset},
										 {offset,1,-1}, {1,offset,-1}, {1,1,-offset},
										 {-offset,1,-1}, {-1,offset,-1}, {-1,1,-offset},
	                                     {offset,-1,-1}, {1,-offset,-1}, {1,-1,-offset},
	                                     {-offset,-1,-1}, {-1,-offset,-1}, {-1,-1,-offset}};

    cyclone::Contact* contact = data->contacts;
    unsigned contactsUsed = 0;
    for (unsigned i = 0; i < 24; i++) {

        // Calculate the position of each vertex
        cyclone::Vector3 vertexPos(mults[i][0], mults[i][1], mults[i][2]);
        vertexPos.componentProductUpdate(dice.halfSize);
        vertexPos = dice.getTransform().transform(vertexPos);

        // Calculate the distance from the plane
        cyclone::real vertexDistance = vertexPos * plane.direction;

        // Compare this to the plane's distance
        if (vertexDistance <= plane.offset)
        {
            // Create the contact data.

            // The contact point is halfway between the vertex and the
            // plane - we multiply the direction by half the separation
            // distance and add the vertex location.
            contact->contactPoint = plane.direction;
            contact->contactPoint *= (vertexDistance-plane.offset);
            contact->contactPoint += vertexPos;
            contact->contactNormal = plane.direction;
            contact->penetration = plane.offset - vertexDistance;

            // Write the appropriate data
            contact->setBodyData(dice.body, NULL, data->friction, data->restitution);

            // Move onto the next contact
            contact++;
            contactsUsed++;
            if (contactsUsed == (unsigned)data->contactsLeft) return contactsUsed;
        }
    }

    data->addContacts(contactsUsed);
    return contactsUsed;
}
