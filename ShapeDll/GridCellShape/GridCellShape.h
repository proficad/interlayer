#pragma once
#include <string>
extern "C" _declspec(dllexport) bool LoadEclipseFile(const std::string& filename, const std::string& outputfilename);
extern "C" _declspec(dllexport) bool Tracking(const std::string& FaceFileName, const std::string& GridFileName, const std::string& SaveFileName);