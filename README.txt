Curie Neuron Simulator
This is a simulator for Curie NeuralMem function. With it, people can try many pattern matching ideas on PC or Mobile-Phone without flashing your Arduino101 again and again.
Different from Curie NeuralMem's hardware design, this simulator has no concept of "context", which is designed for isolating neurons from different usage scenarios. In our design, there is only ¡°Engine¡± for controlling all neurons in it. In this case, you can declare multiple Engines with the same capability and even let them run in multi-threads.
Here is an example of our Engine. Paste it into any C++ compatible IDE and try to run them on your PC.?
Example for RBF:
#include <stdio.h>
#include <stdint.h>
#include "neuronengine.h"

NeuronEngine engine;
void displayNeurons();
int main(int argc, char *argv[])
{
    const int LEN=5;
    const int K=2;
    int cat[LEN]={1,2,2,3,1};
    uint8_t pattern[LEN][K]={
        {0,0},
        {10,0},
        {5,10},
        {10,10},
        {8,8}
    };

    printf("\nRunning test case RBF...");
engine.Begin(NeuronEngine::MODE_RBF);

    for(int i=0;i<LEN;i++){
        engine.Learn(cat[i],pattern[i],K);
    }
    displayNeurons();

    //test a basic vec
    uint8_t data[]={5,6};
    int nidLen;
    uint8_t nid[20];
    int ret = engine.Classify(data,K,nid,&nidLen);

    printf("\n\nRecognized: %d, firing neuron number: %d\n",ret,nidLen);
    for(int i=0;i<nidLen;i++){
        const neuron_data * ptr = engine.ReadNeuron(nid[i]);
        printf("[%d] index=%d, cat=%d, distance=%d\n",
               i,
               ptr->index,
               ptr->cat,
               ptr->firing);
    }

    printf("\nTest cases done.\n");
    return a.exec();
}

void displayNeurons(){
    int count = engine.NeuronSize();
    printf("\nDisplaying Neurons:%d\n",count);
    for(int i=0;i<count;i++){
        const neuron_data * ptr = engine.ReadNeuron(i);
        printf("[%d]\tcat:%d,\taif:%d,\tvector:",
               ptr->index,
               ptr->cat,
               ptr->aif);
        for(int k=0;k<ptr->len;k++){
            printf("%d ",(uint8_t)ptr->vector[k]);
        }
        printf("\n");
    }
}
Here are some function descriptions of this simulator:
In our simulator, all functions are designed according to GV¡¯s Arduino101 CurieNeuron.h and Intel¡¯s PME project interfaces. You can refer to them if there is anything confuses you.s
Begin();
There are 2 versions of this function. In most cases, you only need to run ¡°Begin()¡±  before your case is OK.
While running it, all neurons and memory will be deleted and erase. So actually it looks like a reset.

SetMode()
This function is used for changing your learning and recognition mode with RBF or KNN. 

Learn();
Call this function for teaching your neurons a ¡°pattern¡± with a labeled value ¡°cat¡±. Leaning method is based on your MODE setting. Currently, RBF and KNN mode shares the same learning process, so does the hardware. 

Classify()
Call this function for recognize if a pattern belongs to some category your little neurons aware of. If recognized, a cat will be returned and you can also get more details if you input a big enough nid array.
In RBF mode, the returned nid array contains all neurons¡¯ index that fires during your recognition.
In KNN mode, the returned nid arrays contains all neurons¡¯ index but in an ascending order of the firing distance to your pattern.
ReadNeuron()
Call this function to access the Neuron according to its index. BTW, in our simulator, Neuron¡¯s index starts with 0 instead of 1 on hardware ?
