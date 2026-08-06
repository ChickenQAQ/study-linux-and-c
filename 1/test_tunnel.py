#!/usr/bin/env python3
"""Check SSH key setup and test paramiko reverse port forwarding"""
import paramiko
import socket
import select
import time
import sys
import threading

PW = '123456'
HOST = '172.31.212.123'

ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)

def run(cmd, timeout=10):
    print(f'$ {cmd}')
    stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
    ec = stdout.channel.recv_exit_status()
    out = stdout.read().decode('utf-8', errors='replace').strip()
    err = stderr.read().decode('utf-8', errors='replace').strip()
    if out: print(out[:1500])
    if err: print(f'[e] {err[:300]}')
    print(f'→ exit: {ec}')
    return ec, out, err

print("=" * 60)
print("🔍 Check SSH key setup")
print("=" * 60)

run("ls -la ~/.ssh/")
run("cat ~/.ssh/id_ed25519.pub 2>/dev/null || echo 'NO KEY'")
run("cat ~/.ssh/authorized_keys 2>/dev/null | head -5 || echo 'NO authorized_keys'")
run("cat ~/.ssh/id_ed25519 2>/dev/null | head -1 || echo 'NO PRIVATE KEY'")

# Test localhost SSH with key
print("\n=== Test localhost SSH key auth ===")
run("ssh -o StrictHostKeyChecking=no -o BatchMode=yes chicken@127.0.0.1 'echo SSH_KEY_OK' 2>&1 || echo 'KEY_AUTH_FAILED'", timeout=10)

# Get the private key content
print("\n=== Copying private key ===")
ec, key_content, _ = run("cat ~/.ssh/id_ed25519")

ssh.close()

if key_content and 'NO PRIVATE KEY' not in key_content:
    print("\n✅ Private key found! Will use it for key-based SSH")
    
    # Save the key locally
    import tempfile
    import os as os_module
    
    key_file = os_module.path.join(tempfile.gettempdir(), 'trae_tunnel_key')
    with open(key_file, 'w') as f:
        f.write(key_content)
    os_module.chmod(key_file, 0o600)
    
    print(f"✅ Key saved to {key_file}")
    
    # Now try paramiko reverse tunnel with Transport + key auth
    print("\n" + "=" * 60)
    print("🔗 Testing paramiko reverse port forwarding")
    print("=" * 60)
    
    try:
        # Use RSAKey or Ed25519Key to load the key
        from paramiko import Ed25519Key
        pkey = Ed25519Key(filename=key_file)
        
        transport = paramiko.Transport((HOST, 22))
        transport.connect(username='chicken', pkey=pkey)
        
        print("✅ Transport connected with key auth!")
        
        # Simple forward handler
        tunnel_active = threading.Event()
        tunnel_active.set()
        
        def handler(chan, src_addr, dest_addr):
            print(f"[Tunnel] New connection from {src_addr}")
            # This handler gets called when someone connects to remote:PORT
            # We need to connect to a local service and pipe data
            # For now, just close it to test
            chan.close()
        
        try:
            transport.request_port_forward('127.0.0.1', 18888, handler)
            print("✅ Port forward requested: remote:18888")
            
            # Test if it's listening
            time.sleep(0.5)
            # Connect from remote to test
            ec2, out2, _ = run("timeout 3 bash -c 'echo > /dev/tcp/127.0.0.1/18888' 2>&1 && echo 'PORT_OPEN' || echo 'PORT_CLOSED'", timeout=5)
            
            if 'PORT_OPEN' in out2:
                print("✅ Port 18888 is open and forwarding!")
            else:
                print("❌ Port 18888 is not accessible")
            
            # Cancel forwarding
            transport.cancel_port_forward('127.0.0.1', 18888)
            
        except Exception as e:
            print(f"❌ Port forward request failed: {e}")
        
        transport.close()
        
    except Exception as e:
        print(f"❌ Transport failed: {e}")
        import traceback
        traceback.print_exc()
else:
    print("\n⚠️ No SSH key found. Need to generate one.")
    
    # Let's check if we can use local port forwarding instead (our side)
    print("\n=== Checking if system SSH has GatewayPorts ===")
    ssh2 = paramiko.SSHClient()
    ssh2.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh2.connect(HOST, username='chicken', password=PW, timeout=15)
    run2 = lambda cmd, t=10: ssh2.exec_command(cmd, timeout=t)
    stdin, stdout, stderr = run2("cat /etc/ssh/sshd_config 2>/dev/null | grep -i 'GatewayPorts\\|AllowTcpForwarding' || echo 'Not found'", 5)
    print(stdout.read().decode().strip())
    ssh2.close()

print("\n=== DONE ===")
