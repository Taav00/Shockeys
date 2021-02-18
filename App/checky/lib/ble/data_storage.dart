import 'dart:io';
import 'dart:async';

import 'package:path/path.dart' as path;
import 'package:path_provider/path_provider.dart';
import 'package:downloads_path_provider_28/downloads_path_provider_28.dart';
import 'package:permission_handler/permission_handler.dart';


class TextStorage {

  final String _fileName = "data.txt";

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
      return File(savePath);
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

}