# 开发环境安装指南

本文覆盖 [README.md「环境要求」](../README.md#1-环境要求) 列出的全部工具。按操作系统选一份：

- [Ubuntu](setup-ubuntu.md) — 22.04 / 24.04
- [macOS](setup-macos.md) — Homebrew / MacPorts
- [Windows](setup-windows.md) — 推荐 WSL2

每份给出 1-3 种方式，按推荐程度排序。装完任意一种就能跑下面的验证流程。

> **版本硬约束**：clang-tidy 必须是 **18.x**（19+ 移除了本仓库 lint 配置依赖的命名选项）。其余工具按 README 中的版本范围满足即可。

## 验证

任意平台装完都跑这一句：

```bash
rake check_tools
```

输出会列出每个工具的检测路径与版本，缺哪个或版本不符都会清晰报错。

确认无误后跑一次：

```bash
rake compile_db        # 生成 IDE 编译数据库
rake test              # 完整测试矩阵
```

两步都通过即环境就绪。

## 常见问题

**`clang-tidy --version` 是 19/20，但仓库锁 18**

不要替换系统版本，装一份 18 并通过 `CLANG_TIDY` 环境变量指定：

```bash
export CLANG_TIDY=clang-tidy-18                              # Linux 常见命名
export CLANG_TIDY=/opt/homebrew/opt/llvm@18/bin/clang-tidy   # macOS Homebrew
```

`clang-format` 同理用 `CLANG_FORMAT` 指定。

**`rake check_tools` 报 Ceedling 版本过低**

`gem install ceedling` 默认装最新版；若 PATH 里有旧版（比如 apt 装的 `ceedling`），先 `gem uninstall ceedling` 再装一次。

**仅用于调试时绕过版本检查**

```bash
SKIP_VERSION_CHECK=1 rake test
```

不要在 CI 或提交前用——版本不符的 lint / fmt 结果不可信。
