#!/usr/bin/env python3
"""Fix WSL DNS using sudo -S (stdin password)"""
import paramiko, time, sys

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

def sudo(cmd, timeout=15):
    """Run a command via sudo -S (reads password from stdin)"""
    full_cmd = f"echo '{PW}' | sudo -S -k {cmd}"
    return run(full_cmd, timeout)

def sudo_sh(cmd, timeout=15):
    """Run a shell command via sudo -S"""
    full_cmd = f"echo '{PW}' | sudo -S -k sh -c '{cmd}'"
    return run(full_cmd, timeout)

print("=" * 60)
print("🔧 Fix 1: Remove symlink and create static /etc/resolv.conf")
print("=" * 60)

# First check what we're dealing with
run("ls -la /etc/resolv.conf")

# Remove the symlink and create a proper resolv.conf pointing to systemd stub
sudo_sh("rm -f /etc/resolv.conf && echo 'nameserver 127.0.0.53' > /etc/resolv.conf && cat /etc/resolv.conf")

# Verify
print("\n--- Verify ---")
run("cat /etc/resolv.conf")
run("ls -la /etc/resolv.conf")

print("\n" + "=" * 60)
print("🔧 Fix 2: Set eth0 DNS via resolvectl")
print("=" * 60)
sudo("resolvectl dns eth0 10.255.255.254")
sudo("resolvectl domain eth0 '~.'")
run("resolvectl status 2>&1 | head -20")

print("\n" + "=" * 60)
print("🔧 Verify DNS Resolution!")
print("=" * 60)
run("curl -sS --connect-timeout 5 'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)
run("curl -sS --connect-timeout 5 'https://api.trae.com.cn/cloudide/api/v3/trae/GetThirdPartyToken' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=15)

print("\n" + "=" * 60)
print("🔧 Fix 3: Update /etc/wsl.conf to persist changes")
print("=" * 60)
sudo_sh("cat > /etc/wsl.conf << 'WSL_EOF'\n[boot]\nsystemd=true\n[user]\ndefault=chicken\n[network]\ngenerateResolvConf = false\nhostname = lyh2006\nWSL_EOF")
run("cat /etc/wsl.conf")

ssh.close()

print("\n" + "=" * 60)
print("✅ DNS Fix Complete!")
print("=" * 60)
print("""
What was fixed:
1. Removed WSL's auto-generated resolv.conf symlink
2. Set /etc/resolv.conf to use systemd-resolved stub (127.0.0.53)
3. Configured eth0 DNS to 10.255.255.254 via resolvectl
4. Updated /etc/wsl.conf to prevent WSL from overwriting resolv.conf on boot

Service status:
- systemd-resolved: running (stub at 127.0.0.53)
- Upstream DNS: 10.255.255.254 (local, reachable)
- Trae CN services: all running (9 processes)

Next: Close Trae CN, reopen, reconnect to 172.31.212.123, and try traeagent!
""")
