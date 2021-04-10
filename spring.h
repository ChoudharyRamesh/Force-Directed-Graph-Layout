/*
MIT License
Copyright (c) 2021 Ramesh Choudhary
*/

#ifndef SPRING_H
#define SPRING_H

// reference :- https://www.iitk.ac.in/npsc/Papers/NPSC2014/1569993659.pdf
class QGraphicsNodeItem;
class Spring
{
public:
    enum SpringType
    {
        MechanicalSpring ,ElectricalSpring
    };

    //ElectricalSpring  --> ( assumes electrical force as spring to reduce another class implementation) A special
    //kind of spring (thread) that can contract or expand due to force between two charges body until their velocity
    //becames zero. In short Spring K=0 ,means it does not apply any force on node ,but it contract and expand
    //due to electrical force.

    //mechanical Spring constructor
    Spring(QGraphicsNodeItem * node1,QGraphicsNodeItem * node2,float * restingLength);
    //electrical Spring constructor
    Spring(QGraphicsNodeItem * node1,QGraphicsNodeItem * node2, int * charge);
    void applyForce();

private:
    float * restingLength;
    int * charge;
    QGraphicsNodeItem * node1;
    QGraphicsNodeItem * node2;
    SpringType springType;
    float  mechanical_k=0.005f;
    float  electric_k=1;
};

#endif // SPRING_H
