#pragma once

#include <primitivesCore/PrimitiveContainer.h>
#include <vmobjects/ObjectFormats.h>

class _Krun : public PrimitiveContainer {
public:
  _Krun();
  void Init(Interpreter *, VMFrame *);
  void Done(Interpreter *, VMFrame *);
  void Measure_(Interpreter *, VMFrame *);
  void GetWallclock_(Interpreter *, VMFrame *);
  void GetNumCores(Interpreter *, VMFrame *);
  void GetCoreCyclesDouble_Core_(Interpreter *, VMFrame *);
};
