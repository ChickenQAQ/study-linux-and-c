#!/usr/bin/env python3
"""Check remote ai-agent logs for errors"""
import paramiko

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:3000])
    if err: print(f'[e] {err[:600]}')
    print()

print("=== Latest manager-logs ===")
run("ls -lt /home/chicken/.trae-cn-server/manager-logs/ 2>/dev/null | head -5")

print("=== Trae server main log (last 30 lines) ===")
run("tail -30 /home/chicken/.trae-cn-server/.stable-9b7a82724b02c7e595df5219c0c69d3ff3055458-b2be0caa.log 2>/dev/null")

print("=== Check ai-agent Modular logs on remote ===")
run("ls -la /home/chicken/.trae-cn-server/manager-logs/*/Modular/ 2>/dev/null | tail -20")

print("=== Latest ai-agent log on remote ===")
run("cat /home/chicken/.trae-cn-server/manager-logs/*/Modular/ai-agent_*.log 2>/dev/null | tail -50")

print("=== Check for network errors in logs ===")
run("cat /home/chicken/.trae-cn-server/manager-logs/*/Modular/ai-agent_*.log 2>/dev/null | grep -i 'error\\|timeout\\|fail' | tail -20")

print("=== Check if ai-agent process is running ===")
run("ps aux | grep ai-agent | grep -v grep")

print("=== Test connectivity from remote to api.trae.com.cn ===")
run("curl -sS --connect-timeout 5 --max-time 10 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1")

print("=== Test connectivity to alternate Trae API ===")
run("curl -sS --connect-timeout 5 --max-time 10 'https://trae-api-cn.mchost.guru' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1")

ssh.close()
print("=== DONE ===")
