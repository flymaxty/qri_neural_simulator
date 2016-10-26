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
#include "neuron.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//reset the input neuron
static void _neuron_reset(neuron_data *n);

void _neuron_reset(neuron_data *n){
    if(!n)return;

    //reset vectors
    memset(n->vector,0,NEURON_VECTOR_MAX_LENGTH);
    n->len = 0;

    n->cat=0;
    n->aif = NEURON_AIF_MAX;

    n->firing = 0;
}

bool neuron_init(neuron_data *n, uint8_t *src_vector, int src_len, int cat, int aif)
{
    if(n==NULL ||
       src_vector==NULL ||
       src_len>NEURON_VECTOR_MAX_LENGTH ||
       src_len<=0){
        return false;
    }

    //reset the neuron
    _neuron_reset(n);

    //copy the vector to my vector.
    memcpy(n->vector,src_vector,src_len);
    n->len = src_len;

    n->aif = aif;
    n->cat = cat;

    return true;
}
int neuron_distance_L1(neuron_data *n, uint8_t *src_vector, int src_len){
   int dist = 0;
   int len = 0;

   //check input
   if(n==NULL ||
      src_vector==NULL ||
      src_len>NEURON_VECTOR_MAX_LENGTH){
       return -1; //distance cannot be smaller than zero.
   }

   //check the array size.
   len = (src_len>n->len)?src_len:n->len;
   for(int i=0;i<len;i++){
       if(i<src_len){
           dist = dist + abs(src_vector[i]-n->vector[i]);
       }else{
           dist = dist + abs(n->vector[i]);
       }
   }
   return dist;
}

int neuron_distance_Lsup(neuron_data *n, uint8_t *src_vector, int src_len){
    int dist = 0;
    int len = 0;
    int val=0;

    //check input
    if(n==NULL ||
       src_vector==NULL ||
       src_len>NEURON_VECTOR_MAX_LENGTH){
        return -1; //distance cannot be smaller than zero.
    }

    //check the array size.
    len = (src_len>n->len)?src_len:n->len;
    for(int i=0;i<len;i++){
        if(i<src_len){
            val = abs(src_vector[i]-n->vector[i]);
        }else{
            val = abs(n->vector[i]);
        }
        if(val>dist){
            dist = val;
        }
    }
    return dist;
}
