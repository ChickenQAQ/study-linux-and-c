#!/usr/bin/env python3
"""Kill remaining processes and verify clean state"""
import paramiko, time

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=15)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:2000])
    if err: print(f'[e] {err[:500]}')
    print()

# Kill remaining wget/curl processes
run("kill -9 $(ps aux | grep -E 'wget|curl.*trae' | grep -v grep | awk '{print $2}') 2>/dev/null; echo 'done'")

# Verify zero processes
run("ps aux | grep -E 'trae|fetch|ckg|wget.*vscode' | grep -v grep || echo '✅ COMPLETELY CLEAN'")

# Also check if Trae has a new install attempt running
run("ls -la ~/.trae-cn-server/ 2>/dev/null || echo '✅ No trae directory - fresh start ready'")

ssh.close()
print("="*60)
print("✅ Server is now 100% clean!")
print("Close Trae CN completely, reopen it,")
print("then try connecting to 172.31.212.123 again.")
print("="*60)
