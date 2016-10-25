#include "neuronengine.h"
#include <qdebug.h>
NeuronEngine::NeuronEngine(QObject *parent) : QObject(parent)
{
    forget();
}
NeuronEngine::~NeuronEngine(){
    forget();
}

bool NeuronEngine::begin(int min_aif, int max_aif, int norm)
{
    if(min_aif>max_aif || min_aif<0){
        return false;
    }

    //step1.reset the engine
    forget();

    //step2 norm
    switch(norm){
    case NEURON_NORM_L1:
    case NEURON_NORM_L2:
    case NEURON_NORM_LSUP:
        m_norm = (int)norm;
        break;
    default:
        return false;
    }

    //step3 aif
    m_minAIF = min_aif;
    m_maxAIF = max_aif;


    return true;
}

int NeuronEngine::learn(int cat, uint8_t *vec, int len){
    QList<neuron_data*>::iterator iter = m_neuronList.begin();
    int dist=0;
    int minDist = NEURON_AIF_MAX;
    bool isExist=false;

    //step1 clear the firing list
    m_firingList.clear();

    //step2 traversing all neurons
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        neuron_data * ptr = *iter;
        if(!ptr){
            continue;
        }

        //calc the distance
        if(checkNeuronFiring(ptr,vec,len,&dist))
        {/**Firing!!!**/
            m_firingList.append(ptr);
            if(ptr->cat == cat){
                //if it exists in its own neuron. do not create new neuron
                isExist=true;
            }else{
                //since you are firing, shrink your AIF.
                updateNeuronAIF(ptr,dist);//AIF
            }
        }
        if(dist<minDist){
            minDist = dist;
        }
    }

    //step3. process the new input
    if(!isExist){
        //create neurons only if not exist.
        neuron_data * nptr = (neuron_data*)malloc(sizeof(neuron_data));
        if(nptr==NULL){
            return -1;
        }

        //init the neuron
        neuron_init(nptr,vec,len,cat,minDist);
        nptr->index = m_lastCreateNeuron++; //inidex
        updateNeuronAIF(nptr,minDist);//AIF

        //append the new neuron
        m_neuronList.append(nptr);
    }
    return m_neuronList.size();
}

int NeuronEngine::classify(uint8_t vec[], int len){
    QList<neuron_data*>::iterator iter = m_neuronList.begin();
    int dist=0;
    int minDist = NEURON_AIF_MAX;
    int retCat=0;

    //step1 clear the firing list
    m_firingList.clear();

    //step2 traversing all neurons
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        neuron_data * ptr = *iter;
        if(!ptr){
            continue;
        }

        //calc the distance
        if(checkNeuronFiring(ptr,vec,len,&dist))
        {//firing
            if(dist<minDist){//find the most close neuron
                minDist = dist;
                retCat = ptr->cat;
            }
            m_firingList.append(ptr);
            ptr->firing = dist;
        }
    }

    return retCat;
}

int NeuronEngine::readFiringInfo(int cat[], int dist[])
{
    int len = 0;
    QList<neuron_data*>::iterator iter;

    if(cat==NULL || dist==NULL){
        return 0;
    }

    for(iter = m_firingList.begin();iter<m_firingList.end();iter++){
        neuron_data* ptr = *iter;
        if(!ptr){
            continue;
        }
        cat[len] = ptr->cat;
        dist[len] = ptr->firing;
        len++;
    }
    return len;
}

int NeuronEngine::neuronCount() const{
    return m_neuronList.size();
}
int NeuronEngine::firingNeuronCount() const{
    return m_firingList.size();
}

bool NeuronEngine::checkNeuronFiring(neuron_data *ptr, uint8_t *vec, int len,int* ptrDist)
{
    int dist=0;
    switch(m_norm){
    case NEURON_NORM_L1:
        dist = neuron_distance_L1(ptr,vec,len);
        break;
    case NEURON_NORM_LSUP:
        dist = neuron_distance_Lsup(ptr,vec,len);
        break;
    case NEURON_NORM_L2:
        /*TODO: call the L2 distance calc function*/
        break;
    }
    if(dist<ptr->aif){
        return true;
    }
    *ptrDist = dist;
    return false;
}
void NeuronEngine::updateNeuronAIF(neuron_data *ptr, int aif)
{
    if(aif>m_maxAIF){
        aif = m_maxAIF;
    }else if(aif<m_minAIF){
        aif = m_minAIF;
    }
    ptr->aif = aif;
}

void NeuronEngine::forget(){
    m_norm = NEURON_NORM_L1;
    m_lastCreateNeuron = 0;
    //reset AIF
    m_minAIF = 0;
    m_maxAIF = NEURON_AIF_MAX;

    //clear neuronlist
    clearNeuronList();
    m_firingList.clear();

    //TODO:stop and reset all threads here.
}

void NeuronEngine::clearNeuronList()
{
    QList<neuron_data*>::iterator iter = m_neuronList.begin();
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        neuron_data * ptr = *iter;
        if(ptr){
            free(ptr);
        }
    }
    m_neuronList.clear();
}
const neuron_data* NeuronEngine::neuron(int index)
{
    if(index<0 || index>=m_neuronList.size()){
        return NULL;
    }
    neuron_data* ptr = m_neuronList.at(index);
    return ptr;
}
