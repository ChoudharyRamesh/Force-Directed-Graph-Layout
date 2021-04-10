/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#include "spring.h"
#include "qgraphicsnodeitem.h"

Spring::Spring(QGraphicsNodeItem *node1, QGraphicsNodeItem *node2, float *restingLength)
{
    this->node1=node1;
    this->node2=node2;
    this->restingLength=restingLength;
    this->springType=SpringType::MechanicalSpring;
}

Spring::Spring(QGraphicsNodeItem *node1, QGraphicsNodeItem *node2, int *charge)
{
    this->node1=node1;
    this->node2=node2;
    this->charge=charge;
    this->springType=SpringType::ElectricalSpring;
}

void Spring::applyForce()
{
    // force direction ( vector of two node )
    QVector2D force = QVector2D(node2->pos() - node1->pos());
    float distanceBetweenNodes = force.length();
    // calculate force magnitude
    float magnitude= 0;

    if(springType==MechanicalSpring)
    {
        //hook force
        // to attract let't pretend spring is stretched
        // means currrent(distanceBetweenNodes) length of string should be greater then restingLenght
        magnitude = mechanical_k*(distanceBetweenNodes- *restingLength);
    }
    else
    {
        //coulomb force
        // non adjacent node should repel each other so sign is negative
        //f = k*q/(r^2)
        magnitude = - (electric_k*(*charge))/(distanceBetweenNodes*distanceBetweenNodes);
    }

    // create force vector with this magnitude and directiion
    force.normalize(); // convert to unit vector in order to remove magnitude part
    force*=magnitude;  // got new force vector

    // find acceleration F(vector)= m * A(vector) --> m=1
    //so  F (vector) == A(vector)

    node1->accelerate(force);
    force*=-1; // no matter what is force type (attractive or repulsice )
    // ,either both node should repel each other or attract each other
    // so force multipied by -1 in order to change directiion of one node
    node2->accelerate(force);
    node1->updatePosition();
    node2->updatePosition();
}
