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
#include <stdint.h>
#include <vector>

using namespace std;

#define NEURON_DEFAULT_NUMBER 128
#define NEURON_DEFAULT_MEM_LENGTH 128
#define NEURON_DEFAULT_AIF_MAX (0x4000)

class Neuron;
class NeuronEngine
{
public:

    enum NORM{
        NORM_L1=0,
        NORM_LSUP=1
    };
    enum MODE{
        MODE_RBF=0,
        MODE_KNN=1
    };

    explicit NeuronEngine();
    ~NeuronEngine();

    //begin with the default settings.
    virtual bool Begin(void);
    /* purpose:init the neuron engine with customized settings
     * input:
     * @mode,the classification mode. RBF or KNN
     * @norm, the distance calculation mode. L1 Lsup
     * @minAif, the minimum AIF of all neurons
     * @maxAIF, the max AIF of all neurons
     * @memLen, the memory size of each neuron
     * @maxNeuronNumber, the max number of neurons.
     * return:
     * true, if success
     */
    virtual bool Begin(int mode,
                       int norm=NORM_L1,
                       int minAIF=0,int maxAIF=NEURON_DEFAULT_AIF_MAX,
                       int memLen=NEURON_DEFAULT_MEM_LENGTH,
                       int maxNeuronNumber=NEURON_DEFAULT_NUMBER);
    void SetMode(enum MODE mode);
    int Mode();

    /*purpose:learn the input vectors
     *input:
     * @cat,the input vector's category
     * @vec, the input vector
     * @len, the input length
     *return:
     * @the current neruon size
     */
    virtual int Learn(int cat,uint8_t * vec,int len);
    Neuron * ReadNeuron(int index);
    int NeuronSize() const;

    /*purpose:classify the input vectors
     *input:
     * @vec, the input vector
     * @len, the input length
     *output:
     * @nid, the neurons' index firing in this classify ops
     * @nidLen, the length of the output NID array
     *return:
     * @return the recognized category
     * @return 0 if not recognized
     */
    virtual int Classify(uint8_t vec[],int len,uint8_t nid[],int *nidLen);
    virtual int Classify(uint8_t vec[],int len);

protected:
    virtual void resetEngine();//reset the whole engine

    void clearNeuronList();//clear the neuron list, including the ram.
    bool createNeuron(int cat, int aif, uint8_t vec[],int len);//create a new neuron and insert to neuron List.
    /*purpose:read the firing distances, call this after the classify functoion.
     *input:
     * @nptr, the neuron
     *output:
     * @ptrDist,the distance
     *return:
     * true, firing
     */
    bool checkNeuronFiring(Neuron* nptr,uint8_t *vec, int len, int * ptrDist);

    //learn the pattern in RBF mode.
    virtual int learnRBF(int cat,uint8_t * vec,int len);

    virtual int classifyRBF(uint8_t vec[],int len);
    virtual int classifyKNN(uint8_t vec[],int len);
protected:
    vector<Neuron*> m_firingList;//saves all fired neurons;
    vector<Neuron*> m_neuronList;//saves all generated neuron data

    int m_maxAIF;
    int m_minAIF;
    int m_norm; //the distance calculation method: L1/Lsup
    int m_mode; //the classify mode:KNN/RBF

    int m_maxNeuronNumber;//default max neuron numbers

    int m_lastNeuronIndex;
};

class Neuron{
    friend class NeuronEngine;
public:
    Neuron(int index);
    ~Neuron();

    int index();
    bool init(int cat,uint8_t vec[],int len);

    void setAIF(int aif);
    int aif();

    int firing();
    int cat();

    int length(){return m_memLen;}
    const uint8_t* buffer(){return m_vecMem;}
protected:
    void reset();

    /* calc the L1 distance of this neuron
     * @n, input neuron,
     * @src_vector,the input vector for this neuron
     * @src_len, the length of the vector
     * return:
     * @the L1 distance of this neuron
     */
    int calcDistanceL1(uint8_t *src_vector,int src_len);

    /* calc the L_sup distance of this neuron
     * @n, input neuron,
     * @src_vector,the input vector for this neuron
     * @src_len, the length of the vector
     * return:
     * @the L_sup distance of this neuron
     */
    int calcDistanceLsup(uint8_t *src_vector,int src_len);

protected:
    static int MaxMemLength;
    static int MaxAIF;
    static int MinAIF;

private:
    uint8_t *m_vecMem;//vecotr memory buffer
    int m_memLen;

    int m_cat;
    int m_aif;
    int m_index;

    int m_firing;//the last calculated firing distance.

};

#endif // NEURONENGINE_H
