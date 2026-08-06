#!/usr/bin/env python3
"""Check remote server state after cleanup"""
import paramiko, time

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=30)
    exit_code = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out)
    if err: print(f'[e] {err[:500]}')
    print()

run('echo "=== .trae-cn-server directory ==="')
run('ls -la ~/.trae-cn-server/ 2>/dev/null || echo "NOT FOUND"')

run('echo "=== Process list ==="')
run("ps aux | grep -E 'trae|fetch|vscode-server' | grep -v grep || echo 'NO PROCS'")

run('echo "=== Disk space ==="')
run('df -h /')

run('echo "=== bin directory ==="')
run('ls -la ~/.trae-cn-server/bin/ 2>/dev/null')

# Check installation progress
run('echo "=== Any install lock files ==="')
run('find ~/.trae-cn-server -name "*.lock*" -o -name ".resolve-lock*" 2>/dev/null || echo "no lock files"')

# Check if download is happening
run("echo '=== Active network connections ==='")
run("ss -tlnp 2>/dev/null || netstat -tlnp 2>/dev/null || echo 'no netstat'")

# Check fetch process specifically
run("echo '=== Fetch processes ==='")
run("ps aux | grep fetch | grep -v grep || echo 'NO fetch'")

ssh.close()
print("=== DONE ===")
