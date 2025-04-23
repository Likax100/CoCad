#pragma once
#include <iostream>
#include <glm/glm.hpp>

namespace CoCad {  
  class Color {
  
    public: 
      static glm::vec3 RGB(int r, int g, int b);

    private:
      Color();
      ~Color();
  };
}

