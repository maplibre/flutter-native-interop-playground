import 'dart:ffi' hide Size;

import 'package:ffi/ffi.dart';
import 'package:flmln/keys.dart';
import 'package:flutter/material.dart';
import 'package:flutter_map/flutter_map.dart';
import 'package:latlong2/latlong.dart';

import 'gen/flmln_bindings.dart' as gen;

final bindings = gen.FlMlnBindings(DynamicLibrary.process());

void test() async {
  print(bindings.test_flmln());
}

class FlMlnWindget extends StatefulWidget {
  const FlMlnWindget({super.key});

  @override
  State<FlMlnWindget> createState() => FlMlnWindgetState();
}

class FlMlnWindgetState extends State<FlMlnWindget> with SingleTickerProviderStateMixin {
  // late final _ticker = Ticker(_onTick)..start();
  var enabled = false;

  late gen.mbgl_tile_server_options_t tileServerOptions;
  late gen.flmln_renderer_frontend_t rendererFrontend;
  late gen.flmln_map_observer_t mapObserver;
  late gen.mbgl_map_options_t mapOptions;
  late gen.mbgl_resource_options_t resourceOptions;
  late gen.mbgl_map_t map;

  @override
  void initState() {
    super.initState();

    mapOptions = bindings.mbgl_map_options_create();
    bindings.mbgl_map_options_set_mode(mapOptions, gen.MbglMapMode.MbglMapMode_Continuous);
    tileServerOptions = bindings.mbgl_tile_server_options_maptiler_configuration_create();

    resourceOptions = bindings.mbgl_resource_options_create();
    bindings.mbgl_resource_options_set_api_key(resourceOptions, apiKey.toNativeUtf8().cast());
    bindings.mbgl_resource_options_set_tile_server_options(resourceOptions, tileServerOptions);

    mapObserver = bindings.flmln_map_observer_create();
    rendererFrontend = bindings.flmln_renderer_frontend_create();

    map = bindings.mbgl_map_create(
      rendererFrontend,
      mapObserver,
      mapOptions,
      resourceOptions,
    );

    bindings.mbgl_map_style_load_url(map, styleUrl.toNativeUtf8().cast());
  }

  void _onTick(_) {
    setState(() {});
  }

  @override
  Widget build(BuildContext context) {
    return FlutterMap(
      children: [
        if (enabled)
          CustomPaint(
            painter: _MapPainter(
              pixelRatio: MediaQuery.devicePixelRatioOf(context),
              rendererFrontend: rendererFrontend,
              map: map,
            ),
            child: SizedBox.expand(),
          ),
        Builder(
          builder: (context) {
            final camera = MapCamera.of(context);
            final cameraOptions = bindings.mbgl_camera_options_create();
            bindings.mbgl_camera_options_set_center(cameraOptions, camera.center.latitude, camera.center.longitude);
            bindings.mbgl_camera_options_set_zoom(cameraOptions, camera.zoom - 1);
            bindings.mbgl_camera_options_set_bearing(cameraOptions, -camera.rotation);
            bindings.mbgl_map_jump_to(map, cameraOptions);
            bindings.mbgl_camera_options_destroy(cameraOptions);
            return SizedBox.expand(
              child: Texture(
                textureId: bindings.flmln_renderer_frontend_get_texture_id(rendererFrontend),
              ),
            );
          },
        ),
        // Opacity(
        //   opacity: 0.5,
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
          padding: const EdgeInsets.all(32.0),
          child: ElevatedButton(
            onPressed: () {
              if (!enabled) enabled = true;
              setState(() {});
            },
            child: Text('Tick'),
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

  final gen.flmln_renderer_frontend_t rendererFrontend;
  final gen.mbgl_map_t map;
  final double pixelRatio;

  @override
  void paint(Canvas canvas, Size size) {
    bindings.mbgl_map_set_size(map, size.width.toInt(), size.height.toInt());
    bindings.flmln_renderer_frontend_set_size_and_pixel_ratio(
      rendererFrontend,
      size.width.toInt(),
      size.height.toInt(),
      pixelRatio,
    );
    bindings.flmln_renderer_frontend_render(rendererFrontend);
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}
