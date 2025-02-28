package com.byl.jdkit;

import android.app.Application;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import androidx.annotation.NonNull;

import com.jd.jdsdk.JdHelper;

import java.util.HashMap;
import java.util.Map;

import io.flutter.embedding.engine.plugins.FlutterPlugin;
import io.flutter.plugin.common.EventChannel;
import io.flutter.plugin.common.MethodCall;
import io.flutter.plugin.common.MethodChannel;
import io.flutter.plugin.common.MethodChannel.MethodCallHandler;
import io.flutter.plugin.common.MethodChannel.Result;

/**
 * JdkitPlugin
 */
public class JdkitPlugin implements FlutterPlugin, MethodCallHandler, EventChannel.StreamHandler {
    private static final String TAG = "JdkitPlugin";
    private static final String METHOD_INIT = "init";
    private static final String METHOD_OPEN_JD_URL = "openJDUrl";
    private static final String METHOD_OPEN_JX_URL = "openJXUrl";
    private static final String EVENT_CHANNEL_NAME = "jdkit_events";

    private Context context;
    private MethodChannel methodChannel;
    private EventChannel eventChannel;
    private Boolean initialized = false;
    private EventChannel.EventSink eventSink; // 用于发送多次回调事件

    @Override
    public void onAttachedToEngine(@NonNull FlutterPluginBinding flutterPluginBinding) {
        context = flutterPluginBinding.getApplicationContext();
        methodChannel = new MethodChannel(flutterPluginBinding.getBinaryMessenger(), "jdkit");
        methodChannel.setMethodCallHandler(this);

        // 初始化 EventChannel
        eventChannel = new EventChannel(flutterPluginBinding.getBinaryMessenger(), EVENT_CHANNEL_NAME);
        eventChannel.setStreamHandler(this);
    }

    @Override
    public void onMethodCall(@NonNull MethodCall call, @NonNull Result result) {
        try {
            switch (call.method) {
                case METHOD_INIT:
                    handleInit(call, result);
                    break;
                case METHOD_OPEN_JD_URL:
                    handleOpenUrl(call, result, false);
                    break;
                case METHOD_OPEN_JX_URL:
                    handleOpenUrl(call, result, true);
                    break;
                default:
                    result.notImplemented();
                    break;
            }
        } catch (Exception e) {
            Log.e(TAG, "Error handling method call: " + call.method, e);
            result.error("UNKNOWN_ERROR", e.getMessage(), null);
        }
    }

    @Override
    public void onDetachedFromEngine(@NonNull FlutterPluginBinding binding) {
        methodChannel.setMethodCallHandler(null);
        eventChannel.setStreamHandler(null);
    }

    @Override
    public void onListen(Object arguments, EventChannel.EventSink events) {
        this.eventSink = events; // 保存 EventSink 用于发送事件
    }

    @Override
    public void onCancel(Object arguments) {
        this.eventSink = null; // 取消监听时清空 EventSink
    }

    private void handleInit(@NonNull MethodCall call, @NonNull Result result) {
        Map<String, String> arguments = castArguments(call.arguments, Map.class);
        if (arguments == null) {
            result.error("INVALID_ARGUMENTS", "Arguments must be a Map<String, String>", null);
            return;
        }

        String appKey = arguments.get("appKey");
        String appSecretKey = arguments.get("appSecretKey");

        if (appKey == null || appSecretKey == null) {
            result.error("MISSING_ARGUMENTS", "appKey and appSecretKey are required", null);
            return;
        }
        if(initialized){
            result.success(getResult(0));
            return;
        }
        JdHelper.init((Application) context, appKey, appSecretKey, (success, error) -> {
            if (success) {
                initialized = true;
                result.success(getResult(0));
            } else {
                result.success(getResult(-1, error));
            }
        });
    }

    private void handleOpenUrl(@NonNull MethodCall call, @NonNull Result result, boolean isJX) {
        Map<String, Object> arguments = castArguments(call.arguments, Map.class);
        if (arguments == null) {
            result.error("INVALID_ARGUMENTS", "Arguments must be a Map<String, Object>", null);
            return;
        }

        JdHelper.OpenCallBack callback = (state, error) -> {
            if (eventSink != null) {
                Map<String, Object> event = new HashMap<>();
                event.put("state", state);
                event.put("error", error);
                new Handler(
                        Looper.getMainLooper()).post(()->{
                    eventSink.success(event); // 通过 EventSink 发送事件
                });
            } else {
                Log.w(TAG, "EventSink is null. Cannot send callback event.");
            }
        };

        if (isJX) {
            JdHelper.openJXUrl((Application) context, arguments, callback);
        } else {
            JdHelper.openJDUrl((Application) context, arguments, callback);
        }

        result.success(getResult(0)); // 表示方法调用成功
    }

    private <T> T castArguments(Object arguments, Class<T> type) {
        if (type.isInstance(arguments)) {
            return type.cast(arguments);
        }
        return null;
    }

    private Map<String, Object> getResult(int code) {
        return getResult(code, null);
    }

    private Map<String, Object> getResult(int code, Object msg) {
        Map<String, Object> map = new HashMap<>();
        map.put("code", code);
        if (msg != null) {
            map.put("msg", msg);
        }
        return map;
    }
}