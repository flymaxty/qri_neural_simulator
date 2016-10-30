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
#include "neuronengine.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <string.h>

static bool neuron_compare(Neuron* a,Neuron* b);

NeuronEngine::NeuronEngine()
{
    resetEngine();
}
NeuronEngine::~NeuronEngine(){
    resetEngine();
}

bool NeuronEngine::Begin(){
    return Begin(MODE_RBF,NORM_L1,
                 0,NEURON_DEFAULT_AIF_MAX,
                 NEURON_DEFAULT_MEM_LENGTH,
                 NEURON_DEFAULT_NUMBER);
}

bool NeuronEngine::Begin(int mode,int norm, int minAIF,int maxAIF, int memLen,int maxNeuronNumber)
{
    if(minAIF>maxAIF || maxAIF<0 || minAIF<0){
        return false;
    }

    //step1.reset the engine
    resetEngine();

    //step2 norm
    switch(norm){
    case NORM_L1:
    case NORM_LSUP:
        m_norm = (int)norm;
        break;
    default:
        return false;
    }

    //step3 mode
    SetMode((enum MODE)mode);

    //step4 aif
    m_minAIF = minAIF;
    m_maxAIF = maxAIF;
    Neuron::MaxAIF = maxAIF;
    Neuron::MinAIF = minAIF;
    Neuron::MaxMemLength = memLen;
    m_maxNeuronNumber = maxNeuronNumber;

    return true;
}
void NeuronEngine::SetMode(MODE mode){

    switch(mode){
    case MODE_RBF:
    case MODE_KNN:
        m_mode = mode;
        break;
    }
}

int NeuronEngine::Mode(){
    return m_mode;
}
void NeuronEngine::resetEngine(){
    m_norm = NORM_L1;
    m_mode = MODE_RBF;
    m_lastNeuronIndex = 0;
    //reset AIF
    m_minAIF = 0;
    m_maxAIF = NEURON_DEFAULT_AIF_MAX;
    m_maxNeuronNumber = NEURON_DEFAULT_NUMBER;

    //clear neuronlist
    clearNeuronList();
    m_firingList.clear();

    //TODO:stop and reset all threads here.
}
Neuron* NeuronEngine::ReadNeuron(int index){
    return m_neuronList.at(index);
}
int NeuronEngine::Learn(int cat, uint8_t *vec, int len){
    int ret = 0;
    if(vec==NULL || len==0){
        return -1;
    }
    switch(m_mode){
    case MODE_RBF:
    case MODE_KNN:
        /* TODO: according to the document and test cases,
         * when engine learing in KNN mode, it is the same
         * as RBF. AIF is working.
         * But in the Recognition mode, AIF will be disabled.
         * FIXME: try more test cases on hw to figure out if it works.
         */
        ret = learnRBF(cat,vec,len);
        break;
    }
    return ret;
}

int NeuronEngine::Classify(uint8_t vec[], int len, uint8_t nid[], int *nidLen)
{
    vector<Neuron* >::iterator iter;
    int index=0;
    int ret = 0;

    if(vec==NULL || len==0){
        return 0;
    }

    switch(m_mode){
    case MODE_RBF:
        ret = classifyRBF(vec,len);
        break;
    case MODE_KNN:
        ret = classifyKNN(vec,len);
        break;
    }

    if(nid && nidLen){
        for(iter = m_firingList.begin();iter<m_firingList.end();iter++){
            Neuron* ptr = *iter;
            nid[index] = ptr->index();
            index++;
        }
        *nidLen = m_firingList.size();
    }

    return ret;
}

int NeuronEngine::Classify(uint8_t vec[], int len){
    return Classify(vec,len,NULL,NULL);
}

int NeuronEngine::NeuronSize() const{
    return m_neuronList.size();
}

int NeuronEngine::learnRBF(int cat, uint8_t *vec, int len){
    vector<Neuron*>::iterator iter = m_neuronList.begin();
    int dist=0;
    int minDist = NEURON_DEFAULT_AIF_MAX;
    bool isExist=false;

    //step1 clear the firing list
    m_firingList.clear();

    //step2 traversing all neurons
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        Neuron * ptr = *iter;
        if(!ptr){
            continue;
        }

        //calc the distance
        if(checkNeuronFiring(ptr,vec,len,&dist))
        {/**Firing!!!**/
            m_firingList.push_back(ptr);
            if(ptr->cat() == cat){
                //if it exists in its own neuron. do not create new neuron
                isExist=true;
            }else{
                //since you are firing, shrink your AIF.
                ptr->setAIF(dist);
            }
        }
        if(dist<minDist){
            minDist = dist;
        }
    }

    //step3. process the new input
    if(!isExist ){
        createNeuron(cat,minDist,vec,len);
    }
    return m_neuronList.size();
}

