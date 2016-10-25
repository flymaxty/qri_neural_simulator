#include <QCoreApplication>
#include "neuronengine.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    NeuronEngine engine;
    engine.begin();


    return a.exec();
}
