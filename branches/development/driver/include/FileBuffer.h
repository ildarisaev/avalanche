/*----------------------------------------------------------------------------------------*/
/*------------------------------------- AVALANCHE ----------------------------------------*/
/*------ Driver. Coordinates other processes, traverses conditional jumps tree.  ---------*/
/*----------------------------------- FileBuffer.h ---------------------------------------*/
/*----------------------------------------------------------------------------------------*/

/*
   Copyright (C) 2009-2011 Ildar Isaev
      iisaev@ispras.ru

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __FILE_BUFFER__H__
#define __FILE_BUFFER__H__

#include <stddef.h>
#include <string>

using namespace std;

class FileBuffer
{
public:

  char* buf;
  int size;
  char* name;
  int sd;
  //unsigned int startdepth;
  //bool* prediction;
  //int predictionSize;
  //FileBuffer* parent;

  friend bool operator == (const FileBuffer& arg1, const FileBuffer& arg2);

  FileBuffer(const char* name);

  FileBuffer(const FileBuffer& other);

  FileBuffer(string str);

  virtual FileBuffer* forkInput(char* stpOutputFile);

  virtual void dumpFile(const char* name = NULL);

  void cutQueryAndDump(const char* name, bool do_invert = false);

  virtual void applySTPSolution(char* buf);
  
  bool filterCovgrindOutput ();
  char * filterMemcheckOutput (long errors);

  // Return the number after first appearence of 'str' in 'buf'
  long filterCount (const char * str);

  // Get call stack from 'position' in 'buf'
  string getCallStack (int & position);
  string getErrorType (int & position);

  string getBuf ();

  ~FileBuffer();

protected:

  FileBuffer()
  { }

};

#endif
