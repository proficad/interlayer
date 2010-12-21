#include <string>
#include <Windows.h>
#include <vcclr.h>
using std::string;

//#ifdef _DEBUG
//#using "../../debug/EclipseGridIO.dll"
//#using "../../debug/RunGridCellThick.dll"
//#else
#using "../../release/EclipseGridIO.dll"
#using "../../release/RunGridCellThick.dll"
//#endif

using namespace EclipseGridIO;
using namespace RunGridCellThick;
using namespace System;

__gc class GlobalObjects 
{
public:
	//static EclipseWrite *ptr;   // OK
	static EclipseWrite *eclipseWriter = new EclipseWrite();   // OK
	static RunTrackingBar *trackBar = new RunTrackingBar();
}; 


extern "C" _declspec(dllexport) bool SetGridFileNameNet( const std::string& filename )
{
	bool rl = true;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	//EclipseWriter::Instance()->m_writer;//->GridFileName = file;
	op->eclipseWriter->GridFileName = file;
	return rl;
}

extern "C" _declspec(dllexport) bool SetRedefineNet(bool isRedefine)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isRedefine)
		op->eclipseWriter->WriteCarfinEnable();
	else
		op->eclipseWriter->WriteCarfinDisable();
	return rl;
};

extern "C" _declspec(dllexport) bool SetWritePropertyNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
		op->eclipseWriter->WritePropertyEnable();
	else
		op->eclipseWriter->WritePropertyDisable();
	return rl;
};

extern "C" _declspec(dllexport) bool SetRedefinePropertyNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
		op->eclipseWriter->WriteCarfinPropertyEnable();
	else
		op->eclipseWriter->WriteCarfinPropertyDisable();
	return rl;
};

extern "C" _declspec(dllexport) bool SetTransRateNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
	{
		op->eclipseWriter->WriteMultEnable();
	}
	else
		op->eclipseWriter->WriteMultDisable();
	return rl;
}
//属性
extern "C" _declspec(dllexport) bool	AddPropertyFileNameNet( const std::string& filename )
{
	bool rl = false;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->AddPropertyFileName(file);
	return rl;
}

extern "C" _declspec(dllexport) bool	ClearPropertyFileNameNet( )
{
	bool rl = false;
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->ClearPropertyFileName();
	return rl;
}

extern "C" _declspec(dllexport) bool	PropertyFileNameRemoveAtNet(int index)
{
	bool rl = false;
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->PropertyFileNameRemoveAt(index);
	return rl;
}

extern "C" _declspec(dllexport) int		GetPropertyFileNameCountNet()
{
	int t_size = 0;
	GlobalObjects *op = new GlobalObjects;
	t_size = op->eclipseWriter->GetPropertyFileNameCount();
	return t_size;
}
//夹层网格
extern "C" _declspec(dllexport) bool	AddTrackFileNameNet( const std::string& filename )
{
	bool rl = false;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->AddTrackFileName(file);
	return rl;
}

extern "C" _declspec(dllexport) bool	ClearTrackFileNameNet( )
{
	bool rl = false;
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->ClearTrackFileName();
	return rl;
}

extern "C" _declspec(dllexport) bool	TrackFileNameRemoveAtNet(int index)
{
	bool rl = false;
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->TrackFileNameRemoveAt(index);
	return rl;
}

extern "C" _declspec(dllexport) int		GetTrackFileNameCountNet()
{
	int t_size = 0;
	GlobalObjects *op = new GlobalObjects;
	t_size = op->eclipseWriter->GetTrackFileNameCount();
	return t_size;
}
//导出数据
extern "C" _declspec(dllexport) bool	WriteNet(const std::string& filename)
{
	bool rl = false;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->Write(file);
	return rl;
}

extern "C" _declspec(dllexport) bool	WriteCenterPointNet(const std::string& filename, const std::string& saveFileName, bool isWriteTick, int number )
{
	bool rl = false;
	String *file = new String(filename.c_str());
	String *save = new String(saveFileName.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->eclipseWriter->WriteCenterPoint(file, save, isWriteTick, number);
	return rl;
}

extern "C" _declspec(dllexport) bool AddItem(const std::string& facefile, const std::string& savefile)
{
	bool rl = false;
	String *file = new String(facefile.c_str());
	String *save = new String(savefile.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->trackBar->ADDItem(file, save);
	return rl;
}

extern "C" _declspec(dllexport) bool SetGridFileName(const std::string& filename)
{
	bool rl = false;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	rl = op->trackBar->SetGridFileName(file);
	return rl;
}

extern "C" _declspec(dllexport) bool RunTrack()
{
	bool rl = false;
	GlobalObjects *op = new GlobalObjects;
	rl = op->trackBar->RunEXE();
	op->trackBar->ClearItem();
	return rl;
}

extern "C" _declspec(dllexport) bool SetWriteChangePropertyNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
		op->eclipseWriter->WriteChangePropertyEnable();
	else
		op->eclipseWriter->WriteChangePropertyDisable();
	return rl;
}

extern "C" _declspec(dllexport) bool SetWriteRedefineMultNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
		op->eclipseWriter->WriteCarfinMultEnable();
	else
		op->eclipseWriter->WriteCarfinMultDisable();
	return rl;
}

extern "C" _declspec(dllexport) bool SetWriteRedefineChangePropertyNet(bool isWrite)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	if(isWrite)
		op->eclipseWriter->WriteCarfinChangePropertyEnable();
	else
		op->eclipseWriter->WriteCarfinChangePropertyDisable();
	return rl;
}

extern "C" _declspec(dllexport) bool SetChangePropertyValueNet(double mValue)
{
	bool rl = true;
	GlobalObjects *op = new GlobalObjects;
	op->eclipseWriter->ChangePropertyValue = mValue;
	return rl;
}

extern "C" _declspec(dllexport) bool SetChangePropertyNameNet(const std::string& filename)
{
	bool rl = true;
	String *file = new String(filename.c_str());
	GlobalObjects *op = new GlobalObjects;
	op->eclipseWriter->ChangePropertyName = file;
	return rl;
}