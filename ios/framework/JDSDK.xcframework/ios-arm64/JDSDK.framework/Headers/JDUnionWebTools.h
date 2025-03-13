//
//  JDUnionWebTools.h
//  JDKeplerSDK
//
//  Copyright © 2023 JD.K. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebKit/WebKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface JDUnionWebTools : NSObject

#define kJDUnionIncomeUrl @"incomeUrl"

//媒体配置回调
typedef NSDictionary* _Nullable (^jdUnionConfigHandler)(NSString *__nullable url, NSDictionary *__nullable extraInfo);

//联盟callback回调
typedef BOOL (^jdUnionCallbackHandler)(NSString *__nullable url, NSDictionary *__nullable extraInfo);

+ (void)setDebug:(BOOL)debug;

/// 注入联盟方法
/// - Parameters:
///   - webView: 当前webview容器实例
///   - delegate: 自己指定实现webview的WKNavigationDelegate的实例
///   - configHandler: 媒体侧需要根据返回参数处理逻辑，并给H5返回指定的配置@{@"incomeUrl":@"http://jd.com"} ：incomeUrl是收益链接
///   - callbackHandler: 媒体侧需要根据返回参数处理，并给H5返回是否需要H5自己处理
+ (void)InjectWithWebview:(WKWebView *)webView withDelegate:(id __nullable)delegate withConfigHandler:(jdUnionConfigHandler)configHandler withCallbackHandler:(jdUnionCallbackHandler)callbackHandler;

@end

NS_ASSUME_NONNULL_END
