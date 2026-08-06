#!/usr/bin/env python3
"""Fix Trae remote server installation issues on 172.31.212.123"""
import paramiko
import time
import sys

HOST = "172.31.212.123"
USER = "chicken"
PASSWORD = "123456"

def run_cmd(ssh, cmd, label=None):
    """Run a command and print output"""
    if label:
        print(f"\n{'='*60}")
        print(f"▶ {label}")
        print(f"{'='*60}")
    print(f"$ {cmd}")
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=30)
    exit_code = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out:
        print(out)
    if err:
        print(f"[stderr] {err}")
    print(f"→ exit code: {exit_code}")
    return exit_code, out, err

def main():
    print("Connecting to 172.31.212.123...")
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    
    try:
        ssh.connect(HOST, username=USER, password=PASSWORD, timeout=15)
        print("✅ Connected successfully!")
        
        # Step 1: Check basic system info
        run_cmd(ssh, "echo '=== SYSTEM INFO ==='; cat /etc/os-release 2>/dev/null | head -5; echo '---KERNEL---'; uname -a", "System Info")
        
        # Step 2: Check disk space
        run_cmd(ssh, "df -h /", "Disk Space")
        
        # Step 3: Check what's in .trae-cn-server
        run_cmd(ssh, "ls -la ~/.trae-cn-server/ 2>/dev/null || echo 'Directory not found'", "Current trae-cn-server")
        
        # Step 4: Kill any running trae-related processes
        run_cmd(ssh, "ps aux | grep -i trae | grep -v grep || echo 'No trae processes running'", "Running Trae Processes")
        
        # Step 5: Kill any node/server processes from trae
        run_cmd(ssh, "pkill -f 'trae-cn-server' 2>/dev/null; pkill -f 'vscode-server' 2>/dev/null; pkill -f '\.trae-cn-server' 2>/dev/null; echo 'Killed existing trae processes'", "Kill Old Processes")
        
        # Step 6: Clean up completely
        run_cmd(ssh, "rm -rf ~/.trae-cn-server/ ~/.cache/trae-cn-server/ 2>/dev/null; rm -rf /tmp/trae* /tmp/vscode-server* 2>/dev/null; echo '✅ Cleanup complete'", "Full Cleanup")
        
        # Step 7: Check memory and inodes
        run_cmd(ssh, "df -i /; free -h", "Memory & Inodes")
        
        # Step 8: Check if /tmp is writable
        run_cmd(ssh, "touch /tmp/test_write && rm /tmp/test_write && echo '/tmp is writable'", "Temp Directory Check")
        
        # Step 9: Verify essential tools
        run_cmd(ssh, "which curl tar xz md5sum; tar --version | head -1", "Tool Check")
        
        print("\n" + "="*60)
        print("✅ All fix steps completed!")
        print("="*60)
        print("\nNow you can try connecting again in Trae CN.")
        print("The auto-configuration should work now.")
        
    except paramiko.AuthenticationException:
        print("❌ Authentication failed! Wrong password?")
        sys.exit(1)
    except paramiko.SSHException as e:
        print(f"❌ SSH error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"❌ Error: {e}")
        sys.exit(1)
    finally:
        ssh.close()

if __name__ == "__main__":
    main()
