#ifndef NEURONENGINE_H
#define NEURONENGINE_H

#include <QObject>
#include <QList>
#include "neuron.h"

#define NEURONEGINE_MAX_SIZE 4096
enum NEURON_NORM{
    NEURON_NORM_L1=0,
    NEURON_NORM_LSUP=1,
    NEURON_NORM_L2
};


class NeuronEngine : public QObject
{
    Q_OBJECT
public:
    explicit NeuronEngine(QObject *parent = 0);
    ~NeuronEngine();

    /*
     *purpose:init the neuron engine.
     *input:
     */
    bool begin(int min_aif=0,int max_aif=NEURON_AIF_MAX,int norm = NEURON_NORM_L1);

    /*purpose:learn the input vectors
     *input:
     * @cat,the input vector's category
     * @vec, the input vector
     * @len, the input length
     *return:
     * @the current neruon size
     */
    virtual int learn(int cat,uint8_t * vec,int len);
    const neuron_data * neuron(int index);
    int neuronCount() const;

    /*purpose:classify the input vectors
     *input:
     * @cat,the input vector's category
     * @vec, the input vector
     * @len, the input length
     *return:
     * @return the recognized category
     * @return 0 if not recognized
     */
    virtual int classify(uint8_t vec[],int len);

    /*purpose:read the firing distances, call this after the classify functoion.
     *output:
     * @cat, the firing neuron's cat
     * @dist,the firing neuron's distance
     *return:
     * @the length of the firing neuron
     */
    int readFiringInfo(int cat[],int dist[]);

    int firingNeuronCount() const;
    virtual void forget();//reset the whole engine

protected:
    void clearNeuronList();//clear the neuron list, including the ram.

    /*purpose:read the firing distances, call this after the classify functoion.
     *input:
     * @ptr, the neuron
     *output:
     * @ptrDist,the distance
     *return:
     * true, firing
     */
    bool checkNeuronFiring(neuron_data* ptr,uint8_t *vec, int len, int * ptrDist);
    void updateNeuronAIF(neuron_data * ptr,int aif);

protected:
    QList<neuron_data*> m_firingList;//saves all fired neurons;
    QList<neuron_data*> m_neuronList;//saves all generated neuron data

    int m_maxAIF;
    int m_minAIF;
    int m_norm;

    int m_lastCreateNeuron;
};

#endif // NEURONENGINE_H
