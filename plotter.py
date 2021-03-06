#!/usr/bin/python
import serial
from collections import deque
import matplotlib.pyplot as plt

'''
Script to read in data from serial and from wiiuse program
borrows ideas from https://gist.github.com/electronut/d5e5f68c610821e311b0
'''
SERIAL_PORT = '/dev/tty.usbmodem1411'
DATA_PTS = 300

# add to data buffer
def add(data, point):
    if len(data) < DATA_PTS:
        data.append(point)
    else:
        data.pop()
        data.appendleft(point)


def main():
    # setup serial connection
    ser = serial.Serial(SERIAL_PORT, 9600)

    # data
    weightData = deque([0.0] * DATA_PTS)
    rawData = deque([0.0] * DATA_PTS)

    # setup plot
    livePlot = plt.plot(weightData, rawData, 'bo')[0]
    plt.title('Raw data vs. measured weight')
    plt.xlabel('Wii board measured weight [lbs]')
    plt.ylabel("Our sensor's raw data [V]")
    plt.xlim([0, 300])
    plt.ylim([0, 1024])

    print '=================================\n| Limb Load Monitor Tester v0.1 |\n================================='

    # data collection loop
    while True:
        try:

            # read raw sensordata from serial
            rawDataPt = float(ser.readline().rstrip())
            print rawDataPt
            #print(rawDataPt)
            add(rawData, rawDataPt)

            # update the plot
            livePlot.set_xdata(weightData)
            livePlot.set_ydata(rawData)
            plt.draw()

        except KeyboardInterrupt:
            print('exiting')
            quit()

    # clean up serial
    ser.flush()
    ser.close()


if __name__ == "__main__":
    main()
