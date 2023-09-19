#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char* ssid = "esp82661";
const char* password = "password";

IPAddress ip(192, 168, 0, 1); // From RoboRemo app, connect to this IP
IPAddress netmask(255, 255, 255, 0);
const int port = 80; // and this port

ESP8266WebServer server(80);
DNSServer dnsServer;  // Create a DNS server instance

#define motor1A 2
#define motor1B 0
#define motor2A 1
#define motor2B 3

bool wifiConnected = false;

void setup() {
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);

  ap();

  Stop();
}

void loop() {
  if(!wifiConnected){
      dnsServer.processNextRequest();  // Handle DNS requests
      server.handleClient();
  }
}

void handleRoot(){
  const String html = "<!DOCTYPE html>\n"
              "<html lang='en'>\n"
              "<head>\n"
              "    <meta charset='UTF-8'>\n"
              "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
              "    <title>WiFi RC Car Control</title>\n"
              "    <style>\n"
              "        body {\n"
              "            font-family: Arial, sans-serif;\n"
              "            text-align: center;\n"
              "        }\n"
              "        #title {\n"
              "            font-size: 24px;\n"
              "            margin-top: 20px;\n"
              "        }\n"
              "        #ip {\n"
              "            font-size: 18px;\n"
              "            margin-top: 10px;\n"
              "        }\n"
              "        #buttons {\n"
              "            display: flex;\n"
              "            flex-direction: column;\n"
              "            align-items: center;\n"
              "            margin-top: 30px;\n"
              "        }\n"
              "        .button {\n"
              "            display: block;\n"
              "            width: 150px;\n"
              "            height: 40px;\n"
              "            margin: 10px;\n"
              "            font-size: 16px;\n"
              "            text-align: center;\n"
              "            text-decoration: none;\n"
              "            background-color: #007bff;\n"
              "            color: #fff;\n"
              "            border: none;\n"
              "            cursor: pointer;\n"
              "            border-radius: 5px;\n"
              "        }\n"
              "        .button:hover {\n"
              "            background-color: #0056b3;\n"
              "        }\n"
              "    </style>\n"
              "</head>\n"
              "<body>\n"
              "    <div id='title'>Wifi RC Car</div>\n"
              "    <div id='ip'>IP Address: <!-- Your IP address here --></div>\n"
              "    <div id='buttons'>\n"
              "        <a href='/m' class='button'>Mobile</a>\n"
              "        <a href='/pc' class='button'>PC</a>\n"
              "        <a href='/connectwifi' class='button'>Connect to WiFi</a>\n"
              "    </div>\n"
              "</body>\n"
              "</html>";
  server.send(200, "text/html", html);
}

