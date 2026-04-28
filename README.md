# EC-Shared-Lib

RoboMaster 多个 C 嵌入式项目共用的源码库。

仓库有两个长期分支：

- **`main`**：开发分支。完整仓库，所有改动打到这里。仅有 test 目标，不产出二进制。
- **`release`**：发布分支。由 GitHub Actions 从 `main` 自动抽取并重组而成——**不要手动 push**。业务项目以 Git Submodule 方式引入此分支。

## 使用

### 0. 添加 submodule

业务项目以 Git Submodule 方式引入 `release` 分支：

```bash
# 在业务仓库根
git submodule add -b release git@github.com:RM-DragoPass-EC-Group/EC-Shared-Lib.git third_party/EC-Shared-Lib
git submodule update --init --recursive
```

`release` 分支只含实际库源码，可直接整体接入业务工程。

### 1. 准备一份配置头

业务工程内放一份配置头，路径/文件名自定（例如 `firmware/my_sl_config.h`），用 `#define` 覆盖需要调整的开关：

```c
// firmware/my_sl_config.h
#pragma once

#define SL_USE_FREERTOS   1
#define SL_USE_HAL        1
#define SL_HAL_VERSION_F4 1
#define SL_INCLUDE_HAL    "stm32f4xx_hal.h"
```

可覆盖项的完整清单见 [src/config/sl_config_default.h](src/config/sl_config_default.h)（每项注释了语义、单位、合法值）。**不要**修改 submodule 内的任何文件——所有定制都通过这份配置头完成。

### 2. 接入构建系统

无论用什么构建系统，三件事必须做：

1. **加 include path**：`third_party/EC-Shared-Lib/source`
2. **编译库源码**：`third_party/EC-Shared-Lib/source/**/*.c`。
3. **传入配置头宏**：`-DSL_USER_CONFIG=\"firmware/my_sl_config.h\"`（双引号必须在编译命令里转义）。

### 3. 升级库版本

```bash
cd third_party/EC-Shared-Lib
git fetch
git checkout release          # 或某个具体 tag / commit
cd ../..
git add third_party/EC-Shared-Lib
git commit -m "bump EC-Shared-Lib"
```

## 开发

### 0. 快速开始

```bash
# 1. 安装依赖工具（见下节「环境要求」）
# 2. 克隆 main 分支
git clone git@github.com:RM-DragoPass-EC-Group/EC-Shared-Lib.git
cd EC-Shared-Lib

# 3. 一次性检查工具版本是否符合要求
rake check_tools

# 4. 生成 IDE 编译数据库（切换 mixin 后也要重跑）
#    执行之后，重启 IDE 或 clangd 就能正确解析所有模块与配置选项了
rake compile_db

# 5. 跑完整测试矩阵（pure / rtos+hal_f4 / rtos+dsp+hal_h7 / all_on）
rake test

# 6. 想跑完整 CI（fmt + lint + test + coverage）
rake ci
```

### 1. 环境要求

各平台（Ubuntu / macOS / Windows）的安装步骤见 [docs/setup.md](docs/setup.md)。

| 工具         | 版本   | 备注                                                                          |
| ------------ | ------ | ----------------------------------------------------------------------------- |
| Ruby         | >= 3.0 | Ceedling 1.0+ 要求                                                            |
| Ceedling     | >= 1.0 | `gem install ceedling`；用到 plugin API、mixin、`use_test_preprocessor:tests` |
| clang-format | >= 15  | `IndentPPDirectives` / `PPIndentWidth` / `InsertBraces` 需要 15+              |
| clang-tidy   | ~> 18  | **锁 18.x**：19+ 移除了 `StaticFunctionCase` / `*Pattern` 命名选项            |
| gcc          | 任意   | Ceedling 默认走 gcc；测试 + coverage 用                                       |
| gcovr        | 任意   | 覆盖率报告生成                                                                |

工具二进制名走 ENV 变量覆盖：

```bash
CLANG_TIDY=clang-tidy-18 rake lint     # 系统默认是别的版本时
CLANG_FORMAT=clang-format-15 rake fmt
```

