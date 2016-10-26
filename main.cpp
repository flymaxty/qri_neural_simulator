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

void printCase(case_input_t * c);
void testCase(case_input_t *cases,int len);
void displayNeurons();


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    engine.Begin();

    printf("\nRunning test case 0...");
    testCase(case_0,2);

    //test case 1
    printf("\nRunning test case 1...");
    testCase(case_1,2);

    //test case 2
    printf("\nRunning test case 2...");
    testCase(case_2,3);

    //test case 3
    printf("\nRunning test case 3...");
    testCase(case_3,3);

    //test case 4
    printf("\nRunning test case 4...");
    testCase(case_4,3);

    //test case 5
    printf("\nRunning test case 5...");
    testCase(case_5,3);

    //test case 6
    printf("\nRunning test case 6...");
    testCase(case_6,3);

    //test case 7
    printf("\nRunning test case 7...");
    testCase(case_7,4);

    //test case 8
    printf("\nRunning test case 8...");
    testCase(case_8,4);

    //test case 9
    printf("\nRunning test case 9...");
    testCase(case_9,3);

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
  engine.Forget();
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
