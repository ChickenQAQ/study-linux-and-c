#!/usr/bin/env python3
"""Minimal test: just start proxy and test remote connectivity"""
import paramiko, socket, select, threading, time, os, tempfile

PW = '123456'
HOST = '172.31.212.123'
PORT = 18888

# Simple TCP echo server for testing
def echo_server(port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('127.0.0.1', port))
    s.listen(5)
    s.settimeout(1)
    while True:
        try:
            c, a = s.accept()
            data = c.recv(1024)
            c.sendall(b'ECHO:' + data)
            c.close()
        except socket.timeout:
            continue
        except:
            break

print("[1] Starting echo server...")
t = threading.Thread(target=echo_server, args=(PORT,), daemon=True)
t.start()
time.sleep(0.2)
print("  ✅ Echo server on 127.0.0.1:18888")

print("[2] SSH setup...")
ssh = paramiko.SSHClient()
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
ssh.connect(HOST, username='chicken', password=PW, timeout=15)
print("  ✅ SSH connected")

# Get key
stdin, stdout, stderr = ssh.exec_command("cat ~/.ssh/id_ed25519", timeout=5)
key_data = stdout.read().decode()
key_file = os.path.join(tempfile.gettempdir(), 'trae_key')
with open(key_file, 'w') as f: f.write(key_data)
os.chmod(key_file, 0o600)
from paramiko import Ed25519Key
pkey = Ed25519Key(filename=key_file)

print("[3] Transport + forward...")
transport = paramiko.Transport((HOST, 22))
transport.connect(username='chicken', pkey=pkey)
print("  ✅ Transport connected")

def echo_handler(chan, src_addr, dest_addr):
    """Bidirectional echo/forward handler"""
    print(f"  🔄 Connection from {src_addr}")
    try:
        # Connect to local echo server
        ps = socket.create_connection(('127.0.0.1', PORT), timeout=5)
        data_from_remote = chan.recv(65536)
        if data_from_remote:
            print(f"  📨 Received {len(data_from_remote)} bytes from remote")
            ps.sendall(data_from_remote)
            data_back = ps.recv(65536)
            if data_back:
                print(f"  📤 Sending {len(data_back)} bytes back")
                chan.sendall(data_back)
        ps.close()
    except Exception as e:
        print(f"  ❌ {e}")
    chan.close()

transport.request_port_forward('127.0.0.1', PORT, echo_handler)
print("  ✅ Port forward active")

# Test from remote
print("\n[4] Testing from remote...")
stdin, stdout, stderr = ssh.exec_command(
    "timeout 5 bash -c 'exec 3<>/dev/tcp/127.0.0.1/18888; echo \"HELLO\" >&3; cat <&3' 2>&1 || echo 'FAILED'",
    timeout=10
)
ec = stdout.channel.recv_exit_status()
out = stdout.read().decode().strip()
err = stderr.read().decode().strip()
print(f"  Exit: {ec}")
print(f"  OUT: {out[:300]}")
if err: print(f"  ERR: {err}")

if 'ECHO:HELLO' in out:
    print("\n✅ PORT FORWARDING WORKS!")
else:
    print("\n❌ Port forwarding failed!")

transport.close()
ssh.close()
print("\nDone")
