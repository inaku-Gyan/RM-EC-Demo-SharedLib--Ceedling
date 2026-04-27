# macOS 环境安装

装完任意一种方式后回到 [setup.md#验证](setup.md#验证) 跑一遍。

> clang-tidy 必须是 **18.x**——19+ 移除了本仓库 lint 配置依赖的命名选项。
> macOS 系统自带的是 Apple clang，没有 `clang-tidy`，必须自己装一份 LLVM 18。

## 方式 A：Homebrew（推荐）

#### 1. LLVM 18 / Ruby / gcovr

```bash
brew install llvm@18 ruby gcovr
```

#### 2. 把 LLVM 18 与 brew Ruby 加进 PATH

brew 不软链 keg-only 包到全局，得手动加。Apple Silicon：

```bash
echo 'export PATH="/opt/homebrew/opt/llvm@18/bin:$PATH"' >> ~/.zshrc
echo 'export PATH="/opt/homebrew/opt/ruby/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

Intel Mac 路径前缀替换为 `/usr/local/opt/...`：

```bash
echo 'export PATH="/usr/local/opt/llvm@18/bin:$PATH"' >> ~/.zshrc
echo 'export PATH="/usr/local/opt/ruby/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

#### 3. Ceedling

```bash
gem install ceedling
```

#### 4. gcc（可选）

macOS 默认没 GNU gcc。多数情况下 Apple clang 伪装的 `gcc` 已经够用——本仓库 CI 走 GCC，但本地 macOS 跑测试不强求。

要装 brew gcc：

```bash
brew install gcc
```

`brew install gcc` 会装 `gcc-15`（或当前 brew 版本号），可执行名带版本后缀。Ceedling 默认调 `gcc`，如果系统里只有版本号后缀，加软链或在 [project.yml](../project.yml) 里指定 compiler 路径。

## 方式 B：MacPorts

```bash
sudo port install ruby32 clang-format-18 clang-18 gcovr
sudo port select --set clang-format clang-format-18
sudo port select --set clang clang-18
sudo gem install ceedling
```
