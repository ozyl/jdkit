import 'jdkit_platform_interface.dart';

class Jdkit {
  Future<String?> getPlatformVersion() {
    return JdkitPlatform.instance.getPlatformVersion();
  }

  Future<Map?> init(appKey, appSecretKey) {
    return JdkitPlatform.instance.init(appKey, appSecretKey);
  }

  Stream<Map<dynamic, dynamic>>  openJDUrl(url, {Map? userInfo}) {
    return JdkitPlatform.instance.openJDUrl(url, userInfo:userInfo);
  }

  Stream<Map<dynamic, dynamic>>  openJXUrl(url, {Map? userInfo}) {
    return JdkitPlatform.instance.openJXUrl(url, userInfo: userInfo);
  }
}
