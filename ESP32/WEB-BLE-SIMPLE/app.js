const SERVICE_UUID = '12345678-1234-1234-1234-123456789012';
const CHARACTERISTIC_UUID_1 = '87654321-4321-4321-4321-210987654321';
const CHARACTERISTIC_UUID_2 = '87654321-4321-4321-4321-210987654322';
const CHARACTERISTIC_UUID_3 = '87654321-4321-4321-4321-210987654323';

let bluetoothDevice;
let characteristic1;
let characteristic2;
let characteristic3;

const connectBtn = document.getElementById('connectBtn');
const disconnectBtn = document.getElementById('disconnectBtn');
const statusDiv = document.getElementById('status');
const buttonStateDiv1 = document.getElementById('buttonState1');
const buttonStateDiv2 = document.getElementById('buttonState2');
const buttonStateDiv3 = document.getElementById('buttonState3');
const logDiv = document.getElementById('log');

function addLog(message) {
  const timestamp = new Date().toLocaleTimeString();
  const entry = document.createElement('div');
  entry.className = 'log-entry';
  entry.innerHTML = `<span class="timestamp">${timestamp}</span><span>${message}</span>`;
  logDiv.insertBefore(entry, logDiv.firstChild);

  if (logDiv.children.length > 20) {
    logDiv.removeChild(logDiv.lastChild);
  }
}

function updateStatus(connected) {
  if (connected) {
    statusDiv.textContent = 'Connected to ESP32-C3';
    statusDiv.className = 'status connected';
    connectBtn.style.display = 'none';
    disconnectBtn.style.display = 'block';
  } else {
    statusDiv.textContent = 'Disconnected';
    statusDiv.className = 'status disconnected';
    connectBtn.style.display = 'block';
    disconnectBtn.style.display = 'none';
  }
}

function updateButtonState(buttonNum, value) {
  const buttonStateDiv =
    buttonNum === 1
      ? buttonStateDiv1
      : buttonNum === 2
        ? buttonStateDiv2
        : buttonStateDiv3;
  if (value === 1) {
    buttonStateDiv.textContent = 'Pressed';
    buttonStateDiv.className = 'button-state pressed';
    addLog(`Button ${buttonNum} PRESSED`);
  } else {
    buttonStateDiv.textContent = 'Released';
    buttonStateDiv.className = 'button-state released';
    addLog(`Button ${buttonNum} RELEASED`);
  }
}

async function connect() {
  try {
    addLog('Requesting Bluetooth Device...');

    bluetoothDevice = await navigator.bluetooth.requestDevice({
      acceptAllDevices: true,
      optionalServices: [SERVICE_UUID],
    });

    addLog(`Device selected: ${bluetoothDevice.name}`);

    bluetoothDevice.addEventListener('gattserverdisconnected', onDisconnected);

    addLog('Connecting to GATT Server...');
    const server = await bluetoothDevice.gatt.connect();

    addLog('Getting Service...');
    const service = await server.getPrimaryService(SERVICE_UUID);

    addLog('Getting Characteristics...');
    characteristic1 = await service.getCharacteristic(CHARACTERISTIC_UUID_1);
    characteristic2 = await service.getCharacteristic(CHARACTERISTIC_UUID_2);
    characteristic3 = await service.getCharacteristic(CHARACTERISTIC_UUID_3);

    addLog('Starting Notifications...');
    await characteristic1.startNotifications();
    await characteristic2.startNotifications();
    await characteristic3.startNotifications();

    characteristic1.addEventListener('characteristicvaluechanged', (event) =>
      handleNotification(1, event),
    );
    characteristic2.addEventListener('characteristicvaluechanged', (event) =>
      handleNotification(2, event),
    );
    characteristic3.addEventListener('characteristicvaluechanged', (event) =>
      handleNotification(3, event),
    );

    updateStatus(true);
    addLog('✓ Connected successfully!');

    const value1 = await characteristic1.readValue();
    const value2 = await characteristic2.readValue();
    const value3 = await characteristic3.readValue();
    updateButtonState(1, value1.getUint8(0));
    updateButtonState(2, value2.getUint8(0));
    updateButtonState(3, value3.getUint8(0));
  } catch (error) {
    addLog(`Error: ${error.message}`);
    console.error('Connection error:', error);
  }
}

function handleNotification(buttonNum, event) {
  const value = event.target.value.getUint8(0);
  updateButtonState(buttonNum, value);
}

async function disconnect() {
  if (bluetoothDevice && bluetoothDevice.gatt.connected) {
    addLog('Disconnecting...');
    bluetoothDevice.gatt.disconnect();
  }
}

function onDisconnected(event) {
  addLog('Device disconnected');
  updateStatus(false);
  characteristic1 = null;
  characteristic2 = null;
  characteristic3 = null;
}

connectBtn.addEventListener('click', connect);
disconnectBtn.addEventListener('click', disconnect);

if (!navigator.bluetooth) {
  addLog('⚠ Web Bluetooth API is not available in this browser');
  connectBtn.disabled = true;
  statusDiv.textContent = 'Web Bluetooth not supported';
  statusDiv.className = 'status disconnected';
}
