#!/usr/bin/env python3
"""Recreate /etc/resolv.conf properly"""
import paramiko

PW = '123456'
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect('172.31.212.123', username='chicken', password=PW, timeout=15)

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

def sudo_cmd(cmd, timeout=15):
    return run(f"echo '{PW}' | sudo -S -k {cmd}", timeout)

def sudo_heredoc(filepath, content, timeout=15):
    """Create file with sudo using heredoc"""
    escaped = content.replace("'", "'\\''")
    return run(f"echo '{PW}' | sudo -S -k tee {filepath} > /dev/null 2>&1 << 'SUDOEOF'\n{content}\nSUDOEOF", timeout)

print("=" * 60)
print("🔧 Step 1: Create /etc/resolv.conf")
print("=" * 60)

# Use tee with heredoc to create the file
sudo_cmd("tee /etc/resolv.conf > /dev/null 2>&1 << 'DNSEOF'\nnameserver 127.0.0.53\noptions edns0 trust-ad\nsearch .\nDNSEOF")
run("cat /etc/resolv.conf")
run("ls -la /etc/resolv.conf")

print("=" * 60)
print("🔧 Step 2: Test DNS Resolution")
print("=" * 60)

run("curl -sS --connect-timeout 5 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

# Also verify resolvectl status
run("resolvectl status 2>&1 | head -15")

print("\n" + "=" * 60)
print("📋 Summary")
print("=" * 60)
ec, out, _ = run("cat /etc/resolv.conf")
if "127.0.0.53" in out:
    print("✅ /etc/resolv.conf is set to 127.0.0.53 (systemd-resolved stub)")
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1 | head -1", timeout=15)

ssh.close()
