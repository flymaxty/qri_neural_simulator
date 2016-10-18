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


    /*
     * Classified the input vector.
     * Check if it belongs to this category.
     * input:
     * @src_vector, the input vector
     * @src_len, length of the input vector
     * output:
     * @ptr_count, how many neurons recognized the input vector.
     * @ptr_value, the minimum distance of the recognized neurons.
     * return:
     * -1, not recognized
     * return>0,
     */
    int classify(uint8_t *src_vector, int src_len,
                 int *ptr_count,int *ptr_value);


    const int category();
protected:
    void resetNeuronList();

protected:
    int m_cat;
    QList<Neuron*> m_neuronList;
};

#endif // NEURONSQUAD_H
