QT += core
QT -= gui

CONFIG += c++11

TARGET = qri_neural_simulator
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    neuron.cpp \
    neuronengine.cpp

HEADERS += \
    neuron.h \
    neuronengine.h
