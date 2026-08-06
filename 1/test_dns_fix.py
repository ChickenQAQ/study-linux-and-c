#!/usr/bin/env python3
"""Test DNS and network connectivity after fix"""
import paramiko, time

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
print("1. Check resolv.conf")
print("="*60)
run("cat /etc/resolv.conf")

print("="*60)
print("2. Test CDN download (small MD5 file)")
print("="*60)
run("curl -sS --connect-timeout 10 --max-time 20 'https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /tmp/test_final.md5 2>&1; echo 'EXIT:'$?; if [ -f /tmp/test_final.md5 ]; then echo 'CONTENT:'$(cat /tmp/test_final.md5); rm -f /tmp/test_final.md5; else echo 'FAILED'; fi", timeout=30)

print("="*60)
print("3. Test github access")
print("="*60)
run("curl -sS --connect-timeout 10 --max-time 15 https://github.com -o /dev/null -w '%{http_code}' 2>&1 || echo 'FAILED'")

print("="*60)
print("4. Also test with wget")
print("="*60)
run("wget --timeout=10 -qO- 'https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' 2>&1; echo 'EXIT:'$?", timeout=30)

ssh.close()
