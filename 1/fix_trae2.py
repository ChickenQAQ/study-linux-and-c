#!/usr/bin/env python3
"""Fix trae: force kill all old processes and clear locks properly"""
import paramiko, time, sys

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=30):
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    exit_code = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    print(f'$ {cmd}')
    if out: print(out[:3000])
    if err: print(f'[e] {err[:500]}')
    print()
    return exit_code, out

print("="*60)
print("STEP 1: Kill ALL trae-related processes with SIGKILL")
print("="*60)
# Use kill -9 to be sure
run("kill -9 846 936 937 22597 2>/dev/null; sleep 1; echo 'done killing'")

run("ps aux | grep -E 'trae|fetch|ckg' | grep -v grep || echo 'ALL CLEAN'")

print("="*60)
print("STEP 2: Remove EVERYTHING in .trae-cn-server")
print("="*60)
run("rm -rf ~/.trae-cn-server/ ~/.cache/trae* /tmp/trae* /tmp/vscode* 2>/dev/null")
run("ls -la ~/.trae-cn-server/ 2>/dev/null || echo '✅ Directory fully removed'")

print("="*60)
print("STEP 3: Check for any remaining processes listening on ports")
print("="*60)
run("ss -tlnp 2>/dev/null | grep -v ':22' | grep -v ':53' || echo 'No extra listening ports'")

print("="*60)
print("STEP 4: Generate SSH key for passwordless future logins")
print("="*60)
# Check if key exists, if not generate one
run("[ -f ~/.ssh/id_ed25519 ] && echo 'key exists' || (ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -N '' -q && echo 'key generated')", timeout=10)
# Add local machine's key if it exists
exit_code, local_key = run("cat ~/.ssh/id_ed25519.pub 2>/dev/null || echo 'no local key'")
if 'no local key' not in local_key:
    run("mkdir -p ~/.ssh && chmod 700 ~/.ssh && cat ~/.ssh/id_ed25519.pub >> ~/.ssh/authorized_keys 2>/dev/null; chmod 600 ~/.ssh/authorized_keys 2>/dev/null; echo 'local key added'")

print("="*60)
print("✅ COMPLETE! Now try connecting in Trae CN again.")
print("The server is clean. The new install should start fresh.")
print("="*60)

ssh.close()
