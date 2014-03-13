//
//  Box.cpp
//  Assignment 4
//
//  Created by Alberto Mtnez on 13/03/14.
//  Copyright (c) 2014 Alberto Mtnez. All rights reserved.
//

#include <gl/glut.h>
#include <cyclone/cyclone.h>
#include "app.h"
#include "timing.h"

#include <stdio.h>

using namespace cyclone;

class Box : public CollisionBox
{
    
    real massConstant;

public:
    Box()
    {
        body = new RigidBody;
        massConstant = 8.0f;
    }
    
    ~Box()
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
    
    void setState(cyclone::real z)
    {
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
    
    void increseMass()
    {
        massConstant += 1.0f;
        body->setMass(getMass());
    }
    
    void decreaseMass()
    {
        massConstant -= 0.5f;
        body->setMass(getMass());
    }
    
    real getMass()
    {
        real mass = halfSize.x * halfSize.y * halfSize.z * massConstant;
        return mass;
    }
};


class BoxDemo : public RigidBodyApplication
{
    Box *box;

    virtual void reset();
    
    virtual void generateContacts();
    
    virtual void updateObjects(cyclone::real duration);
    
    void fire();
    
public:
    BoxDemo();
    
    virtual const char* getTitle();
    
    virtual void initGraphics();
    
    virtual void display();
    
    virtual void mouse(int button, int state, int x, int y);
    
    virtual void key(unsigned char key);
};


BoxDemo::BoxDemo()
:
RigidBodyApplication()
{
    pauseSimulation = false;
    box = new Box();
    reset();
}


void BoxDemo::initGraphics()
{
    GLfloat lightAmbient[] = {0.8f,0.8f,0.8f,1.0f};
    GLfloat lightDiffuse[] = {0.9f,0.95f,1.0f,1.0f};
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    
    glEnable(GL_LIGHT0);
    
    Application::initGraphics();
}

void BoxDemo::reset()
{
    box->setState(0.0f);
}

const char* BoxDemo::getTitle()
{
    return "Cyclone > Box Demo";
}


void BoxDemo::fire()
{
    
    box->body->setVelocity(0.0f, 0.0f, 20.0f);
    Vector3 accel;
    accel.y = -0.5f;
    accel.addScaledVector(Vector3::GRAVITY, 1);
    box->body->setAcceleration(accel);
    box->body->setDamping(0.99f, 0.8f);
 
}

void BoxDemo::updateObjects(cyclone::real duration)
{
    box->body->integrate(duration);
    box->calculateInternals();
}

void BoxDemo::display()
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
    box->render();
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    
    // Render the description
    glColor3f(0.0f, 0.0f, 0.0f);
    char text[256];
    sprintf(text, "Click: Fire\nj-k: Increase/decrease mass\nMass:%f", box->getMass());
    renderText(10.0f, 34.0f, text);

    
}

void BoxDemo::generateContacts()
{
    // Create the ground plane data
    cyclone::CollisionPlane plane;
    plane.direction = cyclone::Vector3(0,1,0);
    plane.offset = 0;
    
    // Set up the collision data structure
    cData.reset(maxContacts);
    cData.friction = (cyclone::real)0.9;
    cData.restitution = (cyclone::real)0.1;
    cData.tolerance = (cyclone::real)0.1;
    
    // Check ground plane collisions

    if (!cData.hasMoreContacts()) return;
    CollisionDetector::boxAndHalfSpace(*box, plane, &cData);
    
}

void BoxDemo::mouse(int button, int state, int x, int y)
{
    // Fire the current weapon.
    if (state == GLUT_DOWN) fire();
}

void BoxDemo::key(unsigned char key)
{
    switch(key)
    {
        case '+': case 'k' : case 'K' : box->increseMass(); break;
        case '-': case 'j' : case 'J' : box->decreaseMass(); break;
            
        case 'r': case 'R': reset(); break;
    }
}

Application* getApplication()
{
    return new BoxDemo();
}
