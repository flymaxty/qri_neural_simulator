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

#ifndef NEURON_H
#define NEURON_H
#include <stdint.h>

#define NEURON_VECTOR_MAX_LENGTH 4096
#define NEURON_AIF_MAX (0x4000)

typedef struct neuron_data_s{
    int index;//the index of this neuron
    int aif;
    int cat;
    int firing;//the recent firing distance
    int len;
    uint8_t vector[NEURON_VECTOR_MAX_LENGTH];
}neuron_data;

/* init the neuron
 * @n, input neuron,
 * @src_vector,the input vector for this neuron
 * @src_len, the length of the vector
 * @cat, the categroy of this neuron
 * @aif, the influence field of this neuron.
 * return:
 * @true, if init correctly
 */
bool neuron_init(neuron_data* n, uint8_t *src_vector,int src_len, int cat=0, int aif=NEURON_AIF_MAX);

/* calc the L1 distance of this neuron
 * @n, input neuron,
 * @src_vector,the input vector for this neuron
 * @src_len, the length of the vector
 * return:
 * @the L1 distance of this neuron
 */
int neuron_distance_L1(neuron_data* n, uint8_t *src_vector,int src_len);

/* calc the L_sup distance of this neuron
 * @n, input neuron,
 * @src_vector,the input vector for this neuron
 * @src_len, the length of the vector
 * return:
 * @the L_sup distance of this neuron
 */
int neuron_distance_Lsup(neuron_data* n, uint8_t *src_vector,int src_len);


#endif // NEURON_H
