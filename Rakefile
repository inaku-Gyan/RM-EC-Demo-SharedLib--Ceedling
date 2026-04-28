# Rakefile —— EC-Shared-Lib 单一构建入口
#
# 设计目标：跨平台（Linux/macOS/Windows-native 同一份 Rakefile）。
# 跨平台守则（违反任一条都可能在某个平台炸）：
#   - 用 `system` / `sh` 数组形式，绝不字符串拼 shell 命令
#   - 文件查找用 Dir.glob，不调 POSIX `find`
#   - ceedling 经 `RbConfig.ruby` + `-S` 调用，避开 Windows 的 .bat shim 与 PATH 差异
#   - 工具二进制名（clang-tidy 等）走 ENV，方便不同发行版/包管理器覆盖

require 'fileutils'
require 'open3'
require 'rbconfig'
require 'rake/clean'
require 'rubygems'

# ---------- 工具发现 ----------
# 在 PATH 中搜可执行文件；Windows 上按 PATHEXT 拼后缀（.exe/.bat/...），POSIX 上无后缀
def which(cmd)
  exts = ENV['PATHEXT'] ? ENV['PATHEXT'].split(File::PATH_SEPARATOR) : ['']
  ENV.fetch('PATH', '').split(File::PATH_SEPARATOR).each do |dir|
    exts.each do |ext|
      path = File.join(dir, "#{cmd}#{ext}")
      return path if File.file?(path) && File.executable?(path)
    end
  end
  nil
end

# 候选名按序找 PATH 里第一个能用的；都找不到时返回最后一个候选，让后续版本检查统一报错
def pick_tool(*candidates)
  candidates.find { |c| which(c) } || candidates.last
end

# ---------- 工具与路径 ----------
# 优先选版本化后缀（clang-format-18 / clang-tidy-18），未装时回退到无后缀名
CLANG_FORMAT   = ENV.fetch('CLANG_FORMAT') { pick_tool('clang-format-18', 'clang-format') }
CLANG_TIDY     = ENV.fetch('CLANG_TIDY')   { pick_tool('clang-tidy-18',   'clang-tidy') }
SOURCE_GLOBS   = %w[src/**/*.c src/**/*.h test/**/*.c test/**/*.h].freeze
COMPILE_DB     = 'build/artifacts/compile_commands.json'
COMPLETE_MIXIN = 'all_on'

# CLAUDE.md 中规定的 CI 测试矩阵（不做全排列）
TEST_MATRIX = [
  %w[pure],
  %w[rtos hal_f4],
  %w[rtos hal_h7],
  %w[all_on]
].freeze

# ---------- 工具版本约束 ----------
# 用 Gem::Requirement 语法（'~> 18.0' = '>= 18.0, < 19.0'）。
# 紧急绕过：SKIP_VERSION_CHECK=1 rake ...
REQUIRE_RUBY         = '>= 3.0.0'
REQUIRE_CEEDLING     = '>= 1.0.0'
REQUIRE_CLANG_FORMAT = '>= 15.0.0' # IndentPPDirectives + PPIndentWidth + InsertBraces 需要 15+
REQUIRE_CLANG_TIDY   = '~> 18.0'   # 锁 18.x：19+ 移除了 StaticFunctionCase / *Pattern 命名选项

CLEAN.include 'build'

# ---------- 辅助 ----------
def source_files
  files = SOURCE_GLOBS.flat_map { |g| Dir.glob(g) }.sort
  abort '未找到源文件（src/ 下没有 .c/.h）' if files.empty?
  files
end

# 调用 ceedling：强制走当前 ruby 解释器，Windows 上避开 .bat shim 寻路问题
def ceedling(*args)
  sh RbConfig.ruby, '-S', 'ceedling', *args
end

# 调 `<cmd> <args...>`，从 stdout 抽出第一个 X.Y.Z；找不到 cmd / 抽不到版本都 abort
def detect_version(cmd, *args)
  out, _err, status = Open3.capture3(cmd, *args)
  abort "[版本检查] 调用失败：#{cmd} #{args.join(' ')} (exit #{status.exitstatus})" unless status.success?
  out[/(\d+\.\d+\.\d+)/, 1] or abort "[版本检查] 无法从输出抽取版本：#{cmd}\n输出:\n#{out}"
rescue Errno::ENOENT
  abort "[版本检查] 未找到 `#{cmd}` ——请确认已安装并在 PATH 中"
end

