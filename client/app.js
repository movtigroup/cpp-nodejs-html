const output = document.getElementById('output');
// ارتباط WebSocket به پراکسی؛ در محیط Docker معمولا DNS به نام سرویس استفاده می‌شود.
// اما در مرورگر ما از پنجره‌ی جاری استفاده می‌کنیم. در این نمونه از localhost:8080 استفاده شده است.
const ws = new WebSocket('ws://' + window.location.hostname + ':8080');

ws.onopen = function() {
  writeMessage('ارتباط با سرور WebSocket برقرار شد.');
};

ws.onmessage = function(event) {
  writeMessage('پاسخ از سرور: ' + event.data);
};

ws.onerror = function(error) {
  writeMessage('خطا: ' + error.message);
};

ws.onclose = function() {
  writeMessage('اتصال WebSocket قطع شد.');
};

function sendChoice(choice) {
  if (ws.readyState === WebSocket.OPEN) {
    ws.send(choice);
    let choiceText = choice === '1' ? 'سنگ' : choice === '2' ? 'کاغذ' : choice === '3' ? 'قیچی' : 'خروج';
    writeMessage('انتخاب شما: ' + choiceText);
  } else {
    writeMessage('WebSocket آماده نیست.');
  }
}

function writeMessage(message) {
  const p = document.createElement('p');
  p.textContent = message;
  output.appendChild(p);
}