紧急情况下绕过版本检查：`SKIP_VERSION_CHECK=1 rake ...`（不推荐，仅用于调试）。

跑一次 `rake check_tools` 会一次性检查 Ruby / Ceedling / clang-format / clang-tidy 四个工具，缺哪个或版本不符都会清晰报错。

### 2. 项目结构

```
src/
  source/<module>/         # 库源码 + 紧邻测试
    sl_<module>.{c,h}
    test_sl_<module>.c
  config/                  # 库配置头
    sl_config_default.h    # 用户可调默认值
    internal.h             # 库内部入口
test/
  mixins/                  # Ceedling mixins（pure / rtos / dsp / hal_f4 / hal_h7 / all_on）
  support/                 # 测试期共享头
deps/stubs/                # 测试期 vendor 桩头（FreeRTOS / arm_math / hal_*）
.github/workflows/         # CI
.clang-format / .clang-tidy
project.yml                # Ceedling 主配置
Rakefile                   # 构建入口（详见下节）
CLAUDE.md                  # 设计约束与详细规范
```

### 3. Rakefile 命令

仓库根 [Rakefile](Rakefile) 是日常工作与 CI 的统一入口（跨平台 Ruby，不依赖 shell）。

所有命令可以通过 `rake -T` 查看。

| 任务               | 内容                                                                                            |
| ------------------ | ----------------------------------------------------------------------------------------------- |
| `rake check_tools` | 一次性检查 Ruby / Ceedling / clang-format / clang-tidy 版本                                     |
| `rake fmt`         | `clang-format --dry-run --Werror`（不修改文件，CI 用）                                          |
| `rake fmt:fix`     | `clang-format -i` 批量应用                                                                      |
| `rake compile_db`  | 清空旧 DB → `ceedling test:all --mixin all_on` 全量编译刷新 IDE DB                              |
| `rake lint`        | 自动 `compile_db` → `clang-tidy -p build/artifacts ...` 严格检查                                |
| `rake lint:fix`    | clang-tidy `--fix` 自动修复（修改文件；编译错误的 TU 跳过）                                     |
| `rake test`        | CI 测试矩阵（pure / rtos+hal_f4 / rtos+dsp+hal_h7 / all_on）                                    |
| `rake coverage`    | 清 `build/gcov` → `ceedling gcov:all --mixin all_on`，HTML 报告于 `build/artifacts/gcov/gcovr/` |
| `rake ci`          | fmt + lint + test + coverage，CI workflow 调用此项                                              |
| `rake clean`       | 删 `build/`                                                                                     |

直接调 `ceedling` / `clang-format` / `clang-tidy` 仍然合法——Rakefile 只是聚合常用工作流，不绕开 ceedling。

### 4. IDE / clangd 配置

新克隆后跑一次 `rake compile_db` 生成 `build/artifacts/compile_commands.json`。VSCode + clangd 的设置已写在 [.vscode/settings.json](.vscode/settings.json)，无需额外配置。切换 mixin 想让 IDE 视角同步时重跑 `rake compile_db`。

### 5. 编码规范

#### 命名

| 种类        | 形式                                         | 例                                  |
| ----------- | -------------------------------------------- | ----------------------------------- |
| 公共函数    | `sl_<module>_<verb>`                         | `sl_pid_update`                     |
| 公共类型    | `sl_<module>_t` / `sl_<module>_<noun>_t`     | `sl_pid_t`, `sl_pid_config_t`       |
| 公共宏      | `SL_<MODULE>_<NAME>`                         | `SL_PID_MAX_INTEGRAL`               |
| 配置宏      | `SL_USE_<DEP>` / `SL_INCLUDE_<DEP>`          | `SL_USE_FREERTOS`, `SL_INCLUDE_HAL` |
| 文件名      | `sl_<module>.{c,h}`, `test_sl_<module>.c`    | `sl_pid.h`                          |
| 内部 static | 无前缀，snake_case                           | `static void clamp(...)`            |
| 头文件防重  | **`#pragma once`**（不再使用 include guard） | 所有 `.h` 都用                      |

