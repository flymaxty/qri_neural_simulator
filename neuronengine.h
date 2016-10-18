#ifndef NEURONENGINE_H
#define NEURONENGINE_H

#include <QObject>
#include <QList>
#include "neuron.h"
#include "neuronsquad.h"
#define NEURONEGINE_MAX_SIZE 4096
class NeuronEngine : public QObject
{
    Q_OBJECT
public:
    explicit NeuronEngine(QObject *parent = 0);

protected:
    void resetSquadList();

protected:
    QList<NeuronSquad*> m_squadList;//saves all executing neurons
};

#endif // NEURONENGINE_H
