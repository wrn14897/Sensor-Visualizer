#include "sine.h"

int abs(int i){
  return (i < 0 ? -i : i);
}

int sin(int degree){
  if (degree >= 0){
    degree = degree % 360;
    return sineValueTable[degree];
  }
  else{
    degree = (degree * -1) % 360;
    return (-1) * sineValueTable[degree];
  }
}

int cos(int degree){
  if (degree < 0)
    degree *= -1;
  degree = degree % 360;

  return sin(degree + 90); 
}
