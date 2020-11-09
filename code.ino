#define triggerPin D7 // pwm/input
#define echoPin D3 //output

double rest = 0.0;

double startMillis = 0.0;
double currentMillis;
double elapsedMillis;

double duration, distance;

const double marginOfError = 0.3; // in inches
const double speedOfSound = 0.0135; // speed of sound in inches (0.000343 in meters/seconds)

void setup() {
    Serial.begin(9600); // establish communication w/serial port
    pinMode(triggerPin, OUTPUT); // send pulses out from the Argon
    pinMode(echoPin, INPUT); // receives the pulses from the device
}

double getTime() { 
    currentMillis = millis();
    elapsedMillis = (double)((currentMillis - startMillis) / 1000.000); //in seconds
    return elapsedMillis;
}

double getDistance() { // returns distance from the sensor, NOT from equilibrium
    digitalWrite(triggerPin, LOW); // makes sure the trigger pin is clear
    delayMicroseconds(2); 
    digitalWrite(triggerPin, HIGH); // generates an ultrasonic sound wave 
    delayMicroseconds(10);       
    digitalWrite(triggerPin, LOW);
    duration = pulseIn(echoPin, HIGH); // measures the duration of a pulse in microseconds
    distance = (duration / 2) * speedOfSound; // duration = time it takes for the pulse to go out + bounce back.
    return distance;
}

void loop() {
    if(startMillis == 0.0) {
        startMillis = millis();
    }
    if(rest == 0.0) { // first call to getDistance() should be used to assign a value to rest = the distance at which the speed bag is at equilibrium
       rest = getDistance() - marginOfError;
    } 
    double displacement = (getDistance() - rest) * 1.000;
    double timeElapsed = getTime() * 1.000;

    char data[256];
    snprintf(data, sizeof(data), "{\"displacement\":%f, \"time\":%f}", displacement, timeElapsed);

    Particle.publish("motion-detected", data, PRIVATE);
    
    delay(200);
}
