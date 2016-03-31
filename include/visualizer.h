#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "adc.h"
#include "console.h"
#include "gl.h"
#include "keyboard.h"
#include "sine.h"

void initVisualizerWindow();
void visualizerStart(int enableVisualizer, int mode, int displayVal);
void clearVisualArea();

#endif