# 版本断言：SKIP_VERSION_CHECK=1 时连 yield 都不调用，整段跳过
def assert_version!(label, requirement, note:)
  return if ENV['SKIP_VERSION_CHECK']
  actual = yield
  return if Gem::Requirement.new(requirement).satisfied_by?(Gem::Version.new(actual))
  abort <<~MSG
    [版本检查失败] #{label}
      实际: #{actual}
      要求: #{requirement}
      原因: #{note}
      绕过: SKIP_VERSION_CHECK=1 rake ...
  MSG
end

# ---------- 版本检查 task ----------
task :check_ruby do
  assert_version!('ruby', REQUIRE_RUBY,
                  note: 'Ceedling 1.0+ 要求 Ruby 3.0+') { RUBY_VERSION }
end

task check_ceedling: :check_ruby do
  assert_version!('ceedling', REQUIRE_CEEDLING,
                  note: '本仓库依赖 Ceedling 1.0+ 的 plugin API、mixin 机制与 use_test_preprocessor:tests') do
    detect_version(RbConfig.ruby, '-S', 'ceedling', 'version')
  end
end

task :check_clang_format do
  assert_version!('clang-format', REQUIRE_CLANG_FORMAT,
                  note: 'IndentPPDirectives + PPIndentWidth + InsertBraces 需要 15+') do
    detect_version(CLANG_FORMAT, '--version')
  end
end

task :check_clang_tidy do
  assert_version!('clang-tidy', REQUIRE_CLANG_TIDY,
                  note: 'clang-tidy 19+ 移除了 StaticFunctionCase/*Pattern 命名选项；本仓库锁 18.x') do
    detect_version(CLANG_TIDY, '--version')
  end
end

desc '一次性检查全部工具版本（Ruby / Ceedling / clang-format / clang-tidy）'
task check_tools: %i[check_ruby check_ceedling check_clang_format check_clang_tidy]

# ---------- 主任务 ----------
task default: :test

desc 'CI 全套：fmt + lint + test + coverage'
task ci: %i[fmt lint test coverage]

desc 'clang-format 检查（不修改文件，CI 默认）'
task fmt: :check_clang_format do
  sh CLANG_FORMAT, '--dry-run', '--Werror', *source_files
end

namespace :fmt do
  desc 'clang-format in-place 应用'
  task fix: :check_clang_format do
    sh CLANG_FORMAT, '-i', *source_files
  end
end

desc "刷新 #{COMPILE_DB}（清空旧 DB 后用 --mixin #{COMPLETE_MIXIN} 全量编译）"
task compile_db: :check_ceedling do
  FileUtils.rm_f COMPILE_DB
  ceedling 'test:all', '--mixin', COMPLETE_MIXIN
end

desc 'clang-tidy 严格检查（自动刷新 compile_commands.json）'
task lint: %i[check_clang_tidy compile_db] do
  sh CLANG_TIDY, '-p', 'build/artifacts', *source_files
end

namespace :lint do
  desc 'clang-tidy --fix 自动修复（修改文件；遇编译错误的 TU 跳过该 TU 的 fix）'
  task fix: %i[check_clang_tidy compile_db] do
    sh CLANG_TIDY, '--fix', '-p', 'build/artifacts', *source_files
  end
end

desc 'CI 测试矩阵：pure / rtos+hal_f4 / rtos+hal_h7 / all_on'
task test: :check_ceedling do
  TEST_MATRIX.each do |mixins|
    # 每次切 mixin 前清掉 build/test：ceedling 1.0 在切换 mixin 时不会重生
    # build/test/preprocess/includes/*.yml，导致上一组 mixin 预处理出来的 vendor
    # 头被带进新一组的 runner（如 all_on 留下的 stm32f4xx_hal.h 被 pure 复用）。
    # build/artifacts/compile_commands.json 不在 build/test 下，仍由 compile_db 保留。
    FileUtils.rm_rf 'build/test'
    args = mixins.flat_map { |m| ['--mixin', m] }
    puts "\n>>> ceedling test:all #{args.join(' ')}"
    ceedling 'test:all', *args
  end
end

desc "覆盖率：ceedling gcov:all --mixin #{COMPLETE_MIXIN}"
task coverage: :check_ceedling do
  # 清掉旧 .gcda 防止 libgcov "overwriting profile data with a different checksum"
  FileUtils.rm_rf 'build/gcov'
  ceedling 'gcov:all', '--mixin', COMPLETE_MIXIN
end
