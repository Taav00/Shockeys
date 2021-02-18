import 'dart:async';
import 'dart:ffi';
import 'dart:math';
import 'dart:ui';
import 'dart:io';
import 'dart:core';
import 'dart:convert';

import 'package:fab_circular_menu/fab_circular_menu.dart';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'package:flutter/scheduler.dart';
import 'package:flutter_reactive_ble/flutter_reactive_ble.dart';
import 'package:fluttertoast/fluttertoast.dart';
import 'package:permission_handler/permission_handler.dart';
import 'package:provider/provider.dart';
import 'package:charts_flutter/flutter.dart' as charts;
import 'package:vector_math/vector_math.dart' as VMath;
import 'package:intl/intl.dart';

import 'package:path/path.dart' as path;
import 'package:path_provider/path_provider.dart';
import 'package:downloads_path_provider_28/downloads_path_provider_28.dart';

import 'ble/ble_scanner.dart';
import 'captured_heart_rate.dart';
import 'device_list.dart';
import 'device_detail.dart';
import 'ble/ble_connector.dart';
// import 'ble/data_storage.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  final FlutterReactiveBle _ble = FlutterReactiveBle();
  final BleScanner _scanner = BleScanner(_ble);
  final BleDeviceConnector _connector = BleDeviceConnector(_ble);
  final BleStatusMonitor _monitor = BleStatusMonitor(_ble);


  runApp(
    MultiProvider(
        providers: [
          Provider.value(value: _scanner),
          Provider.value(value: _monitor),
          Provider.value(value: _connector),
          Provider.value(value: _ble),
          StreamProvider<BleScannerState>(
            create: (_) => _scanner.state,
            initialData: const BleScannerState(
              discoveredDevices: [],
              scanIsInProgress: false,
            ),
          ),
          StreamProvider<BleStatus>(
            create: (_) => _monitor.state,
            initialData: BleStatus.unknown,
          ),
          StreamProvider<ConnectionStateUpdate>(
            create: (_) => _connector.state,
            initialData: const ConnectionStateUpdate(
              deviceId: 'Unknown device',
              connectionState: DeviceConnectionState.disconnected,
              failure: null,
            ),
          ),
        ],
        child: HomeRoute()
    ),
  );
}

class HomeRoute extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    requestPerm(Permission.locationWhenInUse);
    return MaterialApp(
      title: 'Head Impact Tracker',
      theme: ThemeData.dark(),
      home: HomePage(title: 'Head Impact Tracker'),
    );
  }

  void requestPerm(Permission permission) async {
    print("Requesting permission: $permission");
    if (await permission.request().isGranted) {
      print("Permission granted");
    } else {
      print("Permission Denied:");
    }
  }
}

