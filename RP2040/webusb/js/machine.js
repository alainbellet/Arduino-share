// ** Event Listener for Page and scripts loaded **  //
document.addEventListener('DOMContentLoaded', function (event) {
  // ** Event Listener on mouse click anywhere on start machine button  //
  const restartMachine = document.querySelector('#restartbutton');
  restartMachine.addEventListener('click', function () {
    //if (!machine_started) {
    startMachine();
    //}
  });
});

// ** Event Listener for User Inputs **  //

document.addEventListener('buttonPressed', function (event) {
  console.log(event.detail.button);
  if (waiting_for_user_input) {
    dialogMachine(event.detail.button);
  } else {
    userInputError();
  }
});

document.addEventListener('potentimeterChange', function (event) {
  console.log(event.detail.potentiometer);
});

// ** Event Listener for Speech **  //

document.addEventListener('speechEnded', function (event) {
  console.log('speech ended');
});

// ** Event Listener for Sound **  //

document.addEventListener('soundEnded', function (event) {
  console.log('sound ended');
});

let machine_started = false;
let waiting_for_user_input = true;
let next_state = '';
let last_state = '';
let button_press_counter = 0;

// ** Prepare audio Objects **  //
const chime_short = new Audio('audio/bell_short.wav');
const chime = new Audio('audio/bell.wav');
const starWars = new Audio('audio/StarWars3.wav');

// ** Start the machine **  //
function startMachine() {
  waiting_for_user_input = true;
  machine_started = true;
  next_state = 'initialisation';
  button_press_counter = 0;
  console.log('Machine started');
  dialogMachine(); // start the machine with first state
}

function dialogMachine(btn = -1) {
  if (next_state != last_state) {
    console.log(`entering State ${next_state}`);
  } else {
    console.log(`Staying in State ${next_state}`);
  }
  last_state = next_state;

  switch (next_state) {
    case 'initialisation':
      console.log('Machine is initialised and ready');
      talkCommands.ledAllOff();
      next_state = 'start';
      break;
    case 'start':
      console.log('we are at the biginning, press button 1 to continue');
      talkVoice.speak('hello', 1);
      talkCommands.ledColor(1, '05');
      //talkSound.playSound(starWars, false);
      next_state = 'two';
      break;

    case 'two':
      if (btn != 1) {
        userInputError();
        console.log('wrong button');
        break;
      }
      console.log('second step');
      talkCommands.ledAllColor('05');
      talkSound.playSound(chime_short, true);
      next_state = 'three';
      break;

    case 'three':
      console.log('last step, all what you do now will be ignored');
      talkSound.pauseSound();
      talkCommands.ledAllOff();
      talkCommands.ledColor(1, '02');
      waiting_for_user_input = false;
      next_state = 'deadend';
      break;

    case 'deadend':
      talkCommands.changeAllLedsColor('05');
      console.log('pfff... you are pressing a button but i ignore it');
      button_press_counter++;
      if (button_press_counter > 2) {
        next_state = 'notamused';
      }
      break;

    case 'notamused':
      button_press_counter++;
      if (button_press_counter < 5) {
        console.log('please stop pressing my buttons');
        break;
      } else if (button_press_counter == 5) {
        console.log('what did i say?');
        break;
      } else {
        next_state = getRandomNextState(['rand1', 'rand2']);
        goToNextState();
      }
      break;

    case 'rand1':
      console.log('yoooo!!!!');
      next_state = 'notamused';
      break;

    case 'rand2':
      console.log('nooooo!!!!');
      next_state = 'notamused';
      break;

    default:
      console.error(`Sorry State: "${next_state}" has no case defined`);
  }
}

function userInputError() {
  talkSound.playErrorBuzzer();
}

function goToNextState() {
  ProcessEvent();
}

function getRandomNextState(statesArray) {
  var rand_index = Math.floor(Math.random() * statesArray.length);
  _next_state = statesArray[rand_index];
  return _next_state;
}
