#!/usr/bin/env python3
"""Fix DNS by pointing /etc/resolv.conf to systemd-resolved stub"""
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
print("🔧 Fixing WSL DNS - Step 1: Try writing to /mnt/wsl/resolv.conf directly")
print("=" * 60)

# Check mnt/wsl
run("ls -la /mnt/wsl/ 2>/dev/null || echo 'no /mnt/wsl'")
run("ls -la /mnt/wsl/resolv.conf 2>/dev/null")
run("stat /mnt/wsl/resolv.conf 2>/dev/null")
run("cat /mnt/wsl/resolv.conf")

# Try to write to /mnt/wsl/resolv.conf
print("\n" + "=" * 60)
print("🔧 Trying to write to /mnt/wsl/resolv.conf")
print("=" * 60)
run("echo 'nameserver 127.0.0.53' | tee /mnt/wsl/resolv.conf 2>&1", timeout=10)
run("cat /etc/resolv.conf", label="Verify")

# If that doesn't work, try the alternative approach:
# Use systemd-resolve to set DNS
print("\n" + "=" * 60)
print("🔧 Alternative: Use systemd-resolve to configure DNS")
print("=" * 60)
run("resolvectl status 2>&1 | head -30", timeout=10)
run("resolvectl dns 2>&1 | head -5", timeout=10)

# Test DNS
print("\n" + "=" * 60)
print("🔧 Testing DNS resolution")
print("=" * 60)
run("curl -sS --resolve 'api.trae.com.cn:443:10.255.255.254' --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 --dns-servers 10.255.255.254 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

ssh.close()

print("\n" + "=" * 60)
print("📋 Summary")
print("=" * 60)
print("""
The remote server is WSL2 with systemd-resolved.
- /etc/resolv.conf is a symlink → /mnt/wsl/resolv.conf
- systemd-resolved already knows about 10.255.255.254
- But /mnt/wsl/resolv.conf has 8.8.8.8 / 1.1.1.1 which are unreachable

Fix options:
1. Write 'nameserver 127.0.0.53' to /mnt/wsl/resolv.conf 
   (the systemd-resolved stub that then forwards to 10.255.255.254)
2. Or reconfigure the symlink
""")
