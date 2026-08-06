#!/usr/bin/env python3
"""
Solution: Create a HTTP proxy tunnel from Windows to WSL for Trae CN ai-agent

How it works:
1. Start a simple HTTP CONNECT proxy server on Windows (port 8889)
2. Use paramiko to set up remote port forwarding: remote:8889 → localhost:8889
3. Configure the remote ai-agent to use this proxy for Trae API calls
"""
import paramiko
import threading
import socket
import select
import os
import signal
import sys
import time
import json

PW = '123456'
HOST = '172.31.212.123'
LOCAL_PROXY_PORT = 8889
REMOTE_PORT = 8889  # Same port on remote

# ====== Simple HTTP CONNECT Proxy Server ======
class HttpProxyHandler:
    """Handle HTTP CONNECT method (for HTTPS) and regular HTTP"""
    
    def __init__(self, client_sock, client_addr):
        self.client = client_sock
        self.addr = client_addr
    
    def handle(self):
        try:
            request = b''
            while b'\r\n\r\n' not in request:
                chunk = self.client.recv(4096)
                if not chunk:
                    return
                request += chunk
            
            first_line = request.split(b'\r\n')[0].decode('utf-8', errors='replace')
            parts = first_line.split()
            
            if len(parts) < 2:
                return
            
            method = parts[0].upper()
            
            if method == 'CONNECT':
                self._handle_connect(request)
            else:
                self._handle_http(request)
        except Exception as e:
            print(f"[Proxy] Error: {e}")
        finally:
            try:
                self.client.close()
            except:
                pass
    
    def _handle_connect(self, request):
        """HTTPS CONNECT tunnel"""
        first_line = request.split(b'\r\n')[0].decode('utf-8', errors='replace')
        _, hostport, _ = first_line.split()
        host, port = hostport.split(':')
        port = int(port)
        
        try:
            remote = socket.create_connection((host, port), timeout=15)
            self.client.sendall(b'HTTP/1.1 200 Connection Established\r\n\r\n')
            
            # Bidirectional forwarding
            sockets = [self.client, remote]
            while True:
                readable, _, _ = select.select(sockets, [], [], 30)
                if not readable:
                    break
                for s in readable:
                    data = s.recv(65536)
                    if not data:
                        return
                    if s is self.client:
                        remote.sendall(data)
                    else:
                        self.client.sendall(data)
        except Exception as e:
            try:
                self.client.sendall(f'HTTP/1.1 502 Bad Gateway\r\n\r\n'.encode())
            except:
                pass
    
    def _handle_http(self, request):
        """Regular HTTP forward proxy"""
        first_line = request.split(b'\r\n')[0].decode('utf-8', errors='replace')
        parts = first_line.split()
        if len(parts) < 2:
            return
        url = parts[1]
        
        # Parse URL
        from urllib.parse import urlparse
        parsed = urlparse(url)
        host = parsed.hostname
        port = parsed.port or 80
        path = parsed.path or '/'
        if parsed.query:
            path += '?' + parsed.query
        
        try:
            remote = socket.create_connection((host, port), timeout=15)
            # Rewrite request to absolute path
            new_request = request.replace(url.encode(), path.encode(), 1)
            remote.sendall(new_request)
            
            response = b''
            while True:
                chunk = remote.recv(65536)
                if not chunk:
                    break
                response += chunk
                try:
                    self.client.sendall(chunk)
                except:
                    break
            remote.close()
        except Exception as e:
            print(f"[Proxy] HTTP error: {e}")


class ProxyServer:
    def __init__(self, port):
        self.port = port
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.running = False
    
    def start(self):
        self.server.bind(('127.0.0.1', self.port))
        self.server.listen(50)
        self.server.settimeout(1.0)
        self.running = True
        print(f"[Proxy] HTTP proxy listening on 127.0.0.1:{self.port}")
        
        while self.running:
            try:
                client, addr = self.server.accept()
                t = threading.Thread(target=lambda: HttpProxyHandler(client, addr).handle(), daemon=True)
                t.start()
            except socket.timeout:
                continue
            except Exception as e:
                if self.running:
                    print(f"[Proxy] Accept error: {e}")
    
    def stop(self):
        self.running = False
        try:
            self.server.close()
        except:
            pass


