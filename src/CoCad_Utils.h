#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <mutex>
#include <deque>

// --------- FILES ----------- //
class FileUtil {
  public:
    static std::string ReadFile(const char* file_path);
    static std::vector<std::string> ReadFileLines(const char* file_path); 
    
    static bool FileExists(const char* file_path);
    static bool MatchesExt(const char* file_path, const char* extension);

  private:
};

// --------- STRING ----------- //
class StringUtil {
  public:
    static std::vector<std::string> SplitString(std::string str, std::string delchar);

  private:
};

// --------- MATH ----------- //
class MathUtil {
  public:
    static double Clamp(double x, double min, double max);
  private:
};


