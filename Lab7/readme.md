Lab 7 - Analog to Digital Conversion
===

Objective
---


Prelab
---
Test your sensors with a DMM. Ensure they are functional. What would good reference values be?

| Distance | Left Voltage Reading | Center Voltage Reading | Right Voltage Reading |
|:--------:|:--------------------:|:----------------------:|:---------------------:|
|   1 in   |         1.67         |          2.50          |          1.55         |
|   2 in   |         1.20         |          1.76          |          1.15         |
|   4 in   |         0.87         |          1.18          |          0.80         |
|   6 in   |         0.77         |          1.06          |          0.63         |
|   8 in   |         0.74         |          0.91          |          0.60         |

Using the following script I wrote up, converting voltages to 10-bit binary values was pretty simple:

```python
bits = 10
mid = 1.5

while True:
    analog = input('value to convert:')
    digital = ''
    compare = mid
    step = mid/2
    for b in xrange(bits):
        if analog > compare:
            digital += '1'
            compare += step
        else:
            digital += '0'
            compare -= step
        step = step/2
    print str(analog) + ' to digital: ' + str(digital)
    print ''
```

| Distance | Quantized Value Left | Quantized Value Center | Quantized Value Right |
|:--------:|:--------------------:|:----------------------:|:---------------------:|
|   1 in   |      1000111010      |       1101010101       |       1000010001      |
|   2 in   |      0110011001      |       1001011000       |       0110001000      |
|   4 in   |      0100101000      |       0110010010       |       0100010001      |
|   6 in   |      0100000110      |       0101101001       |       0011010111      |
|   8 in   |      0011111100      |       0100110110       |       0011001100      |

Consider how you'll setup the ADC10 subsystem. What are the registers you'll need to use? Which bits in those registers are important? What's the initialization sequence you'll need?

|   Register  |              Function             |
|:-----------:|:---------------------------------:|
| `ADC10CTL0` |          Control register         |
| `ADC10CTL1` |      Second control register      |
|  `ADC10AE0` | Determines which pin is the input |
|  `ADC10MEM` |    Holds converted 10-bit value   |

Consider the hardware interface. Which ADC10 channels will you use? Which pins correspond to those channels?

|  Pin |     Function     | Input Channel |
|:----:|:----------------:|:-------------:|
| P1.2 |  Left analog in  |    `INCH_2`   |
| P1.3 | Center analog in |    `INCH_3`   |
| P1.4 |  Right analog in |    `INCH_4`   |

Consider the interface you'll create to your sensors. Will you block or use interrupts? Will you convert one sensor at a time or multiple?

I will use interrupts triggered by a timer to poll the sensors. The order of action will be:

1. Post previous value to the respective global variable

2. Set up the ADC to the next input channel

3. Tell `ADC10CTL0` to convert the current voltage

4. Set a static state variable to tell the next interrupt what sensor was polled
