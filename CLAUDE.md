# EC-Shared-Lib

RoboMaster 多个 C 嵌入式项目共用的源码库。仅有 test 目标，不产出二进制——库以源码形式被业务项目以 `git submodule` 引入。

## 核心约束

读这份文档的任何后续设计/实现决策都必须先满足以下约束：

1. **库代码在业务仓库中只读**。业务方不能修改本仓库代码，所有差异化必须通过覆写 config 实现。任何"靠业务方改一行库代码就行"的方案都不可接受。
2. **公共 API 一律 `sl_` 前缀**（shared lib）。函数/类型用 snake_case (`sl_pid_init`, `sl_pid_t`)；宏用 `SL_` 前缀的 UPPER_SNAKE (`SL_USE_FREERTOS`)。文件内 `static` 标识符不需要前缀。
3. **外部依赖路径由宏配置**，不写死。库代码绝不出现 `#include "FreeRTOS.h"`，必须 `#include SL_INCLUDE_FREERTOS`（路径名由业务 / mixin 覆盖）。每个源文件**自己**用 `#if SL_USE_X` 决定要不要拉 vendor 头——library 不做"一股脑" auto-include。
4. **不向后兼容默认未定义的宏**。每个 `SL_*` 宏都必须在 [config/sl_config_default.h](config/sl_config_default.h) 有默认值；库代码可以直接 `#if` 使用而不需先做 `#ifndef ... #define ... #endif` 兜底。
5. **库代码绝不直接 `#include "sl_config_default.h"`**——必须经由 [config/internal.h](config/internal.h) 拿到 `SL_*` 宏。`sl_config_default.h` 只承载用户可调的配置项；派生宏 / sanity check 集中到 `internal.h`。
6. **业务方覆写通过 `-DSL_USER_CONFIG=\"path/to/header.h\"`**。不用约定 include path、不放 shim 文件——业务在编译命令里给一个宏指向自己的配置头，[config/internal.h](config/internal.h) 在最前面 `#include SL_USER_CONFIG` 拉它。这样我们不必猜业务的 include path 布局。
7. **库内部 include 必须用相对路径**（如 `#include "../../config/internal.h"`）。原因同上：业务方对本库的 include path 配置无法预知，相对路径让库源文件之间的引用基于文件位置解析。**唯一例外**：业务通过 `SL_USER_CONFIG` / `SL_*_INCLUDE` 提供的头按名引入。
8. **代码注释默认中文**。代码内所有非平凡注释用中文（数学算式、TODO 标签、IWYU pragma 等机械文字仍用原文）。

## 分支策略

- `main`：开发分支。完整仓库——`src/`、`test/`、`config/`、`project.yml`、`deps/`、CI workflow。日常工作在这里。
- `release`：发布分支。仅 `src/` 与 `config/` 内容（GitHub Actions 从 `main` 抽取重写）。业务仓库以 submodule 方式引入此分支，目录干净。
- **不要手动 push `release`**——它由 CI 重写，手改会被覆盖。所有改动打到 `main`。

## 目录布局

```
src/
  <module>/
    sl_<module>.c
    sl_<module>.h
    sl_<module>_rtos.c        # 可选，case 1：要求 SL_USE_FREERTOS=1，否则 #error
    test_sl_<module>.c        # 测试紧邻实现
    test_sl_<module>_rtos.c   # 可选，配套门卫的测试文件
config/
  sl_config_default.h         # 库内默认值，仅用户可调项；库代码不直接 include
  internal.h                  # 库内部入口：先 include 业务 SL_USER_CONFIG，再 include 默认头 + 全局 sanity
test/
  support/
    sl_config.h               # 测试用配置（仅 test 构建可见）
  mixins/                     # Ceedling mixins，每个对应一套宏配置
deps/
  stubs/                      # 测试期 vendor 桩头：FreeRTOS / arm_math / hal_f4 / hal_h7
.github/workflows/            # CI: fmt/lint/test/coverage + release 抽取
.clang-format
.clang-tidy
project.yml                   # Ceedling 主配置
```

