# Use Web BLE with esp32 + Chrome 


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
