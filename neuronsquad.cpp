#include "neuronsquad.h"

NeuronSquad::NeuronSquad(QObject *parent) : QObject(parent)
{
    m_neuronList.clear();

}
NeuronSquad::~NeuronSquad()
{
    resetNeuronList();
}

const int
NeuronSquad::category(){
    return m_cat;
}

void NeuronSquad::resetNeuronList()
{
    QList<Neuron*>::iterator iter = m_neuronList.begin();
    for(iter=m_neuronList.begin();iter<m_neuronList.end();iter++){
        Neuron* n = *iter;
        delete(n);
    }
    m_neuronList.clear();
}
