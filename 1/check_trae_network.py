#!/usr/bin/env python3
"""Check Trae CN remote server network and service status"""
import paramiko, time

HOST = "172.31.212.123"
USER = "chicken"
PASSWORD = "123456"

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username=USER, password=PASSWORD, timeout=15)

def run(cmd, timeout=20):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:2500])
    if err: print(f'[e] {err[:600]}')
    print()
    return ec, out, err

print("=" * 60)
print("STEP 1: DNS Resolution Test")
print("=" * 60)
run("nslookup api.trae.com.cn 2>/dev/null || host api.trae.com.cn 2>/dev/null || (getent hosts api.trae.com.cn 2>/dev/null) || echo 'DNS failed'")
run("cat /etc/resolv.conf")
run("nslookup www.baidu.com 2>/dev/null || echo 'DNS test failed'")

print("=" * 60)
print("STEP 2: HTTP Connectivity Test")
print("=" * 60)
run("curl -v --connect-timeout 5 'https://api.trae.com.cn/cloudide/api/v3/trae/GetThirdPartyToken' 2>&1 | head -30", timeout=15)
run("curl -v --connect-timeout 5 'https://trae-api-cn.mchost.guru' 2>&1 | head -10", timeout=15)
run("curl -sS --connect-timeout 5 'http://127.0.0.1:51000/' 2>&1 | head -20", timeout=10)

print("=" * 60)
print("STEP 3: CKG Server Status")
print("=" * 60)
run("cat /home/chicken/.trae-cn-server/ckg_server/*.log 2>/dev/null | tail -30", timeout=10)

print("=" * 60)
print("STEP 4: AI Agent Status on Remote")
print("=" * 60)
run("cat /home/chicken/.trae-cn-server/manager-logs/*/ai-agent*.log 2>/dev/null | tail -30", timeout=10)

print("=" * 60)
print("STEP 5: Check if there's an existing workspace (code) open")
print("=" * 60)
run("ls -la /home/chicken/ 2>/dev/null | head -20")
run("cat /home/chicken/.trae-cn-server/data/User/workspaceStorage/*/workspace.json 2>/dev/null | head -10", timeout=10)

print("=" * 60)
print("STEP 6: Docker container DNS settings")
print("=" * 60)
run("cat /etc/hosts")
run("env | grep -i proxy || echo 'No proxy env vars'")

ssh.close()
print("=== DONE ===")
