import 'dart:ffi';

import 'package:ffi/ffi.dart';
import 'package:flmln/gen/flmln_bindings.dart' as g;

void test() {
  final unitBezier = malloc<g.MBGL_UTIL_UNIT_BEZIER>(sizeOf<g.MBGL_UTIL_UNIT_BEZIER>());
  
  unitBezier.ref.p1x = 1.0;
  unitBezier.ref.p1y = 1.0;
  unitBezier.ref.p2x = 1.0;
  unitBezier.ref.p2y = 1.0;

  malloc.free(unitBezier);
}