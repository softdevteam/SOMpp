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

#include "VMArray.h"

#include <vm/Universe.h>

const long VMArray::VMArrayNumberOfFields = 0;

VMArray::VMArray(long size, long nof) :
        VMObject(nof + VMArrayNumberOfFields) {
    // initialize fields with nilObject
    // SetIndexableField is not used to prevent the write barrier to be called
    // too often
    pVMObject* arrFields = (pVMObject*)&clazz + GetNumberOfFields();
    for (long i = 0; i < size; ++i) {
        arrFields[i] = nilObject;
    }
}

pVMObject VMArray::GetIndexableField(long idx) const {
    if (idx > GetNumberOfIndexableFields() - 1) {
        cout << "Array index out of bounds: Accessing " << idx
        << ", but array size is only " << GetNumberOfIndexableFields() - 1
        << endl;
        _UNIVERSE->ErrorExit("Array index out of bounds");
    }
    return GetField(GetNumberOfFields() + idx);
}

void VMArray::SetIndexableField(long idx, pVMObject value) {
    if (idx > GetNumberOfIndexableFields() - 1) {
        cout << "Array index out of bounds: Accessing " << idx
        << ", but array size is only " << GetNumberOfIndexableFields() - 1
        << endl;
        _UNIVERSE->ErrorExit("Array index out of bounds");
    }
    SetField(GetNumberOfFields() + idx, value);
}

pVMArray VMArray::CopyAndExtendWith(pVMObject item) const {
    size_t fields = GetNumberOfIndexableFields();
    pVMArray result = _UNIVERSE->NewArray(fields+1);
    this->CopyIndexableFieldsTo(result);
    result->SetIndexableField(fields, item);
    return result;
}

pVMArray VMArray::Clone() const {
    long addSpace = objectSize - sizeof(VMArray);
#if GC_TYPE==GENERATIONAL
    pVMArray clone = new (_HEAP, addSpace, true) VMArray(*this);
#else
    pVMArray clone = new (_HEAP, addSpace) VMArray(*this);
#endif
    void* destination = SHIFTED_PTR(clone, sizeof(VMArray));
    const void* source = SHIFTED_PTR(this, sizeof(VMArray));
    size_t noBytes = GetObjectSize() - sizeof(VMArray);
    memcpy(destination, source, noBytes);
    return clone;
}

void VMArray::CopyIndexableFieldsTo(pVMArray to) const {
    long numIndexableFields = GetNumberOfIndexableFields();
    for (long i = 0; i < numIndexableFields; ++i) {
        to->SetIndexableField(i, GetIndexableField(i));
    }
}

void VMArray::WalkObjects(VMOBJECT_PTR (*walk)(VMOBJECT_PTR)) {
    long noOfFields = GetNumberOfFields();
    long numIndexableFields = GetNumberOfIndexableFields();
    pVMObject* fields = (pVMObject*)(&clazz);
    for (long i = 0; i < noOfFields + numIndexableFields; i++) {
        fields[i] = walk(AS_POINTER(fields[i]));
    }
}
