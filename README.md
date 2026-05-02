# EC-Shared-Lib

RoboMaster 多个 C 嵌入式项目共用的源码库。

## 使用指南

### 0. 添加 submodule

业务项目建议以 Git Submodule 方式引入此分支：

```bash
# 在业务仓库根
git submodule add -b release <url-to-this-repo.git> ./EC-Shared-Lib
git submodule update --init --recursive
```

### 1. 准备一份配置头

业务工程内放一份配置头，路径/文件名自定（例如 `firmware/my_sl_config.h`），用 `#define` 覆盖需要调整的开关：

```c
// firmware/my_sl_config.h
#pragma once

#define SL_USE_FREERTOS   1
#define SL_USE_HAL_F4 1
#define SL_INCLUDE_HAL    "stm32f4xx_hal.h"
```

可覆盖项的完整清单见 [config/sl_config_default.h](config/sl_config_default.h)（每项注释了语义、单位、合法值）。**不要**修改 submodule 内的任何文件——所有定制都通过这份配置头完成。

### 2. 接入构建系统

无论用什么构建系统，三件事必须做：

1. **加 include path**：`./EC-Shared-Lib/source`
2. **编译库源码**：`./EC-Shared-Lib/source/**/*.c`。
3. **传入配置头宏**：`-DSL_USER_CONFIG=\"firmware/my_sl_config.h\"`（双引号必须在编译命令里转义）。
    
### 3. 升级库版本

```bash
# 进入 Submodule 目录，将 Submodule 签出到目标版本（release 分支上的某个 commit）
cd ./EC-Shared-Lib
git fetch
git checkout release        # 签出到最新的 release，或手动指定某个具体 tag / commit

# 回到项目根目录，在主仓库中提交 Submodule 的变更
cd ..
git add ./EC-Shared-Lib
git commit -m "update EC-Shared-Lib"
```
