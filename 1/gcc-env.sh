# 加载此脚本后再编译：source ./gcc-env.sh
export LIBRARY_PATH="$HOME/.local/lib/x86_64-linux-gnu:$LIBRARY_PATH"
export C_INCLUDE_PATH="/tmp/libc-include:/tmp/libc-include/x86_64-linux-gnu:$C_INCLUDE_PATH"
export CPLUS_INCLUDE_PATH="/tmp/libc-include:/tmp/libc-include/x86_64-linux-gnu:$CPLUS_INCLUDE_PATH"
echo "gcc 编译环境已就绪"
