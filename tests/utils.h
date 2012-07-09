#include <string>
#include <stdio.h>
#include <fcntl.h>

#if WIN32

  #include <io.h>
  #include <sys/stat.h>
  #define S_IRUSR _S_IREAD
  #define S_IWUSR _S_IWRITE

#else

  #include <unistd.h>
  #include <sys/fcntl.h>

#endif

using namespace std;

inline string copyFile(const string &filename, const string &ext)
{
  #if WIN32
    const int INF_OFLAG = O_RDONLY | O_BINARY;
    const int OUTF_OFLAG = O_CREAT | O_RDWR | O_BINARY | O_TRUNC;
  #else
    const int INF_OFLAG = O_RDONLY;
    const int OUTF_OFLAG = O_CREAT | O_EXCL | O_RDWR;
  #endif
  string newname = string(tempnam(NULL, NULL)) + ext;
  string oldname = string("data/") + filename + ext;
#ifdef _WIN32
  CopyFile(oldname.c_str(), newname.c_str(), FALSE);
  SetFileAttributes(newname.c_str(), GetFileAttributes(newname.c_str()) & ~FILE_ATTRIBUTE_READONLY);
#else
  char buffer[4096];
  int bytes;
  int inf = open(oldname.c_str(), INF_OFLAG);
  int outf = open(newname.c_str(), OUTF_OFLAG, S_IRUSR | S_IWUSR);
  while((bytes = read(inf, buffer, sizeof(buffer))) > 0)
    write(outf, buffer, bytes);
  close(outf);
  close(inf);
#endif
  return newname;
}

inline void deleteFile(const string &filename)
{
  remove(filename.c_str());
}

class ScopedFileCopy
{
public:
  ScopedFileCopy(const string &filename, const string &ext, bool deleteFile=true)
  {
    m_deleteFile = deleteFile;
    m_filename = copyFile(filename, ext);
  }

  ~ScopedFileCopy()
  {
    if(m_deleteFile)
      deleteFile(m_filename);
  }

  string fileName()
  {
    return m_filename;
  }

private:
  bool m_deleteFile;
  string m_filename;
};
