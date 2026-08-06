#!/usr/bin/env python3
"""Check DNS/hosts and try alternative approaches"""
import paramiko, socket, time

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=30):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:4000])
    if err: print(f'[e] {err[:500]}')
    print()

print("="*60)
print("1. Check /etc/hosts for redirections")
print("="*60)
run("cat /etc/hosts")

print("="*60)
print("2. Check /etc/resolv.conf in detail")
print("="*60)
run("cat /etc/resolv.conf")

print("="*60)
print("3. Can the host connect to gateway?")
print("="*60)
run("ping -c 2 -W 2 172.31.208.1 2>&1 || echo 'gateway unreachable'")

print("="*60)
print("4. Check WSL network config")
print("="*60)
run("cat /etc/wsl.conf 2>/dev/null || echo 'no wsl.conf'")
run("ip addr show eth0 2>/dev/null | head -20")

print("="*60)
print("5. Check if download works through HTTP (port 80) with longer timeout")
print("="*60)
run("curl -sS --connect-timeout 30 --max-time 60 'https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /tmp/test2.md5 2>&1; echo 'EXIT:'$?; if [ -f /tmp/test2.md5 ]; then cat /tmp/test2.md5; rm -f /tmp/test2.md5; fi", timeout=90)

ssh.close()
