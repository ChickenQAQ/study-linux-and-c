#!/usr/bin/env python3
"""Fix DNS - try sudo or alternative approaches"""
import paramiko
import sys

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15):
    print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:2000])
    if err: print(f'[e] {err[:500]}')
    print(f'→ exit: {ec}')
    return ec, out, err

# Check sudo status
run('sudo -n true 2>&1 || echo "sudo requires password"')
run('whoami')
run('id')

# Try to use sudo with password
run("echo '123456' | sudo -S tee /etc/resolv.conf <<< '' 2>&1 || echo 'sudo failed'")

# Check if resolv.conf is a symlink (WSL often uses a symlink)
run('ls -la /etc/resolv.conf')
run('ls -la /etc/')

# If it's a symlink to /run, we need to deal with WSL's auto-generation
# Check if systemd-resolved is running
run('systemctl status systemd-resolved 2>&1 | head -10')
run('cat /run/systemd/resolve/stub-resolv.conf 2>/dev/null || echo "no stub"')
run('cat /run/systemd/resolve/resolv.conf 2>/dev/null || echo "no resolved conf"')

ssh.close()
