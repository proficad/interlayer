#include <string>
#include <Windows.h>
#include <vcclr.h>
using std::string;

#using "../../debug/EclipseGridIO.dll"
#using "../../debug/Track.dll"
//#using "../../debug/octc.dll"
//#using "../../debug/Microsoft.DirectX.dll"
using namespace EclipseGridIO;
using namespace Track;
//using namespace 
using namespace System;

extern "C" _declspec(dllexport)bool LoadEclipseFile(const string& filename, const string& outfilename)
{
	GridIO ^gio = gcnew GridIO();

	String ^input = gcnew String(filename.c_str());
	String ^ouput = gcnew String(outfilename.c_str());

	bool IsTrue = false;
	IsTrue = gio->LoadEclipseGridToFile(input,ouput);

	return IsTrue;
}

extern "C" _declspec(dllexport) bool Tracking(const string& FaceFileName, const string& GridFileName, const string& SaveFileName)
{
	GridTracking ^gtk = gcnew GridTracking();

	String ^face = gcnew String(FaceFileName.c_str());
	String ^grid = gcnew String(GridFileName.c_str());
	String ^save = gcnew String(SaveFileName.c_str());

	bool IsTrue = false;

	IsTrue = gtk->Tracking(face, grid, save);

	return IsTrue;
}

extern "C" _declspec(dllexport) bool Tracking2(const string& GridFileName, const string& SaveFileName)
{
	GridTracking ^gtk = gcnew GridTracking();

	String ^grid = gcnew String(GridFileName.c_str());
	String ^save = gcnew String(SaveFileName.c_str());

	bool IsTrue = false;
	IsTrue = gtk->Tracking(grid, save);

	return IsTrue;
}
