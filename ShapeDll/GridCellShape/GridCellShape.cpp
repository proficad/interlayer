#include <string>
#include <vcclr.h>
using std::string;

#using "../../debug/EclipseGridIO.dll"

using namespace EclipseGridIO;
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