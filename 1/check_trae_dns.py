#!/usr/bin/env python3
"""Check DNS and network on remote server"""
import paramiko

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:2000])
    if err: print(f'[e] {err[:600]}')
    print()

print("=== DNS Config ===")
run("cat /etc/resolv.conf")

print("=== DNS Resolution Test (via Python) ===")
run('python3 -c "import socket; print(socket.gethostbyname(\"api.trae.com.cn\"))" 2>&1 || python -c "import socket; print(socket.gethostbyname(\'api.trae.com.cn\'))" 2>&1 || echo "Python test failed"', timeout=10)

print("=== Network routes ===")
run("ip route 2>/dev/null || route -n 2>/dev/null || netstat -rn 2>/dev/null || echo 'no route cmd'")

print("=== Check if remote is WSL or Docker ===")
run("cat /proc/1/cgroup 2>/dev/null | head -10")
run("systemd-detect-virt 2>/dev/null || echo 'no virt detect'")

print("=== ping test ===")
run("ping -c 2 -W 3 8.8.8.8 2>&1 || echo 'ping failed'", timeout=10)

print("=== ping DNS server ===")
run("ping -c 2 -W 3 10.255.255.254 2>&1 || echo 'ping local DNS failed'", timeout=10)
run("ping -c 2 -W 3 127.0.0.54 2>&1 || echo 'ping 127.0.0.54 failed'", timeout=10)
run("ping -c 2 -W 3 1.1.1.1 2>&1 || echo 'ping cloudflare failed'", timeout=10)

ssh.close()
print("=== DONE ===")