每个模块自带源、头、测试。`release` 分支保持 `src/` 与 `config/` 同样的相对结构（仅 `sl_config_default.h` + `internal.h` 两个 config 头），业务方 include path 加上 submodule 根目录即可。

## 命名约定

| 种类        | 形式                                                 | 例                                  |
| ----------- | ---------------------------------------------------- | ----------------------------------- |
| 公共函数    | `sl_<module>_<verb>`                                 | `sl_pid_update`                     |
| 公共类型    | `sl_<module>_t` / `sl_<module>_<noun>_t`             | `sl_pid_t`, `sl_pid_config_t`       |
| 公共宏      | `SL_<MODULE>_<NAME>`                                 | `SL_PID_MAX_INTEGRAL`               |
| 配置宏      | `SL_USE_<DEP>` / `SL_<DEP>_INCLUDE`                  | `SL_USE_FREERTOS`, `SL_INCLUDE_HAL` |
| 文件名      | `sl_<module>.{c,h}`, `test_sl_<module>.c`            | `sl_pid.h`                          |
| 内部 static | 无前缀，snake_case                                   | `static void clamp(...)`            |
| 头文件防重  | **`#pragma once`**（不再使用 `#ifndef ... #define`） | 所有 `.h` 都用                      |

包含规则：

- **完全 pure 的模块** → 不要 include 任何 config 头
- **需要 `SL_USE_*` 或 `SL_*_INCLUDE`** → `#include "../../config/internal.h"`，再用 `#if SL_USE_X #include SL_X_INCLUDE #endif` 自己拉 vendor 头
- 内部 include 一律相对路径（同目录 `"sl_pid.h"`、跨目录 `"../../config/internal.h"`）；vendor 头与业务 `SL_USER_CONFIG` 是例外，按名/按宏引入
- 转发型 wrapper 头中的 `#include` 加 `// IWYU pragma: export`；库内 wrapper-only 引入或 vendor 隐式依赖（如 FreeRTOS 要求 `FreeRTOS.h` 先于 `task.h`）加 `// IWYU pragma: keep`，避免 clang-tidy `misc-include-cleaner` 误删

## 配置系统

include chain：

```
业务 my_config.h          # 业务方提供，#define 想覆盖的 SL_*；放哪里、叫什么名都行
   ▲
   │ #include SL_USER_CONFIG  ← 由 -DSL_USER_CONFIG=\"my_config.h\" 指定路径
internal.h                # 库入口：先 include SL_USER_CONFIG（可选），
   │                       再 include sl_config_default.h，最后做全局 sanity check
   │ #include "sl_config_default.h"
   ▼
sl_config_default.h       # 库内默认值，#ifndef 守卫保留业务覆写
```

业务方流程：

1. 业务仓库工程内放一份配置头（路径/文件名自定，比如 `firmware/my_sl_config.h`），里面 `#define SL_USE_FREERTOS 1` 等。
2. 业务方编译命令加 `-DSL_USER_CONFIG=\"firmware/my_sl_config.h\"`（路径需在业务自己的 include path 内可解析，或用相对/绝对路径）。
3. 库代码视情况 `#include "../../config/internal.h"`——**永远不**直接 include `sl_config_default.h` 或业务的配置头。
4. 在仓库内 `ceedling test` 时**不**定义 `SL_USER_CONFIG`，mixin 直接通过 `-DSL_USE_X=1` 注入个别开关，未注入项由 `sl_config_default.h` 兜底。

[config/sl_config_default.h](config/sl_config_default.h) 只放两类用户可调项：

**功能开关**——`#if` 用，0/1：
```c
#ifndef SL_USE_FREERTOS
#define SL_USE_FREERTOS 0
#endif
#ifndef SL_USE_ARM_DSP
#define SL_USE_ARM_DSP 0
#endif
#ifndef SL_USE_HAL
#define SL_USE_HAL 0
#endif
```

