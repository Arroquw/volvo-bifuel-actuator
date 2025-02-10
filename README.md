# volvo-bifuel-actuator
Linear actuator replacement for the volvo type-B stepper motor/linear actuator/control valve on top of the LPG distributor

Personally I have a Volvo S60 from 2002, the bi-fuel edition. It started cutting out while driving, and putting contact off and on while driving would make it work again, also sometimes it wouldn't. I brought it to an LPG specialist who "rebuilt" the stepper, but I expect all he did was clean the system and resolder the through-hole pins on the stepper. I did some research:

The LPG system works by having the vaporiser feed gas into a necam MEGA distributor, which has a linear actuator (which is really just a repurposed idle air control valve) on top of it. Volvo Part Number 31216348. This linear actuator has the exact same mechanical properties as an idle air control valve, for instance the topran 206 167 or the magneti marelli 820003395010, but the main and crucial difference is that it has an integrated driver board on top of the valve.

This driver board gets some PWM signal from the ECM/ECU or whichever module does this, and sends back some sort of "READY_OUT" (I found some semblance of a datasheet, it is in docs/) back. The PWM signal's duty cycle maps to how far it extends, so 20% means completely retracted and 80% means completely extended. Beyond 80% it breaks down because it will eject its shaft. Below 20% it clips into itself, potentially burning the driver board if you're unlucky.

In this repositaory I made some sort of PoC with an arduino nano and a TMC2209 stepper driver, which does seem to kind of work to move the topran or magneti marelli (I ordered both) I mentioned earlier, but if I put it in my car, it won't switch over to LPG, and I expect that is due to how I implemented the READY_OUT signal.

The datasheet seems to imply that the READY_OUT signal goes low when the motor is in a bad state (e.g. coils of the stepper motor shorted - either to ground or each other, PWM signal bad (duty cycle wrong or wrong frequency), too low input voltage, etc.), and I just set it to high.