int NeuronEngine::classifyRBF(uint8_t vec[], int len)
{
    vector<Neuron*>::iterator iter = m_neuronList.begin();
    int dist=0;
    int minDist = NEURON_DEFAULT_AIF_MAX;
    int retCat=0;

    //step1 clear the firing list
    m_firingList.clear();

    //step2 traversing all neurons
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        Neuron * ptr = *iter;
        if(!ptr){
            continue;
        }

        //calc the distance
        if(checkNeuronFiring(ptr,vec,len,&dist))
        {//firing
            if(dist<minDist){//find the most close neuron
                minDist = dist;
                retCat = ptr->cat();
            }
            m_firingList.push_back(ptr);
            ptr->m_firing = dist;//update the firing distance
        }
    }

    return retCat;
}
int NeuronEngine::classifyKNN(uint8_t vec[], int len){
    vector<Neuron*>::iterator iter = m_neuronList.begin();
    int dist=0;
    int minDist = NEURON_DEFAULT_AIF_MAX;
    int retCat=0;

    //step1 clear the firing list
    m_firingList.clear();

    //step2 traversing all neurons
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        Neuron * ptr = *iter;
        if(!ptr){
            continue;
        }

        //calc the distance
        checkNeuronFiring(ptr,vec,len,&dist);//no firing checking.
        if(dist<minDist){//find the most close neuron
            minDist = dist;
            retCat = ptr->cat();
        }
        ptr->m_firing = dist;//save the dist
        m_firingList.push_back(ptr);
    }

    /*TODO: add a post processing function to sort the result*/
    sort(m_firingList.begin(),m_firingList.end(),neuron_compare);

    return retCat;
}
bool NeuronEngine::checkNeuronFiring(Neuron *nptr, uint8_t *vec, int len,int* ptrDist)
{
    int dist=0;

    switch(m_norm){
    case NORM_L1:
        dist = nptr->calcDistanceL1(vec,len);
        break;
    case NORM_LSUP:
        dist = nptr->calcDistanceLsup(vec,len);
        break;
    }

    *ptrDist = dist;
    if(dist<nptr->aif()){
        return true;
    }
    return false;
}
bool NeuronEngine::createNeuron(int cat, int aif, uint8_t vec[], int len){
    if(m_neuronList.size()>=m_maxNeuronNumber){
        return false;
    }
    //create neurons only if not exist.
    Neuron * nptr = new Neuron(m_lastNeuronIndex);
    m_lastNeuronIndex++;

    //init the neuron
    nptr->init(cat,vec,len);
    nptr->setAIF(aif);

    //append the new neuron
    m_neuronList.push_back(nptr);
    return true;
}

void NeuronEngine::clearNeuronList()
{
    vector<Neuron*>::iterator iter = m_neuronList.begin();
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        Neuron * ptr = *iter;
        if(ptr){
            delete(ptr);
        }
    }
    m_neuronList.clear();
}

bool neuron_compare(Neuron* a,Neuron* b){
    return a->firing() < b->firing();
}

int Neuron::MaxMemLength=NEURON_DEFAULT_MEM_LENGTH;
int Neuron::MaxAIF = NEURON_DEFAULT_AIF_MAX;
int Neuron::MinAIF = 0;
Neuron::Neuron(int index):m_vecMem(NULL),m_memLen(0),m_aif(0),m_firing(0),m_cat(0)
{
    m_index=index;
}
Neuron::~Neuron()
{
    reset();
}

void Neuron::reset(){
    //clear the memory
    if(m_vecMem!=NULL){
        free(m_vecMem);
    }
    m_vecMem=NULL;
    m_memLen=0;

    m_firing = 0;
    m_cat=0;

    //set aif;
    setAIF(0);
}
bool Neuron::init(int cat, uint8_t src_vector[], int src_len){
    if(src_vector==NULL ||
       src_len>MaxMemLength ||
       src_len<=0){
        return false;
    }

    //reset the neuron
    reset();

    //malloc the memory
    m_vecMem = (uint8_t*)malloc(MaxMemLength);
    if(m_vecMem==NULL){
        return false;
    }
    memset(m_vecMem,0,MaxMemLength);

    //copy the vector to my vector.
    memcpy(m_vecMem,src_vector,src_len);
    m_memLen = src_len;
    m_cat = cat;

    return true;
}

void Neuron::setAIF(int aif){
    if(m_aif<MinAIF){
        m_aif = MinAIF;
    }else if(m_aif>MaxAIF){
        m_aif = MaxAIF;
    }else{
        m_aif = aif;
    }
}
int Neuron::aif(){
    return m_aif;
}

int Neuron::calcDistanceL1(uint8_t *src_vector, int src_len){
    int dist = 0;
    int len = 0;

    //check input
    if(src_vector==NULL ||
       src_len>MaxMemLength || src_len<0){
        return -1; //distance cannot be smaller than zero.
    }

    //check the array size.
    len = (src_len>m_memLen)?src_len:m_memLen;
    for(int i=0;i<len;i++){
        if(i<src_len){
            dist = dist + abs(src_vector[i]-m_vecMem[i]);
        }else{
            dist = dist + abs(m_vecMem[i]);
        }
    }
    return dist;
}
int Neuron::calcDistanceLsup(uint8_t *src_vector, int src_len){
    int dist = 0;
    int len = 0;
    int val=0;

    //check input
    if(src_vector==NULL ||
       src_len>MaxMemLength ||  src_len<=0){
        return -1; //distance cannot be smaller than zero.
    }

    //check the array size.
    len = (src_len>m_memLen)?src_len:m_memLen;
    for(int i=0;i<len;i++){
        if(i<src_len){
            val = abs(src_vector[i]-m_vecMem[i]);
        }else{
            val = abs(m_vecMem[i]);
        }
        if(val>dist){
            dist = val;
        }
    }
    return dist;
}

int Neuron::firing(){
    return m_firing;
}

int Neuron::cat(){
    return m_cat;
}

int Neuron::index(){
    return m_index;
}