**vendor 头路径**——告诉库去哪里 include：
```c
#ifndef SL_INCLUDE_FREERTOS
#define SL_INCLUDE_FREERTOS      "FreeRTOS.h"
#endif
#ifndef SL_INCLUDE_HAL
#define SL_INCLUDE_HAL           "stm32f4xx_hal.h"
#endif
/* …类似 */
```

[config/internal.h](config/internal.h) 的职责（轻量）：

- 顶部 `#ifdef SL_USER_CONFIG #include SL_USER_CONFIG #endif`：把业务覆盖头拉进来（如有）。
- 然后 `#include "sl_config_default.h"`：默认值通过 `#ifndef` 守卫填空业务未覆写的项。
- 最后放与具体依赖无关的全局 sanity check（如 `SL_USE_HAL=1` 必须恰好选中一个 `SL_HAL_VERSION_*`）。
- **不**自动 include 任何 vendor 头——库源文件自己用 `#if SL_USE_X #include SL_X_INCLUDE #endif` 按需引入。

**新增任何配置项的硬性要求**：

- 用户可调项加到 `sl_config_default.h` 并给默认值，注释说明语义、单位、合法值；
- 全局派生宏 / sanity check 加到 `internal.h`；
- **新增依赖**：默认值（`SL_USE_<DEP>` + `SL_<DEP>_INCLUDE`）放 `sl_config_default.h`；不需要给依赖另外加 wrapper 头——直接在用到的源文件里写 `#if SL_USE_<DEP> #include SL_<DEP>_INCLUDE #endif`；
- 加 mixin 或扩展现有 mixin 覆盖该项的非默认取值（`all_on` 也要更新）。

## 外部依赖：两种处理方式

文件引入外部依赖时，先想清楚是"必须"还是"可选适配"：

### Case 1: 必须依赖该外部库（`#if ... #else #error ... #endif`）

文件无 fallback——只在依赖启用时才有意义。整文件 body 包在 `#if SL_USE_X` 内，配 `#else #error` 拒绝错误配置：

```c
// sl_motor_dji_rtos.c
#include "../../config/internal.h"

#if SL_USE_FREERTOS

#include SL_INCLUDE_FREERTOS      // IWYU pragma: keep — FreeRTOS 要求本头先于其他 FreeRTOS 头
#include SL_INCLUDE_FREERTOS_TASK
#include "sl_motor_dji.h"

int16_t sl_motor_dji_get_speed_threadsafe(...) {
    taskENTER_CRITICAL();
    ...
    taskEXIT_CRITICAL();
}

#else
#error "sl_motor_dji_rtos.c 需要 SL_USE_FREERTOS=1"
#endif
```

**配套要求**：这种文件 **必须不能在依赖关闭的 mixin 下被编译**——`#error` 触发即失败。Ceedling 通过测试文件控制是否挂入：

```c
// test_sl_motor_dji_rtos.c
#include "../../config/internal.h"  // IWYU pragma: keep — 提供 SL_USE_FREERTOS
#include "unity.h"
#include "sl_motor_dji.h"           // 顶层！否则 Ceedling 找不到 sl_motor_dji.c

#if SL_USE_FREERTOS
TEST_SOURCE_FILE("sl_motor_dji_rtos.c")  // 仅 RTOS 启用时才挂入
#endif

void setUp(void) {}
void tearDown(void) {}

#if SL_USE_FREERTOS
void test_xxx(void) { ... }
#endif
```

适用于 RTOS 任务/队列封装、HAL 中断回调注册等"专为某依赖存在的"代码。

### Case 2: 视依赖是否提供自适配（`#if ... #else ... #endif`）

文件有 fallback 路径——依赖启用走加速版，否则走软件实现。vendor 头自己写一行 `#if + #include + #endif` 拉，函数体内同款 `#if/#else` 切实现：

