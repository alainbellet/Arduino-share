(function () {
  'use strict';

  document.addEventListener('DOMContentLoaded', (event) => {
    let connectButton = document.querySelector('#connect');
    let statusDisplay = document.querySelector('#status');
    let port;

    function addLine(linesId, text) {
      var senderLine = document.createElement('div');
      senderLine.className = 'line';
      var textnode = document.createTextNode(text);
      senderLine.appendChild(textnode);
      document.getElementById(linesId).appendChild(senderLine);
      return senderLine;
    }

    let currentReceiverLine;

    function appendLine(linesId, text) {
      if (currentReceiverLine) {
        currentReceiverLine.innerHTML = currentReceiverLine.innerHTML + text;
      } else {
        currentReceiverLine = addLine(linesId, text);
      }
    }

    function connect() {
      port.connect().then(
        () => {
          statusDisplay.textContent = '';
          connectButton.textContent = 'Disconnect';

          port.onReceive = (data) => {
            let textDecoder = new TextDecoder();
            //console.log(textDecoder.decode(data));
            if (data.getInt8() === 13) {
              currentReceiverLine = null;
            } else {
              //commandInterpretter(textDecoder.decode(data));
              talkCommands.commandInterpret(textDecoder.decode(data));
              // appendLine('receiver_lines', textDecoder.decode(data));
            }
          };
          port.onReceiveError = (error) => {
            console.error(error);
            port.disconnect();
            connectButton.textContent = 'Connect';
            statusDisplay.textContent = error;
            port = null;
          };
        },
        (error) => {
          statusDisplay.textContent = error;
        }
      );
    }

    function commandInterpretter(data) {
      const command = data.substring(0, 1);
      const val = data.substring(1);
      if (command == 'M') {
        // message
        console.log('Message: ' + val);
        appendLine('receiver_lines', 'Message: ' + val);
      }
      if (command == 'B') {
        // Button
        console.log('Button: ' + val);
        appendLine('receiver_lines', 'Button: ' + val);
      }
      if (command == 'P') {
        // BPotentiometter
        console.log('Potentiometer: ' + val);
        // appendLine('receiver_lines', 'Potentiometer:' + val);
      }
    }

    function commandSender(data) {
      console.log(data);
      port.send(new TextEncoder('utf-8').encode(data));
    }

    function changeLedColor(led_index, led_color_code, led_blink = 0) {
      talkCommands.commandSend('L' + led_index + led_color_code + led_blink);
    }
    function changeAllLedsColor(led_color_code, led_blink = 0) {
      for (let i = 0; i < 10; i++) {
        talkCommands.commandSend('L' + i + led_color_code + led_blink);
      }
    }
    function allLedsOff() {
      for (let i = 0; i < 10; i++) {
        talkCommands.commandSend('L' + i + '00' + 0);
      }
    }

    let redButton = document.querySelector('#btnred');
    redButton.addEventListener('click', function () {
      changeLedColor(0, '02', 0);
    });
    let greenButton = document.querySelector('#btngreen');
    greenButton.addEventListener('click', function () {
      changeAllLedsColor('03', 1);
    });
    let ledsAllOff = document.querySelector('#btnalloff');
    ledsAllOff.addEventListener('click', function () {
      allLedsOff();
    });

    let ledsMagenta = document.querySelector('#btnamagenta');
    ledsMagenta.addEventListener('click', function () {
      changeLedColor(9, '05', 0);
    });

    connectButton.addEventListener('click', function () {
      if (port) {
        port.disconnect();
        connectButton.textContent = 'Connect';
        statusDisplay.textContent = '';
        port = null;
      } else {
        serial
          .requestPort()
          .then((selectedPort) => {
            port = selectedPort;
            connect();
          })
          .catch((error) => {
            statusDisplay.textContent = error;
          });
      }
    });

    serial.getPorts().then((ports) => {
      if (ports.length === 0) {
        statusDisplay.textContent =
          'No device found. Plug your device and press Connect';
      } else {
        statusDisplay.textContent = 'Connecting...';
        port = ports[0];
        connect();
      }
    });

    let commandLine = document.getElementById('command_line');

    commandLine.addEventListener('keypress', function (event) {
      if (event.keyCode === 13) {
        if (commandLine.value.length > 0) {
          addLine('sender_lines', commandLine.value);
          commandLine.value = '';
        }
      }

      /*port.send(
        new TextEncoder('utf-8').encode(
          String.fromCharCode(event.which || event.keyCode)
        )
      );*/
      // Led L[index][color code 00-99][mode 0-9]
      port.send(new TextEncoder('utf-8').encode('L0032'));
    });
  });
})();
