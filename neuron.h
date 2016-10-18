#ifndef NEURON_H
#define NEURON_H


#define NEURON_VECTOR_MAX_LENGTH 4096
#define NEURON_AIF_MAX (0xFFFF-1)
class Neuron : public QObject
{
    Q_OBJECT
public:
    explicit Neuron(QObject *parent = 0);

    /**
     * @update this neuron's with the new one
     * @src_vecotr,the input vector
     * @src_len,the length of the input vector
     * @cat,category of this neuron
     * @aif,the pre-calculated aif value from the NeuronQuad
     */
    void init(int cat,int aif,uint8_t *src_vector,int src_len);

    /**
     * @Recognize the input vector.
     * @src_vector, the input vector
     * @src_len, the length of the input vector
     * @return -1, not recognized. [return>=0] is the distance of the input vector.
     **/
    int recognize(uint8_t * src_vector,int src_len);

    //aif value.
    void setAIF(int aif);
    int aif();

    //category of this neuron.
    int category();
protected:
    /**
     * @Calculate the "distance" between this neuron and the input vector
     * @src_vecotr,the input vector
     * @src_len,the length of the input vector
     */
    int distance(uint8_t *src_vector,int src_len);

    /**
     * @clean this neuron, all set to zero.
     */
    void reset();
protected:
    uint8_t m_vector[NEURON_VECTOR_MAX_LENGTH];
    int m_vectorLen;//the real length of the vector
    int m_cat;
    int m_aif;//the AIF distance of this neuron

};

#endif // NEURON_H
