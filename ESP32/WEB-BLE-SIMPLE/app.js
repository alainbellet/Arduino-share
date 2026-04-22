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
    statusDiv.textContent = `Connected to ${bluetoothDevice ? bluetoothDevice.name : 'Device'}`;
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
      filters: [{ services: [SERVICE_UUID] }],
      optionalServices: [SERVICE_UUID],
    });

    addLog(`Device selected: ${bluetoothDevice.name}`);

    bluetoothDevice.addEventListener('gattserverdisconnected', onDisconnected);

    addLog('Connecting to GATT Server...');
    const server = await bluetoothDevice.gatt.connect();

    addLog('Getting Service...');
    const service = await server.getPrimaryService(SERVICE_UUID);

    addLog('Getting Characteristics...');
    const characteristics = await service.getCharacteristics();
    
    for (const characteristic of characteristics) {
      if (characteristic.uuid === CHARACTERISTIC_UUID_1) characteristic1 = characteristic;
      if (characteristic.uuid === CHARACTERISTIC_UUID_2) characteristic2 = characteristic;
      if (characteristic.uuid === CHARACTERISTIC_UUID_3) characteristic3 = characteristic;
    }

    addLog('Starting Notifications...');
    
    if (characteristic1) {
      await characteristic1.startNotifications();
      characteristic1.addEventListener('characteristicvaluechanged', (event) => handleNotification(1, event));
      const value1 = await characteristic1.readValue();
      handleInitialValue(1, value1);
    }
    
    if (characteristic2) {
      await characteristic2.startNotifications();
      characteristic2.addEventListener('characteristicvaluechanged', (event) => handleNotification(2, event));
      const value2 = await characteristic2.readValue();
      handleInitialValue(2, value2);
    }
    
    if (characteristic3) {
      await characteristic3.startNotifications();
      characteristic3.addEventListener('characteristicvaluechanged', (event) => handleNotification(3, event));
      const value3 = await characteristic3.readValue();
      handleInitialValue(3, value3);
    }

    updateStatus(true);
    addLog('✓ Connected successfully!');

  } catch (error) {
    addLog(`Error: ${error.message}`);
    console.error('Connection error:', error);
  }
}

function handleNotification(buttonNum, event) {
  const value = event.target.value;
  processValue(buttonNum, value);
}

function handleInitialValue(buttonNum, value) {
  processValue(buttonNum, value);
}

function processValue(buttonNum, value) {
  if (!value || value.byteLength === 0) {
    return; // Ignore empty values which happen on initial connection before the device writes to the characteristic
  }

  const decoder = new TextDecoder('utf-8');
  const textValue = decoder.decode(value);
  
  // Try to determine if it's a text message or a button state
  if (textValue.startsWith('Heartbeat') || /^[a-zA-Z0-9\s:_-]{2,}$/.test(textValue)) {
    // If it's the Heartbeat sketch, it will send messages
    addLog(`<span style="color: #4facfe;">📩 Message:</span> ${textValue}`);
  } else {
    // Otherwise handle as a legacy button press (uint8 value)
    const uint8Value = value.getUint8(0);
    updateButtonState(buttonNum, uint8Value);
  }
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