# ====== Main ======
def main():
    print("=" * 60)
    print("🌐 Starting HTTP Proxy + SSH Tunnel for Trae CN ai-agent")
    print("=" * 60)
    
    # Step 1: Start proxy server in background thread
    proxy = ProxyServer(LOCAL_PROXY_PORT)
    proxy_thread = threading.Thread(target=proxy.start, daemon=True)
    proxy_thread.start()
    time.sleep(0.5)  # Give it time to start
    
    print("\n✅ Proxy server started")
    
    # Step 2: SSH into remote and set up remote port forwarding
    print(f"\n🔗 Setting up SSH reverse tunnel: remote:{REMOTE_PORT} → localhost:{LOCAL_PROXY_PORT}")
    
    try:
        transport = paramiko.Transport((HOST, 22))
        transport.connect(username='chicken', password=PW)
        
        # Request remote port forwarding
        # This makes port REMOTE_PORT on the remote server forward to localhost:LOCAL_PROXY_PORT on our side
        def handler(chan, src_addr, dest_addr):
            """Forward connections from remote to our proxy"""
            # Connect to our local proxy
            try:
                proxy_sock = socket.create_connection(('127.0.0.1', LOCAL_PROXY_PORT), timeout=10)
            except Exception as e:
                print(f"[Tunnel] Failed to connect to proxy: {e}")
                chan.close()
                return
            
            # Bidirectional forwarding
            while True:
                r, w, e = select.select([chan, proxy_sock], [], [], 30)
                for sock in r:
                    try:
                        data = sock.recv(65536)
                        if not data:
                            chan.close()
                            proxy_sock.close()
                            return
                        if sock is chan:
                            proxy_sock.sendall(data)
                        else:
                            chan.sendall(data)
                    except:
                        try: chan.close()
                        except: pass
                        try: proxy_sock.close()
                        except: pass
                        return
        
        transport.request_port_forward('', REMOTE_PORT, handler)
        print(f"✅ Reverse tunnel established: remote:{REMOTE_PORT} → Windows proxy")
        
        # Step 3: SSH again (via exec) to set environment variables on remote
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        ssh.connect(HOST, username='chicken', password=PW, timeout=15)
        
        def run(cmd, timeout=10):
            stdin, stdout, stderr = ssh.exec_command(cmd, timeout=timeout)
            ec = stdout.channel.recv_exit_status()
            out = stdout.read().decode('utf-8', errors='replace').strip()
            err = stderr.read().decode('utf-8', errors='replace').strip()
            if out: print(out[:1000])
            if err: print(f'[e] {err[:300]}')
            return ec, out, err
        
        # Set proxy environment variables for the ai-agent
        print("\n⚙️ Configuring remote ai-agent to use proxy...")
        
        # Write proxy config to /etc/environment
        run(f"echo '{PW}' | sudo -S -k sh -c 'cat > /etc/profile.d/trae-proxy.sh << \"SHEOF\"\n" +
            "export http_proxy=http://127.0.0.1:8889\n" +
            "export https_proxy=http://127.0.0.1:8889\n" +
            "export HTTP_PROXY=http://127.0.0.1:8889\n" +
            "export HTTPS_PROXY=http://127.0.0.1:8889\n" +
            "export no_proxy=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n" +
            "export NO_PROXY=localhost,127.0.0.1,10.0.0.0/8,172.16.0.0/12,192.168.0.0/16\n" +
            "SHEOF\nchmod +x /etc/profile.d/trae-proxy.sh'")
        
        # Also set it live for the current session
        run("export http_proxy=http://127.0.0.1:8889 && " +
            "export https_proxy=http://127.0.0.1:8889 && " +
            "echo 'Proxy env vars set for current session'")
        
        # Verify file was created
        run("cat /etc/profile.d/trae-proxy.sh 2>/dev/null || echo 'NOT CREATED'")
        
        # Test the proxy
        print("\n🔍 Testing proxy connectivity...")
        run("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 " +
            "'https://api.trae.com.cn' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
        
        run("curl -sS --connect-timeout 10 --proxy http://127.0.0.1:8889 " +
            "'https://www.baidu.com' -o /dev/null -w 'HTTP %{http_code} (%{time_total}s)' 2>&1", timeout=20)
        
        ssh.close()
        
        print("\n" + "=" * 60)
        print("✅ Setup Complete!")
        print("=" * 60)
        print("""
The HTTP proxy tunnel is running. To use it:
1. RESTART Trae CN (close completely then reopen)
2. Reconnect to remote server 172.31.212.123
3. The remote ai-agent should now be able to reach Trae's APIs

The tunnel will keep running as long as this script is active.
Press Ctrl+C to stop.
""")
        
        # Keep the tunnel running
        while True:
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n\n🛑 Stopping...")
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
    finally:
        proxy.stop()
        try:
            transport.close()
        except:
            pass
        print("✅ Tunnel closed")

if __name__ == '__main__':
    main()
