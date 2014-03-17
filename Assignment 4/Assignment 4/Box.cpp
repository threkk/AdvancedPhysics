//
//  Box.cpp
//  Assignment 4
//
//  Created by Alberto Martinez de Murga Ramirez, 500693826
//  and Jose Manuel Carmona Roldan, 500693836

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"

#include <stdio.h>

using namespace cyclone;

class Projectile : public CollisionBox {
    
    real massConstant;

public:
    Projectile(){
        body = new RigidBody;
        massConstant = 8.0f;
    }
    
    ~Projectile(){
        delete body;
    }
    
    void render(){
        GLfloat mat[16];
        body->getGLTransform(mat);
        
        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x*2, halfSize.y*2, halfSize.z*2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    void setState(cyclone::real z){
        body->setPosition(0, 3, z);
        body->setOrientation(1,0,0,0);
        body->setVelocity(0,0,0);
        body->setAcceleration(Vector3::GRAVITY);
        body->setRotation(cyclone::Vector3(0,0,0));
        halfSize = cyclone::Vector3(1,1,1);
        
        body->setMass(getMass());
        
        cyclone::Matrix3 tensor;
        tensor.setBlockInertiaTensor(halfSize, getMass());
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
    
    void increseMass(){
        massConstant += 1.0f;
        body->setMass(getMass());
    }
    
    void decreaseMass(){
        massConstant -= 0.5f;
        body->setMass(getMass());
    }
    
    real getMass(){
        real mass = halfSize.x * halfSize.y * halfSize.z * massConstant;
        return mass;
    }
};

class Box : public cyclone::CollisionBox
{
public:

    Box(){  body = new cyclone::RigidBody; }

    ~Box(){ delete body; }

    /** Draws the box, excluding its shadow. */
    void render()
    {
        // Get the OpenGL transformation
        GLfloat mat[16];
        body->getGLTransform(mat);

        glPushMatrix();
        glMultMatrixf(mat);
        glScalef(halfSize.x*2, halfSize.y*2, halfSize.z*2);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    /** Sets the box to a specific location. */
    void setState(const cyclone::Vector3 &position)
    {
        body->setPosition(position);
        body->setOrientation(1,0,0,0);
        body->setVelocity(0,0,0);
        body->setRotation(cyclone::Vector3(0,0,0));
        halfSize = cyclone::Vector3(2,2,2);

        cyclone::real mass = halfSize.x * halfSize.y * halfSize.z * 8.0f;
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
class WallOfBoxes : public RigidBodyApplication
{

	Projectile *projectile;

    /**
    * Holds the number of boxes in the simulation.
    */
    const static unsigned boxes = 16;

    /** Holds the box data. */
    Box boxData[boxes];

    /** Resets the position of all the boxes and primes the explosion. */
    virtual void reset();

    /** Build the contacts for the current situation. */
    virtual void generateContacts();

    /** Processes the objects in the simulation forward in time. */
    virtual void updateObjects(cyclone::real duration);

    /** Dispatches a round. */
    void fire();

public:

    /** Creates a new demo object. */
    WallOfBoxes();
    virtual const char* getTitle();
    virtual void initGraphics();
    virtual void display();
    virtual void mouse(int button, int state, int x, int y);
    virtual void key(unsigned char key);
};


WallOfBoxes::WallOfBoxes() : RigidBodyApplication()
{
    pauseSimulation = false;
	projectile = new Projectile();
    reset();
}


void WallOfBoxes::initGraphics()
{
    GLfloat lightAmbient[] = {0.8f,0.8f,0.8f,1.0f};
    GLfloat lightDiffuse[] = {0.9f,0.95f,1.0f,1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_LIGHT0);

    Application::initGraphics();
}

void WallOfBoxes::reset()
{
    
	projectile->setState(0.0f);

    // Initialise the boxes
	cyclone::real x = 0;
	cyclone::real y = 0;
    cyclone::real z = 20.0;
	int i=0;

	for (Box *box = boxData; box < boxData+boxes; box++)
    {
		if(i%4 == 0){
			if(i == 0)
				x = - box->halfSize.x;
			else
				x += box->halfSize.x + 0.5;
			y = box->halfSize.y + 0.5;
		}

		cyclone::Vector3* position = new cyclone::Vector3(x,y,z);
        box->setState(*position);
		y += box->halfSize.y*2 + 0.5;
		i++;
    }

}

const char* WallOfBoxes::getTitle()
{
    return "Advanced Physics > Wall of Boxes (Assignment 4)";
}

void WallOfBoxes::fire()
{
    projectile->body->setVelocity(0.0f, 0.0f, 20.0f);
    Vector3 accel;
    accel.y = -0.5f;
    accel.addScaledVector(Vector3::GRAVITY, 1);
    projectile->body->setAcceleration(accel);
    projectile->body->setDamping(0.99f, 0.8f);
}

void WallOfBoxes::updateObjects(cyclone::real duration)
{
    
	projectile->body->integrate(duration);
    projectile->calculateInternals();

    // Update the boxes
    for (Box *box = boxData; box < boxData+boxes; box++)
    {
        // Run the physics
        box->body->integrate(duration);
        box->calculateInternals();
    }
}

void WallOfBoxes::display()
{
    const static GLfloat lightPosition[] = {-1,1,0,0};
    
    // Clear the viewport and set the camera direction
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(-25.0, 8.0, 5.0,  0.0, 5.0, 22.0,  0.0, 1.0, 0.0);

    // Draw some scale lines
    glColor3f(0.75f, 0.75f, 0.75f);
    glBegin(GL_LINES);
    for (unsigned i = 0; i < 200; i += 10)
    {
        glVertex3f(-5.0f, 0.0f, i);
        glVertex3f(5.0f, 0.0f, i);
    }
    glEnd();

    // Render the box
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1,0,0);
    for (Box *box = boxData; box < boxData+boxes; box++)
    {
        box->render();
    }
	projectile->render();
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // Render the description
    glColor3f(0.0f, 0.0f, 0.0f);
    char text[256];
    sprintf(text, "Click: Fire\nj-k: Increase/decrease mass\nMass:%f", projectile->getMass());
    renderText(10.0f, 34.0f, text);
}

void WallOfBoxes::generateContacts()
{
    // Create the ground plane data
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0,1,0);
    plane.offset = 0;

    // Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = 0.9;
    cData.restitution = 0.2;
    cData.tolerance = 0.1;

	//Check collisions of the shot with the ground plane
	if (!cData.hasMoreContacts()) return;
    CollisionDetector::boxAndHalfSpace(*projectile, plane, &cData);

    // Check collisions
    for (Box *box = boxData; box < boxData+boxes; box++)
    {
		// Check for collisions with the ground plane
        if (!cData.hasMoreContacts()) return;
        cyclone::CollisionDetector::boxAndHalfSpace(*box, plane, &cData);
		
		//Check for collisions with the projectile box
		if (!cData.hasMoreContacts()) return;
			cyclone::CollisionDetector::boxAndBox(*box, *projectile, &cData);

		// Check for collisions with the other boxes
		for (Box *other = box+1; other < boxData+boxes; other++){
			if (!cData.hasMoreContacts()) return;
			cyclone::CollisionDetector::boxAndBox(*box, *other, &cData);
		}
    }
}

void WallOfBoxes::mouse(int button, int state, int x, int y)
{
    // Fire the current weapon.
    if (state == GLUT_DOWN) fire();
}

void WallOfBoxes::key(unsigned char key)
{
    switch(key)
    {
        case '+': case 'k' : case 'K' : projectile->increseMass(); break;
        case '-': case 'j' : case 'J' : projectile->decreaseMass(); break;
            
        case 'r': case 'R': reset(); break;
    }
}

/**
 * Called by the common demo framework to create an application
 * object (with new) and return a pointer.
 */
Application* getApplication()
{
    return new WallOfBoxes();
}
