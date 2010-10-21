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

extern "C" _declspec(dllexport) bool cExportTriangle(const string& FaceFileName, const string& SaveFileName, int divide, int index)
{
	//GridTracking ^gtk = gcnew GridTracking();
	EclipseWrite ^triExport = gcnew EclipseWrite();
	String ^face = gcnew String(FaceFileName.c_str());
	String ^save = gcnew String(SaveFileName.c_str());

	bool IsTrue = false;
	//IsTrue = gtk->Tracking(grid, save);
	IsTrue = triExport->WriteTrianglePoint( save, face, divide, index);

	return IsTrue;
}

extern "C" _declspec(dllexport) bool Operation2(const string& filename1, const string& filename2, const string& savefilename, 
			   const string& operator1, const string& operator2, const string& operator0, float para1, float para2)
{
	String ^file1 = gcnew String(filename1.c_str());
	String ^file2 = gcnew String(filename2.c_str());
	String ^savefile = gcnew String(savefilename.c_str());
	String ^op1 = gcnew String(operator1.c_str());
	String ^op2 = gcnew String(operator2.c_str());
	String ^op0 = gcnew String(operator0.c_str());
	bool IsTrue = false;
	PropertyOperation ^op = gcnew PropertyOperation();
	IsTrue = op->Operation(file1, file2, savefile, op1, op2, op0, para1, para2);
	return IsTrue;
}

extern "C" _declspec(dllexport) bool Operation1(const string& filename1, float para, const string& savefilename, const string& operator0)
{
	String ^file1 = gcnew String(filename1.c_str());
	String ^savefile = gcnew String(savefilename.c_str());

	String ^op0 = gcnew String(operator0.c_str());
	bool IsTrue = false;
	PropertyOperation ^op = gcnew PropertyOperation();
	IsTrue = op->Operation(file1, para, savefile,  op0 );
	return IsTrue;
}

extern "C" _declspec(dllexport) bool Average(const string& filename, const string& savename)
{
	String ^file = gcnew String(filename.c_str());
	String ^savefile = gcnew String(savename.c_str());

	bool IsTrue = false;
	PropertyAverage ^pa = gcnew PropertyAverage();
	IsTrue = pa->Average(file, savefile);
	return IsTrue;
}