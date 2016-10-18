#ifndef NEURONSQUAD_H
#define NEURONSQUAD_H

#include <QObject>
#include <QList>
#include "neuron.h"
/**
 * @abstract
 * this class will manage all the neurons with the same CAT value.
 * Each NeuronSquad will run in a separate thread, if hw permits.
 **/
class NeuronSquad : public QObject
{
    Q_OBJECT
public:
    explicit NeuronSquad(QObject *parent = 0);
    ~NeuronSquad();

    int category();

    int classify(uint8_t *src_vector, int src_len);

protected:
    void resetNeuronList();

protected:
    int m_cat;
    QList<Neuron*> m_neuronList;
};

#endif // NEURONSQUAD_H