class HomePage extends StatefulWidget {
  HomePage({Key key, this.title}) : super(key: key);
  final String title;

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  final GlobalKey<FabCircularMenuState> fabKey = GlobalKey();
  final maxAccController = TextEditingController();
  final Uuid checkyService = Uuid.parse("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
  final List<CapturedHeadAccel> _linAccHistory = new List<CapturedHeadAccel>();
  final List<CapturedHeadAccel> _angAccHistory = new List<CapturedHeadAccel>();
  DiscoveredDevice headAccMonitor;
  ConnectionStateUpdate connectionStateUpdate;
  DeviceDetail _deviceDetail;
  Future warning;
  int _maxHeadAcc = 0;
  int _currentLinearAcc = 0;
  int _currentAngularAcc = 0;
  List<charts.Series> seriesList;
  bool _workoutInProgress = false;
  StreamSubscription _workoutStream;
  // TextStorage storage;
  final String _fileName = "data.csv";
  bool firstData = false;


  @override
  void initState() {
    super.initState();
  }

  @override
  void dispose() {
    maxAccController.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    checkHeartAcc();
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Padding(
        // Leave space for button on bottom
        padding: EdgeInsets.only(bottom: 100),
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: <Widget>[
            Wrap(
              children: [
                Row(
                  mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                  crossAxisAlignment: CrossAxisAlignment.center,
                  children: <Widget>[
                    _buildInfoColumn("Current linear \n acceleration", firstData == false ?
                    "N/A" : "$_currentLinearAcc", Icons.north_east_rounded),
                    _buildInfoColumn("Current angular \n acceleration", firstData == false ?
                    "N/A" : "$_currentAngularAcc", Icons.refresh_rounded),
                    _buildInfoColumn("Max acceleration", _maxHeadAcc == 0 ?
                    "N/A" : "$_maxHeadAcc", Icons.warning_rounded),
                  ],
                ),
                Consumer3<FlutterReactiveBle, BleDeviceConnector, ConnectionStateUpdate>(
                    builder: (_, ble, connector, connectionStateUpdate, __) =>
                    _deviceDetail = _buildDeviceDetail(ble, connector, connectionStateUpdate, headAccMonitor)
                )
              ],
            ),
            _buildChart(),
          ],
        ),
      ),
      floatingActionButton: _buildCircularMenu(),
    );
  }

  Widget _buildCircularMenu() => FabCircularMenu(
      key: fabKey,
      ringDiameter: 500,
      ringColor: ThemeData.dark().bottomAppBarColor,
      fabColor: Colors.tealAccent.shade700,
      children: <Widget>[
        _buildMenuItem("Bluetooth Scan", Icons.bluetooth, () {
          fabKey.currentState.close();
          startBluetoothScan();
        }),
        _buildMenuItem("Set Max Acceleration", Icons.warning_rounded, () {
          return showDialog<void>(
              context: context,
              barrierDismissible: false,
              builder: (BuildContext context) {
                return AlertDialog(
                  contentPadding: const EdgeInsets.all(15.0),
                  content: new TextField(
                    controller: maxAccController,
                    autofocus: true,
                    maxLength: 3, /* Heart rate is 3 digit max */
                    keyboardType: TextInputType.number,
                    decoration: new InputDecoration(
                      hintText: "Max Acceleration",
                    ),
                  ),
                  actions: <Widget>[
                    new FlatButton(
                        child: const Text('CANCEL'),
                        onPressed: () {
                          Navigator.pop(context);
                        }),
                    new FlatButton(
                        child: const Text('SET'),
                        onPressed: () {
                          Navigator.pop(context);
                          setState(() {
                            _maxHeadAcc = int.tryParse(maxAccController.text);
                          });
                        })
                  ],
                );
              }
          );
        }),
        _workoutInProgress ?
        _buildMenuItem("Stop Game", Icons.run_circle, () {
          stopWorkout();
          fabKey.currentState.close();
        }) :
        _buildMenuItem("Start Game", Icons.directions_run, () {
          startWorkout();
          fabKey.currentState.close();
        })
      ]
  );

  Widget _buildDeviceDetail(FlutterReactiveBle ble, BleDeviceConnector connector, ConnectionStateUpdate connectionStateUpdate, DiscoveredDevice device) => DeviceDetail(
    device: device,
    connectionUpdate: connectionStateUpdate != null &&
        connectionStateUpdate.deviceId == device?.id
        ? connectionStateUpdate
        : ConnectionStateUpdate(
      deviceId: device?.id,
      connectionState: DeviceConnectionState.disconnected,
      failure: null,
    ),
    connect: connector.connect,
    disconnect: connector.disconnect,
    discoverServices: connector.discoverServices,
    requestMtu: ble.requestMtu,//added
    subscribe: ble.subscribeToCharacteristic,
    readCharacteristic: ble.readCharacteristic,

  );

  Widget _buildInfoColumn(String label, String value, IconData icon) => Container(
      margin: const EdgeInsets.all(5),
      padding: const EdgeInsets.all(5) ,
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceEvenly,
        children: <Widget>[
          Column(
            mainAxisSize: MainAxisSize.min,
            children: <Widget>[
              Text(
                value,
                style: Theme.of(context).textTheme.headline4,
              ),
              Text(
                label,
              ),
            ],
          ),
          Icon(
              icon,
              color: Colors.redAccent
          ),
        ],
      )
  );

  Widget _buildMenuItem(String label, IconData icon, VoidCallback pressed) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: <Widget>[
        IconButton(
            splashRadius: 20,
            onPressed: pressed,
            icon: new Icon(
                icon,
                color: Colors.redAccent
            )
        ),
        Text(label),
      ],
    );
  }

  Widget _buildChart() {
    return Flexible(
      child: charts.LineChart(
          createSeriesList(),
          animate: false,
          defaultRenderer: new charts.LineRendererConfig(
            includePoints: false,
          ),
          behaviors: [
            new charts.SeriesLegend(),
            new charts.SlidingViewport(),
            new charts.ChartTitle(
              "Time (per second)",
              behaviorPosition: charts.BehaviorPosition.bottom,
              titleOutsideJustification: charts.OutsideJustification.middleDrawArea,
            ),
            new charts.ChartTitle(
              "Acceleration",
              behaviorPosition: charts.BehaviorPosition.end,
              titleOutsideJustification: charts.OutsideJustification.middleDrawArea,
            ),
          ]
      ),
    );
  }

  List<charts.Series<CapturedHeadAccel, double>> createSeriesList() {
    return [
      charts.Series(
        id: "Linear Acceleration",
        data: _linAccHistory,
        displayName: "linear",
        colorFn: (_, __) => charts.MaterialPalette.red.shadeDefault,
        domainFn: (CapturedHeadAccel capture, _) => capture.time,
        measureFn: (CapturedHeadAccel capture, _) => capture.headAcc,
      ),
      charts.Series(
        id: "Angular Acceleration",
        data: _angAccHistory,
        displayName: "angular",
        colorFn: (_, __) => charts.MaterialPalette.teal.shadeDefault,
        domainFn: (CapturedHeadAccel capture, _) => capture.time,
        measureFn: (CapturedHeadAccel capture, _) => capture.headAcc,
      )
    ];
  }

  void stopWorkout() {
    setState(() {
      _workoutInProgress = false;
    });
    _workoutStream.cancel();
    Fluttertoast.showToast(
        msg: "Game finished!",
        toastLength: Toast.LENGTH_LONG,
        gravity: ToastGravity.BOTTOM
    );
  }

  void startWorkout() {
    if (headAccMonitor == null) {
      // No device connected
      Fluttertoast.showToast(
          msg: "No router connected",
          toastLength: Toast.LENGTH_LONG,
          gravity: ToastGravity.BOTTOM
      );
      return;
    } else {
      Fluttertoast.showToast(
          msg: "Start game",
          toastLength: Toast.LENGTH_LONG,
          gravity: ToastGravity.BOTTOM
      );
      setState(() {
        _workoutInProgress = true;
      });
      _linAccHistory.clear();
      _angAccHistory.clear();
      print("Game started!");
      setState(() {
        _workoutStream = watchHeadAcc().listen((event) { });
      });
    }
  }

  void showAlert(String title, String message) {
    warning = showDialog<void>(
        context: context,
        barrierDismissible: false,
        builder: (BuildContext context) {
          return AlertDialog(
            contentPadding: const EdgeInsets.all(15.0),
            title: Text("Warning!"),
            content: Text("The player's head sustained a strong impact exceeding the "
            "safety threshold! Consider doing a checkup."),
            actions: <Widget>[
              new FlatButton(
                  child: const Text('Ok'),
                  onPressed: () {
                    Navigator.pop(context);
                    // Delay alert or else you get them repeatedly
                    Future.delayed(const Duration(seconds: 5), () => warning = null);
                  })
            ],
          );
        }
    );
  }

  Stream watchHeadAcc() {
    // add new value every 50 ms (≈ 20 Hz -> headband sampling frequency)
    return Stream.periodic(const Duration(milliseconds: 50), (count) {
      print ("Linear acceleration at ${count+1}: $_currentLinearAcc");
      setState(() {
        _linAccHistory.add(CapturedHeadAccel(count*0.05, _currentLinearAcc));
        _angAccHistory.add(CapturedHeadAccel(count*0.05, _currentAngularAcc));
      });
    });
  }

  void checkHeartAcc() {
    // display an alert if the measured acceleration exceeds the user's safety threshold
    if ((_currentLinearAcc > _maxHeadAcc || _currentAngularAcc > _maxHeadAcc) &&
        (_maxHeadAcc != 0) &&
        (warning == null)) {
      print("Current acceleration exceeds maximum!");
      SchedulerBinding.instance.addPostFrameCallback((timeStamp) {
        showAlert("Warning!", "The player's head sustained a strong impact exceeding the "
            "safety threshold! Consider doing a checkup.");
      });
    }
  }

  Future<bool> _requestStoragePermission() async {
    var permission = await Permission.storage.status;

    if (permission != PermissionStatus.granted) {
      await Permission.storage.request();
      permission = await Permission.storage.status;
    }

    return permission == PermissionStatus.granted;
  }

  Future<Directory> _getDownloadDirectory() async {
    if (Platform.isAndroid) {
      return await DownloadsPathProvider.downloadsDirectory;
    }

    // in this example we are using only Android and iOS so I can assume
    // that you are not trying it for other platforms and the if statement
    // for iOS is unnecessary

    // iOS directory visible to user
    return await getApplicationDocumentsDirectory();
  }

  Future<File> get _localFile async {
    final dir = await _getDownloadDirectory();
    final isPermissionStatusGranted = await _requestStoragePermission();

    if (isPermissionStatusGranted) {
      final savePath = path.join(dir.path, _fileName);

      if (await File(savePath).exists()) {
        return File(savePath);
      } else {
        // if the file doesn't already exist, add header
        var file = new File(savePath);
        String header = 'time, imu1x, imu1y, imu1z, imu2x, imu2y, '
            'imu2z, 200x, 200y, 200z, imu1gx, imu1gy, imu1gz, '
            'imu2gx, imu2gy, imu2gz';
        await file.writeAsString('$header\r\n');
        return file;
      }

    } else {
      print("The permission to access local storage was not granted.");
    }
  }

  Future<File> writeFile(String text) async {
    final file = await _localFile;
    return file.writeAsString('$text\r\n', mode: FileMode.append);
  }

  Future<File> cleanFile() async {
    final file = await _localFile;
    return file.writeAsString('');
  }

  String _dataToString(List<int> dataFromDevice) {
    var receivedString = utf8.decode(dataFromDevice);
    final dateTime = DateTime.now();
    // convert current time to a string
    final String now = dateTime.millisecondsSinceEpoch.toString();
    final String dataToStore = now+', '+receivedString; // concatenate both strings
    return dataToStore;

  }

  // method extracting sensor accelerations from the received array of char
  VMath.Vector2 _dataParser(List<int> dataFromDevice) {
    var receivedString = utf8.decode(dataFromDevice);
    var receivedList = receivedString.split(',');
    List<double> accList = new List(9);
    List<double> gyroList = new List(6);
    double parsed = 0;
    // we expect to receive 15 different values from the sensors
    // accX, accY, accZ, accX, accY, accZ, accX, accY, accZ, gyroX, gyroY, gyroZ, gyroX, gyroY, gyroZ
    if (receivedList.length == 15) {
      print("Received a string with 15 values");
      for (int i=0; i<receivedList.length; i++) {
        parsed = double.tryParse(receivedList[i]) ?? 0;
        if(i < 6){
          accList[i] = parsed.abs();
        } else if (i < 9){
          accList[i] = 0;
        } else {
          gyroList[i-9] = parsed.abs();
        }
      }
      double maxAcc = accList.reduce(max);
      double maxGyro = gyroList.reduce(max);
      return VMath.Vector2(maxAcc, maxGyro);
    } else {
      print("Received a string with an incorrect number of values");
      return VMath.Vector2(0, 0);
    }
  }

  void connectToDevice() async {
    Uuid current = Uuid.parse("beb5483e-36e1-4688-b7f5-ea07361b26a8");
    QualifiedCharacteristic currentCharacteristic = QualifiedCharacteristic(
        characteristicId: current,
        serviceId: checkyService,
        deviceId: headAccMonitor.id
    );
    _deviceDetail.connect(headAccMonitor.id).whenComplete(() =>
    // Delayed since Android's BLE stack needs some time after connection before discovery
    Future.delayed(const Duration(seconds: 5), () async { // added async
      // request a larger MTU size because messages from sensors are about 90 bytes
      // (which exceeds the 23-byte BLE standard)
      final mtu = await  _deviceDetail.requestMtu(deviceId:headAccMonitor.id, mtu: 150);
      print("Negotiated MTU: $mtu bytes");
      int dataCounter = 0;
      Future.delayed(const Duration (seconds: 2), () {
        firstData = true;
        _deviceDetail.discoverServices(headAccMonitor.id)
            .whenComplete(() async {
          // Subscribe to Current Heart Rate Characteristic
          _deviceDetail.subscribe(currentCharacteristic).listen((data) async {
            print("Received current data $data");
            var currentValue = _dataParser(data);
            setState(() {
              _currentLinearAcc = currentValue.x.round();
              _currentAngularAcc = currentValue.y.round();
            });
            dataCounter++;
            print("Number of data packets received: $dataCounter");
            // store string to local file
            String dataToStore = _dataToString(data);
            // var file = await storage.writeFile(dataToStore);
            var file = await writeFile(dataToStore);
          }, onError: (error) => print("Error listening to charac $error"));
        }
        ); // when discoverServices completed
      }
      );
    }
    )
    );
  }

  void startBluetoothScan() async {
    headAccMonitor = await Navigator.push(
        context,
        MaterialPageRoute(builder: (context) => DeviceListScreen())
    );
    if (headAccMonitor == null) return;
    print("Received device: " + headAccMonitor.name);
    setState(() {
      connectToDevice();
    });
  }
}