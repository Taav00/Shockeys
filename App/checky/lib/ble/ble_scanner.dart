import 'dart:async';
import 'package:flutter_reactive_ble/flutter_reactive_ble.dart';
import 'package:checky/ble/reactive_state.dart';
import 'package:meta/meta.dart';

/// Created by following the example project provided by flutter_reactive_ble
/// GitHub: https://github.com/PhilipsHue/flutter_reactive_ble/tree/master/example/lib/src
/// pub.dev: https://pub.dev/packages/flutter_reactive_ble
class BleStatusMonitor implements ReactiveState<BleStatus> {
  const BleStatusMonitor(this._ble);

  final FlutterReactiveBle _ble;

  @override
  Stream<BleStatus> get state => _ble.statusStream;
}

class BleScanner implements ReactiveState<BleScannerState> {
  BleScanner(this._ble);

  final FlutterReactiveBle _ble;
  final StreamController<BleScannerState> _stateStreamController =
  StreamController();

  final _devices = <DiscoveredDevice>[];

  @override
  Stream<BleScannerState> get state => _stateStreamController.stream;

  void startScan(List<Uuid> serviceIds) {
    _devices.clear();
    _subscription?.cancel();
    _subscription =
        _ble.scanForDevices(withServices: serviceIds).listen((device) {
          final knownDeviceIndex = _devices.indexWhere((d) => d.id == device.id);
          if (knownDeviceIndex >= 0) {
            _devices[knownDeviceIndex] = device;
          } else {
            _devices.add(device);
          }
          _pushState();
        });
    _pushState();
  }

  void _pushState() {
    _stateStreamController.add(
      BleScannerState(
        discoveredDevices: _devices,
        scanIsInProgress: _subscription != null,
      ),
    );
  }

  Future<void> stopScan() async {
    await _subscription?.cancel();
    _subscription = null;
    _pushState();
  }

  Future<void> dispose() async {
    await _stateStreamController.close();
  }

  StreamSubscription _subscription;
}

@immutable
class BleScannerState {
  const BleScannerState({
    @required this.discoveredDevices,
    @required this.scanIsInProgress,
  });

  final List<DiscoveredDevice> discoveredDevices;
  final bool scanIsInProgress;
}