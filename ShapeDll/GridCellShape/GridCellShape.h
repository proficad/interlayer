#pragma once
#include <string>

extern "C" _declspec(dllexport) bool LoadEclipseFile(const std::string& filename, const std::string& outputfilename);
extern "C" _declspec(dllexport) bool Tracking(const std::string& FaceFileName, const std::string& GridFileName, const std::string& SaveFileName);
extern "C" _declspec(dllexport) bool Tracking2(const std::string& GridFileName, const std::string& SaveFileName);
extern "C" _declspec(dllexport) bool cExportTriangle(const std::string& FaceFileName, const std::string& SaveFileName, int divide, int index);
extern "C" _declspec(dllexport) bool Operation2(const std::string& filename1, const std::string& filename2, const std::string& savefilename, 
											   const std::string& operator1, const std::string& operator2, const std::string& operator0, float para1, float para2); 
extern "C" _declspec(dllexport) bool Operation1(const std::string& filename1, float para, const std::string& savefilename, const std::string& operator0); 
extern "C" _declspec(dllexport) bool Average(const std::string& filename, const std::string& savename);

//导出文件
extern "C" _declspec(dllexport) bool SetGridFileNameNet( const std::string& filename );
extern "C" _declspec(dllexport) bool SetRedefineNet(bool isRedefine);
extern "C" _declspec(dllexport) bool SetWritePropertyNet(bool isWrite);
extern "C" _declspec(dllexport) bool SetRedefinePropertyNet(bool isWrite);

extern "C" _declspec(dllexport) bool	AddPropertyFileNameNet( const std::string& filename );
extern "C" _declspec(dllexport) bool	ClearPropertyFileNameNet( );
extern "C" _declspec(dllexport) bool	PropertyFileNameRemoveAtNet(int index);
extern "C" _declspec(dllexport) int		GetPropertyFileNameCountNet();

extern "C" _declspec(dllexport) bool	AddTrackFileNameNet( const std::string& filename );
extern "C" _declspec(dllexport) bool	ClearTrackFileNameNet( );
extern "C" _declspec(dllexport) bool	TrackFileNameRemoveAtNet(int index);
extern "C" _declspec(dllexport) int		GetTrackFileNameCountNet();

extern "C" _declspec(dllexport) bool	WriteNet(const std::string& filename);

extern "C" _declspec(dllexport) bool	WriteCenterPointNet(const std::string& filename, const std::string& saveFileName, bool isWriteTick, int number );
