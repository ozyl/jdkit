#import "JdkitPlugin.h"
#import "JDSDK/JDKeplerSDK.h"

@interface JdkitPlugin()<FlutterStreamHandler>
@property(nonatomic, strong) FlutterEventSink eventSink;
@end

@implementation JdkitPlugin

+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
    FlutterMethodChannel* channel = [FlutterMethodChannel methodChannelWithName:@"jdkit"
                                                                binaryMessenger:[registrar messenger]];
    JdkitPlugin* instance = [[JdkitPlugin alloc] init];
    [registrar addMethodCallDelegate:instance channel:channel];
    
    // 初始化 EventChannel
    FlutterEventChannel* eventChannel = [FlutterEventChannel eventChannelWithName:@"jdkit_events"
                                                                  binaryMessenger:[registrar messenger]];
    [eventChannel setStreamHandler:instance];
}

- (void)handleMethodCall:(FlutterMethodCall*)call result:(FlutterResult)result {
    if ([@"init" isEqualToString:call.method]) {
        NSDictionary *params = call.arguments;
        [[KeplerApiManager sharedKPService] asyncInitSdk:params[@"appKey"] secretKey:params[@"appSecretKey"] sucessCallback:^{
            NSLog(@"初始化成功");
            result(@{@"code": @0});
        } failedCallback:^(NSError *error) {
            NSLog(@"初始化失败");
            result(@{@"code": @-1, @"msg": error.description});
        }];
    } else if ([@"openJDUrl" isEqualToString:call.method]) {
        [self handleOpenUrl:call result:result isJX:NO];
    } else if ([@"openJXUrl" isEqualToString:call.method]) {
        [self handleOpenUrl:call result:result isJX:YES];
    } else {
        result(FlutterMethodNotImplemented);
    }
}

- (void)handleOpenUrl:(FlutterMethodCall*)call result:(FlutterResult)result isJX:(BOOL)isJX {
    NSDictionary *params = call.arguments;
    NSString *url = params[@"url"];
    NSDictionary *userInfo = params[@"userInfo"] ?: @{};
    
    void (^successCallback)(void) = ^{
        if (self.eventSink) {
            self.eventSink(@{@"state": @0, @"error": [NSNull null]});
        }
    };
    
    void (^failureCallback)(NSInteger, NSString *) = ^(NSInteger code, NSString *failedUrl) {
        if (self.eventSink) {
            self.eventSink(@{@"state": @(code), @"error": failedUrl ?: @"Unknown error"});
        }
    };
    
    if (isJX) {
        [[KeplerApiManager sharedKPService] openJXPageWithUrl:url userInfo:userInfo success:successCallback failure:failureCallback];
    } else {
        [[KeplerApiManager sharedKPService] openKeplerPageWithURL:url userInfo:userInfo successCallback:successCallback failedCallback:failureCallback];
    }
    
    result(@{@"code": @0}); // 方法调用成功
}

#pragma mark - FlutterStreamHandler

- (FlutterError *)onListenWithArguments:(id)arguments eventSink:(FlutterEventSink)events {
    self.eventSink = events;
    return nil;
}

- (FlutterError *)onCancelWithArguments:(id)arguments {
    self.eventSink = nil;
    return nil;
}

@end
