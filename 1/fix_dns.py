#!/usr/bin/env python3
"""Fix WSL2 DNS and network issues on the remote server"""
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
    return ec, out

print("="*60)
print("STEP 1: Fix DNS resolution")
print("="*60)
# First try to write resolv.conf with proper DNS
run("sudo bash -c 'echo \"nameserver 8.8.8.8\" > /etc/resolv.conf && echo \"nameserver 1.1.1.1\" >> /etc/resolv.conf && cat /etc/resolv.conf'", timeout=10)

# Also try without sudo
print("--- Trying without sudo ---")
run("echo 'nameserver 8.8.8.8' > /tmp/resolv.conf 2>/dev/null; cat /tmp/resolv.conf")

print("="*60)
print("STEP 2: Test DNS resolution after fix")
print("="*60)
run("nslookup lf-cdn.trae.com.cn 8.8.8.8 2>/dev/null | head -10 || host lf-cdn.trae.com.cn 8.8.8.8 2>/dev/null || dig @8.8.8.8 lf-cdn.trae.com.cn 2>/dev/null || getent hosts lf-cdn.trae.com.cn 2>/dev/null || echo 'DNS still failing'")

print("="*60)
print("STEP 3: Try downloading directly with explicit DNS")
print("="*60)
run("curl -sS --connect-timeout 15 --resolve 'lf-cdn.trae.com.cn:443:223.247.121.61' 'https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /tmp/test3.md5 2>&1; echo 'EXIT:'$?; if [ -f /tmp/test3.md5 ]; then cat /tmp/test3.md5; rm -f /tmp/test3.md5; fi", timeout=30)

print("="*60)
print("STEP 4: Check if it's a DNS or network issue")
print("="*60)
# Try direct IP with correct Host header
run("curl -sS --connect-timeout 15 -H 'Host: lf-cdn.trae.com.cn' 'https://223.247.121.61/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5' -o /tmp/test4.md5 2>&1; echo 'EXIT:'$?; if [ -f /tmp/test4.md5 ]; then cat /tmp/test4.md5; rm -f /tmp/test4.md5; fi", timeout=30)

print("="*60)
print("STEP 5: Check if Windows host can reach the CDN")
print("="*60)
# We can check from the Windows side using WSL
run("powershell.exe -Command \"curl.exe -sS --connect-timeout 10 https://lf-cdn.trae.com.cn/obj/trae-com-cn/pkg/server/releases/stable/9b7a82724b02c7e595df5219c0c69d3ff3055458/linux-debian10/Trae%20CN-linux-x64-1191350185730_10.tar.xz.md5 -o C:\\Users\\21367\\AppData\\Local\\Temp\\trae_test.md5 2>&1; if(Test-Path 'C:\\Users\\21367\\AppData\\Local\\Temp\\trae_test.md5') { Get-Content 'C:\\Users\\21367\\AppData\\Local\\Temp\\trae_test.md5'; Remove-Item 'C:\\Users\\21367\\AppData\\Local\\Temp\\trae_test.md5' } else { Write-Output 'DOWNLOAD FAILED' }\" 2>&1", timeout=20)

ssh.close()
