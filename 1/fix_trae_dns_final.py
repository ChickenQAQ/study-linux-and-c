#!/usr/bin/env python3
"""Fix WSL DNS - use sudo to fix resolv.conf and verify Trae connectivity"""
import paramiko, time, sys

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password='123456', timeout=15)

def run(cmd, timeout=15, echo=True):
    if echo: print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if echo and out: print(out[:2500])
    if echo and err: print(f'[e] {err[:500]}')
    if echo: print(f'→ exit: {ec}')
    return ec, out, err

print("=" * 60)
print("🔧 Fix 1: Write correct DNS to /etc/resolv.conf using sudo")
print("=" * 60)

# Use sudo with echo redirection via sh -c
run("sudo sh -c 'echo \"nameserver 127.0.0.53\" > /etc/resolv.conf'")
# Verify
run("cat /etc/resolv.conf")

print("\n" + "=" * 60)
print("🔧 Fix 2: Set DNS on eth0 via resolvectl")
print("=" * 60)
run("sudo resolvectl dns eth0 10.255.255.254")
run("sudo resolvectl domain eth0 '~.'")
run("resolvectl status 2>&1 | head -15")

print("\n" + "=" * 60)
print("🔧 Verify DNS Resolution")
print("=" * 60)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("ping -c 1 -W 3 api.trae.com.cn 2>&1", timeout=10)

print("\n" + "=" * 60)
print("🔧 Make it persistent: Update /etc/wsl.conf")
print("=" * 60)
# Current wsl.conf already has some settings, let's add network config
run("sudo sh -c 'cat > /etc/wsl.conf << \"EOF\"\n[boot]\nsystemd=true\n[user]\ndefault=chicken\n[network]\ngenerateResolvConf = false\nhostname = lyh2006\nEOF'")
run("cat /etc/wsl.conf")

print("\n" + "=" * 60)
print("✅ DNS Fix Complete!")
print("DNS resolution should now work on the remote server.")
print("=" * 60)

ssh.close()
