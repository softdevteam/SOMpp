#pragma once
#ifndef UNIVERSE_H_
#define UNIVERSE_H_

/*
 *
 *
 Copyright (c) 2007 Michael Haupt, Tobias Pape, Arne Bergmann
 Software Architecture Group, Hasso Plattner Institute, Potsdam, Germany
 http://www.hpi.uni-potsdam.de/swa/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

//#define __DEBUG
#include <map>
#include <vector>

#include "../misc/defs.h"
#include "../misc/Timer.h"
#include "../misc/ExtendedList.h"

#include "../vmobjects/ObjectFormats.h"

#include "../memory/PagedHeap.h"

#if GC_TYPE==PAUSELESS
#include "../memory/pauseless/Worklist.h"
#endif

class Interpreter;

class SourcecodeCompiler;

//Convenience macro for Singleton access
#define _UNIVERSE GetUniverse()

//Convenience macro to get access to an interpreters memory page
#define _PAGE _UNIVERSE->GetInterpreter()->GetPage()

// for runtime debug
extern short dumpBytecodes;
extern short gcVerbosity;

//global VMObjects
extern GCObject* nilObject;
extern GCObject* trueObject;
extern GCObject* falseObject;

extern GCClass* objectClass;
extern GCClass* classClass;
extern GCClass* metaClassClass;

extern GCClass* nilClass;
extern GCClass* integerClass;
extern GCClass* bigIntegerClass;
extern GCClass* arrayClass;
extern GCClass* methodClass;
extern GCClass* symbolClass;
extern GCClass* primitiveClass;
extern GCClass* stringClass;
extern GCClass* systemClass;
extern GCClass* blockClass;
extern GCClass* doubleClass;

extern GCClass* trueClass;
extern GCClass* falseClass;

extern GCSymbol* symbolIfTrue;
extern GCSymbol* symbolIfFalse;

extern GCClass* threadClass;
extern GCClass* mutexClass;
extern GCClass* signalClass;

using namespace std;
class Universe {
public:
    //static methods
    static void Start(long argc, char** argv);
    static void Quit(long);
    static void ErrorExit(const char*);

    //Globals accessor (only for GC, could be considered be
    //declared as a private friend method for the GC)
    map<GCSymbol*, GCAbstractObject*> GetGlobals() {
        return globals;
    }
    PagedHeap* GetHeap() {
        return heap;
    }
    
    Interpreter* GetInterpreter();
    Interpreter* NewInterpreter();
    void RemoveInterpreter();
    
#if GC_TYPE==PAUSELESS
    unique_ptr<vector<Interpreter*>> GetInterpretersCopy();
#else
    vector<Interpreter*>* GetInterpreters();
#endif

    void Assert(bool) const;

    pVMSymbol SymbolFor(const StdString&);
    pVMSymbol SymbolForChars(const char*);

    //VMObject instanciation methods. These should probably be refactored to a new class
    pVMArray NewArray(long) const;
    pVMArray NewArrayList(ExtendedList<pVMObject>& list) const;
    pVMArray NewArrayList(ExtendedList<pVMSymbol>& list) const;
    pVMArray NewArrayFromStrings(const vector<StdString>&) const;
    pVMBlock NewBlock(pVMMethod, pVMFrame, long);
    pVMClass NewClass(pVMClass) const;
    pVMFrame NewFrame(pVMFrame, pVMMethod) const;
    pVMMethod NewMethod(pVMSymbol, size_t, size_t) const;
    pVMObject NewInstance(pVMClass) const;
    pVMInteger NewInteger(long) const;
    pVMBigInteger NewBigInteger(int64_t) const;
    pVMDouble NewDouble(double) const;
    pVMClass NewMetaclassClass(void) const;
    pVMString NewString(const StdString&) const;
    pVMSymbol NewSymbol(const StdString&);
    pVMString NewString(const char*) const;
    pVMSymbol NewSymbol(const char*);
    pVMClass NewSystemClass(void) const;
    pVMMutex NewMutex() const;
    pVMSignal NewSignal() const;
    pVMThread NewThread() const;
   
#if GC_TYPE==PAUSELESS
    void MarkGlobals();
    void  CheckMarkingGlobals(void (AbstractVMObject*));
#else
    void WalkGlobals(VMOBJECT_PTR (*walk)(VMOBJECT_PTR));
#endif

    void InitializeSystemClass(pVMClass, pVMClass, const char*);

    pVMObject GetGlobal(pVMSymbol);
    void SetGlobal(pVMSymbol name, pVMObject val);
    void InitializeGlobals();
    pVMClass GetBlockClass(void) const;
    pVMClass GetBlockClassWithArgs(long);

    pVMClass LoadClass(pVMSymbol);
    void LoadSystemClass(pVMClass);
    pVMClass LoadClassBasic(pVMSymbol, pVMClass);
    pVMClass LoadShellClass(StdString&);

    Universe();
    ~Universe();
    
#ifdef LOG_RECEIVER_TYPES
    struct stat_data {
        long noCalls;
        long noPrimitiveCalls;
    };
    map<StdString, long> receiverTypes;
    map<StdString, stat_data> callStats;
#endif
    //
    
    static bool IsValidObject(const pVMObject obj);
    
    //void PrintGlobals();
    
private:
    
    pthread_mutex_t testMutex;
    
    pthread_mutex_t interpreterMutex;
    pthread_key_t interpreterKey;
    vector<Interpreter*> interpreters;

    pthread_mutexattr_t attrclassLoading;
    pthread_mutex_t classLoading;
    
    vector<StdString> handleArguments(long argc, char** argv);
    long getClassPathExt(vector<StdString>& tokens, const StdString& arg) const;

    friend Universe* GetUniverse();
    static Universe* theUniverse;

    long setupClassPath(const StdString& cp);
    long addClassPath(const StdString& cp);
    void printUsageAndExit(char* executable) const;

    void initialize(long, char**);

    PagedHeap* heap;
    long heapSize;
    long pageSize;
    map<GCSymbol*, GCAbstractObject*> globals;
    map<long, GCClass*> blockClassesByNoOfArgs;
    vector<StdString> classPath;
};


//Singleton accessor
inline Universe* GetUniverse() __attribute__ ((always_inline));
Universe* GetUniverse() {
    /*if (DEBUG && !Universe::theUniverse) {
        Universe::ErrorExit("Trying to access uninitialized Universe, exiting.");
    }*/
    return Universe::theUniverse;
}


#endif
