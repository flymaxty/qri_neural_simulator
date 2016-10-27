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

static bool neuron_compare(neuron_data* a,neuron_data* b);

NeuronEngine::NeuronEngine()
{
    resetEngine();
}
NeuronEngine::~NeuronEngine(){
    resetEngine();
}

bool NeuronEngine::Begin(){
    return Begin(MODE_RBF,NORM_L1,0,NEURON_AIF_MAX);
}

bool NeuronEngine::Begin(int mode,int norm, int minAIF,int maxAIF)
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
    m_lastCreateNeuron = 0;
    //reset AIF
    m_minAIF = 0;
    m_maxAIF = NEURON_AIF_MAX;

    //clear neuronlist
    clearNeuronList();
    m_firingList.clear();

    //TODO:stop and reset all threads here.
}
const neuron_data* NeuronEngine::ReadNeuron(int index)
{
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
    vector<neuron_data* >::iterator iter;
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
            neuron_data* ptr = *iter;
            nid[index] = ptr->index;
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

bool NeuronEngine::checkNeuronFiring(neuron_data *ptr, uint8_t *vec, int len,int* ptrDist)
{
    int dist=0;

    switch(m_norm){
    case NORM_L1:
        dist = neuron_distance_L1(ptr,vec,len);
        break;
    case NORM_LSUP:
        dist = neuron_distance_Lsup(ptr,vec,len);
        break;
    }

    *ptrDist = dist;
    if(dist<ptr->aif){
        return true;
    }
    return false;
}
int NeuronEngine::learnRBF(int cat, uint8_t *vec, int len){
    vector<neuron_data*>::iterator iter = m_neuronList.begin();
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
            m_firingList.push_back(ptr);
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
        m_neuronList.push_back(nptr);
    }
    return m_neuronList.size();
}

int NeuronEngine::classifyRBF(uint8_t vec[], int len)
{
    vector<neuron_data*>::iterator iter = m_neuronList.begin();
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
            m_firingList.push_back(ptr);
            ptr->firing = dist;
        }
    }

    return retCat;
}
int NeuronEngine::classifyKNN(uint8_t vec[], int len){
    vector<neuron_data*>::iterator iter = m_neuronList.begin();
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
        checkNeuronFiring(ptr,vec,len,&dist);//no firing checking.
        if(dist<minDist){//find the most close neuron
            minDist = dist;
            retCat = ptr->cat;
        }
        ptr->firing = dist;//save the dist
        m_firingList.push_back(ptr);
    }

    /*TODO: add a post processing function to sort the result*/
    sort(m_firingList.begin(),m_firingList.end(),neuron_compare);

    return retCat;
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


void NeuronEngine::clearNeuronList()
{
    vector<neuron_data*>::iterator iter = m_neuronList.begin();
    for(iter = m_neuronList.begin();iter<m_neuronList.end();iter++){
        neuron_data * ptr = *iter;
        if(ptr){
            free(ptr);
        }
    }
    m_neuronList.clear();
}

bool neuron_compare(neuron_data* a,neuron_data* b){
    return a->firing < b->firing;
}
