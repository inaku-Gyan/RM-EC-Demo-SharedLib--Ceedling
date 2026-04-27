# Ubuntu 环境安装

适用 Ubuntu 22.04 / 24.04。装完任意一种方式后回到 [setup.md#验证](setup.md#验证) 跑一遍。

> clang-tidy 必须是 **18.x**——19+ 移除了本仓库 lint 配置依赖的命名选项。

## 方式 A：发行版包 + 官方 LLVM apt 源（推荐）

#### 1. Ruby / gcc / clang-format

```bash
sudo apt update
sudo apt install -y ruby-full ruby-dev build-essential clang-format
```

#### 2. clang-tidy 18

`clang-tidy-18` 在 24.04 仓库里有，22.04 需从 [apt.llvm.org](https://apt.llvm.org/) 拉。

Ubuntu 24.04：直接 apt。

```bash
sudo apt install -y clang-tidy-18
```

Ubuntu 22.04 或系统默认非 18：用 LLVM 官方脚本指定版本。

```bash
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" -- 18
sudo apt install -y clang-tidy-18
```

#### 3. Ceedling

使用 Ruby 的包管理器 gem 安装：

```bash
sudo gem install ceedling
```

#### 4. gcovr

使用 Python 包管理器安装（推荐）：

```bash
# 如果你使用 uv
uv tool install gcovr

# 或者你有 pipx
pipx install gcovr

# 再或者直接用 pip
pip install gcovr
```

或者走 apt（版本可能偏旧）：

```bash
sudo apt install -y gcovr
```

## 方式 B：用 asdf 统一管理 Ruby

适合本机已经有别的 Ruby 项目、或 apt 的 Ruby 版本太旧时。

```bash
# 安装 asdf 后
asdf plugin add ruby
asdf install ruby 3.3.5
asdf global ruby 3.3.5
gem install ceedling
```

clang-format / clang-tidy / gcc / gcovr 仍按方式 A 装。
