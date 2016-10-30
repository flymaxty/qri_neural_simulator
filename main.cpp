#include <QCoreApplication>
#include "neuronengine.h"

#include <stdio.h>
#include <stdint.h>

NeuronEngine engine;
#define LEN 2
typedef struct case_input{
  int cat;
  uint8_t pattern[LEN];
}case_input_t;

#define PATTERN_LEN 10
case_input_t  case_0[PATTERN_LEN]={
  {1,{0,0}},
  {1,{5,0}}
};
case_input_t  case_1[PATTERN_LEN]={
  {1,{0,0}},
  {2,{5,0}}
};

case_input_t  case_2[PATTERN_LEN]={
  {1,{0,0}},
  {2,{5,0}},
  {1,{12,0}}
};

case_input_t  case_3[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {3,{4,0}}
};

case_input_t  case_4[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {3,{5,8}}
};

case_input_t  case_5[PATTERN_LEN]={
  {1,{0,0}},
  {2,{5,8}},
  {3,{10,0}}
};

case_input_t  case_6[PATTERN_LEN]={
  {1,{0,0}},
  {2,{5,8}},
  {3,{15,8}}
};

case_input_t  case_7[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {3,{5,8}},
  {4,{10,8}}
};

case_input_t  case_8[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {3,{5,8}},
  {2,{10,8}}
};


case_input_t  case_9[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {2,{10,10}}
};


case_input_t  case_10[PATTERN_LEN]={
  {1,{0,0}},
  {2,{10,0}},
  {2,{10,10}},
  {3,{5,10}},
};

void printCase(case_input_t * c);
void testCase(case_input_t *cases,int len);
void displayNeurons();


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //engine.Begin(NeuronEngine::MODE_RBF);
    //engine.Begin(NeuronEngine::MODE_KNN);

    printf("\nRunning test case 0...");
    engine.Begin();
    testCase(case_0,2);

    //test case 1
    printf("\nRunning test case 1...");
    engine.Begin();
    testCase(case_1,2);

    //test case 2
    printf("\nRunning test case 2...");
    engine.Begin();
    testCase(case_2,3);

    //test case 3
    printf("\nRunning test case 3...");
    engine.Begin();
    testCase(case_3,3);

    //test case 4
    printf("\nRunning test case 4...");
    engine.Begin();
    testCase(case_4,3);

    //test case 5
    printf("\nRunning test case 5...");
    engine.Begin();
    testCase(case_5,3);

    //test case 6
    printf("\nRunning test case 6...");
    engine.Begin();
    testCase(case_6,3);

    //test case 7
    printf("\nRunning test case 7...");
    engine.Begin();
    testCase(case_7,4);

    //test case 8
    printf("\nRunning test case 8...");
    engine.Begin();
    testCase(case_8,4);

    //test case 9
    printf("\nRunning test case 9...");
    engine.Begin();
    testCase(case_9,3);

    //test case 10
    printf("\nRunning test case 10...");
    engine.Begin(NeuronEngine::MODE_KNN);
    testCase(case_10,4);

    //test a basic vec
    uint8_t pattern []={5,6};
    int len;
    uint8_t nid[20];
    int cat = engine.Classify(pattern,2,nid,&len);

    printf("\n\nMode:%d, Recognized: %d, firing neuron number: %d\n",engine.Mode(),cat,len);

    for(int i=0;i<len;i++){
        Neuron * ptr = engine.ReadNeuron(nid[i]);
        printf("[%d] index=%d, cat=%d, distance=%d\n",
               i,
               ptr->index(),
               ptr->cat(),
               ptr->firing());
    }

    printf("\n==========================Test cases done===============================\n");
    return a.exec();
}

void printCase(case_input_t * c){
  int i=0;
  printf("Category:%d, Pattern:[",c->cat);
  for(i=0;i<LEN;i++){
    printf("%d,",(uint8_t)c->pattern[i]);
  }
  printf("]\n");
}

void testCase(case_input_t *cases,int len){
  printf("Learning:\n");
  for(int i=0;i<len;i++){
    int cat=cases[i].cat;
    if(cat==0)continue;

    engine.Learn(cat,cases[i].pattern,LEN);
    printCase(&cases[i]);
  }
  displayNeurons();
}

void displayNeurons(){
    int count = engine.NeuronSize();
    printf("\nDisplaying Neurons:%d\n",count);
    for(int i=0;i<count;i++){
        Neuron * ptr = engine.ReadNeuron(i);
        const uint8_t *buffer = ptr->buffer();
        printf("[%d]\tcat:%d,\taif:%d,\tvector:",
               ptr->index(),
               ptr->cat(),
               ptr->aif());
        for(int k=0;k<ptr->length();k++){
            printf("%d ",(uint8_t)buffer[k]);
        }
        printf("\n");
    }
}
