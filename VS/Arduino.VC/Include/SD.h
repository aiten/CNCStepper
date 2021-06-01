/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) Herbert Aitenbichler

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/

#pragma once

#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////

#define FILE_READ 1
#define FILE_WRITE 2

#define SDPATH "c:\\tmp\\SD"

////////////////////////////////////////////////////////////

class SDClass
{
public:

	SDClass() = default;

	bool begin(uint8_t) { return true; };

	class File open(const char* fileName, uint8_t mode = FILE_READ);

	bool remove(const char* fileName)
	{
		return ::remove(GetFilename(fileName)) == 0;
	}

	bool exists(const char* fileName)
	{
		return _access(GetFilename(fileName), 0) != -1;
	}

	bool mkdir(const char* /*fileName*/)
	{
		return true;
	}

private:

	char _fullfileName[512];

	char* GetFilename(const char* fileName)
	{
		sprintf_s<512>(_fullfileName, SDPATH"\\%s", fileName);
		return _fullfileName;
	}
};

extern SDClass SD;

////////////////////////////////////////////////////////////

class MyDirFile
{
public:
	virtual bool IsFile() = 0;
	virtual void close() = 0;
	virtual void open(int mode) = 0;
	virtual bool isopen() = 0;

	char _OSfileName[_MAX_PATH] = { 0 };
	char _pathname[_MAX_PATH] = { 0 };
	char _name[_MAX_PATH] = { 0 };

	bool isDirectory() const
	{
		struct stat st;
		return stat(_OSfileName, &st) != -1 && (st.st_mode & _S_IFDIR) != 0;
	}

	MyDirFile() { _refCount = 0; }

	virtual ~MyDirFile() = default;

	int  _refCount;
	void IncRef() { _refCount++; };

	void DecRef()
	{
		_refCount--;
		if (_refCount == 0)
		{
			close();
			delete this;
		}
	};
};

////////////////////////////////////////////////////////////

class MyFile : public MyDirFile
{
public:

	MyFile()
	{
		_f = nullptr;
	}

	FILE* _f;

	virtual bool IsFile() override { return true; };

	virtual void close() override
	{
		if (_f) fclose(_f);
		_f = nullptr;
	};
	virtual bool isopen() override { return _f != nullptr; }

	virtual void open(int mode) override
	{
		if (mode == FILE_READ)
			fopen_s(&_f, _OSfileName, "rb");
		else
		{
			fopen_s(&_f, _OSfileName, "wb");
			if (_f != nullptr)
			{
				fclose(_f);
				fopen_s(&_f, _OSfileName, "r+");
			}
		}
	}
};

////////////////////////////////////////////////////////////

class MyDir : public MyDirFile
{
public:

	MyDir()
	{
		_dir = nullptr;
	}

	virtual bool IsFile() override { return false; };

	virtual void close() override
	{
		if (_dir) FindClose(_dir);
		_dir = nullptr;
	};
	virtual bool isopen() override { return _dir != nullptr; }

	HANDLE           _dir;
	bool             _dirEof;
	WIN32_FIND_DATAA ffd;
	char             _dirfindmask[512];

	class File openNextFile();

	virtual void open(int /* mode */) override
	{
		close();
		if (!isDirectory()) return;
		strcpy_s(_dirfindmask, _OSfileName);
		strcat_s(_dirfindmask, "\\*");

		_dir = FindFirstFileA(_dirfindmask, &ffd);
	}
};

////////////////////////////////////////////////////////////

class File : public Stream
{
private:

	MyDirFile* _dirfile;

	bool IsDirHandle() const { return _dirfile != nullptr && !_dirfile->IsFile(); };
	bool IsFileHandle() const { return _dirfile != nullptr && _dirfile->IsFile(); };

	MyFile* GetF() const { return IsFileHandle() ? static_cast<MyFile*>(_dirfile) : nullptr; }
	MyDir*  GetD() const { return IsDirHandle() ? static_cast<MyDir*>(_dirfile) : nullptr; }

public:

	File operator=(const File& src)
	{
		_dirfile = src._dirfile;
		if (_dirfile)
		{
			_dirfile->IncRef();
		}
		return *this;
	}