void handleRootMobile() {
  String ipString = WiFi.localIP().toString();
  const String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32-CAM Robot Control</title>
        <style>
            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin: 0;
                padding: 0;
                background-color: #f0f0f0;
            }
    
            .container {
                display: flex;
                flex-direction: row;
                justify-content: center;
                align-items: center;
                height: 100vh;
            }
    
            .button-container {
                display: flex;
                flex-direction: column;
                align-items: center;
            }
    
            .button {
                background-color: #2f4468;
                border: none;
                color: white;
                padding: 15px 30px;
                text-align: center;
                text-decoration: none;
                display: inline-block;
                font-size: 18px;
                margin: 10px;
                cursor: pointer;
                border-radius: 10px;
                transition: background-color 0.3s ease;
            }
    
            .button.clicked {
                background-color: #ff5733; /* Change to a different color when clicked */
            }
    
            .arrow-button {
                font-size: 24px;
            }
    
            .up-down-container {
                display: flex;
                flex-direction: column; /* Align up and down buttons vertically */
                align-items: center;
            }
    
            .left-right-container {
                display: flex;
                flex-direction: row; /* Align left and right buttons horizontally */
                align-items: center;
            }
    
            .stop-button-container {
                display: flex;
                flex-direction: column;
                align-items: center;
            }
    
            /* Adjust button sizes */
            .up-down-container .button {
                width: 150px; /* Wider up and down buttons */
                height: 100px;
            }
    
            .left-right-container .button {
                height: 170px; /* Taller left and right buttons */
                width: 100px;
            }
        </style>
    </head>
    <body>
        <div class="container">
            <div class="up-down-container"> <!-- Container for up and down buttons -->
                <button class="button arrow-button" id="forward-button">▲</button>
                <button class="button arrow-button" id="backward-button">▼</button>
            </div>
            <div class="stop-button-container">
                <button class="button" id="stop-button">STOP</button>
            </div>
            <div class="left-right-container"> <!-- Container for left and right buttons -->
                <button class="button arrow-button" id="left-button">◀</button>
                <button class="button arrow-button" id="right-button">▶</button>
            </div>
        </div>
    
        <script>
            var xhr = new XMLHttpRequest();
            var activeCommands = [];
    
            function sendCommands(commands) {
                var combinedCommand = commands.join("");
                xhr.open("GET", "/control?cmd=" + combinedCommand, true);
                xhr.send();
            }
    
            function addCommand(command) {
                if (!activeCommands.includes(command)) {
                    activeCommands.push(command);
                }
            }
    
            function removeCommand(command) {
                var index = activeCommands.indexOf(command);
                if (index !== -1) {
                    activeCommands.splice(index, 1);
                }
            }
    
            function updateCommands() {
                if (activeCommands.length > 0) {
                    sendCommands(activeCommands);
                } else {
                    sendCommands(["stop"]);
                }
            }
    
            function handleButtonEvent(button, command) {
                button.addEventListener("mousedown", function () {
                    addCommand(command);
                    this.classList.add("clicked"); // Add the "clicked" class
                    updateCommands();
                });
    
                button.addEventListener("mouseup", function () {
                    removeCommand(command);
                    this.classList.remove("clicked"); // Remove the "clicked" class
                    updateCommands();
                });
    
                // Handle touch events for mobile devices
                button.addEventListener("touchstart", function (event) {
                    event.preventDefault(); // Prevent default touch events
                    addCommand(command);
                    this.classList.add("clicked"); // Add the "clicked" class
                    updateCommands();
                });
    
                button.addEventListener("touchend", function (event) {
                    event.preventDefault(); // Prevent default touch events
                    removeCommand(command);
                    this.classList.remove("clicked"); // Remove the "clicked" class
                    updateCommands();
                });
            }
    
            // Attach event listeners to the buttons
            handleButtonEvent(document.getElementById("forward-button"), "forward");
            handleButtonEvent(document.getElementById("backward-button"), "backward");
            handleButtonEvent(document.getElementById("left-button"), "left");
            handleButtonEvent(document.getElementById("right-button"), "right");
    
            document.getElementById("stop-button").addEventListener("click", function () {
                activeCommands = []; // Stop all commands
                updateCommands();
            });
        </script>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

void handleRootPc(){

}

void ap(){
  // Connect to Wi-Fi
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, netmask); // Set the subnet mask
  WiFi.softAP(ssid, password);

  // Initialize the DNS server
  dnsServer.start(53, "*", ip);

  // Set up routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/m", HTTP_GET, handleRootMobile);
  server.on("/pc", HTTP_GET, handleRootPc);
  server.on("/connectwifi", HTTP_GET, wifi);
  server.on("/connectwifi", HTTP_POST, handleConnect);
  server.on("/control", HTTP_GET, [](){
    String command = server.arg("cmd"); 
    // Handle the received command here (e.g., control your motors)
    // You can use the "command" variable to determine the action to take.
    // Implement your motor control logic here.
    if (command == "forward") MotorForward();
    else if (command == "backward") MotorBackward();
    else if (command == "left") TurnLeft();
    else if (command == "right") TurnRight();
    else if (command == "forwardleft") ForwardLeft();
    else if (command == "forwardright") ForwardRight();
    else if (command == "backwardleft") BackwardLeft();
    else if (command == "backwardright") BackwardRight();
    else if (command == "rightforward") ForwardRight();
    else if (command == "leftforward") ForwardLeft();
    else if (command == "rightbackward") BackwardRight();
    else if (command == "leftbackward") BackwardLeft();
    else if (command == "stop") Stop();
    server.send(200, "text/plain", "OK");
  });
  server.begin();

  server.onNotFound([]() {
    server.sendHeader("Location", String("/"), true);
    server.send(302, "text/plain", "Redirecting to the root page...");
  });
}

void MotorForward() {
  digitalWrite(motor1A, 1);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Forward");
}
void MotorBackward() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 1);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Backward");
}
void TurnLeft() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 1);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Left");
}
void TurnRight() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 1);
  server.send(302, "text/plain", "Right");
}
void ForwardLeft() {
  digitalWrite(motor1A, 1);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 1);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Forward Left");
}
void ForwardRight() {
  digitalWrite(motor1A, 1);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 1);
  server.send(302, "text/plain", "Forward Right");
}
void BackwardLeft() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 1);
  digitalWrite(motor2A, 1);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Backward Left");
}
void BackwardRight() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 1);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 1);
  server.send(302, "text/plain", "Backward Right");
}
void Stop() {
  digitalWrite(motor1A, 0);
  digitalWrite(motor1B, 0);
  digitalWrite(motor2A, 0);
  digitalWrite(motor2B, 0);
  server.send(302, "text/plain", "Stop");
}

