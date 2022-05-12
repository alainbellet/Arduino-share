let myDevice;
let myService = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
let bluetoothDevice;

function connect() {
  navigator.bluetooth
    .requestDevice({
      filters: [{ services: ['6e400001-b5a3-f393-e0a9-e50e24dcca9e'] }],
      optionalServices: [myService],
    })
    .then(function (device) {
      // save the device returned so you can disconnect later:
      myDevice = device;
      console.log(device);
      // connect to the device once you find it:
      return device.gatt.connect();
    })
    .then(function (server) {
      // get the primary service:
      return server.getPrimaryService(myService);
    })
    .then(function (service) {
      // get the  characteristic:
      return service.getCharacteristics();
    })
    .then(function (characteristics) {
      // subscribe to the characteristic:
      //console.log(characteristics);
      bluetoothDevicePrinter = characteristics[1];
      bluetoothDeviceButton = characteristics[0];
      bluetoothDeviceButton.startNotifications().then(subscribeToChanges);
    })
    .catch(function (error) {
      // catch any errors:
      console.error('Connection failed!', error);
    });
}

// subscribe to changes from the meter:
function subscribeToChanges(characteristic) {
  characteristic.oncharacteristicvaluechanged = handleData;
}

// handle incoming data:
function handleData(event) {
  const buttonState = event.target.value.getInt8();
  console.log(buttonState);
  if (buttonState == 1) {
    document.getElementById('output').innerHTML += 'button pressed' + '<br>';
  } else {
    document.getElementById('output').innerHTML += 'button released' + '<br>';
  }
}

function sendData(command) {
  const enc = new TextEncoder('utf-8');

  if (bluetoothDevicePrinter) {
    console.log('message sent');
    document.getElementById('output').innerHTML += 'message sent <br>';
    bluetoothDevicePrinter.writeValue(enc.encode(command));
  }
}

// disconnect function:
function disconnect() {
  if (myDevice) {
    // disconnect:
    myDevice.gatt.disconnect();
  }
}
