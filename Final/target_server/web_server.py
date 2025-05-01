import argparse
import http.server
import socketserver
import requests
import json
import os
import sys

DEFAULT_PORT = 8000
SERVER_ROOT_DIR = "./"
MOCK_API_ENDPOINT = "http://jsonplaceholder.typicode.com"

class CustomHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=SERVER_ROOT_DIR, **kwargs)

    def do_GET(self):
        # Handle dynamic endpoints
        if self.path.startswith("/api"):
            self.handle_dynamic_content()
        else:
            # Serve static content by default
            super().do_GET()

    def handle_dynamic_content(self):
        if len(self.path) > len("/api/"):
            self.fetch_and_serve_json(f"{MOCK_API_ENDPOINT}/{self.path[5:]}")
        else:
            self.send_error(http.HTTPStatus.NOT_FOUND, "Invalid API endpoint. Missing resource (e.g. /api/users)")

    def fetch_and_serve_json(self, url):
        try:
            response = requests.get(url, proxies={"http": None, "https": None})
            response.raise_for_status()  # Raise an error for HTTP issues
            data = response.json()

            # Serve the JSON response to the client
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(data, indent=4).encode("utf-8"))
        except requests.RequestException as e:
            self.send_error(http.HTTPStatus.INTERNAL_SERVER_ERROR, f"Error fetching data from external API: {e}")

class ThreadingHTTPServer(socketserver.ThreadingMixIn, http.server.HTTPServer):
    pass

def run_multithreaded_server(port):
    with ThreadingHTTPServer(("localhost", port), CustomHTTPRequestHandler) as server:
        print(f"Serving on port {port} (Multithreaded)")
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")
            sys.exit(0)

def run_server(port):
    with socketserver.TCPServer(("localhost", port), CustomHTTPRequestHandler) as server:
        print(f"Serving on port {port} (Single-threaded)")
        try:
            server.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")
            sys.exit(0)

def main():
    if not os.path.exists(SERVER_ROOT_DIR):
        print(f"Server's content directory {SERVER_ROOT_DIR} does not exist.")
        sys.exit(1)

    parser = argparse.ArgumentParser()
    parser.add_argument("-p", "--port", help="port number that server listens on",
                        type=int, default=DEFAULT_PORT)
    parser.add_argument("-s", "--singlethreaded", help="Run a single-threaded server (default is multithreaded)",
                        action="store_true")
    args = parser.parse_args()

    if args.singlethreaded:
        run_server(args.port)
    else:
        run_multithreaded_server(args.port)

if __name__ == "__main__":
    main()
