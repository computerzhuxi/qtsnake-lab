# 里程碑 3：logging 日志系统

## 做什么

提供跨模块通用的日志系统，支持控制台和文件双输出、级别过滤、时间戳。

## 怎么做

- 单例 Logger，通过 `LOG_DEBUG/INFO/WARN/ERROR` 宏调用
- 支持 setMinLevel() 动态控制输出级别
- 支持 setFile() 将日志同时写入文件
- 输出格式：`YYYY-MM-DD HH:MM:SS [LEVEL] [TAG] message`

## 为什么这样做

- **单例模式**：全局唯一，避免各模块重复创建。线程安全由 C++11 static 局部变量保证
- **宏接口比函数调用更简洁**：`LOG_INFO("tag", "msg")` 比 `Logger::instance().log(...)` 易读
- **级别过滤在运行时**：调试时开 Debug，发布时开 Warning，不需要重新编译
