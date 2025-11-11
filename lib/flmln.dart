import 'dart:ffi' hide Size;

import 'package:ffi/ffi.dart';
import 'package:flmln/keys.dart';
import 'package:flmln/src/style/style.dart';
import 'package:flmln/style_inspector.dart';
import 'package:flutter/material.dart' hide Visibility;
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';
import 'package:metal_capture_util/metal_capture_util.dart';

import 'gen/flmln_bindings.dart';

void test() async {
  print(test_flmln());
}

class FlMlnWindget extends StatefulWidget {
  const FlMlnWindget({super.key});

  @override
  State<FlMlnWindget> createState() => FlMlnWindgetState();
}

typedef VoidCallback = Void Function();

class FlMlnWindgetState extends State<FlMlnWindget> with WidgetsBindingObserver {
  var enabled = false;

  late mbgl_tile_server_options_t tileServerOptions;
  late flmln_renderer_frontend_t rendererFrontend;
  late flmln_map_observer_t mapObserver;
  late mbgl_map_options_t mapOptions;
  late mbgl_resource_options_t resourceOptions;
  late mbgl_map_t map;

  final repaint = ChangeNotifier();

  @override
  void initState() {
    super.initState();
    flmln_initialize();

    mapOptions = mbgl_map_options_create();
    mbgl_map_options_set_mode(mapOptions, MbglMapMode.MbglMapMode_Continuous);
    tileServerOptions = mbgl_tile_server_options_maptiler_configuration_create();

    resourceOptions = mbgl_resource_options_create();
    mbgl_resource_options_set_api_key(resourceOptions, apiKey.toNativeUtf8().cast());
    mbgl_resource_options_set_tile_server_options(resourceOptions, tileServerOptions);

    mapObserver = flmln_map_observer_create();
    rendererFrontend = flmln_renderer_frontend_create();

    map = mbgl_map_create(
      rendererFrontend,
      mapObserver,
      mapOptions,
      resourceOptions,
    );

    mbgl_map_style_load_url(map, styleUrl.toNativeUtf8().cast());

    final onTick = NativeCallable<VoidCallback>.listener(_onTick);
    flmln_renderer_frontend_set_invalidate_callback(rendererFrontend, onTick.nativeFunction);

    WidgetsBinding.instance.addPersistentFrameCallback((_) {
      flmln_utils_run_loop_once();
    });

    WidgetsBinding.instance.addObserver(this);
  }

  @override
  void didHaveMemoryPressure() {
    print('didHaveMemoryPressure');
    super.didHaveMemoryPressure();
  }

  void _onTick() {
    // print('tick');
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (mounted) {
        repaint.notifyListeners();
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        Drawer(
          width: 320.0,
          child: enabled ? StyleInspector(style: Style.fromNative(mbgl_map_get_style(map))) : null,
        ),
        Expanded(
          child: FlutterMap(
            options: MapOptions(
              backgroundColor: Colors.black,
            ),
            children: [
              if (enabled)
                CustomPaint(
                  painter: _MapPainter(
                    pixelRatio: MediaQuery.devicePixelRatioOf(context),
                    rendererFrontend: rendererFrontend,
                    map: map,
                    repaint: repaint,
                  ),
                  child: SizedBox.expand(),
                ),
              Builder(
                builder: (context) {
                  final camera = MapCamera.of(context);
                  final cameraOptions = mbgl_camera_options_create();
                  mbgl_camera_options_set_center(cameraOptions, camera.center.latitude, camera.center.longitude);
                  mbgl_camera_options_set_zoom(cameraOptions, camera.zoom - 1);
                  mbgl_camera_options_set_bearing(cameraOptions, -camera.rotation);
                  mbgl_map_jump_to(map, cameraOptions);
                  mbgl_camera_options_destroy(cameraOptions);
                  return SizedBox.expand(
                    child: Texture(
                      textureId: flmln_renderer_frontend_get_texture_id(rendererFrontend),
                    ),
                  );
                },
              ),
              // Opacity(
              //   opacity: 0.0,
              //   child: TileLayer(
              //     urlTemplate: 'https://tile.openstreetmap.org/{z}/{x}/{y}.png',
              //     userAgentPackageName: 'com.kekland.flmln_example',
              //   ),
              // ),
              MarkerLayer(
                markers: [
                  // Kyiv
                  Marker(
                    point: LatLng(50.4501, 30.5234),
                    width: 80,
                    height: 80,
                    child: Icon(
                      Icons.location_on,
                      color: Colors.red,
                      size: 40,
                    ),
                  ),
                ],
              ),
              Padding(
                padding: const EdgeInsets.all(64.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  spacing: 8.0,
                  children: [
                    ElevatedButton(
                      onPressed: () {
                        if (!enabled) enabled = true;
                        setState(() {});
                      },
                      child: Text('Tick'),
                    ),
                    ElevatedButton(
                      onPressed: () {
                        final style = Style.fromNative(mbgl_map_get_style(map));
                        final backgroundLayer = style.getLayer<BackgroundLayer>('Background');
                        print(backgroundLayer.backgroundColor.asConstant);
                        if (backgroundLayer.backgroundColor.asConstant == Colors.blue) {
                          backgroundLayer.backgroundColor = PropertyValue.constant(Colors.green);
                        } else {
                          backgroundLayer.backgroundColor = PropertyValue.constant(Colors.blue);
                        }
                      },
                      child: Text('Toggle Background'),
                    ),
                    ElevatedButton(
                      onPressed: () {
                        final layer = FillLayer(
                          id: 'my-fill-layer',
                          sourceId: 'maptiler_planet',
                          fillColor: PropertyValue.constant(Colors.purple),
                          fillOpacity: PropertyValue.constant(0.5),
                          sourceLayer: 'landuse',
                        );

                        // change parameters
                        layer.fillColor = PropertyValue.constant(Colors.orange);

                        // insert layer
                        final style = Style.fromNative(mbgl_map_get_style(map));
                        style.addLayer(layer);
                      },
                      child: Text('Insert layer'),
                    ),
                    ElevatedButton(
                      onPressed: () {
                        MetalCapture.captureNextFrame(reassemble: true, openFile: true);
                      },
                      child: Text('Metal capture'),
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }
}

class _MapPainter extends CustomPainter {
  _MapPainter({
    super.repaint,
    required this.rendererFrontend,
    required this.map,
    required this.pixelRatio,
  });

  final flmln_renderer_frontend_t rendererFrontend;
  final mbgl_map_t map;
  final double pixelRatio;

  @override
  void paint(Canvas canvas, Size size) {
    // print('paint');
    mbgl_map_set_size(map, size.width.toInt(), size.height.toInt());
    flmln_renderer_frontend_set_size_and_pixel_ratio(
      rendererFrontend,
      size.width.toInt(),
      size.height.toInt(),
      pixelRatio,
    );
    flmln_renderer_frontend_render(rendererFrontend);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}
