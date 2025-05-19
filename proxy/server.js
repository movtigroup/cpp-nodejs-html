const WebSocket = require('ws');
const net = require('net');

// در Docker Compose، نام سرویس سرور همان "server" است.
const SERVER_HOST = "server";
const SERVER_PORT = 12345;

// راه‌اندازی WebSocket Server روی پورت 8080
const wss = new WebSocket.Server({ port: 8080 }, () => {
  console.log("WebSocket Server روی پورت 8080 راه‌اندازی شد.");
});

wss.on('connection', ws => {
  console.log("یک کلاینت WebSocket متصل شد.");
  
  ws.on('message', message => {
    console.log("پیام دریافتی از کلاینت:", message.toString());
    
    // ایجاد یک اتصال TCP به سرور C++
    const tcpClient = new net.Socket();
    tcpClient.connect(SERVER_PORT, SERVER_HOST, () => {
      tcpClient.write(message);
    });
    
    tcpClient.on('data', (data) => {
      ws.send(data.toString());
      tcpClient.end();
    });
    
    tcpClient.on('error', (err) => {
      console.error("خطای TCP:", err.message);
      ws.send("خطای ارتباط با سرور C++.");
    });
  });
  
  ws.on('close', () => {
    console.log("اتصال WebSocket کلاینت قطع شد.");
  });
});

wss.on('error', (err) => {
  console.error("خطای WebSocket Server:", err.message);
});
