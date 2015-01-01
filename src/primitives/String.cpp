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

#include <stdio.h>

#include <vmobjects/VMObject.h>
#include <vmobjects/VMFrame.h>
#include <vmobjects/VMSymbol.h>
#include <vmobjects/VMInteger.h>

#include <vm/Universe.h>

#include <misc/defs.h>

#include "String.h"
#include "../primitivesCore/Routine.h"

_String::_String() : PrimitiveContainer() {
    SetPrimitive("concatenate_", new Routine<_String>(this, &_String::Concatenate_));
    SetPrimitive("asSymbol",     new Routine<_String>(this, &_String::AsSymbol));
    SetPrimitive("hashcode",     new Routine<_String>(this, &_String::Hashcode));
    SetPrimitive("length",       new Routine<_String>(this, &_String::Length));
    SetPrimitive("equal",        new Routine<_String>(this, &_String::Equal));
    SetPrimitive("primSubstringFrom_to_", new Routine<_String>(this, &_String::PrimSubstringFrom_to_));
}

void _String::Concatenate_(pVMObject /*object*/, VMFrame* frame) {
    pVMString arg  = static_cast<pVMString>(frame->Pop());
    pVMString self = static_cast<pVMString>(frame->Pop());
    StdString a = arg->GetChars();
    StdString s = self->GetChars();

    StdString result = s + a;

    frame->Push(GetUniverse()->NewString(result));
}

void _String::AsSymbol(pVMObject /*object*/, VMFrame* frame) {
    pVMString self = static_cast<pVMString>(frame->Pop());
    StdString result = self->GetStdString();
    frame->Push(GetUniverse()->SymbolFor(result));
}

void _String::Hashcode(pVMObject /*object*/, VMFrame* frame) {
    pVMString self = static_cast<pVMString>(frame->Pop());
    frame->Push(NEW_INT(self->GetHash()));
}

void _String::Length(pVMObject /*object*/, VMFrame* frame) {
    pVMString self = static_cast<pVMString>(frame->Pop());

    size_t len = self->GetStringLength();
    frame->Push(NEW_INT(len));
}

void _String::Equal(pVMObject /*object*/, VMFrame* frame) {
    oop_t op1 = frame->Pop();
    pVMString op2 = static_cast<pVMString>(frame->Pop());

    if (IS_TAGGED(op1)) {
        frame->Push(falseObject);
        return;
    }

    VMClass* otherClass = CLASS_OF(op1);
    if(otherClass == stringClass) {
        StdString s1 = static_cast<pVMString>(op1)->GetStdString();
        StdString s2 = op2->GetStdString();

        if(s1 == s2) {
            frame->Push(trueObject);
            return;
        }
    }
    frame->Push(falseObject);
}

void _String::PrimSubstringFrom_to_(pVMObject /*object*/, VMFrame* frame) {
    oop_t end   = frame->Pop();
    oop_t start = frame->Pop();

    pVMString self = static_cast<pVMString>(frame->Pop());
    StdString str = self->GetStdString();

    long s = INT_VAL(start) - 1;
    long e = INT_VAL(end) - 1;

    StdString result = str.substr(s, e - s + 1);

    frame->Push( GetUniverse()->NewString(result));
}