	File(const File& src) : Stream(src)
	{
		_dirfile = src._dirfile;
		if (_dirfile)
		{
			_dirfile->IncRef();
		}
	}

	~File()
	{
		if (_dirfile)
		{
			_dirfile->DecRef();
		}
		_dirfile = nullptr;
	}

	File() { _dirfile = nullptr; }

	void close()
	{
		if (_dirfile)
		{
			_dirfile->close();
			_dirfile->DecRef();
			_dirfile = nullptr;
		}
	}

	void open(const char* name, const char* osfileName, const char* pathname, int mode)
	{
		if (isDirectory(osfileName))
		{
			_dirfile = new MyDir();
		}
		else
		{
			_dirfile = new MyFile();
		}
		_dirfile->IncRef();

		strcpy_s(_dirfile->_name, name);
		strcpy_s(_dirfile->_OSfileName, osfileName);
		strcpy_s(_dirfile->_pathname, pathname);

		_dirfile->open(mode);
	}

	operator bool() const { return _dirfile != nullptr && _dirfile->isopen(); }

	virtual int  available() override { return feof(GetF()->_f) ? 0 : 1; }
	virtual char read() override { return char(fgetc(GetF()->_f)); }

	uint32_t size() const
	{
		struct stat st;
		stat(_dirfile->_OSfileName, &st);
		return st.st_size;
	}

	char* name() const { return _dirfile->_name; }

	bool isDirectory(const char* name)
	{
		struct stat st;
		return stat(name, &st) != -1 && (st.st_mode & _S_IFDIR) != 0;
	}

	bool isDirectory()
	{
		return isDirectory(_dirfile->_OSfileName);
	}

	File openNextFile() const
	{
		return GetD()->openNextFile();
	}

	void rewindDirectory() { }

	//	virtual int peek();
	//	virtual void flush();
	//	int read(void *buf, uint16_t nbyte);
	boolean  seek(uint32_t pos) { return fseek(GetF()->_f, pos, SEEK_SET) == 0; }
	uint32_t position() const { return ftell(GetF()->_f); }
};

////////////////////////////////////////////////////////////

inline File MyDir::openNextFile()
{
	File ret;

	if (_dir)
	{
		while (ffd.cFileName[0] == '.')
		{
			if (!FindNextFileA(_dir, &ffd))
			{
				FindClose(_dir);
				_dir    = nullptr;
				_dirEof = true;
				break;
			}
		}
		if (!_dirEof)
		{
			char tmp[256];
			tmp[0] = 0;
			if (strcmp(_pathname, "/") != 0)
			{
				strcpy_s(tmp, _pathname);
			}
			strcat_s(tmp, "/");
			strcat_s(tmp, ffd.cFileName);

			ret = SD.open(tmp);

			if (!FindNextFileA(_dir, &ffd))
			{
				FindClose(_dir);
				_dir    = nullptr;
				_dirEof = true;
			}
		}
	}
	else if (!_dirEof)
	{
		if (!isDirectory()) return ret;
		strcpy_s(_dirfindmask, _OSfileName);
		strcat_s(_dirfindmask, "\\*");

		_dir = FindFirstFileA(_dirfindmask, &ffd);
		if (_dir)
		{
			return openNextFile();
		}
	}

	return ret;
}


inline File SDClass::open(const char* fileName, uint8_t mode)
{
	char osfileName[256];
	char pathname[256];
	char name[256];

	File file;

	if (fileName[0] == 0)
	{
		return file;
	}

	//	file._mode = mode;

	if (fileName[0] == '/')
	{
		if (fileName[1] == 0)
		{
			sprintf_s<256>(osfileName, SDPATH);
		}
		else
		{
			sprintf_s<256>(osfileName, SDPATH"%s", fileName);
		}
	}
	else
	{
		sprintf_s<256>(osfileName, SDPATH"\\%s", fileName);
	}

	for (char* t = osfileName; *t; t++)
	{
		if (*t == '/')
		{
			*t = '\\';
		}
	}

	strcpy_s(pathname, fileName);
	char* dirend = strrchr(pathname, '/');
	if (dirend)
	{
		strcpy_s(name, dirend + 1);
	}
	else
	{
		strcpy_s(name, fileName);
	}

	file.open(name, osfileName, pathname, mode);

	return file;
}
