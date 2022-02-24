# Use Web BLE with esp32 + Chrome 

## Better to use NimbleBLE Library

My first test was based on ESP32BLE Library which works fine but need a lot of memory, the use of [NimbleBLE](https://github.com/h2zero/NimBLE-Arduino) is much better

**ESP32BLE**
Sketch uses 986178 bytes (**75%**) of program storage space. Maximum is 1310720 bytes.
Global variables use 30412 bytes (9%) of dynamic memory, leaving 297268 bytes for local variables. Maximum is 327680 bytes.
**NimbleBLE**
Sketch uses 558850 bytes (**42%**) of program storage space. Maximum is 1310720 bytes.
Global variables use 27044 bytes (8%) of dynamic memory, leaving 300636 bytes for local variables. Maximum is 327680 bytes.

## Usage

Test here once flashed on your esp32, you should get one value

https://www.notdefined.net/web-ble/

```javascript
let myCharacteristic;

  // Function to display logs on the website instead of console
  function Log(msg) {
    document.querySelector('#logs').innerHTML =
      document.querySelector('#logs').innerHTML + '<br>' + msg;
  }
  function UpdateOutput(msg) {
    document.querySelector('#outputfield').innerHTML = '<b>' + msg + '</b>';
  }
  let options = {
    filters: [{ services: ['4fafc201-1fb5-459e-8fcc-c5c9c331914b'] }],
  };
  document.querySelector('button').addEventListener('click', async (event) => {
    navigator.bluetooth
      .requestDevice(options)
      .then((device) => {
        Log('connecting to ' + device.name);
        // Attempts to connect to remote GATT Server.
        return device.gatt.connect();
      })
      .then((server) => {
        // Getting Service…
        return server.getPrimaryService('4fafc201-1fb5-459e-8fcc-c5c9c331914b');
      })
      .then((service) => {
        // Getting Characteristic…
        return service.getCharacteristic(
          'beb5483e-36e1-4688-b7f5-ea07361b26a8'
        );
      })
      .then((characteristic) => {
        myCharacteristic = characteristic;
        //const testValue = Uint8Array.of("1");
        //myCharacteristic.writeValue(testValue).then(function() { console.log("written") })
        myCharacteristic.addEventListener(
          'characteristicvaluechanged',
          handleValueChanged
        );
        myCharacteristic.startNotifications();
        // Reading value
        return characteristic.readValue();
      })
      .then((value) => {
        Log(`Value is ${value.getUint8(0)}`);
      })
      .catch((error) => {
        console.error(error);
      });
  });

  function handleValueChanged(event) {
    let newValue = event.target.value.getUint8(0);
    UpdateOutput(`Recieved: ${newValue}`);
  }
```
