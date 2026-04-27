# Windows 环境安装

装完任意一种方式后回到 [setup.md#验证](setup.md#验证) 跑一遍。

> clang-tidy 必须是 **18.x**——19+ 移除了本仓库 lint 配置依赖的命名选项。

> **强烈建议在 WSL2 (Ubuntu) 下开发**——直接走 [Ubuntu 一节](setup-ubuntu.md)。原生 Windows 工具链对 Ceedling / Rake 有更多坑（路径分隔符、`.bat` shim 寻路等）。
>
> 本仓库 [Rakefile](../Rakefile) 已对 Windows 做过适配（用 `RbConfig.ruby + -S` 调 ceedling 绕开 `.bat` shim 寻路问题），但仍推荐 WSL。

## 方式 A：WSL2 + Ubuntu（推荐）

进 Ubuntu 后按 [setup-ubuntu.md](setup-ubuntu.md) 装。

## 方式 B：Scoop（原生 Windows）

#### 1. 安装 scoop（如未装）

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
irm get.scoop.sh | iex
```

#### 2. Ruby / gcc / gcovr

```powershell
scoop install ruby gcc gcovr
```

#### 3. LLVM 18

`llvm@18.1.8` 是最后一个 18.x 版本。默认源（main bucket）若有：

```powershell
scoop install llvm@18.1.8
```

默认源里没有时，加 versions bucket：

```powershell
scoop bucket add versions
scoop install versions/llvm18
```

#### 4. Ceedling

```powershell
gem install ceedling
```

## 方式 C：手动安装

- **Ruby**：[RubyInstaller](https://rubyinstaller.org/) 选 3.x WITH DevKit 版本。
- **LLVM 18**：[releases.llvm.org](https://releases.llvm.org/) 下 `LLVM-18.1.8-win64.exe`，安装时勾选「Add LLVM to system PATH」。
- **MinGW gcc**：[MSYS2](https://www.msys2.org/) 装好后 `pacman -S mingw-w64-ucrt-x86_64-gcc`。
- **gcovr**：`pip install gcovr`（Python 已经装好的话）。
- **Ceedling**：`gem install ceedling`。
