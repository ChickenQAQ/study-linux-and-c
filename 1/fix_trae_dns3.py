#!/usr/bin/env python3
"""Fix WSL DNS via resolvectl or alternative approaches"""
import paramiko, sys

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15):
    print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:2500])
    if err: print(f'[e] {err[:500]}')
    print(f'→ exit: {ec}')
    return ec, out, err

print("=" * 60)
print("🔧 Approach 1: resolvectl without sudo")
print("=" * 60)
run("resolvectl status 2>&1 | head -20", timeout=10)
run("resolvectl dns 2>&1", timeout=10)
run("resolvectl dns eth0 10.255.255.254 2>&1", timeout=10)
run("resolvectl domain eth0 '~.' 2>&1", timeout=10)

print("=" * 60)
print("🔧 Test DNS after resolvectl")
print("=" * 60)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("ping -c 1 -W 3 api.trae.com.cn 2>&1", timeout=10)

# If it still doesn't work, check what's in the stub resolver
print("=" * 60)
print("🔧 Check systemd-resolved stub resolver state")
print("=" * 60)
run("cat /run/systemd/resolve/stub-resolv.conf")

# Try another approach - use the stub resolver directly via env
print("=" * 60)
print("🔧 Test: connect via stub resolver IP directly")
print("=" * 60)
run("curl -sS --connect-timeout 5 --resolve 'api.trae.com.cn:443:10.255.255.254' "
    "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

# If everything fails, try using sudo with correct password handling
print("=" * 60)
print("🔧 Approach 2: Try sudo with password from stdin")
print("=" * 60)
run("echo '123456' | sudo -S -k resolvectl dns eth0 10.255.255.254 2>&1", timeout=10)
run("echo '123456' | sudo -S -k resolvectl dns 10.255.255.254 2>&1", timeout=10)

print("=" * 60)
print("🔧 Test DNS after sudo resolvectl")
print("=" * 60)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

ssh.close()
print("\n=== DONE ===")
