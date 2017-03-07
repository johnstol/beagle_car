#include <SimpleTimer.h>

#include <IRremote.h>

#define RECEIVERS 4
#define message_length 9

IRrecv *irrecvs[RECEIVERS];

decode_results results;

SimpleTimer timer;

char data[RECEIVERS][message_length];

char id = "Jo";
int counter[RECEIVERS];
int i, j;
int command;
int command_hist = 0;

bool clear_hist = false;

char * chr_position;

void read_receivers() {
  //6826
  command = B0000;

  for (i = 0; i < RECEIVERS; i++) {
    if (irrecvs[i]->decode(&results)) {
      if (counter[i] < message_length) { //pass value to buffer
        data[i][counter[i]] = results.value, HEX;
        irrecvs[i]->resume();
        counter[i]++;

      }
      else {
        memset(data[i], 0, sizeof(data[i]));
        data[i][0] = results.value, HEX;
        irrecvs[i]->resume();
        counter[i] = 1;
      }
    }
  }


  //check buffers

  for (i = 0; i < RECEIVERS; i++ ) {

    chr_position = strchr(data[i], 'J');
    if (chr_position) {
      chr_position = strchr(chr_position + 1, 'o');
      if (chr_position) {
        chr_position = strchr(chr_position + 1, 'h');
        if (chr_position) {
          chr_position = strchr(chr_position + 1, 'n');
          if (chr_position) { //found!
            if (i == 0) {
              command = command | B0001;
            }
            else if (i == 1) {
              command = command | B0010;
            }
            else if (i == 2) {
              command = command | B0100;
            }
            else {
              command = command | B1000;
            }
            //memset(data[i], 0, sizeof(data[i]));
          }
        }
      }
      if (clear_hist == true) {
        clear_hist = false;
        command_hist = B0000;
      }
      command_hist = command_hist | command;
    }
  }
}

void printcommand() {
  Serial.print("#");
  Serial.print(command_hist, BIN);
  Serial.println("#");
  clear_hist = true;
}


void setup() {
  Serial.begin(9600);

  irrecvs[0] = new IRrecv(2); // Receiver #0: pin 2
  irrecvs[1] = new IRrecv(3); // Receiver #1: pin 3
  irrecvs[2] = new IRrecv(4); // Receiver #2: pin 4
  irrecvs[3] = new IRrecv(5); // Receiver #3: pin 5

  for (i = 0; i < RECEIVERS; i++) {
    irrecvs[i]->enableIRIn();
    counter[i] = 0;
  }

  timer.setInterval(20, read_receivers);
  timer.setInterval(500, printcommand);
}



void loop() {
  timer.run();
}