公共 API 一律 `sl_` 前缀（shared lib），文件内 `static` 标识符不要前缀。命名约束由 clang-tidy 在 lint 层强制（这是把 clang-tidy 锁在 18.x 的原因）。

#### 格式

由 [.clang-format](.clang-format) 锁定

#### 业务方覆写

业务方**不能修改**本仓库代码。所有差异化必须通过覆写 config 实现：

1. 业务工程内放一份配置头（路径/文件名自定，例如 `firmware/my_sl_config.h`），里面 `#define SL_USE_FREERTOS 1` 等。
2. 编译命令加 `-DSL_USER_CONFIG=\"firmware/my_sl_config.h\"`（路径在业务自己的 include path 内可解析即可）。
3. 业务自己的 include path 里加 submodule 根目录下的 `src/`，库代码与配置头都能解析到。

仓库内 `ceedling test` 时**不**定义 `SL_USER_CONFIG`，mixin 直接通过 `-DSL_USE_X=1` 注入个别开关，未注入项由 `sl_config_default.h` 兜底。

### 6. 提交前检查清单

```bash
rake fmt           # 不通过先 rake fmt:fix
rake test          # 完整测试矩阵
rake ci            # 想做完整 CI 验证（含 lint + coverage）
```

### 7. 发布

发布的唯一入口是 **GitHub Release**。在 GitHub UI 上 Publish 一个 Release 后，[GHA](.github/workflows/release.yml) 自动从 Release 关联的 tag 抽取 `src/` 内容，重写到 `release` 分支。**不要手动 push `release` 分支**——会被下次发布覆盖。

#### Tag 命名规范

Tag 必须是 **以 `v` 开头的 SemVer**（[semver.org](https://semver.org/)）：

```
v<MAJOR>.<MINOR>.<PATCH>[-<prerelease>][+<build>]
```

| 形式            | 例                              | 说明                                     |
| --------------- | ------------------------------- | ---------------------------------------- |
| 正式版本        | `v1.0.0`、`v2.3.1`              | 给业务方默认追的稳定版                   |
| 预发布          | `v1.0.0-rc.1`、`v1.0.0-alpha.2` | SemVer 规则下排序在同号正式版**之前**    |
| 带 build 元数据 | `v1.2.3+build.5`                | 极少用；build 元数据不参与 SemVer 优先级 |

不合法的例子：`1.0.0`（缺 `v`）、`v1.0`（缺 patch）、`v1.0.0.0`（多了一段）、`release-1.0.0`（前缀错）——workflow 会拒绝。

#### 发布步骤

1. 在 `main` 上把要发的 commit 合进去（走正常 PR 流程）。
2. GitHub → **Releases** → **Draft a new release**：
   - **Choose a tag**：填新 tag（如 `v1.2.3`），target 选 `main`（GitHub 会顺手建 tag 并打在 `main` HEAD）；或者预先在 `main` 上某个 commit 打好 tag 再选已有 tag。
   - **Release title / notes**：自由填。
   - 如果是预发布，勾上 **Set as a pre-release**。
3. **Publish release**。Actions 里能看到 `Release` workflow 自动跑起来；通过后 `release` 分支已更新。

#### Workflow 校验

发布过程中会逐项校验，任一不过则发布失败、`release` 分支不动：

- **SemVer 格式**：tag 必须匹配上面的规范。
- **tag 落在 `main` 上**：禁止从 feature 分支或非主线 commit 发布——避免业务方拿到没经过 `main` CI 的代码。
- **防倒灌**：新 tag 的 SemVer 必须 ≥ `release` 分支上次发布的 tag。例如已发 `v2.0.0` 之后想再发 `v1.5.1` 会被拒（这是设计上的取舍：本仓库不维护旧 major 的 hotfix 线，`release` 分支永远跟随主线最新版本）。
- 与上次完全相同的 tag 重发**幂等**：内容无变化跳过 push，不会失败。

#### 触发类型

`release` 事件的 `published` type 同时覆盖正式发布和预发布；草稿（draft）和编辑（edit）不会触发。预发布同样会更新 `release` 分支——业务方若想固定在某个稳定版本，应在 submodule 里 pin 具体 tag 而非追 `release` 分支。
