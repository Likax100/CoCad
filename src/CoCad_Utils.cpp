#include "CoCad_Utils.h"

// ========================== FILE UTILS ============================= //
std::string FileUtil::ReadFile(const char* file_path) {
  std::ifstream file(file_path);
  if(!file.is_open()) {
    std::cerr << "[UTILS::readFile] Could not open file." << std::endl;
    return "";
  }

  std::string contents;
  file.seekg(0, file.end);
  size_t size = file.tellg();
  file.seekg(0, file.beg);

  contents.resize(size);
  file.read(&contents[0], size);
  file.close();

  return contents;
}

std::vector<std::string> FileUtil::ReadFileLines(const char* file_path) {
  std::string file_content = ReadFile(file_path);
  std::vector<std::string> file_lines;
  
  std::istringstream stream(file_content);

  std::string line;
  while (std::getline(stream, line)) { file_lines.push_back(line); }
  
  return file_lines;
}

bool FileUtil::FileExists(const char* file_path) {
  if (FILE* file = fopen(file_path, "r")) {
    fclose(file);
    return true;
  } else { return false; }
}

bool FileUtil::MatchesExt(const char* file_path, const char* extension) {
  std::filesystem::path file = file_path;
  if (file.extension() == extension) {
    return true;
  } else { return false; }
}


// ========================== STRING UTILS ============================= //

std::vector<std::string> StringUtil::SplitString(std::string str, std::string delchar) {
  std::vector<std::string> result;
  std::string constituent;
  
  size_t position = str.find(delchar); 
  
  while(position != std::string::npos) {
    constituent = str.substr(0, position);
    result.push_back(constituent);
    str.erase(0, position + delchar.length());
    position = str.find(delchar);
  }

  result.push_back(str);
  return result;
}

// ========================== MATH UTILS ============================= //
double MathUtil::Clamp(double x, double min, double max) {
  if (x <= min) { return min; }
  else if (x >= max) { return max; }
  else { return x; }
}

// ========================== NET UTILS ============================= //






