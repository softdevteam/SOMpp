#include <inttypes.h>

#include "Krun.h"

#include "../primitivesCore/Routine.h"

#include <vmobjects/VMArray.h>
#include <vmobjects/VMFrame.h>
#include <vmobjects/VMInteger.h>
#include <vmobjects/VMObject.h>

#include <vm/Universe.h>

extern "C" {
void krun_init(void);

void krun_done(void);

void krun_measure(int mdata_idx);

double krun_get_wallclock(int mdata_idx);

int krun_get_num_cores(void);

double krun_get_core_cycles_double(int mdata_idx, int core);
}

_Krun::_Krun() : PrimitiveContainer() {
  SetPrimitive("krunInit", new Routine<_Krun>(this, &_Krun::Init, false));
  SetPrimitive("krunDone", new Routine<_Krun>(this, &_Krun::Done, false));
  SetPrimitive("krunMeasure_",
               new Routine<_Krun>(this, &_Krun::Measure_, false));
  SetPrimitive("krunGetWallclock_",
               new Routine<_Krun>(this, &_Krun::GetWallclock_, false));
  SetPrimitive("krunGetNumCores",
               new Routine<_Krun>(this, &_Krun::GetNumCores, false));
  SetPrimitive(
      "krunGetCoreCyclesDouble_Core_",
      new Routine<_Krun>(this, &_Krun::GetCoreCyclesDouble_Core_, false));
}

void _Krun::Init(Interpreter *, VMFrame *frame) { krun_init(); }

void _Krun::Done(Interpreter *, VMFrame *frame) { krun_done(); }

void _Krun::Measure_(Interpreter *, VMFrame *frame) {
  vm_oop_t mdata_idx = frame->Pop();
  krun_measure(INT_VAL(mdata_idx));
}

void _Krun::GetWallclock_(Interpreter *, VMFrame *frame) {
  vm_oop_t mdata_idx = frame->Pop();
  frame->Pop();
  VMDouble *wallclock =
      GetUniverse()->NewDouble(krun_get_wallclock(INT_VAL(mdata_idx)));
  frame->Push(AS_OBJ(wallclock));
}

void _Krun::GetNumCores(Interpreter *, VMFrame *frame) {
  int cores = krun_get_num_cores();
  frame->Pop();
  frame->Push(NEW_INT(cores));
}

void _Krun::GetCoreCyclesDouble_Core_(Interpreter *, VMFrame *frame) {
  vm_oop_t core = frame->Pop();
  vm_oop_t mdata_idx = frame->Pop();
  frame->Pop();
  VMDouble *cycles = GetUniverse()->NewDouble(
      krun_get_core_cycles_double(INT_VAL(mdata_idx), INT_VAL(core)));
  frame->Push(AS_OBJ(cycles));
}
