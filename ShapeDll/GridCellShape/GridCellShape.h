#pragma once
#include <string>

extern "C" _declspec(dllexport) bool LoadEclipseFile(const std::string& filename, const std::string& outputfilename);
extern "C" _declspec(dllexport) bool Tracking(const std::string& FaceFileName, const std::string& GridFileName, const std::string& SaveFileName);
extern "C" _declspec(dllexport) bool Tracking2(const string& GridFileName, const string& SaveFileName);
extern "C" _declspec(dllexport) bool cExportTriangle(const string& FaceFileName, const string& SaveFileName, int divide, int index);
extern "C" _declspec(dllexport) bool Operation2(const string& filename1, const string& filename2, const string& savefilename, 
											   const string& operator1, const string& operator2, const string& operator0, float para1, float para2); 
extern "C" _declspec(dllexport) bool Operation1(const string& filename1, float para, const string& savefilename, const string& operator0); 
extern "C" _declspec(dllexport) bool Average(const string& filename, const string& savename);