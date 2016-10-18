#include "neuron.h"
#include <stdlib.h>
#include <math.h>
Neuron::Neuron(QObject *parent) : QObject(parent)
{
    reset();
}

void Neuron::init(int cat, uint8_t *src_vector, int src_len)
{
    if(src_len>NEURON_VECTOR_MAX_LENGTH ||  src_len<=0){
        return;
    }

    //reset this neuron
    reset();
    m_cat = cat;

    //copy the vector to my vector.
    memcpy(m_vector,src_vector,src_len);
    m_vectorLen = src_len;
}
void Neuron::reset()
{
    //reset vectors
    memset(m_vecotr,0,NEURON_VECTOR_MAX);
    m_vectorLen = 0;
    //reset cat
    m_cat=0;
    //reset aif
    m_aif = NEURON_AIF_MAX; //max of int16_6
}
int Neuron::distance(uint8_t *src_vector, int src_len)
{
    int dist=0;
    int len=0;
    //check input
    if(src_vector==NULL || src_len>NEURON_VECTOR_MAX){
        return -1; //distance cannot be smaller than zero.
    }

    //check the array size.
    len = (src_len>m_vectorLen)?src_len:m_vectorLen;
    for(int i=0;i<len;i++){
        if(i<src_len){
            dist = dist + abs(src_vector[i]-m_vector[i]);
        }else{
            dist = dist + abs(m_vector[i]);
        }
    }
    return dist;
}
int Neuron::classify(uint8_t *src_vector, int src_len){
    int value = distance(src_vector,src_len);
    if(value<m_aif){
        return value;
    }
    return -1;
}

int Neuron::category(){return m_cat;}
void Neuron::setAIF(int aif){
    m_aif = aif;
}
int Neuron::aif(){return m_aif;}