```c
// sl_filter.c
#include "../../config/internal.h"
#include "sl_filter.h"

#if SL_USE_ARM_DSP
#include SL_INCLUDE_ARM_MATH
#endif

float sl_filter_dot(const float *a, const float *b, uint32_t n) {
#if SL_USE_ARM_DSP
    float result;
    arm_dot_prod_f32(a, b, n, &result);
    return result;
#else
    float result = 0.0f;
    for (uint32_t i = 0; i < n; ++i) result += a[i] * b[i];
    return result;
#endif
}
```

适用于算法的 DSP 加速版/纯 C 版切换、可选 RTOS 锁保护等"按需切换"代码。

**硬上限**：单文件行内 `#if` 块 > 3 处时，拆成 case-1 风格独立文件，避免圣诞树。

## 构建入口（Rakefile）

仓库根 [Rakefile](Rakefile) 是日常工作与 CI 的统一入口（跨平台 Ruby，不依赖 shell）：

| 任务              | 内容                                                         |
| ----------------- | ------------------------------------------------------------ |
| `rake fmt`        | `clang-format --dry-run --Werror`（CI 用）                   |
| `rake fmt:fix`    | `clang-format -i` 批量应用                                   |
| `rake compile_db` | 清空旧 DB → `ceedling test:all --mixin all_on` 全量编译      |
| `rake lint`       | 自动 `compile_db` → `clang-tidy -p build/artifacts ...`      |
| `rake test`       | CI 测试矩阵（pure / rtos+hal_f4 / rtos+dsp+hal_h7 / all_on） |
| `rake coverage`   | 清 `build/gcov` → `ceedling gcov:all --mixin all_on`         |
| `rake ci`         | fmt + lint + test + coverage（CI workflow 调用此项）         |
| `rake clean`      | 删 `build/`                                                  |

工具二进制名走 ENV：`CLANG_FORMAT` / `CLANG_TIDY` 可覆盖（默认 `clang-format` / `clang-tidy`）。lint 固定 clang-tidy 18（理由见下文「CI」一节）；若系统默认是别的版本，本地用 `CLANG_TIDY=clang-tidy-18 rake lint` 指定。Ceedling 经 `RbConfig.ruby + -S` 调用，避开 Windows 的 .bat shim 寻路问题。

直接调 `ceedling`/`clang-format`/`clang-tidy` 仍然合法——Rakefile 只是聚合常用工作流，不绕开 ceedling。

## 测试（Ceedling）

入口：`rake test`（CI 矩阵）/ `ceedling test:all`（单次）。详见 [project.yml](project.yml)。

Mixins 一文件一配置，每个 mixin 设置一组宏 + include path 调整：

| Mixin               | 用途                                                    |
| ------------------- | ------------------------------------------------------- |
| `pure`              | 不开任何 `SL_USE_*`，只测算法层。CI 必跑。              |
| `rtos`              | `SL_USE_FREERTOS=1`，链 mock FreeRTOS。                 |
| `dsp`               | `SL_USE_ARM_DSP=1`。                                    |
| `hal_f4` / `hal_h7` | 切换 STM32 HAL 版本（不同 deps include 路径）。         |
| `all_on`            | 所有宏都开（语法兜底，不必对应真实业务配置）。CI 必跑。 |

CI 跑代表性组合，不做全排列（这一矩阵已编入 [Rakefile](Rakefile) `:test` 任务）：
```bash
ceedling test:all --mixin pure
ceedling test:all --mixin rtos --mixin hal_f4
ceedling test:all --mixin rtos --mixin dsp --mixin hal_h7
ceedling test:all --mixin all_on
```

**`all_on` 的意义**：整文件门卫的副作用是被 guard 的代码在某些 mixin 下从未编译，语法/类型错误溜走。`all_on` 确保所有分支至少被编译一次。

测试文件自身也要门卫——RTOS-only 测试整体包在 `#if SL_USE_FREERTOS`，`pure` mixin 下变空，Unity 不会失败。