void wifi(){
    String html = "<!DOCTYPE html>\n"
                "<html lang='en'>\n"
                "<head>\n"
                "    <meta charset='UTF-8'>\n"
                "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
                "    <title>ESP8266 Wi-Fi Setup</title>\n"
                "    <style>\n"
                "        body {\n"
                "            font-family: Arial, sans-serif;\n"
                "            text-align: center;\n"
                "        }\n"
                "        h1 {\n"
                "            font-size: 24px;\n"
                "            margin: 20px;\n"
                "        }\n"
                "        select {\n"
                "            font-size: 16px;\n"
                "            padding: 5px;\n"
                "        }\n"
                "        input[type='password'] {\n"
                "            font-size: 16px;\n"
                "            padding: 5px;\n"
                "        }\n"
                "        .button {\n"
                "            display: block;\n"
                "            width: 150px;\n"
                "            height: 40px;\n"
                "            margin: 20px auto;\n"
                "            font-size: 16px;\n"
                "            text-align: center;\n"
                "            text-decoration: none;\n"
                "            background-color: #007bff;\n"
                "            color: #fff;\n"
                "            border: none;\n"
                "            cursor: pointer;\n"
                "            border-radius: 5px;\n"
                "        }\n"
                "        .button:hover {\n"
                "            background-color: #0056b3;\n"
                "        }\n"
                "    </style>\n"
                "</head>\n"
                "<body>\n"
                "    <h1>ESP8266 Wi-Fi Setup</h1>\n"
                "    <form method='post' action='/'>\n"
                "        <label for='wifi'>Select a Wi-Fi network:</label>\n"
                "        <select id='wifi' name='wifi'>\n";

  // Scan for available Wi-Fi networks and add them to the list
  int numNetworks = WiFi.scanNetworks();
  for (int i = 0; i < numNetworks; i++) {
    html += "            <option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + "</option>\n";
  }

  html += "        </select>\n"
          "        <br>\n"
          "        <label for='password'>Enter Wi-Fi password:</label>\n"
          "        <input type='password' id='password' name='password' required>\n"
          "        <br>\n"
          "        <input type='submit' value='Connect' class='button'>\n"
          "    </form>\n"
          "    <script>\n"
          "        function updateWiFiList() {\n"
          "            var wifiSelect = document.getElementById('wifi');\n"
          "            wifiSelect.innerHTML = '';\n"
          "            var networks = <?php echo json_encode($availableNetworks); ?>; // Arduino code will provide this JSON data\n"
          "            for (var i = 0; i < networks.length; i++) {\n"
          "                var option = document.createElement('option');\n"
          "                option.value = networks[i];\n"
          "                option.text = networks[i];\n"
          "                wifiSelect.appendChild(option);\n"
          "            }\n"
          "</body>\n"
          "</html>";

  server.send(200, "text/html", html);
}

void handleConnect() {
  String ssid = server.arg("wifi");
  String password = server.arg("password");

  // Connect to the selected Wi-Fi network
  WiFi.begin(ssid.c_str(), password.c_str());
  int connectionAttempts = 0;

  while (WiFi.status() != WL_CONNECTED && connectionAttempts < 300) {
    delay(50);
    connectionAttempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    
    // Redirect to a success page
    server.sendHeader("Location", "/success");
    server.send(302, "text/plain", "Connected to Wi-Fi!");
  } else {
    wifiConnected = false;
    // Redirect back to the connection page with an error message
    server.sendHeader("Location", "/connectwifi");
    server.send(302, "text/plain", "Wi-Fi connection failed. Please try again.");
  }
}

void handleSuccess() {
  // Handle the success page (you can customize this page)
  String html = "<!DOCTYPE html>\n"
                "<html lang='en'>\n"
                "<head>\n"
                "    <meta charset='UTF-8'>\n"
                "    <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n"
                "    <title>ESP8266 Wi-Fi Setup</title>\n"
                "    <!-- CSS styles here -->\n"
                "</head>\n"
                "<body>\n"
                "    <h1>Connected to Wi-Fi!</h1>\n"
                "    <!-- Add your content here -->\n"
                "</body>\n"
                "</html>";

  server.send(200, "text/html", html);

  delay(1000);
  // Disconnect from the AP and stop the DNS server
  WiFi.softAPdisconnect(true); // true parameter also releases the IP address
  dnsServer.stop();
}
