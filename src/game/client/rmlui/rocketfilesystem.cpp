#include "rocketfilesystem.h"

#include "FileSystem.h"
// #include "interface.h"
#include "tier2/tier2.h"

RocketFileSystem RocketFileSystem::m_Instance;

RocketFileSystem::RocketFileSystem() { }

Rml::FileHandle RocketFileSystem::Open(const Rml::String &path)
{
    Rml::String rocketPath = "rocketui/";
    rocketPath += path;
	return (Rml::FileHandle)g_pFullFileSystem->Open(rocketPath.c_str(), "rb", "GAME");
}

void RocketFileSystem::Close(Rml::FileHandle file)
{
    g_pFullFileSystem->Close( (FileHandle_t)file );
}

size_t RocketFileSystem::Read(void *buffer, size_t size, Rml::FileHandle file)
{
    return g_pFullFileSystem->Read( buffer, size, (FileHandle_t)file );
}

bool RocketFileSystem::Seek(Rml::FileHandle file, long offset, int origin)
{
    g_pFullFileSystem->Seek( (FileHandle_t)file, offset, (FileSystemSeek_t)origin );
    return true;
}

size_t RocketFileSystem::Tell(Rml::FileHandle file)
{
    return g_pFullFileSystem->Tell( (FileHandle_t)file );
}

size_t RocketFileSystem::Length(Rml::FileHandle file)
{
    return g_pFullFileSystem->Size( (FileHandle_t)file );
}
