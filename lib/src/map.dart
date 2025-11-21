import 'package:flmln/gen/flmln_bindings.dart';
import 'package:flutter/widgets.dart';

class FlMlnMap extends ChangeNotifier {
  FlMlnMap() {
    _rendererFrontendPtr = flmln_renderer_frontend_create();
    _mapObserverPtr = flmln_map_observer_create();
    // _mapPtr = mbgl_map_create();
  }

  late final flmln_renderer_frontend_t _rendererFrontendPtr;
  late final flmln_map_observer_t _mapObserverPtr;
  late final mbgl_map_t _mapPtr;
}
