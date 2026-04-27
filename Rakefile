# Rakefile —— EC-Shared-Lib 单一构建入口
#
# 设计目标：跨平台（Linux/macOS/Windows-native 同一份 Rakefile）。
# 跨平台守则（违反任一条都可能在某个平台炸）：
#   - 用 `system` / `sh` 数组形式，绝不字符串拼 shell 命令
#   - 文件查找用 Dir.glob，不调 POSIX `find`
#   - ceedling 经 `RbConfig.ruby` + `-S` 调用，避开 Windows 的 .bat shim 与 PATH 差异
#   - 工具二进制名（clang-tidy 等）走 ENV，方便不同发行版/包管理器覆盖

require 'fileutils'
require 'rbconfig'
require 'rake/clean'

# ---------- 工具与路径 ----------
CLANG_FORMAT   = ENV.fetch('CLANG_FORMAT', 'clang-format')
CLANG_TIDY     = ENV.fetch('CLANG_TIDY',   'clang-tidy')
SOURCE_GLOBS   = %w[src/**/*.c src/**/*.h config/**/*.c config/**/*.h].freeze
COMPILE_DB     = 'build/artifacts/compile_commands.json'
COMPLETE_MIXIN = 'all_on'

# CLAUDE.md 中规定的 CI 测试矩阵（不做全排列）
TEST_MATRIX = [
  %w[pure],
  %w[rtos hal_f4],
  %w[rtos dsp hal_h7],
  %w[all_on]
].freeze

CLEAN.include 'build'

# ---------- 辅助 ----------
def source_files
  files = SOURCE_GLOBS.flat_map { |g| Dir.glob(g) }.sort
  abort '未找到源文件（src/ 与 config/ 下没有 .c/.h）' if files.empty?
  files
end

# 调用 ceedling：强制走当前 ruby 解释器，Windows 上避开 .bat shim 寻路问题
def ceedling(*args)
  sh RbConfig.ruby, '-S', 'ceedling', *args
end

# ---------- 任务 ----------
task default: :test

desc 'CI 全套：fmt + lint + test + coverage'
task ci: %i[fmt lint test coverage]

desc 'clang-format 检查（不修改文件，CI 默认）'
task :fmt do
  sh CLANG_FORMAT, '--dry-run', '--Werror', *source_files
end

namespace :fmt do
  desc 'clang-format in-place 应用'
  task :fix do
    sh CLANG_FORMAT, '-i', *source_files
  end
end

desc "刷新 #{COMPILE_DB}（清空旧 DB 后用 --mixin #{COMPLETE_MIXIN} 全量编译）"
task :compile_db do
  FileUtils.rm_f COMPILE_DB
  ceedling 'test:all', '--mixin', COMPLETE_MIXIN
end

desc 'clang-tidy 严格检查（自动刷新 compile_commands.json）'
task lint: :compile_db do
  sh CLANG_TIDY, '-p', 'build/artifacts', *source_files
end

desc 'CI 测试矩阵：pure / rtos+hal_f4 / rtos+dsp+hal_h7 / all_on'
task :test do
  TEST_MATRIX.each do |mixins|
    args = mixins.flat_map { |m| ['--mixin', m] }
    puts "\n>>> ceedling test:all #{args.join(' ')}"
    ceedling 'test:all', *args
  end
end

desc "覆盖率：ceedling gcov:all --mixin #{COMPLETE_MIXIN}"
task :coverage do
  # 清掉旧 .gcda 防止 libgcov "overwriting profile data with a different checksum"
  FileUtils.rm_rf 'build/gcov'
  ceedling 'gcov:all', '--mixin', COMPLETE_MIXIN
end
