#pragma once

struct IQuad {
  static const float vertices3D[12];
  static const float vertices2D[8];
  static const int indices[6];
  static const float uvCoords[8];
};


const float IQuad::vertices3D[12] = {
     1.0f,  1.0f, 0.0f,  
     1.0f,  0.0f, 0.0f,  
     0.0f,  0.0f, 0.0f,  
     0.0f,  1.0f, 0.0f    
};

const float IQuad::vertices2D[8] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f
};

const int IQuad::indices[6] = {
    0, 1, 3, 1, 2, 3 
};

const float IQuad::uvCoords[8] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 1.0f 
};

struct Quad {
  static const float vertices3D[18];
  static const float vertices2D[12];
  static const float uvCoords[12];
};

const float Quad::vertices3D[18] = {
    0.0f, 1.0f, 0.0f, 
    1.0f, 0.0f, 0.0f, 
    0.0f, 0.0f, 0.0f, 

    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f, 
};

const float Quad::vertices2D[12] = {
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

const float Quad::uvCoords[12] = {
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};






