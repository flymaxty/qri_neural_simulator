/***************************************************************************
**
** Autohr : Yunpeng Song
** Contact: 413740951@qq.com
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#ifndef NEURONENGINE_H
#define NEURONENGINE_H

#include <QObject>
#include <QList>
#include "neuron.h"

#define NEURONENGINE_MAX_NUM 4096

class NeuronEngine : public QObject
{
    Q_OBJECT
public:

    enum NORM{
        NORM_L1=0,
        NORM_LSUP=1
    };
    enum MODE{
        MODE_RBF=0,
        MODE_KNN=1
    };

    static const int MaxNeuronSize=NEURONENGINE_MAX_NUM;

    explicit NeuronEngine(QObject *parent = 0);
    ~NeuronEngine();

    //begin with the default settings.
    virtual bool Begin(void);
    /* purpose:init the neuron engine with customized settings
     * input:
     * @mode,the classification mode. RBF or KNN
     * @norm, the distance calculation mode. L1 Lsup
     * @minAif, the minimum AIF of all neurons
     * @maxAIF, the max AIF of all neurons
     * return:
     * true, if success
     */
    virtual bool Begin(int mode,int norm,int minAIF,int maxAIF);
    virtual void Forget();//reset the whole engine
    const int Mode();

    /*purpose:learn the input vectors
     *input:
     * @cat,the input vector's category
     * @vec, the input vector
     * @len, the input length
     *return:
     * @the current neruon size
     */
    virtual int Learn(int cat,uint8_t * vec,int len);
    const neuron_data * ReadNeuron(int index);
    int NeuronSize() const;

    /*purpose:classify the input vectors
     *input:
     * @vec, the input vector
     * @len, the input length
     *return:
     * @return the recognized category
     * @return 0 if not recognized
     */
    virtual int Classify(uint8_t vec[],int len);

    /*purpose: read the classify detail report.
     *output:
     * @nid, the firing/sorting neuron's index array
     * @len, the length of the nid Array
     */
    void ReadClassifyReport(uint8_t nid[],int *len);

    /*purpose: read the neurons related to the Classify operation.
     *input:
     * @index, the firing/sorting neuron's index
     *return:
     * the neuron_data.
     */
    const neuron_data * ReadClassifyNeuron(int index);

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

    //learn the pattern in RBF mode.
    virtual int learnRBF(int cat,uint8_t * vec,int len);

    virtual int classifyRBF(uint8_t vec[],int len);
    virtual int classifyKNN(uint8_t vec[],int len);
protected:
    QList<neuron_data*> m_firingList;//saves all fired neurons;
    QList<neuron_data*> m_neuronList;//saves all generated neuron data

    int m_maxAIF;
    int m_minAIF;
    int m_norm;

    int m_lastCreateNeuron;

    int m_mode;
};

#endif // NEURONENGINE_H