### IDE 配置

Ceedling 启用了 `compile_commands_json_db` 插件，每次构建会写入 `build/artifacts/compile_commands.json`。Compile DB 的路径通过 `.vscode/settings.json` 告知 clangd（不在仓库根放符号链接），新克隆后需要先跑一次：

```bash
rake compile_db        # = 清旧 DB + ceedling test:all --mixin all_on
```

`compile_commands.json` 反映的是**最近一次构建用的 mixin**。`compile_commands_json_db` 是增量合并（按 file 去重，后写覆盖前写），所以跨 mixin 的连续构建会留下混合条目——`rake compile_db` 在跑 ceedling 前先 `rm` 旧 DB 来保证一致。切换 mixin 后想让 IDE 视角同步，重跑 `rake compile_db` 即可。

[.clangd](.clangd) 在 `.gitignore` 中排除（每个开发者本地配），用于把 Ceedling 的 `TEST_SOURCE_FILE` / `TEST_INCLUDE_PATH` 元指令在 IDE/clang-tidy 视角下声明为空宏，避免误报语法错误。模板：

```yaml
CompileFlags:
  Add:
    - -DTEST_SOURCE_FILE(x)=
    - -DTEST_INCLUDE_PATH(x)=
```

### Ceedling 用法注意

[project.yml](project.yml) 已经处理好下列要点，写新测试时遵守即可：

1. **`:use_test_preprocessor: :tests` 必须开**。否则 Ceedling 在生成 runner 前不展开 `#if`，会引用被门卫掉的 `test_*` 函数导致链接失败。
2. **`#include` 必须在顶层，不能在 `#if` 里**。Ceedling 的源文件发现器扫描原始（未预处理）文件，把 `#include "sl_motor_dji.h"` 反推到 `sl_motor_dji.c`。如果把 include 放进 `#if SL_USE_FREERTOS`，Ceedling 看不到 → 不会编译/链接对应 `.c`，链接报缺符号。
3. **多 `.c` 模块用 `TEST_SOURCE_FILE` 显式拉源文件**。`sl_motor_dji.h` 触发不了 `sl_motor_dji_rtos.c` 的拉取，要在测试文件顶部声明 `TEST_SOURCE_FILE("sl_motor_dji_rtos.c")`。Case-1 文件需把这条声明也包在 `#if` 里，避免无依赖时被强行编译触发 `#error`。
4. **`setUp` / `tearDown` 必须放在文件级 `#if` 之外**。否则 mixin 把所有测试都门卫掉时，runner 仍会引用这两个符号，链接失败。文件级门卫的测试模板：顶层 `#include` + 顶层空 `setUp/tearDown`，测试用例在 `#if` 里。
5. **mixin `:defines:` 中字符串值要 `\"escaped\"`**。例：`SL_INCLUDE_HAL=\"stm32h7xx_hal.h\"`。单引号 YAML 包整串无法穿过 Ceedling 的 shell 装配。

## 覆盖率（gcov / gcovr）

[project.yml](project.yml) 已启用 `gcov` 插件，工具链是 `gcc --coverage` + `gcovr`。本地用法：

```bash
ceedling gcov:all --mixin all_on        # 跑测试 + 收集覆盖率
# HTML 报告：build/artifacts/gcov/gcovr/
```

`:gcov.summaries` 开启后控制台直接给每文件 `Lines/Branches/Calls executed` 摘要。报告类型在 `:gcov.reports` 配置（当前 `HtmlBasic`）。新增覆盖率类型加到该列表即可。

CI 在 `all_on` 上跑 `ceedling gcov:all`，把生成的 HTML 报告作为 artifact 上传。覆盖率阈值（`html_medium_threshold`/`html_high_threshold`）目前为 75/90，按需调整。

## CI（GitHub Actions）

`main` 分支每次 push/PR 跑 `rake ci`，等价于按顺序执行：

