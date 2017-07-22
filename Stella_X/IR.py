import sys
import time
import serial
import numpy as np
import matplotlib.pyplot as plt

SETUP_TIMEOUT = 2
LOOP_TIMEOUT = 0.2

def graph():
    figure = plt.gcf()
    figure.show()

    with serial.Serial(port=sys.argv[1], baudrate=sys.argv[2]) as ser:
        ser.timeout = SETUP_TIMEOUT
        if ser.readline() == 'vic initialized\n': # wait for arduino setup
            ser.read(3) # prompt
            ser.timeout = LOOP_TIMEOUT
            while True:
                figure.clear()
                ser.write('IRra\n')
                data = map(int, ser.readline().split()[:-2])
                while data == []:
                    data = map(int, ser.readline().split()[:-2])
                ser.read(3)

                plt.bar(range(-1, len(data) + 1), data[-1:]+data+data[:1])
                plt.ylim([0, 1024])
                plt.xlim([-1, len(data)])
                figure.canvas.draw()
        else:
            print 'communication timeout'

def measure():
    LOOP_COUNT = 5
    with serial.Serial(port=sys.argv[1], baudrate=sys.argv[2]) as ser:
        ser.timeout = SETUP_TIMEOUT
        if ser.readline() == 'vic initialized\n': # wait for arduino setup
            ser.read(3) # prompt
            ser.timeout = LOOP_TIMEOUT

            print 'ball off'
            data = []
            for i in range(LOOP_COUNT):
                ser.write('IRra\n')
                ser.read(3)
                data_part = map(int, ser.readline().split())
                while data_part == []:
                    data_part = map(int, ser.readline().split())
                data.append(data_part)
            count = len(data_part)
            print map(lambda a: sum(a) / len(a), zip(*data))

            print 'ball on'
            for i in range(count):
                print '\nsensor', i, '\nhit enter',
                raw_input()
                data = 0
                for j in range(LOOP_COUNT):
                    ser.write('IRr %d\n' % i)
                    ser.read(3)
                    data += int(ser.readline().strip())
                data /= count
                print data

        else:
            print 'communication timeout'


if __name__ == '__main__':
    if not len(sys.argv) == 3:
        print "usage: %s 'port' 'baudrate'" % sys.argv[0]
        quit()

    if raw_input('graph? [y/n]: ') == 'y':
        graph()
    elif raw_input('measure? [y/n]: ') == 'y':
        measure()
