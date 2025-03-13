//
//  KeplerApiManager.h
//  KeplerApp
//  提供Kepler服务
//  Created by JD.K on 16/6/20.
//  Copyright © 2016年 JD.K. All rights reserved.

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/** 初始化成功回调 */
typedef void (^initSuccessCallback)();
/** 初始化失败回调 */
typedef void (^initFailedCallback)(NSError * _Nonnull error);
/**
 打开成功回调
 */
typedef void(^OpenSuccessBlock)();

/**
 * 响应失败的错误回调，返回一个错误的链接或信息
 */
typedef void(^OpenFailedBlock)(NSInteger code, NSString * _Nonnull url);

typedef enum {
    KeplerErrorCodeUnInstalled = 422, //未安装京东app
    KeplerErrorCodeWhited, //本地url不在白名单中
    KeplerErrorCodeResponsed, //网络返回数据错误或code不为0
    KeplerErrorCodeNoSid, //sid为空
    KeplerErrorCodeInValidApp, //应用不合法
    KeplerErrorCodeUnInstalledJXAPP, //未安装京喜app
    KeplerErrorCodeNoNeedOpenJXFailed, //无法打开京喜商品，返回h5链接
    KeplerErrorCodeOpenJDFaild, //呼起京东失败
    KeplerErrorCodeOpenJXFaild, //呼起京喜失败
    KeplerErrorCodeUnInstalledJXLiteAPP, //未安装京喜特价app
    KeplerErrorCodeNoNeedOpenJXLiteFailed, //无法打开京喜特价商品，返回h5链接
    KeplerErrorCodeOpenJXLiteFaild, //呼起京喜特价失败

} KeplerErrorCode;

typedef enum {
    KeplerJXErrorCodeSucceed = 0,           //正常呼起
    KeplerJXErrorCodeReturnUrl = 1,         //兜底url
    KeplerJXErrorCodeNoSkuUrl = 2,          //非sku链接
    KeplerJXErrorCodeAnalysisFailed = 3,    //逆解析失败
    KeplerJXErrorCodeNoParameter = -1,      //参数为空
    KeplerJXErrorCodeUnusual = -99,         //异常
} KeplerJXErrorCode;


FOUNDATION_EXTERN const NSString *kJDKeplerReturnUrl_appName;
FOUNDATION_EXTERN const NSString *kJDKeplerReturnUrl_appSchema;
FOUNDATION_EXTERN const NSString *kJDKeplerReturnUrl_appBundle;

@interface KeplerApiManager : NSObject

/**
 分佣的 AppKey2
 */
@property (nonatomic, copy) NSString *secondAppKey;
//*********************************     通过京东APP打开链接相关参数      ************************************

/**
 当isOpenByH5为 NO 时，准备跳转到JD APP时会调用这些代码。可以把开启 Loading动画的代码放到这里
 为避免造成混乱，在关闭Kepler界面时，会置为nil。因此需要在每次打开Kepler之前单独设置。
 */
@property (nonatomic, copy) void(^startOpenJDAppBlock)();

/**
 当isOpenByH5为 NO 时，跳转JD APP准备工作完成时会调用这些代码，success为YES表示成功，可以打开JD APP，为NO时表示打开失败。
 为避免造成混乱，在关闭Kepler界面时，会置为nil。因此需要在每次打开Kepler之前单独设置。
 
 update: success为NO且错误码等于-1005时，安全id为空
 */
@property (nonatomic, copy) void(^finishOpenJDAppBlock)(BOOL success,NSError *error);

/**
 *  京东达人内容ID 关闭kepler界面时会清除 如果需要此值 再次打开需要再次设置
 **/
@property (nonatomic, copy) NSString *actId;

/**
 *  京东达人 内容渠道扩展字段 关闭kepler界面时会清除 如果需要此值 再次打开需要再次设置
 **/
@property (nonatomic, copy) NSString *ext;

/**
 * 是否走服务端的渠道，默认走本地渠道
 */
@property (nonatomic, assign) BOOL isServerChannel;

/**
 打开京东超时时间设置 关闭 Kepler 界面时不会重置 默认为60
 */
@property (nonatomic, assign) NSTimeInterval openJDTimeout;


//*******************************************************************************************************

/**
 *  KeplerApiManager 单例
 *
 *  @return KeplerApiManager 单例
 */
+ (KeplerApiManager *)sharedKPService;

/**
 *  注册Kepler 服务
 *
 *  @param appKey      注册的appKey
 *  @param appSecret   注册的secretKey
 */
- (void)asyncInitSdk:(NSString *)appKey
           secretKey:(NSString *)appSecret
      sucessCallback:(initSuccessCallback)sucessCallback
      failedCallback:(initFailedCallback)failedCallback;

/**
 跳转京东app

 @param url 调用方传入的URL参数
 @param userInfo 调用sdk时传入的kepler自定义参数
 @param failedCallback code 表示错误编码, url 引起调用失败的url
 */
- (void)openKeplerPageWithURL:(NSString *)url
                     userInfo:(NSDictionary *)userInfo
              successCallback:(OpenSuccessBlock _Nullable)success
               failedCallback:(OpenFailedBlock _Nonnull)failure;


/**
 根据url跳转京喜App，根据接口返回结果兼容京东App商品跳转
 @param url  商品链接
 @param userInfo 自定义参数
 @param successBlock 呼起App成功的回调
 @param failureBlock 呼起失败回调。打开原生App失败会以code+url方式返回，可用webview打开url
 @discussion 返回键信息通过userInfo参数传入，需通过外部变量kJDKeplerReturnUrl_appName，kJDKeplerReturnUrl_appSchema，kJDKeplerReturnUrl_appBundle设置key值，对应value分别为接入应用的应用名，schema，Bundle Identifier。
 */
- (void)openJXPageWithUrl:(NSString *)url
                 userInfo:(NSDictionary *)userInfo
                  success:(OpenSuccessBlock)successBlock
                  failure:(OpenFailedBlock)failureBlock;

/**
 根据url跳转京喜特价App，根据接口返回结果兼容京东App商品跳转
 @param url  商品链接
 @param userInfo 自定义参数
 @param successBlock 呼起App成功的回调
 @param failureBlock 呼起失败回调。打开原生App失败会以code+url方式返回，可用webview打开url
 @discussion 返回键信息通过userInfo参数传入，需通过外部变量kJDKeplerReturnUrl_appName，kJDKeplerReturnUrl_appSchema，kJDKeplerReturnUrl_appBundle设置key值，对应value分别为接入应用的应用名，schema，Bundle Identifier。
 */
- (void)openJXLitePageWithUrl:(NSString *)url
                     userInfo:(NSDictionary *)userInfo
                      success:(OpenSuccessBlock)successBlock
                      failure:(OpenFailedBlock)failureBlock;
@end

NS_ASSUME_NONNULL_END