1. **fmt**（`rake fmt`）：`clang-format --dry-run --Werror`，仓库根 [.clang-format](.clang-format) 锁定风格（基于 LLVM；4 空格缩进；列宽 100；嵌套在 `#if` 内的预编译指令缩进 `IndentPPDirectives: BeforeHash`；`SortIncludes: Never` 保护 IWYU/vendor 顺序）。
   注：clang-format 仅缩进 `#if` 内的**预编译指令**（如 `#include`/嵌套 `#if`）；`#if` 块内的普通 C 代码不会因 PP 嵌套而额外缩进——这是 clang-format 的固有限制。
2. **lint**（`rake lint`）：先依赖 `:compile_db` 刷新 `build/artifacts/compile_commands.json`（用 `--mixin all_on`），再用 `clang-tidy 18` 严格规则集扫描（启用 `bugprone-*`, `cert-*`, `misc-*`, `portability-*`, `readability-*`）。仓库根 [.clang-tidy](.clang-tidy) 锁定，`WarningsAsErrors: '*'` 严格。允许的 disable 必须在文件内 `// NOLINT(check-name)` 标注理由。
   **lint 版本约束**：固定 clang-tidy 18。19+ 在 `readability-identifier-naming` 移除了 `StaticFunctionCase`/`StaticFunctionPrefix` 与 `*Pattern` 等粒度选项——本仓库的命名约定（公共 API 必须 `sl_*`，static 内部无前缀）依赖这些选项才能在 lint 层强制；用更新版本会丢掉这部分检查能力。
   **未支持的检查**：
   - 不报警「未生效的 IWYU pragma」（需 include-what-you-use 工具本身，不在依赖中）。
   - 不报警「未生效的 NOLINT 注释」（LLVM 18/22 都不内建该检查）。
3. **test**（`rake test`）：上述 mixin 组合全部跑，含 `all_on`。
4. **coverage**（`rake coverage`）：清 `build/gcov` → `ceedling gcov:all --mixin all_on`，HTML 报告做 artifact。

`main` 通过全部检查 + 合并到 `main` 后，单独的 release workflow 触发：抽取 `src/` 与 `config/`（含 `sl_config_default.h` 与 `internal.h`），重写到 `release` 分支。`release` 分支不跑 fmt/lint/test/coverage（已经是 `main` 验证过的快照）。

## 给 AI Agent 的工作约束

- 修改库代码时**永远不要假设业务方能改任何东西**。需要新行为？通过新配置项实现，并在 `sl_config_default.h` 给默认值。
- 新增模块的 checklist：
  1. 建 `src/<module>/`；
  2. 加 `sl_<module>.{c,h}` + `test_sl_<module>.c`，所有公共标识符 `sl_` 前缀，头文件 `#pragma once`；
  3. 跨目录 include 用相对路径（如 `"../../config/internal.h"`），同目录 include 直接 `"sl_<module>.h"`；
  4. 文件需要 `SL_USE_*` 或 `SL_*_INCLUDE` 时 `#include "../../config/internal.h"`；纯算法模块不 include 任何 config 头；
  5. 若依赖外部库，按 case 1（`#if/#else #error`）/ case 2（`#if/#else`）选门卫风格，自己写 `#if SL_USE_X #include SL_X_INCLUDE #endif` 拉 vendor 头。
- 新增外部依赖：
  1. 在 `sl_config_default.h` 加 `SL_USE_<DEP>` + `SL_<DEP>_INCLUDE` 默认值；
  2. 加 mixin（同时更新 `all_on` 包含新依赖）；
  3. 必要时在 `deps/stubs/<dep>/` 放测试桩。
- **不要直接修改 `release` 分支**——它由 CI 生成。
- 提交前本地至少跑 `rake test`（覆盖完整 CI 矩阵）；想做完整 CI 验证用 `rake ci`。
- 涉及新公共 API 时检查命名是否符合上面的命名约定表，特别是宏。
